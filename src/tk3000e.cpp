/****************************************************************************
*
*  APPLE //E EMULATOR FOR WINDOWS                    
*
*  Copyright (C) 1994-96, Michael O'Brien.  All rights reserved.
*
***/

/*  Emulador do computador TK3000 //e (Microdigital)
 *  por Fábio Belavenuto - Copyright (C) 2004
 *
 *  Adaptado do emulador Applewin por Michael O'Brien
 *  Part of code is Copyright (C) 2003-2004 Tom Charlesworth
 *
 *  Este arquivo é distribuido pela Licença Pública Geral GNU.
 *  Veja o arquivo Licenca.txt distribuido com este software.
 *
 *  ESTE SOFTWARE NÃO OFERECE NENHUMA GARANTIA
 *
 */

// Módulo Principal

#include "stdhdr.h"
#include "tk3000e.h"
#include "memoria.h"
#include "cpu.h"
#include "disco.h"
#include "disco_rigido.h"
#include "joystick.h"
#include "video.h"
//#include "som_nucleo.h"
#include "SoundCore.h"
//#include "som.h"
#include "Speaker.h"
#include "serial.h"
#include "impressora.h"
#include "teclado.h"
#include "debug.h"
#include "registro.h"
#include "janela.h"
#include "imagem.h"
#include "tkclock.h"
#include "ide.h"
#include "tape.h"
#include "timer.h"
#include "Mockingboard.h"

// Variáveis:

#ifdef CPUDEBUG
FILE *arquivocpu;
#endif

BOOL   CPUAtual          = CPU65C02;
DWORD  cumulativecycles  = 0;
DWORD  cyclenum          = 0;
BOOL   fullspeed         = 0;
int    instance          = 0;
DWORD  lastfastpaging    = 0;
DWORD  lasttrimimages    = 0;
int    mode              = MODE_LOGO;
int    lastmode          = MODE_LOGO;
char   progdir[MAX_PATH] = "";
char   tempdir[MAX_PATH] = "";
BOOL   resettiming       = 0;
BOOL   restart           = 0;
DWORD  speed             = 10;
DWORD  dummy             = 0;
int	   numexpages        = 16;
int    g_nCpuCyclesFeedback = 0;
double g_fCurrentCLK6502 = CLK_6502;
unsigned __int64 g_nCumulativeCycles = 0;
double g_fMHz            = 1.0;	// Affected by Config dialog's speed slider bar
FILE*  g_fh              = NULL;

//===========================================================================
void ContinueExecution ()
{
	static   DWORD dwVideoUpdateTick = 0;
	static   BOOL pageflipping    = 0; //?
	int      nNumCyclesPerFrame = 23191; // 23191
	DWORD    nCyclesToExecute;
	DWORD    executedcycles;
	DWORD    CLKS_PER_MS;
	BOOL     diskspinning;
	BOOL     screenupdated;
	BOOL     systemidle;

	fullspeed = ( (speed == SPEED_MAX) || 
				g_bScrollLock_FullSpeed ||
//				 (GetKeyState(VK_SCROLL) < 0) ||
				 (DiskIsSpinning() && enhancedisk ) ||
				 (ColarRapido && KeybIsPasting())
				 );

	if(fullspeed)
	{
		Timer_StopTimer();
	}
	else
	{
		Timer_StartTimer((ULONG)((double)nNumCyclesPerFrame / g_fMHz));
	}

	nCyclesToExecute = nNumCyclesPerFrame + g_nCpuCyclesFeedback;
	executedcycles = CpuExecute(nCyclesToExecute);

	cyclenum = executedcycles;
	//g_nCumulativeCycles += executedcycles;
	//cumulativecycles += executedcycles;
	
	// Atualizações:
	DiskUpdatePosition(executedcycles);
	JoyUpdatePosition(executedcycles);
	HDAtualiza(executedcycles);
	IDEAtualiza(executedcycles);
	TKClockAtualiza(executedcycles);
	TapeAtualiza(executedcycles);
	CommUpdate(executedcycles);
	ImpressoraAtualiza(executedcycles);
	MB_UpdateCycles(executedcycles);
	SpkrUpdate(executedcycles);
	CommUpdate(executedcycles);
	//
	
	if (cpuemtype == CPU_FASTPAGING)	//?
	{
		if ((!pages) && (cumulativecycles-lastfastpaging > 500000))
		{
			MemSetFastPaging(0);
		}
		else if (cumulativecycles-lasttrimimages > 500000)
		{
			MemTrimImages();
			lasttrimimages = cumulativecycles;
		}
	}

	CLKS_PER_MS = (DWORD)g_fCurrentCLK6502 / 1000;

	emulmsec_frac += executedcycles;
	if(emulmsec_frac > CLKS_PER_MS)
	{
		emulmsec += emulmsec_frac / CLKS_PER_MS;
		emulmsec_frac %= CLKS_PER_MS;
	}

	pages = 0;	//?
	

	//
	// DETERMINE WHETHER THE SCREEN WAS UPDATED, THE DISK WAS SPINNING,
	// OR THE KEYBOARD I/O PORTS WERE BEING EXCESSIVELY QUERIED THIS CLOCKTICK
	VideoCheckPage(0);
	diskspinning  = DiskIsSpinning();
	screenupdated = VideoHasRefreshed();
	systemidle    = 0;	//(KeybGetNumQueries() > (clockgran << 2));	//  && (!ranfinegrain);	// TODO

	if(screenupdated)
		pageflipping = 3;

	//
	// IF A TWENTIETH OF A SECOND HAS ELAPSED AND THE SCREEN HAS NOT BEEN
	// UPDATED BUT IT APPEARS TO NEED UPDATING, THEN REFRESH IT
	if(mode != MODE_LOGO)
	{
		const DWORD _50ms = 50;	// 50ms == 20Hz
		DWORD dwCyclesPerScreenUpdate = _50ms * (DWORD) (g_fCurrentCLK6502 / 1000.0);

		if((GetTickCount() - dwVideoUpdateTick) > _50ms)
		{
			static BOOL  anyupdates     = 0;
			static DWORD lastcycles     = 0;
			static BOOL  lastupdates[2] = {0,0};

			dwVideoUpdateTick = GetTickCount();
			VideoUpdateFlash();

			anyupdates |= screenupdated;

			if ((cumulativecycles-lastcycles) >= dwCyclesPerScreenUpdate)
			{
				lastcycles = cumulativecycles;
				if ((!anyupdates) && (!lastupdates[0]) && (!lastupdates[1]) &&
					VideoApparentlyDirty())
				{
					static DWORD lasttime = 0;
					DWORD currtime = GetTickCount();

					VideoCheckPage(1);
					if ((!fullspeed) ||
						(currtime-lasttime >= (DWORD)((graphicsmode || !systemidle) ? 100 : 25)))
					{
						VideoRefreshScreen();
						lasttime = currtime;
					}
					screenupdated = 1;
				}

				lastupdates[1] = lastupdates[0];
				lastupdates[0] = anyupdates;
				anyupdates     = 0;

				if (pageflipping)
					pageflipping--;
			}
			MB_EndOfVideoFrame();
		}
	}
	if(!fullspeed)
	{
		Timer_WaitTimer();
	}
}

//===========================================================================
void SetCurrentCLK6502()
{
	static DWORD dwPrevSpeed = (DWORD) -1;

	if(dwPrevSpeed == speed)
		return;

	dwPrevSpeed = speed;

	// SPEED_MIN    =  0 = 0.50 Mhz
	// SPEED_NORMAL = 10 = 1.00 Mhz
	//                20 = 2.00 MHz
	// SPEED_MAX-1  = 39 = 3.90 Mhz
	// SPEED_MAX    = 40 = ???? MHz (run full-speed, /g_fCurrentCLK6502/ is ignored)

	if(speed < SPEED_NORMAL)
		g_fMHz = 0.5 + (double)speed * 0.05;
	else
		g_fMHz = (double)speed / 10.0;

	g_fCurrentCLK6502 = CLK_6502 * g_fMHz;

	//
	// Now re-init modules that are dependent on /g_fCurrentCLK6502/
	//

	SpkrReinitialize();
	MB_Reinitialize();
}

//===========================================================================
void EnterMessageLoop()
{
	MSG message;
	while (GetMessage(&message,0,0,0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
		while ((mode == MODE_RUNNING) || (mode == MODE_STEPPING))
		{
			if (PeekMessage(&message,0,0,0,PM_REMOVE))
			{
				if (message.message == WM_QUIT)
					return;
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			else if (mode == MODE_STEPPING)
				DebugContinueStepping();
			else
			{
				ContinueExecution();
				if (fullspeed)
					ContinueExecution();
			}
		}
	}
	while (PeekMessage(&message,0,0,0,PM_REMOVE));
}

//===========================================================================
void GetProgramDirectory()
{
	int loop;

	GetModuleFileName((HINSTANCE)0, progdir, MAX_PATH);
	progdir[MAX_PATH-1] = 0;
	loop = strlen(progdir);
	while (loop--)
		if ((progdir[loop] == '\\') ||
					(progdir[loop] == ':'))
		{
			progdir[loop+1] = 0;
			break;
		}
	GetTempPath(MAX_PATH, tempdir);
}

//===========================================================================
void LoadConfiguration()
{
	int i;

#define LOAD(a,b) RegLoadValue(CONFIG,a,1,(unsigned long *)b);
	LOAD(EMUJOY,         &joytype);
	LOAD(EMUSOM,         &soundtype);
//	LOAD(EMUSOMVOL,      &VolumeSom);
	LOAD(EMUSERIAL,      &serialport);
	LOAD(EMUVELCPU,      &speed);
	LOAD(EMUDISCO,       (DWORD *)&enhancedisk);
	LOAD(EMUVIDEO,       &videotype);
	LOAD(EMUVIDEOM,      &monochrome);
	LOAD(EMUBANCOS,      (DWORD *)&numexpages);
	LOAD(EMUIMPRESSORA,  &ImpressoraPorta);
	LOAD(EMUDISCORIG,    &HDTamanho);
	LOAD(EMUSCANLINES,   &ScanLines); // Scan Lines
	LOAD(EMUCOLARRAPIDO, &ColarRapido);
	LOAD(SLOT1, &NewSlots[1]);
	LOAD(SLOT2, &NewSlots[2]);
//	LOAD(SLOT3, &NewSlots[3]);
	LOAD(SLOT4, &NewSlots[4]);
	LOAD(SLOT5, &NewSlots[5]);
	LOAD(SLOT6, &NewSlots[6]);
	LOAD(SLOT7, &NewSlots[7]);
#undef LOAD
	for (i=0; i<8; i++)
		Slots[i] = 0;
	SetCurrentCLK6502();
}


//===========================================================================
void RegisterExtensions ()
{
	char  command[MAX_PATH];
	char  icon[MAX_PATH];

	GetModuleFileName((HMODULE)0, command, MAX_PATH);
	command[MAX_PATH-1] = 0;
	sprintf(icon,"%s,1",(char*)command);
	strcat(command," %1");

#define CHAVE "ImagemDisco"

	RegSetValue(HKEY_CLASSES_ROOT,".do" ,REG_SZ,CHAVE,10);
	RegSetValue(HKEY_CLASSES_ROOT,".dsk",REG_SZ,CHAVE,10);
	RegSetValue(HKEY_CLASSES_ROOT,".nib",REG_SZ,CHAVE,10);
	RegSetValue(HKEY_CLASSES_ROOT,".po" ,REG_SZ,CHAVE,10);
	RegSetValue(HKEY_CLASSES_ROOT,".hdv",REG_SZ,CHAVE,10);
	RegSetValue(HKEY_CLASSES_ROOT,".ide",REG_SZ,CHAVE,10);
	RegSetValue(HKEY_CLASSES_ROOT,CHAVE, REG_SZ,CHAVE,21);
	RegSetValue(HKEY_CLASSES_ROOT,CHAVE"\\DefaultIcon",
				REG_SZ,icon,strlen(icon)+1);
	RegSetValue(HKEY_CLASSES_ROOT,CHAVE"\\shell\\open\\command",
				REG_SZ,command,strlen(command)+1);
#undef CHAVE
}

//===========================================================================
void SaveSlots()
{
#define SAVE(a,b) RegSaveValue(CONFIG, a, 1, b);
					SAVE(SLOT1, Slots[1]);
					SAVE(SLOT2, Slots[2]);
//					SAVE(SLOT3, Slots[3]);
					SAVE(SLOT4, Slots[4]);
					SAVE(SLOT5, Slots[5]);
					SAVE(SLOT6, Slots[6]);
					SAVE(SLOT7, Slots[7]);
#undef SAVE
}

//===========================================================================
void Finalizar()
{
	if (!restart)
	{
		DiskDestroy();
		ImageDestroy();
		ImpressoraFinaliza();
		HDFinaliza();
		IDEFinaliza();
		TapeFinaliza();
	}
	MemRetiraTodosSlots();
	DebugDestroy();
	JoyDestroy();
	CpuDestroy();
	MemDestroy();
	VideoDestroy();
	//Som_Finaliza();
	SpkrDestroy();
	MB_Destroy();
	SpkrDestroy();
	Timer_UninitTimer();
	CommDestroy();
//	for (
}

//===========================================================================
int APIENTRY WinMain (HINSTANCE passinstance,
					  HINSTANCE previnstance,
					  LPSTR     lpCmdLine,
					  int       nCmdShow)
{
	char imagefilename[MAX_PATH];
#ifdef CPUDEBUG
	char nomearq[MAX_PATH];
#endif

	// DO ONE-TIME INITIALIZATION
	instance = (int)passinstance;

	// Initialize COM
	CoInitialize( NULL );

	GdiSetBatchLimit(512);
	GetProgramDirectory();
	RegisterExtensions();
	FrameRegisterClass();
	CreateColorMixMap();	// For tv emulation mode

	// Inicializações:
//	bool bSysClkOK = SysClk_InitTimer();
	ImageInitialize();
	DiskInitialize();
	ImpressoraInicializa();
	HDInicializa();
	IDEInicializa();
	TapeInicializa();

	strcpy(imagefilename,progdir);
	strcat(imagefilename,"TKDOS33.dsk");
	if (lpCmdLine[0] != '\0')
	{
		CharLowerBuff(&lpCmdLine[strlen(lpCmdLine)-3],3);
		strcpy(imagefilename, lpCmdLine);
	}

#ifdef CPUDEBUG
	strcpy(nomearq, progdir);
	strcat(nomearq, "debugCPU.txt");
	DeleteFile(nomearq);
	arquivocpu = fopen(nomearq, "wb");
/*
	arquivocpu = CreateFile(nomearq,
						GENERIC_WRITE,
						FILE_SHARE_WRITE,
						(LPSECURITY_ATTRIBUTES)NULL,
						OPEN_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	SetFilePointer(arquivocpu, 0, NULL,FILE_END);
*/
#endif

	do
	{
		// DO INITIALIZATION THAT MUST BE REPEATED FOR A RESTART
		restart = 0;
		mode    = MODE_LOGO;
		LoadConfiguration();
		
		// Inicializações:
		DebugInitialize();
		JoyInitialize();
		MemInitialize();
		VideoInitialize();
		Timer_InitTimer();
		FrameCreateWindow();
		MemAtualizaSlots();		// Deve ser chamado após FrameCreateWindow()
		DSInit();
//		Som_Inicializa();		// Deve ser chamado após FrameCreateWindow()
		SpkrInitialize();		// Deve ser chamado após FrameCreateWindow()
		MB_Initialize();

		if (imagefilename[0] != '\0')
		{
			if (!strncmp(&imagefilename[strlen(imagefilename)-3],"hdv",3))
			{
				HDAbreImagem(imagefilename,0);
			}
			else if (!strncmp(&imagefilename[strlen(imagefilename)-3],"ide",3))
			{
				IDEAbreImagem(imagefilename);
			}
			else
			{
				DiskInsert(0,imagefilename,FALSE,FALSE);
			}
			imagefilename[0] = '\0';
		}

		// ENTER THE MAIN MESSAGE LOOP
		EnterMessageLoop();
		MB_Reset();
	} while (restart);

#ifdef CPUDEBUG
	//CloseHandle(arquivocpu);
	fclose(arquivocpu);
#endif

	// Necessary otherwise debug build crashes in NTDLL/WinXP (after booting a disk)
	Sleep(20);	// Use 20ms to be safe (9ms=NG, 10ms=OK)

//	SysClk_UninitTimer();
	// Release COM
	CoUninitialize();

	return 0;
}

// EOF