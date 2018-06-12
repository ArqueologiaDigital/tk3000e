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

// Manipula a janela principal e diálogos

#include "stdhdr.h"
#include "janela.h"
#include "tk3000e.h"
#include "joystick.h"
#include "registro.h"
#include "teclado.h"
#include "debug.h"
//#include "som_nucleo.h"
#include "SoundCore.h"
//#include "som.h"
#include "Speaker.h"
#include "tape.h"
#include "disco_rigido.h"
#include "ide.h"
#include "imagem.h"
#include "impressora.h"
#include "disco.h"
#include "serial.h"
#include "cpu.h"
#include "memoria.h"
#include "video.h"
#include "mouse.h"
#include "cpu.h"
#include "z80emu.h"
#include "timer.h"
#include "../recursos/resource.h"
#include "utilitarios.h"
#include "Mockingboard.h"

// Definições
#define	WIDTH_DIALOGO	240
#define	HEIGHT_DIALOGO	120
#define POSX_ROTULO		10
#define POSY_ROTULO		15
#define POSX_CEDIT		166
#define POSY_CEDIT		13
#define POSX_BTNOK      20
#define POSY_BTNOK      55
#define POSX_BTNCANC    130
#define POSY_BTNCANC    55

// Definições
#define STATUSHEIGHT	26
#define STATUSYPOS1		3
#define STATUSYPOS2		STATUSYPOS1 + 11
#define STATUSYPOS3		STATUSYPOS1 + 8
#define STATUSYPOS4		STATUSYPOS3 + 8
#define STATUSXPOS1		4
#define STATUSXPOS2		STATUSXPOS1 + 173
#define STATUSXPOS3		STATUSXPOS2 + 173
#define STATUSXPOS4		STATUSXPOS3 + 173
#define VIDEOLEFT		0
#define VIDEOTOP		0
#define VIDEOWIDTH		560
#define VIDEOHEIGHT		384

#define VIEWPORTCX	VIDEOWIDTH
#define VIEWPORTCY	VIDEOHEIGHT
#define FSVIEWPORTX ((640-VIDEOWIDTH)>>1)
#define FSVIEWPORTY (1)

// Variaveis
BOOL	ativo			= 0;
BOOL	pintando		= 0;
BOOL	usingcursor		= 0;
int 	framewindow		= 0;
BOOL    fullscreen      = 0;
HDC		framedc			= (HDC)0;
RECT	framerect		= {0,0,0,0};
HFONT	smallfont		= (HFONT)0;
HBITMAP	capsbitmap[2];
HBITMAP	diskbitmap[3];
HBITMAP	modebitmap[2];
HBITMAP	z80bitmap[2];
HBRUSH	btnfacebrush    = (HBRUSH)0;
HPEN	btnfacepen      = (HPEN)0;
HPEN	btnhighlightpen = (HPEN)0;
HPEN	btnshadowpen    = (HPEN)0;
LPDIRECTDRAW        directdraw = (LPDIRECTDRAW)0;
LPDIRECTDRAWSURFACE surface    = (LPDIRECTDRAWSURFACE)0;
int     viewportx       = 0;
int     viewporty       = 0;
bool	g_bScrollLock_FullSpeed = false;

HWND Janela;
HWND Rotulo;
HWND edtValor;
HWND btnOK;
HWND btnCancelar;
int  BotaoApertado = 0;

char   opsJoystick[]   = "Desabilitado\0"
                          "Joystick do PC\0"
                          "Teclado (padrão)\0"
                          "Teclado (centralizado)\0"
                          "Mouse\0";

char   opsSerial[]     = "Nenhuma\0"
                          "COM1\0"
                          "COM2\0"
                          "COM3\0"
                          "COM4\0"
                          "Arquivo 'Serial.txt'\0";

char	opsImpressora[]	= "Nenhum\0"
                          "LPT1\0"
                          "LPT2\0"
                          "Arquivo 'Impressora.txt'\0";

char   opsSom[]        = "Desabilitado\0"
                          "PC Speaker (direto)\0"
                          "PC Speaker (translatado)\0"
                          "Placa de Som\0";

char   opsVideo[]      = "Monocromático\0"
                          "Colorido (padrão)\0"
                          "Colorido (otimizado)\0"
                          "Emulação de TV\0" ;

char   opsDisco[]      = "Velocidade Real\0"
                          "Velocidade Rápida\0";

char   opsDiscoRigido[]= "Tamanho Padrão de  4MB\0"
                          "Tamanho Padrão de  8MB\0"
                          "Tamanho Padrão de 16MB\0"
                          "Tamanho Padrão de 32MB\0";

char	opsBancos[]	    = " 16 Bancos (1024 KB)\0"
                          "128 Bancos (8192 KB)\0";

char	opsSLOTS[]		= "Vazio\0"
                          "Super Parallel Card\0"
                          "Super Serial\0"
						  "AppleClock\0"
						  "TK CLOCK\0"
						  "Mouse\0"
						  "CP/M Z80\0"
						  "Interface Disk II\0"
						  "Mass Storage (HD)\0";
//						  "Mockingboard\0";

char	opsSLOTS7[]		= "Vazio\0"
                          "Super Parallel Card\0"
                          "Super Serial\0"
						  "AppleClock\0"
						  "TK CLOCK\0"
						  "Mouse\0"
						  "CP/M Z80\0"
						  "Interface Disk II\0"
						  "Mass Storage (HD)\0"
						  "Interface IDE Richard\0";

// Prototipos
LRESULT CALLBACK WndProcInteiro(
	HWND   Handle,		// handle of window
	UINT   Mensagem,	// message identifier
	WPARAM wParam,		// first message parameter
	LPARAM lParam 		// second message parameter
);

// Protótipos
LRESULT CALLBACK FrameWndProc(  HWND   window,
								UINT   message,
								WPARAM wparam,
								LPARAM lparam);
BOOL   CALLBACK ConfigDlgProc(  HWND   window,
								UINT   message,
								WPARAM wparam,
								LPARAM lparam);
BOOL CALLBACK ImportarDlgProc(  HWND   window,
								UINT   message,
								WPARAM wparam,
								LPARAM lparam);
BOOL CALLBACK ExportarDlgProc(  HWND   window,
								UINT   message,
								WPARAM wparam,
								LPARAM lparam);

void ProcessaMenu(int ID, HWND window);
void Drawframewindow();
void ResetMachineState();
void DrawStatusArea(HDC passdc, BOOL drawflags);
void DrawBitmapRect (HDC dc, int x, int y, LPRECT rect, HBITMAP bitmap);
void CreateGdiObjects(void);
void DeleteGdiObjects();
void SetUsingCursor (BOOL newvalue);
void EnableTrackbar (HWND window, BOOL enable);
void FillComboBox(HWND window, int controlid, LPCTSTR choices, int currentchoice);
void DrawCrosshairs (int x, int y);
void HabilitaMenu(UINT);

// Funções
LRESULT CALLBACK WndProcInteiro(
	HWND   Handle,		// handle of window
	UINT   Mensagem,	// message identifier
	WPARAM wParam,		// first message parameter
	LPARAM lParam 		// second message parameter
)
{
	switch (Mensagem)
	{
		case WM_PAINT:
			//return 0;
		break;

		case WM_CLOSE:
			//
		break;

		case WM_CREATE:
			BotaoApertado = 0;
		break;

		case WM_DESTROY:
			//
		break;

		case WM_COMMAND:
		{
			if ((HWND)lParam == btnOK)
			{
				BotaoApertado = 1;
				return 0;
			}
			else if ((HWND)lParam == btnCancelar)
			{
				BotaoApertado = 2;
				return 0;
			}
		}
		break;
	}
	return DefWindowProc(Handle,Mensagem,wParam,lParam);
}

// Funções Callback
LRESULT CALLBACK FrameWndProc (HWND   window,
                               UINT   message,
                               WPARAM wparam,
                               LPARAM lparam)
{
	switch (message)
	{
		case WM_ACTIVATE:
			JoyReset();
			SetUsingCursor(0);
		break;

		case WM_ACTIVATEAPP:
			ativo = wparam;
		break;

		case WM_CLOSE:
			if (fullscreen)
				SetNormalMode();
			if (!restart && (mode != MODE_LOGO))
			{
				//Som_Mute();
				if (MessageBox((HWND)framewindow,
							"Deseja realmente sair?",
							"Sair do Emulador",
							MB_ICONQUESTION | MB_YESNO | MB_SETFOREGROUND) == IDNO)
				{
					return 0;
				}
				//Som_Demute();
			}
			if (!IsIconic(window))
				GetWindowRect(window,&framerect);
			RegSaveValue(PREFERENCIAS,POSX,1,framerect.left);
			RegSaveValue(PREFERENCIAS,POSY,1,framerect.top);
			FrameReleaseDC();
			SetUsingCursor(0);
#ifdef USAR_HELP
			if (helpquit)
			{
				helpquit = 0;
				HtmlHelp(NULL,NULL,HH_CLOSE_ALL,0);
			}
#endif
		break;

		case WM_CHAR:
			if ((mode == MODE_RUNNING) || (mode == MODE_LOGO) ||
					((mode == MODE_STEPPING) && (wparam != '\x1B')))
				KeybQueueKeypress((int)wparam,1);
			else if ((mode == MODE_DEBUG) || (mode == MODE_STEPPING))
				DebugProcessChar((char)wparam);
		break;

		case WM_CREATE:
			framewindow = (int)window;
			CreateGdiObjects();
			DragAcceptFiles(window, 1);
		break;

		case WM_DESTROY:
			DragAcceptFiles(window, 0);
			DeleteGdiObjects();
			Finalizar();
			PostQuitMessage(0);
		break;

		case WM_DISPLAYCHANGE:
			VideoReinitialize();
		break;

		case WM_DROPFILES:
			{
				char filename[MAX_PATH];
				int  error;

				DragQueryFile((HDROP)wparam, 0, filename, sizeof(filename));
				error = DiskInsert(0, filename, 0, 0);
				if (error)
			        DiskNotifyInvalidImage(filename, error);
				DragFinish((HDROP)wparam);
			}
		break;

		case WM_KEYDOWN:
			if (((wparam >= VK_F1) && (wparam <= VK_F8))  &&
				(GetKeyState(VK_SHIFT) >= 0) &&
				(GetKeyState(VK_CONTROL) >= 0) &&
				(GetKeyState(VK_MENU) >= 0))
			{
				switch (wparam)
				{
					case VK_F1:
						ProcessaMenu(mnuAjuda, window);
					break;

					case VK_F2:
						ProcessaMenu(mnuDepurar, window);
					break;

					case VK_F3:
						ProcessaMenu(mnuConfig, window);
					break;

					case VK_F4:
						ProcessaMenu(mnuConfigSlots, window);
					break;

					case VK_F5:
						ProcessaMenu(mnuResetar, window);
					break;

					case VK_F6:
						ProcessaMenu(mnuDisco1, window);
					break;

					case VK_F7:
						ProcessaMenu(mnuDisco2, window);
					break;

					case VK_F8:
						ProcessaMenu(mnuFullScreen, window);
					break;
				}
			}

			if ((wparam == VK_F9) &&
				(GetKeyState(VK_SHIFT) >= 0) &&
				(GetKeyState(VK_CONTROL) >= 0) &&
				(GetKeyState(VK_MENU) >= 0))
			{
				if (++videotype == VT_NUM_MODES)
					videotype = VT_MONO;
				VideoReinitialize();
				if ((mode != MODE_LOGO) && (mode != MODE_DEBUG))
					VideoRedrawScreen();
				RegSaveValue(CONFIG,EMUVIDEO,1,videotype);
			}
			else if (wparam == VK_CAPITAL)
				KeybAjustaCapsLock();
			else if (wparam == VK_PAUSE)
			{
				SetUsingCursor(0);
				switch (mode)
				{
					case MODE_RUNNING:  mode = MODE_PAUSED;              break;
					case MODE_PAUSED:   mode = MODE_RUNNING;             break;
					case MODE_STEPPING: DebugProcessChar('\x1B');  break;
				}
				DrawStatusArea((HDC)0,DRAW_TITLE);
				if ((mode != MODE_LOGO) && (mode != MODE_DEBUG))
					VideoRedrawScreen();
				resettiming = 1;
			}
			else if ((wparam == VK_SCROLL))
			{
				g_bScrollLock_FullSpeed = !g_bScrollLock_FullSpeed;
				DrawStatusArea((HDC)0,DRAW_TITLE);
			}
			else if ((mode == MODE_RUNNING) || (mode == MODE_LOGO) || (mode == MODE_STEPPING))
			{
				BOOL autorep  = ((lparam & 0x40000000) != 0);
				BOOL extended = ((lparam & 0x01000000) != 0);
				if ((!JoyProcessKey((int)wparam,extended,1,autorep)) &&
									(mode != MODE_LOGO))
					KeybQueueKeypress((int)wparam,0);
			}
			else if (mode == MODE_DEBUG)
			{
				DebugProcessCommand(wparam);
				return 0;
			}
		break;

		case WM_KEYUP:
			JoyProcessKey((int)wparam,((lparam & 0x01000000) != 0),0,0);
		break;


		case WM_LBUTTONDOWN:
		{
			int x = LOWORD(lparam);
			int y = HIWORD(lparam);
			int xx = 0, yy = VIDEOHEIGHT;
			RECT rect = {0,0,30,8};

			MouseBotao = 1;
			if (usingcursor)
				if (wparam & (MK_CONTROL | MK_SHIFT))
					SetUsingCursor(0);
				else
					JoySetButton(0,1);
				else if (JoyUsingMouse() &&
							((mode == MODE_RUNNING) || (mode == MODE_STEPPING)))
					SetUsingCursor(1);
			// Verifica se o usuario clicou sobre o icone da tecla CAPSLOCK
			if ((x >= (xx + STATUSXPOS4)) && (x <= (xx + STATUSXPOS4 + 30)) &&
				(y >= (yy + STATUSYPOS1)) && (y <= (yy + STATUSYPOS1 + 8)))
			{
				KeybToggleCapsLock();
			}
			// Verifica se o usuario clicou sobre o icone da tecla MODE
			if ((x >= (xx + STATUSXPOS4)) && (x <= (xx + STATUSXPOS4 + 30)) &&
				(y >= (yy + STATUSYPOS3)) && (y <= (yy + STATUSYPOS3 + 8)))
			{
				KeybToggleModeKey();
			}
		}
		break;

		case WM_LBUTTONUP:
			MouseBotao = 0;
			if (usingcursor)
				JoySetButton(0,0);
		break;

		case WM_MOUSEMOVE:
		{
			int x = LOWORD(lparam);
			int y = HIWORD(lparam);
			MousePosX = x;
			MousePosY = y;
			if (usingcursor)
			{
				DrawCrosshairs(x,y);
				JoySetPosition(x-VIDEOLEFT-2,VIDEOWIDTH-4,
								y-VIDEOTOP-2,VIDEOHEIGHT-4);
			}
		}
		break;

		case WM_PAINT:
			if (GetUpdateRect(window, NULL, 0))
			{
				pintando = 1;
				Drawframewindow();
				pintando = 0;
			}
		break;

		case WM_PALETTECHANGED:
			if ((HWND)wparam == window)
				break;
      // fall through

		case WM_QUERYNEWPALETTE:
			Drawframewindow();
		break;

		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			if (usingcursor)
				JoySetButton(1,(message == WM_RBUTTONDOWN));
		break;

		case WM_SYSCOLORCHANGE:
			DeleteGdiObjects();
			CreateGdiObjects();
		break;

		case WM_SYSCOMMAND:

			switch (wparam & 0xFFF0)
			{
				case SC_KEYMENU:
					if (fullscreen && ativo)
						return 0;
				break;

				case SC_MINIMIZE:
					GetWindowRect(window,&framerect);
				break;

				case SC_MOUSEMENU:
					//Som_Mute();
				break;
			}
		break;

		case WM_SYSKEYDOWN:
			PostMessage(window, WM_KEYDOWN, wparam, lparam);
			if ((wparam == VK_F10) || (wparam == VK_MENU))
				return 0;
		break;

		case WM_SYSKEYUP:
			PostMessage(window,WM_KEYUP,wparam,lparam);
		break;

		case WM_USER+1:
		{
			HCURSOR oldcursor;

			if (mode != MODE_LOGO)
			{
				//Som_Mute();
				if (MessageBox((HWND)framewindow,
							"Ao rodar o benchmarks o estado da máquina será "
							"resetado, causando perda de qualquer trabalho "
							"não salvo.\n\n"
							"Você quer continuar?",
							"Benchmarks",
							MB_ICONQUESTION | MB_YESNO | MB_SETFOREGROUND) == IDNO)
				{
					break;
				}
				//Som_Demute();
			}
			UpdateWindow(window);
			ResetMachineState();
			mode = MODE_LOGO;
			DrawStatusArea((HDC)0,DRAW_TITLE);
			oldcursor = SetCursor(LoadCursor(0,IDC_WAIT));
			VideoBenchmark();
			ResetMachineState();
			SetCursor(oldcursor);
		break;
		}

		case WM_USER+2:
			if (mode != MODE_LOGO)
			{
				//Som_Mute();
				if (MessageBox((HWND)framewindow,
							"Ao reiniciar o emulador o estado da máquina será "
							"resetado, causando perda de qualquer trabalho "
							"não salvo.\n\n"
							"Você quer continuar?",
							"Configuração",
							MB_ICONQUESTION | MB_YESNO | MB_SETFOREGROUND) == IDNO)
				{
					break;
				}
				//Som_Demute();
			}
			restart = 1;
			PostMessage(window,WM_CLOSE,0,0);
		break;

		case WM_COMMAND:
		{
			if (HIWORD(wparam) == 0)
			{
				ProcessaMenu(LOWORD(wparam), window);
				return 0;
			}
		}
		break;
	}
	return DefWindowProc(window,message,wparam,lparam);
}

//===========================================================================
BOOL CALLBACK ConfigDlgProc (HWND   window,
                             UINT   message,
                             WPARAM wparam,
                             LPARAM lparam)
{
	static BOOL afterclose = 0;
	int new2numexpages     = 0;
	int novo2discorigido   = 0;

	switch (message)
	{
		case WM_COMMAND:
			switch (LOWORD(wparam))
			{
				case IDOK:
				{
					DWORD newvidtype      = (DWORD)SendDlgItemMessage(window, cbVideo,       CB_GETCURSEL,0,0);
					DWORD newjoytype      = (DWORD)SendDlgItemMessage(window, cbJoystick,    CB_GETCURSEL,0,0);
					DWORD newsoundtype    = (DWORD)SendDlgItemMessage(window, cbSom,         CB_GETCURSEL,0,0);
					DWORD newserialport   = (DWORD)SendDlgItemMessage(window, cbSerial,      CB_GETCURSEL,0,0);
					DWORD novoimpressora  = (DWORD)SendDlgItemMessage(window, cbImpressora,  CB_GETCURSEL,0,0);
					BOOL  newdisktype     = (BOOL) SendDlgItemMessage(window, cbDisco,       CB_GETCURSEL,0,0);
					int   newVolumeSom    = (int)  SendDlgItemMessage(window, slVolume,      TBM_GETPOS,0,0);
					int   newnumexpages = 0;

					switch ((int) SendDlgItemMessage(window, cbDiscoRigido, CB_GETCURSEL,0,0))
					{
						case 0: HDTamanho =  4;	break;
						case 1: HDTamanho =  8;	break;
						case 2: HDTamanho = 16;	break;
						case 3: HDTamanho = 32;	break;
					}
					switch ((int)  SendDlgItemMessage(window, cbBancos, CB_GETCURSEL,0,0))
					{
						case 0: newnumexpages =  16; break;
						case 1: newnumexpages = 128; break;
					}
					if (newdisktype != enhancedisk)
					{
						if (MessageBox(window,
									"Você mudou a velocidade de acesso ao disco. "
									"Essa mudança terá efeito somente da próxima "
									"vez que o emulador for reiniciado.\n\n"
									"Você gostaria de reiniciar agora?",
									"Configuração",
									MB_ICONQUESTION | MB_YESNO | MB_SETFOREGROUND) == IDYES)
						{
							afterclose = 2;
						}
					}
					if (!JoySetEmulationType(window,newjoytype))
					{
						afterclose = 0;
						return 0;
					}
					if (!SpkrSetEmulationType(window,newsoundtype))
					{
						afterclose = 0;
						return 0;
					}
					if (videotype != newvidtype)
					{
						videotype = newvidtype;
						VideoReinitialize();
						if ((mode != MODE_LOGO) && (mode != MODE_DEBUG))
							VideoRedrawScreen();
					}
					numexpages = newnumexpages;
					// Scan Lines
					int NewScanLines = IsDlgButtonChecked(window, cbScanLines) ? 1 : 0;
					if (ScanLines != NewScanLines)
					{
						ScanLines = NewScanLines;
						VideoRedrawScreen();
					}

					ColarRapido = IsDlgButtonChecked(window, cbColarRapido) ? 1 : 0;

					CommSetSerialPort(window,newserialport);
					if (novoimpressora != ImpressoraPorta) 
						ImpressoraDefinePorta(novoimpressora);
					if (IsDlgButtonChecked(window, rbVelReal))
						speed = SPEED_NORMAL;
					else
						speed = SendDlgItemMessage(window, slVel, TBM_GETPOS,0,0);
					SetCurrentCLK6502();
					//Som_ConfiguraVolume(newVolumeSom);
					SpkrSetVolume(newVolumeSom, 100);

#define SAVE(a,b) RegSaveValue(CONFIG, a, 1, b);
					SAVE(EMUJOY,         joytype);
					SAVE(EMUSOM,         soundtype);
//					SAVE(EMUSOMVOL,      VolumeSom);
					SAVE(EMUSERIAL,      serialport);
					SAVE(EMUIMPRESSORA,  ImpressoraPorta);
					SAVE(EMUVEL,         IsDlgButtonChecked(window, rbOutraVel));
					SAVE(EMUVELCPU,      speed);
					SAVE(EMUVIDEO,       videotype);
					SAVE(EMUDISCO,       newdisktype);
					SAVE(EMUDISCORIG,    HDTamanho);
					SAVE(EMUBANCOS,      newnumexpages);
					SAVE(EMUSCANLINES,   ScanLines); // Scan Lines
					SAVE(EMUCOLARRAPIDO, ColarRapido);
#undef SAVE
					EndDialog(window,1);
					if (afterclose)
						PostMessage((HWND)framewindow,WM_USER+afterclose,0,0);
				break;
				}
				case IDCANCEL:
					EndDialog(window,0);
				break;

				case rbVelReal:
					SendDlgItemMessage(window, slVel, TBM_SETPOS, 1, SPEED_NORMAL);
					EnableTrackbar(window,0);
				break;

				case rbOutraVel:
					SetFocus(GetDlgItem(window,slVel));
					EnableTrackbar(window,1);
				break;

				case slVel:
					CheckRadioButton(window,rbVelReal,rbOutraVel,rbOutraVel);
					EnableTrackbar(window,1);
				break;

				case btnBench:
					afterclose = 1;
					PostMessage(window, WM_COMMAND, IDOK, (LPARAM)GetDlgItem(window,IDOK));
				break;
			}
			break;

		case WM_HSCROLL:
			CheckRadioButton(window,rbVelReal, rbOutraVel, rbOutraVel);
		break;

		case WM_INITDIALOG:
			FillComboBox(window, cbVideo,      opsVideo,     videotype);
			FillComboBox(window, cbJoystick,   opsJoystick,  joytype);
			FillComboBox(window, cbSom,        opsSom,       soundtype);
			FillComboBox(window, cbSerial,     opsSerial,    serialport);
			FillComboBox(window, cbImpressora, opsImpressora,ImpressoraPorta);
			FillComboBox(window, cbDisco,      opsDisco,     enhancedisk);
			switch (HDTamanho)
			{
				case  4: novo2discorigido=0;	break;
				case  8: novo2discorigido=1;	break;
				case 16: novo2discorigido=2;	break;
				case 32: novo2discorigido=3;	break;
			}
			FillComboBox(window, cbDiscoRigido,opsDiscoRigido,novo2discorigido);
			switch (numexpages)
			{
				case  16: new2numexpages = 0; break;
				case 128: new2numexpages = 1; break;
			}
			FillComboBox(window, cbBancos, opsBancos, new2numexpages);
			SendDlgItemMessage(window, slVel, TBM_SETRANGE,1,MAKELONG(0,40));
			SendDlgItemMessage(window, slVel, TBM_SETPAGESIZE,0,5);
			SendDlgItemMessage(window, slVel, TBM_SETTICFREQ,10,0);
			SendDlgItemMessage(window, slVel, TBM_SETPOS,1,speed);
			{
				BOOL custom = 1;
				if (speed == 10)
				{
					custom = 0;
					RegLoadValue(CONFIG, EMUVEL, 1,(DWORD *)&custom);
				}
				CheckRadioButton(window, rbVelReal, rbOutraVel, rbVelReal + custom);
				SetFocus(GetDlgItem(window, custom ? slVel : rbVelReal));
				EnableTrackbar(window,custom);
			}
			SendDlgItemMessage(window, slVolume, TBM_SETRANGEMIN, 1, 0);
			SendDlgItemMessage(window, slVolume, TBM_SETRANGEMAX, 1, 100);
			SendDlgItemMessage(window, slVolume, TBM_SETPAGESIZE, 0, 5);
			SendDlgItemMessage(window, slVolume, TBM_SETTICFREQ, 10, 0);
//			SendDlgItemMessage(window, slVolume, TBM_SETPOS, 1, VolumeSom);
			CheckDlgButton(window, cbScanLines, ScanLines ? BST_CHECKED : BST_UNCHECKED); // Scan Lines
			afterclose = 0;
		break;
	}
	return 0;
}

//===========================================================================
void PreencherCombos(HWND window)
{
	FillComboBox(window, cbSLOT1, opsSLOTS,  Slots[1]);
	FillComboBox(window, cbSLOT2, opsSLOTS,  Slots[2]);
//	FillComboBox(window, cbSLOT3, opsSLOTS,  Slots[3]);
	FillComboBox(window, cbSLOT4, opsSLOTS,  Slots[4]);
	FillComboBox(window, cbSLOT5, opsSLOTS,  Slots[5]);
	FillComboBox(window, cbSLOT6, opsSLOTS,  Slots[6]);
	FillComboBox(window, cbSLOT7, opsSLOTS7, Slots[7]);
	SetFocus(window);
}

//===========================================================================
// SLOTS
BOOL CALLBACK ConfigSlotsDlgProc (HWND   window,
                                  UINT   message,
                                  WPARAM wparam,
                                  LPARAM lparam)
{
	switch (message)
	{
		case WM_COMMAND:
			switch (LOWORD(wparam))
			{
				case IDOK:
				{
					int erro;

					NewSlots[1] = (BYTE)SendDlgItemMessage(window, cbSLOT1, CB_GETCURSEL,0,0);
					NewSlots[2] = (BYTE)SendDlgItemMessage(window, cbSLOT2, CB_GETCURSEL,0,0);
//					NewSlots[3] = (BYTE)SendDlgItemMessage(window, cbSLOT3, CB_GETCURSEL,0,0);
					NewSlots[4] = (BYTE)SendDlgItemMessage(window, cbSLOT4, CB_GETCURSEL,0,0);
					NewSlots[5] = (BYTE)SendDlgItemMessage(window, cbSLOT5, CB_GETCURSEL,0,0);
					NewSlots[6] = (BYTE)SendDlgItemMessage(window, cbSLOT6, CB_GETCURSEL,0,0);
					NewSlots[7] = (BYTE)SendDlgItemMessage(window, cbSLOT7, CB_GETCURSEL,0,0);
					erro = MemAtualizaSlots();
					if (erro != ERR_OK)
					{
						PreencherCombos(window);
						return 0;
					}
					SaveSlots();
					FrameRefreshStatus(DRAW_BACKGROUND | DRAW_LEDS);
					EndDialog(window,1);
				}
				break;

				case IDCANCEL:
					EndDialog(window,0);
				break;
			} // switch LOWORD(wparam)
		break; // case WM_COMMAND

		case WM_INITDIALOG:
			PreencherCombos(window);
			SetFocus(GetDlgItem(window, cbSLOT1));
		break;
	} // switch message
  return 0;
}

// SLOTS

//===========================================================================
BOOL CALLBACK ImportarDlgProc(  HWND   window,
								UINT   message,
								WPARAM wparam,
								LPARAM lparam)
{
	switch (message)
	{
		case WM_COMMAND:
			switch (LOWORD(wparam))
			{
				DWORD  EndInicial;
				char EndI[MAX_PATH];

				case IDOK:
					GetDlgItemText(window, edtImpEndInicial, EndI, MAX_PATH);
					if (EndI[0] == '$')
					{
						char temp[MAX_PATH];

						strcpy(temp,"0x");
						strcat(temp,&EndI[1]);
						sscanf(temp,"%x",&EndInicial);
					}
					else if (!strncmp(EndI,"0x",2))
					{
						sscanf(EndI,"%x",&EndInicial);
					}
					else
						EndInicial = atoi(EndI);
					if (EndInicial < 0xBFFF && strlen(EndI))
					{
						if (!MemImportar((WORD)EndInicial))
						{
							RegSaveString(PREFERENCIAS, ULTIMPEND, 1, EndI);
							EndDialog(window,1);
						}
					}
					else
					{
						MessageBox(window,
									"Digite os valores corretos",
									"Erro",
									MB_OK | MB_ICONWARNING);
					}
				break;

				case IDCANCEL:
					EndDialog(window,0);
				break;
			} // switch loword
		break;

		case WM_INITDIALOG:
			{
				char Endereco[MAX_PATH] = "";

				RegLoadString(PREFERENCIAS, ULTIMPEND, 1, Endereco, MAX_PATH);
				SetDlgItemText(window, edtImpEndInicial, Endereco);
				SetFocus(GetDlgItem(window, edtImpEndInicial));
			}
		break;
	} // switch message
	return 0;
}

//===========================================================================
BOOL CALLBACK ExportarDlgProc(  HWND   window,
								UINT   message,
								WPARAM wparam,
								LPARAM lparam)
{
	switch (message)
	{
		case WM_COMMAND:
			switch (LOWORD(wparam))
			{
				WORD  EndInicial;
				WORD  EndFinal;
				char EndI[MAX_PATH];
				char EndF[MAX_PATH];

				case IDOK:
					GetDlgItemText(window, edtExpEndInicial, EndI, MAX_PATH);
					GetDlgItemText(window, edtExpEndFinal,   EndF, MAX_PATH);
					if (EndI[0] == '$')
					{
						char temp[MAX_PATH];
						strcpy(temp,"0x");
						strcat(temp,&EndI[1]);
						sscanf(temp,"%x",&EndInicial);
					}
					else if (!strncmp(EndI,"0x",2))
					{
						sscanf(EndI,"%x",&EndInicial);
					}
					else
						EndInicial = atoi(EndI);
					if (EndF[0] == '$')
					{
						char temp[MAX_PATH];
						strcpy(temp,"0x");
						strcat(temp,&EndF[1]);
						sscanf(temp,"%x",&EndFinal);
					}
					else if (!strncmp(EndF,"0x",2))
					{
						sscanf(EndF,"%x",&EndFinal);
					}
					else
						EndFinal = atoi(EndF);
					if (EndInicial < 0xC000 && EndFinal < 0xC000 &&
						(EndInicial || EndFinal) &&
						(EndInicial < EndFinal))
					{
						if (!MemExportar(EndInicial, EndFinal))
						{
							RegSaveString(PREFERENCIAS, ULTEXPENDI, 1, EndI);
							RegSaveString(PREFERENCIAS, ULTEXPENDF, 1, EndF);
							EndDialog(window,1);
						}
					}
					else
					{
						MessageBox(window,
									"Digite os valores corretos",
									"Erro",
									MB_OK | MB_ICONWARNING);
					}
				break;

				case IDCANCEL:
					EndDialog(window,0);
				break;
			} // switch loword
		break;

		case WM_INITDIALOG:
			{
				char Endereco[MAX_PATH] = "";

				RegLoadString(PREFERENCIAS, ULTEXPENDI, 1, Endereco, MAX_PATH);
				SetDlgItemText(window, edtExpEndInicial, Endereco);
				RegLoadString(PREFERENCIAS, ULTEXPENDF, 1, Endereco, MAX_PATH);
				SetDlgItemText(window, edtExpEndFinal, Endereco);
				SetFocus(GetDlgItem(window, edtExpEndInicial));
			}
		break;
	} // switch message
	return 0;
}

// Funções Internas
//===========================================================================
void ProcessaMenu(int ID, HWND window)
{
//	Som_Mute();
	switch(ID)
	{
	case mnuSair:
		SendMessage(window, WM_CLOSE, 0, 0);
		break;

	case mnuDepurar:
		if (mode == MODE_LOGO)
			ResetMachineState();
		if (mode == MODE_STEPPING)
			DebugProcessChar('\x1B');
		else if (mode == MODE_DEBUG)
			ProcessaMenu(mnuResetar, window);
		else
		{
			DebugBegin();
		}
		DrawStatusArea((HDC)0,DRAW_TITLE);
		return;
		break;

	case mnuConfig:
		DialogBox((HINSTANCE)instance,
			"DIALOGO_CONFIG",
			(HWND)framewindow,
			(DLGPROC)ConfigDlgProc);
		break;

	case mnuConfigSlots:
		DialogBox((HINSTANCE)instance,
			"DIALOGO_SLOTS",
			(HWND)framewindow,
			(DLGPROC)ConfigSlotsDlgProc);
		break;

	case mnuSobre:
		MessageBox(window,
			"Emulador TK3000 //e para windows, baseado "
			"no emulador Applewin por Michael O'Brien.\n\n"
			"Parte do código foi usado do emulador Applewin do Tom Charlesworth.\n\n"
			"Adaptado por Fábio Belavenuto, Copyright 2004.",
			"Sobre o TK3000 //e",
			MB_OK | MB_ICONINFORMATION
			);
		break;

	case mnuResetar:
		if (mode == MODE_LOGO)
			DiskBoot();
		else if (mode == MODE_RUNNING)
			ResetMachineState();
		if ((mode == MODE_DEBUG) || (mode == MODE_STEPPING))
			DebugEnd();
		mode = MODE_RUNNING;
		CPUAtual = CPU65C02;
		DrawStatusArea((HDC)0,DRAW_TITLE);
		VideoRedrawScreen();
		resettiming = 1;
		break;

	case mnuCor:
		VideoChooseColor();
		break;

	case mnuDisco1:
		DiskSelect(0);
		DrawStatusArea(0, DRAW_BACKGROUND | DRAW_LEDS);
		break;

	case mnuRemDisco1:
		DiskRemove(0);
		DrawStatusArea(0, DRAW_BACKGROUND | DRAW_LEDS);
		break;

	case mnuVolDisco1:
		DiskEscolheVolume(0);
		break;

	case mnuDisco2:
		DiskSelect(1);
		DrawStatusArea(0, DRAW_BACKGROUND | DRAW_LEDS);
		break;

	case mnuRemDisco2:
		DiskRemove(1);
		DrawStatusArea(0, DRAW_BACKGROUND | DRAW_LEDS);
		break;

	case mnuVolDisco2:
		DiskEscolheVolume(1);
		break;

	case mnuHD1:
		HDEscolheImagem(0);
		DrawStatusArea(0, DRAW_BACKGROUND | DRAW_LEDS);
		break;

	case mnuRemHD1:
		HDFechaImagem(0);
		DrawStatusArea(0, DRAW_BACKGROUND | DRAW_LEDS);
		break;

	case mnuHD2:
		HDEscolheImagem(1);
		DrawStatusArea(0, DRAW_BACKGROUND | DRAW_LEDS);
		break;

	case mnuRemHD2:
		HDFechaImagem(1);
		DrawStatusArea(0, DRAW_BACKGROUND | DRAW_LEDS);
		break;

	case mnuIDE:
		IDEEscolheImagem();
		DrawStatusArea(0, DRAW_BACKGROUND | DRAW_LEDS);
		break;

	case mnuRemIDE:
		IDEFechaImagem();
		DrawStatusArea(0, DRAW_BACKGROUND | DRAW_LEDS);
		break;

	case mnuImportar:
		if (mode == MODE_RUNNING) {
			DialogBox((HINSTANCE)instance,
				"DIALOGO_IMPORTAR",
				(HWND)framewindow,
				(DLGPROC)ImportarDlgProc);
		}		
		break;

	case mnuExportar:
		if (mode == MODE_RUNNING) {
			DialogBox((HINSTANCE)instance,
				"DIALOGO_EXPORTAR",
				(HWND)framewindow,
				(DLGPROC)ExportarDlgProc);
		}
		break;

	case mnuImportarB:
		if (mode == MODE_RUNNING) {
			MemImportarB();
		}
		break;

	case mnuFullScreen:
		if (!restart && (mode != MODE_LOGO))
		{
			if (fullscreen)
				SetNormalMode();
			else
			{
				SetFullScreenMode();
				VideoRedrawScreen();
			}
		}
		//DrawStatusArea((HDC)0, DRAW_TITLE | DRAW_LEDS);
		break;

	case mnuSalvaEstado:
		//			MemSalvarEstado();
		break;

	case mnuCarregaEstado:
		//			MemCarregarEstado();
		break;

	case mnuExtrairBasic:
		if (mode == MODE_RUNNING) {
			char *applesoft = UtilExtrairApplesoft();
			if (!applesoft) {
				FrameMostraMensagemErro("Erro ao extrair!");
			} else {
				if (!OpenClipboard((HWND)framewindow)) {
					FrameMostraMensagemErro("Erro ao abrir a Área de Transferência");
					free(applesoft);
				} else {
					EmptyClipboard();
					HGLOBAL hText = GlobalAlloc(GMEM_DDESHARE, strlen(applesoft)+1);
					if (hText == NULL) {
						FrameMostraMensagemErro("Erro ao abrir a Área de Transferência");
						CloseClipboard();
						free(applesoft);
					} else {
						char *pText = (char *)GlobalLock(hText);
						strcpy(pText, LPCSTR(applesoft));
						GlobalUnlock(hText); 
						SetClipboardData(CF_TEXT, hText);
						CloseClipboard();
						free(applesoft);
						FrameMostraMensagemAdvertencia("Copiado para área de transferência!");
					}
				}
			}
		}
		break;
	}
//	Som_Demute();
}

//===========================================================================
void HabilitaMenu(UINT ID)
{
	HMENU Menu;
	MENUITEMINFO item;

	Menu = GetMenu((HWND)framewindow);
	Menu = GetSubMenu(Menu, 1);
	ZeroMemory(&item, sizeof(item));
	item.cbSize     = sizeof(item);
	item.fMask      = MIIM_ID | MIIM_STATE;
	item.fState		= MFS_ENABLED;
	item.wID        = ID;
	SetMenuItemInfo(Menu, ID, FALSE, &item);
}

//===========================================================================
void DesabilitaMenu(UINT ID)
{
	HMENU Menu;
	MENUITEMINFO item;

	Menu = GetMenu((HWND)framewindow);
	Menu = GetSubMenu(Menu, 1);
	ZeroMemory(&item, sizeof(item));
	item.cbSize     = sizeof(item);
	item.fMask      = MIIM_ID | MIIM_STATE;
	item.fState		= MFS_GRAYED;
	item.wID        = ID;
	SetMenuItemInfo(Menu, ID, FALSE, &item);
}

//===========================================================================
void CreateGdiObjects(void)
{
#define LOADBUTTONBITMAP(bitmapname)  LoadImage((HINSTANCE)instance,bitmapname,   \
                                                IMAGE_BITMAP,0,0,      \
                                                LR_CREATEDIBSECTION |  \
                                                LR_LOADMAP3DCOLORS |   \
                                                LR_LOADTRANSPARENT);
	capsbitmap[0] = (HBITMAP)LOADBUTTONBITMAP("CAPSOFF_BITMAP");
	capsbitmap[1] = (HBITMAP)LOADBUTTONBITMAP("CAPSON_BITMAP");
	diskbitmap[0] = (HBITMAP)LOADBUTTONBITMAP("DISKOFF_BITMAP");
	diskbitmap[1] = (HBITMAP)LOADBUTTONBITMAP("DISKREAD_BITMAP");
	diskbitmap[2] = (HBITMAP)LOADBUTTONBITMAP("DISKWRITE_BITMAP");
	modebitmap[0] = (HBITMAP)LOADBUTTONBITMAP("MODEOFF_BITMAP");
	modebitmap[1] = (HBITMAP)LOADBUTTONBITMAP("MODEON_BITMAP");
	z80bitmap[0]  = (HBITMAP)LOADBUTTONBITMAP("Z80OFF_BITMAP");
	z80bitmap[1]  = (HBITMAP)LOADBUTTONBITMAP("Z80ON_BITMAP");
	btnfacebrush    = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	btnfacepen      = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNFACE));
	btnhighlightpen = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNHIGHLIGHT));
	btnshadowpen    = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
	smallfont = CreateFont(11,6,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,
							OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY,VARIABLE_PITCH | FF_SWISS,
							"Small Fonts");
}

//===========================================================================
void DeleteGdiObjects()
{
	int loop;
	for (loop = 0; loop < 2; loop++)
		DeleteObject(capsbitmap[loop]);
	for (loop = 0; loop < 3; loop++)
		DeleteObject(diskbitmap[loop]);
	DeleteObject(btnfacebrush);
	DeleteObject(btnfacepen);
	DeleteObject(btnhighlightpen);
	DeleteObject(btnshadowpen);
	DeleteObject(smallfont);
}

//===========================================================================
void SetFullScreenMode ()
{
	viewportx  = FSVIEWPORTX;
	viewporty  = FSVIEWPORTY;
	fullscreen = 1;
	GetWindowRect((HWND)framewindow,&framerect);
	SetWindowLong((HWND)framewindow, GWL_STYLE, WS_POPUP | WS_SYSMENU | WS_VISIBLE);
	DDSURFACEDESC ddsd;
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if (/*DirectDrawCreate(NULL, &directdraw, NULL) != DD_OK ||
			directdraw->SetCooperativeLevel((HWND)framewindow, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) != DD_OK ||
			directdraw->SetDisplayMode(640, 480, 8) != DD_OK ||
			directdraw->CreateSurface(&ddsd,&surface,NULL) != DD_OK*/1)
	{
		SetNormalMode();
		return;
	}
	InvalidateRect((HWND)framewindow, NULL, 1);
}

//===========================================================================
void SetNormalMode ()
{
	viewportx  = 0;
	viewporty  = 0;
	fullscreen = 0;
	if (directdraw) {
		directdraw->RestoreDisplayMode();
		directdraw->SetCooperativeLevel(NULL,DDSCL_NORMAL);
	}
	SetWindowLong((HWND)framewindow,GWL_STYLE,
					WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX |
					WS_VISIBLE);
	SetWindowPos((HWND)framewindow,0,framerect.left,
								framerect.top,
								framerect.right - framerect.left,
								framerect.bottom - framerect.top,
								SWP_NOZORDER | SWP_FRAMECHANGED);
	if (surface)
	{
		surface->Release();
		surface = (LPDIRECTDRAWSURFACE)0;
	}
	if (directdraw) {
		directdraw->Release();
	}
	directdraw = (LPDIRECTDRAW)0;
}

//===========================================================================
void ResetMachineState()
{
	MemReset();
	DiskBoot();
	VideoResetState();
	CommReset();
	JoyReset();
	InitTables();
	CPUAtual = CPU65C02;
}

//===========================================================================
void DrawBitmapRect (HDC dc, int x, int y, LPRECT rect, HBITMAP bitmap)
{
	HDC memdc = CreateCompatibleDC(dc);
	SelectObject(memdc,bitmap);
	BitBlt(dc,x,y,
			rect->right  + 1 - rect->left,
			rect->bottom + 1 - rect->top,
			memdc,
			rect->left,
			rect->top,
			SRCCOPY);
	DeleteDC(memdc);
}

//===========================================================================
void DrawStatusArea(HDC passdc, int drawflags)
{
	HDC  dc;
	int  x;
	int  y;
	int  drive1;
	int  drive2;
	int  HD1;
	int  HD2;
	int  IDELed;
	BOOL caps, modekey;
	BOOL Z80;

	FrameReleaseDC();
	dc     = (passdc ? passdc : GetDC((HWND)framewindow));
	x      = 0;
	y      = VIDEOHEIGHT;
	drive1 = 0;
	drive2 = 0;
	HD1    = 0;
	HD2    = 0;
	IDELed = 0;
	caps   = 0;
	Z80    = 0;
	DiskGetLightStatus(&drive1,&drive2);
	HDStatusLeds(&HD1, &HD2);
	IDEStatusLed(&IDELed);
	KeybGetCapsStatus(&caps);
	KeybGetModeStatus(&modekey);
	CPMZ80StatusLed(&Z80);

	if (drawflags & DRAW_BACKGROUND)
	{
		if (!fullscreen)
		{
			char temp[MAX_PATH];

			SelectObject(dc,GetStockObject(NULL_PEN));
			SelectObject(dc,btnfacebrush);
			Rectangle(dc, x, y, x+VIDEOWIDTH+2, y+ STATUSHEIGHT+2);
			SelectObject(dc,smallfont);
			SetTextAlign(dc, TA_TOP);
			SetTextColor(dc,RGB(0,0,0));
			SetBkMode(dc,TRANSPARENT);
			strcpy(temp, "Drive 1 - ");
			strcat(temp, DiskGetName(0));
			TextOut(dc,
					x + STATUSXPOS1+10,
					y + STATUSYPOS1-1,
					temp,
					strlen(temp));
			strcpy(temp, "Drive 2 - ");
			strcat(temp, DiskGetName(1));
			TextOut(dc,
				    x + STATUSXPOS1+10,
					y + STATUSYPOS2-1,
					temp,
					strlen(temp));

			strcpy(temp, "HD 1 - ");
			strcat(temp, HDNomeImagem(0));
			TextOut(dc,
				    x + STATUSXPOS2+10,
				    y + STATUSYPOS1-1,
					temp,
					strlen(temp));
			strcpy(temp, "HD 2 - ");
			strcat(temp, HDNomeImagem(1));
			TextOut(dc,
				    x + STATUSXPOS2+10,
					y + STATUSYPOS2-1,
					temp,
					strlen(temp));
			strcpy(temp, "IDE - ");
			strcat(temp, IDENomeImagem());
			TextOut(dc,
				    x + STATUSXPOS3+10,
					y + STATUSYPOS1-1,
					temp,
					strlen(temp));
		}
	}
	if (drawflags & DRAW_LEDS)
	{
		if (!fullscreen)
		{
			RECT rect = {0,0,8,8};
			DrawBitmapRect(dc, x + STATUSXPOS1, y + STATUSYPOS1, &rect,diskbitmap[drive1]);
			DrawBitmapRect(dc, x + STATUSXPOS1, y + STATUSYPOS2, &rect,diskbitmap[drive2]);
			DrawBitmapRect(dc, x + STATUSXPOS2, y + STATUSYPOS1, &rect,diskbitmap[HD1]);
			DrawBitmapRect(dc, x + STATUSXPOS2, y + STATUSYPOS2, &rect,diskbitmap[HD2]);
			DrawBitmapRect(dc, x + STATUSXPOS3, y + STATUSYPOS1, &rect,diskbitmap[IDELed]);
			{
				RECT rect = {0,0,30,8};
				DrawBitmapRect(dc,x + STATUSXPOS4, y + STATUSYPOS1, &rect, capsbitmap[caps != 0]);
				DrawBitmapRect(dc,x + STATUSXPOS4, y + STATUSYPOS3, &rect, modebitmap[modekey != 0]);
				DrawBitmapRect(dc,x + STATUSXPOS4, y + STATUSYPOS4, &rect, z80bitmap [Z80 != 0]);
			}
		}
		else
		{
			y = VIDEOHEIGHT + FSVIEWPORTY;
			SelectObject(dc, smallfont);
			SetBkMode(dc, OPAQUE);
			SetBkColor(dc, RGB(0,0,0));
			SetTextAlign(dc, TA_LEFT | TA_TOP);
			
			SetTextColor(dc, RGB((drive1==2 ? 255 : 0), (drive1==1 ? 255 : 0), 0));
			TextOut(dc, x +   3, y + 2, TEXT("DSK1"), 4);
			SetTextColor(dc, RGB((drive2==2 ? 255 : 0), (drive2==1 ? 255 : 0), 0));
			TextOut(dc, x +  33, y + 2, TEXT("DSK2"), 4);

			SetTextColor(dc, RGB((HD1==2 ? 255 : 0), (HD1==1 ? 255 : 0), 0));
			TextOut(dc, x +  63, y + 2, TEXT("HD1"), 3);
			SetTextColor(dc, RGB((HD2==2 ? 255 : 0), (HD2==1 ? 255 : 0), 0));
			TextOut(dc, x +  93, y + 2, TEXT("HD2"), 3);

			SetTextColor(dc, RGB((IDELed==2 ? 255 : 0), (IDELed==1 ? 255 : 0), 0));
			TextOut(dc, x + 123, y + 2, TEXT("IDE"), 3);

			SetTextColor(dc,(caps ? RGB(128,128,128) :
									RGB(  0,  0,  0)));
			TextOut(dc, x +  3, y + 12, TEXT("Caps"), 4);
			
			SetTextColor(dc,(modekey ? RGB(128,128,128) :
									RGB(  0,  0,  0)));
			TextOut(dc, x +  33, y + 12, TEXT("Mode"), 4);

			SetTextColor(dc,(Z80 ? RGB(128,128,128) :
									RGB(  0,  0,  0)));
			TextOut(dc, x +  63, y + 12, TEXT("Z80"), 3);
			drawflags |= DRAW_TITLE;
		}
	}
	if (drawflags & DRAW_TITLE)
	{
		char title[255];
		strcpy(title,TITULO);
		if (fullscreen)
		{
			y = VIDEOHEIGHT + FSVIEWPORTY;
			SelectObject(dc, smallfont);
			SetBkMode(dc, OPAQUE);
			SetBkColor(dc, RGB(0,0,0));
			SetTextAlign(dc, TA_LEFT | TA_TOP);
		}
		switch (mode)
		{
			case MODE_PAUSED:
//				Som_Mute();
				strcat(title," [Pausado]");
				if (fullscreen)
				{
					strcpy(title, "Pausado      ");
				}
			break;

			case MODE_RUNNING:
				strcat(title," [Rodando]");
				if (fullscreen)
				{
					strcpy(title, "Rodando      ");
				}
			break;

			case MODE_DEBUG:
//				Som_Mute();
				strcat(title," [Depurando]");
				if (fullscreen)
				{
					strcpy(title, "Depurando    ");
				}
			break;

			case MODE_STEPPING:
//				Som_Mute();
				strcat(title," [Passo à Passo]");
				if (fullscreen)
				{
					strcpy(title, "Passo à Passo");
				}
			break;
		}
		if (g_bScrollLock_FullSpeed) {
			strcat(title, " (Fullspeed)");
		}
		if (fullscreen)
		{
			SetTextColor(dc,RGB(255,255,255));
			TextOut(dc, x +  93, y + 12, title, 13);
		}
		else
			SendMessage((HWND)framewindow,WM_SETTEXT,0,(LPARAM)title);
	}
	if (!passdc)
		ReleaseDC((HWND)framewindow,dc);
}

//===========================================================================
void Drawframewindow(void)
{
	PAINTSTRUCT ps;
	HDC dc;

	FrameReleaseDC();
	dc = (pintando ? BeginPaint((HWND)framewindow,&ps)
					: GetDC((HWND)framewindow));
	VideoRealizePalette(dc);
	DrawStatusArea(dc, DRAW_BACKGROUND | DRAW_LEDS);

	if (pintando)
		EndPaint((HWND)framewindow, &ps);
	else
		ReleaseDC((HWND)framewindow, dc);

	// DRAW THE CONTENTS OF THE EMULATED SCREEN
	if (mode == MODE_LOGO)
		VideoDisplayLogo();
	else if (mode == MODE_DEBUG)
		DebugDisplay(1);
	else
		VideoRedrawScreen();
}

//===========================================================================
void DrawCrosshairs(int x, int y)
{
/*
#define LINE(x1,y1,x2,y2) MoveToEx(dc,x1,y1,NULL); LineTo(dc,x2,y2);

	static int lastx = 0;
	static int lasty = 0;
	HDC dc;

	FrameReleaseDC();
	dc = GetDC((HWND)framewindow);

	// ERASE THE OLD CROSSHAIRS
	if (lastx && lasty) {
		int loop = 3;
		while (loop--) {
//			switch (loop) {
//				case 0: SelectObject(dc,GetStockObject(BLACK_PEN));  break;
//				case 1: // fall through
//				case 2: SelectObject(dc,btnshadowpen);               break;
//				case 3: // fall through
//				case 4: SelectObject(dc,btnfacepen);                 break;
//			}
			SelectObject(dc,GetStockObject(BLACK_PEN));
			LINE(lastx-1,VIDEOTOP+loop,
					lastx+1,VIDEOTOP+loop);
			LINE(VIDEOLEFT+loop,lasty-1,
					VIDEOLEFT+loop,lasty+1);
//			if ((loop == 1) || (loop == 2))
//				SelectObject(dc,btnhighlightpen);
			LINE(lastx-1,VIDEOTOP+VIDEOHEIGHT+loop-3,
					lastx+1,VIDEOTOP+VIDEOHEIGHT+loop-3);
			LINE(VIDEOLEFT+VIDEOWIDTH+loop-3,lasty-1,
					VIDEOLEFT+VIDEOWIDTH+loop-3,lasty+1);
		}
	}

	// DRAW THE NEW CROSSHAIRS
	if (x && y) {
		int loop = 4;
		while (loop--) {
			if ((loop == 1) || (loop == 2))
				SelectObject(dc,GetStockObject(WHITE_PEN));
			else
				SelectObject(dc,GetStockObject(BLACK_PEN));
			LINE(x+loop-2, VIDEOTOP, x+loop-2, VIDEOTOP+3);
			LINE(x+loop-2,VIDEOTOP+VIDEOHEIGHT-3,
					x+loop-2,VIDEOTOP+VIDEOHEIGHT);
			LINE(VIDEOLEFT, y+loop-2,
					VIDEOLEFT+3, y+loop-2);
			LINE(VIDEOLEFT+VIDEOWIDTH-3,y+loop-2,
					VIDEOLEFT+VIDEOWIDTH,y+loop-2);
		}
	}
#undef LINE
	lastx = x;
	lasty = y;
	ReleaseDC((HWND)framewindow,dc);
*/
}

//===========================================================================
void EnableTrackbar (HWND window, BOOL enable)
{
	int loop = slVel;

	EnableWindow(GetDlgItem(window,slVel), enable);
	while (loop++ < 124)
		EnableWindow(GetDlgItem(window,loop),enable);
}

//===========================================================================
void SetUsingCursor (BOOL newvalue)
{
  static HCURSOR cursorvelho;

  if (newvalue == usingcursor)
    return;
  usingcursor = newvalue;
  if (usingcursor) {
    RECT rect;
    POINT pt;

    SetCapture((HWND)framewindow);
    rect.left   = VIDEOLEFT;
	rect.top    = VIDEOTOP;
    rect.right  = VIDEOLEFT+VIDEOWIDTH;
	rect.bottom = VIDEOTOP+VIDEOHEIGHT;
    ClientToScreen((HWND)framewindow,(LPPOINT)&rect.left);
    ClientToScreen((HWND)framewindow,(LPPOINT)&rect.right);
    ClipCursor(&rect);
//    ShowCursor(0);
	cursorvelho = SetCursor(LoadCursor(0,IDC_CROSS));
    GetCursorPos(&pt);
    ScreenToClient((HWND)framewindow,&pt);
//    DrawCrosshairs(pt.x,pt.y);
  }
  else {
    DrawCrosshairs(0,0);
//    ShowCursor(1);
	SetCursor(cursorvelho);
    ClipCursor(NULL);
    ReleaseCapture();
  }
}

//===========================================================================
void FillComboBox(HWND window, int controlid, LPCTSTR choices, int currentchoice)
{
  HWND combowindow = GetDlgItem(window,controlid);
  SendMessage(combowindow,CB_RESETCONTENT,0,0);
  while (*choices) {
    SendMessage(combowindow,CB_ADDSTRING,0,(LPARAM)(LPCTSTR)choices);
    choices += strlen(choices)+1;
  }
  SendMessage(combowindow,CB_SETCURSEL,currentchoice,0);
}

//===========================================================================
/*
void RelayEvent (UINT message, WPARAM wparam, LPARAM lparam)
{
	MSG msg;

  if (fullscreen)
    return;
  msg.hwnd    = (HWND)framewindow;
  msg.message = message;
  msg.wParam  = wparam;
  msg.lParam  = lparam;
  SendMessage(tooltipwindow,TTM_RELAYEVENT,0,(LPARAM)&msg);
}
*/


//
// ----- ALL GLOBALLY ACCESSIBLE FUNCTIONS ARE BELOW THIS LINE -----
//

//===========================================================================
void FrameCreateWindow()
{
	int xpos, ypos, width, height;

	width  = VIDEOWIDTH  +  (GetSystemMetrics(SM_CXBORDER)<<1) + 4;
	height = VIDEOHEIGHT +  (GetSystemMetrics(SM_CYBORDER)<<1) +
							GetSystemMetrics(SM_CYCAPTION) +
							GetSystemMetrics(SM_CYMENU) + 
							STATUSHEIGHT + 4;

	if (!RegLoadValue(PREFERENCIAS,POSX,1,(DWORD *)&xpos))
		xpos = (GetSystemMetrics(SM_CXSCREEN)-width) >> 1;
	if (!RegLoadValue(PREFERENCIAS,POSY,1,(DWORD *)&ypos))
		ypos = (GetSystemMetrics(SM_CYSCREEN)-height) >> 1;

	framewindow = (int)CreateWindow("TK3000eFRAME",
								TITULO,
								WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
								WS_MINIMIZEBOX | WS_VISIBLE,
								xpos,
								ypos,
								width,
								height,
								HWND_DESKTOP,
								(HMENU)LoadMenu((HINSTANCE)instance, "MENU"),
								(HINSTANCE)instance,
								NULL);
}

//================================================================================
int FrameGetDC()
{
  if (!framedc)
  {
    framedc = GetDC((HWND)framewindow);
    SetViewportOrgEx(framedc, viewportx, viewporty, NULL);
  }
  return (int)framedc;
}

//================================================================================
int FrameGetVideoDC(unsigned char* *addr, LONG *pitch)
{
//	return FrameGetDC();
	if (fullscreen /*&& ativo && !pintando*/)
	{
		RECT rect =
		{
					FSVIEWPORTX,
					FSVIEWPORTY+GetSystemMetrics(SM_CYMENU),
					FSVIEWPORTX+VIEWPORTCX,
					FSVIEWPORTY+VIEWPORTCY+GetSystemMetrics(SM_CYMENU)
		};
		DDSURFACEDESC surfacedesc;
		surfacedesc.dwSize = sizeof(surfacedesc);
		if (surface->Lock(&rect,&surfacedesc,0,NULL) == DDERR_SURFACELOST)
		{
			surface->Restore();
			surface->Lock(&rect,&surfacedesc,0,NULL);
		}
		*addr  = (LPBYTE)surfacedesc.lpSurface+(VIEWPORTCY-1)*surfacedesc.lPitch;
		*pitch = -surfacedesc.lPitch;
		return 0;
	}
	else
		return FrameGetDC();
}

//================================================================================
void FrameRefreshStatus(int drawflags)
{
	DrawStatusArea((HDC)0, drawflags);
}

//================================================================================
void FrameRegisterClass()
{
  WNDCLASSEX wndclass;

  ZeroMemory(&wndclass,sizeof(WNDCLASSEX));
  wndclass.cbSize        = sizeof(WNDCLASSEX);
  wndclass.style         = CS_OWNDC | CS_BYTEALIGNCLIENT;
  wndclass.lpfnWndProc   = FrameWndProc;
  wndclass.hInstance     = (HINSTANCE)instance;
  wndclass.hIcon         = LoadIcon((HINSTANCE)instance,"ICONE_TK3000e");
  wndclass.hIconSm       = (HICON)LoadImage((HINSTANCE)instance,"ICONE_TK3000e",
                                            IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
  wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wndclass.lpszClassName = "TK3000eFRAME";
  wndclass.lpszMenuName  = "MENU";
  RegisterClassEx(&wndclass);
}

//================================================================================
void FrameReleaseDC()
{
  if (framedc)
  {
    SetViewportOrgEx(framedc, 0, 0, NULL);
    ReleaseDC((HWND)framewindow,framedc);
    framedc = (HDC)0;
  }
}

//================================================================================
void FrameReleaseVideoDC()
{
  if (fullscreen /*&& ativo && !pintando*/)
  {
    // THIS IS CORRECT ACCORDING TO THE DIRECTDRAW DOCS
    RECT rect = {FSVIEWPORTX,
                 FSVIEWPORTY,
                 FSVIEWPORTX+VIEWPORTCX,
                 FSVIEWPORTY+VIEWPORTCY};
    surface->Unlock(&rect);

    // BUT THIS SEEMS TO BE WORKING
    surface->Unlock(NULL);
  }
}

//================================================================================
void FrameMenuDiskete(int Habilitar)
{
	if (Habilitar)
	{
		HabilitaMenu(mnuDisco1);
		HabilitaMenu(mnuDisco2);
		HabilitaMenu(mnuRemDisco1);
		HabilitaMenu(mnuRemDisco2);
		HabilitaMenu(mnuVolDisco1);
		HabilitaMenu(mnuVolDisco2);
	}
	else
	{
		DesabilitaMenu(mnuDisco1);
		DesabilitaMenu(mnuDisco2);
		DesabilitaMenu(mnuRemDisco1);
		DesabilitaMenu(mnuRemDisco2);
		DesabilitaMenu(mnuVolDisco1);
		DesabilitaMenu(mnuVolDisco2);
	}
}

//================================================================================
void FrameMenuHD(int Habilitar)
{
	if (Habilitar)
	{
		HabilitaMenu(mnuHD1);
		HabilitaMenu(mnuHD2);
		HabilitaMenu(mnuRemHD1);
		HabilitaMenu(mnuRemHD2);
	}
	else
	{
		DesabilitaMenu(mnuHD1);
		DesabilitaMenu(mnuHD2);
		DesabilitaMenu(mnuRemHD1);
		DesabilitaMenu(mnuRemHD2);
	}
}

//================================================================================
void FrameMenuIDE(int Habilitar)
{
	if (Habilitar)
	{
		HabilitaMenu(mnuIDE);
		HabilitaMenu(mnuRemIDE);
	}
	else
	{
		DesabilitaMenu(mnuIDE);
		DesabilitaMenu(mnuRemIDE);
	}
}

//================================================================================
void FrameMostraMensagemAdvertencia(char *Mensagem)
{
//	Som_Mute();
	MessageBox((HWND)framewindow,
				Mensagem,
				TITULO,
				MB_ICONEXCLAMATION | MB_SETFOREGROUND);
//	Som_Demute();
}

//================================================================================
void FrameMostraMensagemErro(char *Mensagem)
{
//	Som_Mute();
	MessageBox((HWND)framewindow,
				Mensagem,
				TITULO,
				MB_ICONSTOP | MB_SETFOREGROUND);
//	Som_Demute();
}

/*
 *  Funções Auxiliares
 *  Versão 0.1
 *  by Fábio Belavenuto
 */

//================================================================================
int FramePerguntaInteiro(char *Titulo, int Default)
{
	int			X, Y, result;
	WNDCLASSEX	wndclass;
	MSG			Mensagem;
	char		temp[MAX_PATH];

	X = (GetSystemMetrics(SM_CXSCREEN) >> 1) - (WIDTH_DIALOGO  >> 1);
	Y = (GetSystemMetrics(SM_CYSCREEN) >> 1) - (HEIGHT_DIALOGO >> 1);

	ZeroMemory(&wndclass,sizeof(WNDCLASSEX));
	wndclass.cbSize        = sizeof(WNDCLASSEX);
	wndclass.style         = CS_OWNDC | CS_BYTEALIGNCLIENT;
	wndclass.lpfnWndProc   = WndProcInteiro;
	wndclass.hInstance     = (HINSTANCE)instance;
	wndclass.hIcon         = LoadIcon  (0,IDI_QUESTION);
	wndclass.hIconSm       = NULL; //LoadIcon  (0,IDI_QUESTION);
	wndclass.hCursor       = LoadCursor(0,IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wndclass.lpszClassName = "DialogoInteiro";
	wndclass.lpszMenuName  = NULL;
	RegisterClassEx(&wndclass);

	Janela = CreateWindow(  "DialogoInteiro",
							Titulo,
							WS_DLGFRAME	| WS_VISIBLE | WS_SYSMENU,
							X,
							Y,
							WIDTH_DIALOGO,
							HEIGHT_DIALOGO,
							(HWND)framewindow,
							NULL,
							(HINSTANCE)instance,
							NULL);

	Rotulo = CreateWindow(  "STATIC",
							"Digite um valor inteiro:",
							WS_CHILD | WS_VISIBLE,
							POSX_ROTULO,
							POSY_ROTULO,
							200,
							16,
							Janela,
							NULL,
							(HINSTANCE)instance,
							NULL);

	edtValor = CreateWindowEx( WS_EX_CLIENTEDGE,
							"EDIT",
							"",
							WS_CHILD | WS_VISIBLE | WS_BORDER |
							WS_TABSTOP | ES_AUTOHSCROLL | ES_NUMBER,
							POSX_CEDIT,
							POSY_CEDIT,
							55,
							24,
							Janela,
							NULL,
							(HINSTANCE)instance,
							NULL);

	btnOK = CreateWindowEx( 0,
							"BUTTON",
							"OK",
							WS_CHILD | WS_VISIBLE |
							WS_TABSTOP | BS_DEFPUSHBUTTON,
							POSX_BTNOK,
							POSY_BTNOK,
							80,
							25,
							Janela,
							NULL,
							(HINSTANCE)instance,
							NULL);

	btnCancelar = CreateWindowEx( 0,
							"BUTTON",
							"CANCELAR",
							WS_CHILD | WS_VISIBLE |
							WS_TABSTOP,
							POSX_BTNCANC,
							POSY_BTNCANC,
							80,
							25,
							Janela,
							NULL,
							(HINSTANCE)instance,
							NULL);

	sprintf(temp,"%d",Default);
	SetWindowText(edtValor,temp);
	while (1) 
	{
		if (BotaoApertado == 2)
		{
			DestroyWindow(Janela);
			UnregisterClass("DialogoInteiro", (HINSTANCE)instance);
			return Default;
		}
		if (BotaoApertado == 1)
		{
			GetWindowText(edtValor, temp, MAX_PATH);
			result = atoi(temp);
			DestroyWindow(Janela);
			UnregisterClass("DialogoInteiro", (HINSTANCE)instance);
			return result;
		}
		if (PeekMessage(&Mensagem,0,0,0,PM_REMOVE))
		{
			if (Mensagem.message == WM_QUIT)
			{
				UnregisterClass("DialogoInteiro", (HINSTANCE)instance);
				return Default;
			}
			TranslateMessage(&Mensagem);
			DispatchMessage(&Mensagem);
		}
	}
	UnregisterClass("DialogoInteiro", (HINSTANCE)instance);
	return Default;
}

// EOF