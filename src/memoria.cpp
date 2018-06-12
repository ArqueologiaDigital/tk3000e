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

// Gerencia a memória

//#define MEMDEBUG

#include "stdhdr.h"
#include "tk3000e.h"
#include "memoria.h"
#include "disco.h"
#include "cpu.h"
#include "disco_rigido.h"
#include "joystick.h"
#include "video.h"
//#include "som.h"
#include "Speaker.h"
#include "serial.h"
#include "impressora.h"
#include "teclado.h"
#include "debug.h"
#include "registro.h"
#include "tape.h"
#include "tkclock.h"
#include "impressora.h"
#include "mouse.h"
#include "janela.h"
#include "relogio.h"
#include "z80emu.h"
#include "ide.h"
#include "Mockingboard.h"

#define  MF_80STORE    0x00000001
#define  MF_ALTZP      0x00000002
#define  MF_AUXREAD    0x00000004
#define  MF_AUXWRITE   0x00000008
#define  MF_BANK2      0x00000010
#define  MF_HIGHRAM    0x00000020
#define  MF_HIRES      0x00000040
#define  MF_PAGE2      0x00000080
#define  MF_SLOTC3ROM  0x00000100
#define  MF_SLOTCXROM  0x00000200
#define  MF_WRITERAM   0x00000400
#define  MF_IMAGEMASK  0x000003F7

#define  SW_80STORE    (memmode & MF_80STORE)
#define  SW_ALTZP      (memmode & MF_ALTZP)
#define  SW_AUXREAD    (memmode & MF_AUXREAD)
#define  SW_AUXWRITE   (memmode & MF_AUXWRITE)
#define  SW_BANK2      (memmode & MF_BANK2)
#define  SW_HIGHRAM    (memmode & MF_HIGHRAM)
#define  SW_HIRES      (memmode & MF_HIRES)
#define  SW_PAGE2      (memmode & MF_PAGE2)
#define  SW_SLOTC3ROM  (memmode & MF_SLOTC3ROM)
#define  SW_SLOTCXROM  (memmode & MF_SLOTCXROM)
#define  SW_WRITERAM   (memmode & MF_WRITERAM)
#define  SW_80COL      (vidmode & 0x00000001)

iofunction ioread[0x100]  = {KeybReadData,       // $C000
                             KeybReadData,       // $C001
                             KeybReadData,       // $C002
                             KeybReadData,       // $C003
                             KeybReadData,       // $C004
                             KeybReadData,       // $C005
                             KeybReadData,       // $C006
                             KeybReadData,       // $C007
                             KeybReadData,       // $C008
                             KeybReadData,       // $C009
                             KeybReadData,       // $C00A
                             KeybReadData,       // $C00B
                             KeybReadData,       // $C00C
                             KeybReadData,       // $C00D
                             KeybReadData,       // $C00E
                             KeybReadData,       // $C00F
                             KeybReadFlag,       // $C010
                             MemCheckPaging,     // $C011
                             MemCheckPaging,     // $C012
                             MemCheckPaging,     // $C013
                             MemCheckPaging,     // $C014
                             MemCheckPaging,     // $C015
                             MemCheckPaging,     // $C016
                             MemCheckPaging,     // $C017
                             MemCheckPaging,     // $C018
                             VideoCheckVbl,      // $C019
                             VideoCheckMode,     // $C01A
                             VideoCheckMode,     // $C01B
                             MemCheckPaging,     // $C01C
                             MemCheckPaging,     // $C01D
                             VideoCheckMode,     // $C01E
                             VideoCheckMode,     // $C01F
                             TapeEscrita,        // $C020
                             NullIo,             // $C021
                             NullIo,             // $C022
                             NullIo,             // $C023
                             NullIo,             // $C024
                             NullIo,             // $C025
                             NullIo,             // $C026
                             NullIo,             // $C027
                             NullIo,             // $C028
                             NullIo,             // $C029
                             NullIo,             // $C02A
                             NullIo,             // $C02B
                             NullIo,             // $C02C
                             NullIo,             // $C02D
                             NullIo,             // $C02E
                             NullIo,             // $C02F
                             SpkrToggle,         // $C030
                             SpkrToggle,         // $C031
                             SpkrToggle,         // $C032
                             SpkrToggle,         // $C033
                             SpkrToggle,         // $C034
                             SpkrToggle,         // $C035
                             SpkrToggle,         // $C036
                             SpkrToggle,         // $C037
                             SpkrToggle,         // $C038
                             SpkrToggle,         // $C039
                             SpkrToggle,         // $C03A
                             SpkrToggle,         // $C03B
                             SpkrToggle,         // $C03C
                             SpkrToggle,         // $C03D
                             SpkrToggle,         // $C03E
                             SpkrToggle,         // $C03F
                             NullIo,             // $C040
                             NullIo,             // $C041
                             NullIo,             // $C042
                             NullIo,             // $C043
                             NullIo,             // $C044
                             NullIo,             // $C045
                             NullIo,             // $C046
                             NullIo,             // $C047
                             NullIo,             // $C048
                             NullIo,             // $C049
                             NullIo,             // $C04A
                             NullIo,             // $C04B
                             NullIo,             // $C04C
                             NullIo,             // $C04D
                             NullIo,             // $C04E
                             NullIo,             // $C04F
                             VideoSetMode,       // $C050
                             VideoSetMode,       // $C051
                             VideoSetMode,       // $C052
                             VideoSetMode,       // $C053
                             MemSetPaging,       // $C054
                             MemSetPaging,       // $C055
                             MemSetPaging,       // $C056
                             MemSetPaging,       // $C057
                             NullIo,             // $C058
                             NullIo,             // $C059
                             NullIo,             // $C05A
                             NullIo,             // $C05B
                             NullIo,             // $C05C
                             NullIo,             // $C05D
                             VideoSetMode,       // $C05E
                             VideoSetMode,       // $C05F
                             TapeLeitura,        // $C060
                             JoyReadButton,      // $C061
                             JoyReadButton,      // $C062
                             JoyReadButton,      // $C063
                             JoyReadPosition,    // $C064
                             JoyReadPosition,    // $C065
                             NullIo,             // $C066
                             NullIo,             // $C067
                             NullIo,             // $C068
                             NullIo,             // $C069
                             NullIo,             // $C06A
                             NullIo,             // $C06B
                             NullIo,             // $C06C
                             NullIo,             // $C06D
                             NullIo,             // $C06E
                             NullIo,             // $C06F
                             JoyResetPosition,   // $C070
                             NullIo,             // $C071
                             NullIo,             // $C072
                             NullIo,             // $C073
                             NullIo,             // $C074
                             NullIo,             // $C075
                             NullIo,             // $C076
                             NullIo,             // $C077
                             NullIo,             // $C078
                             NullIo,             // $C079
                             NullIo,             // $C07A
                             NullIo,             // $C07B
                             NullIo,             // $C07C
                             NullIo,             // $C07D
                             NullIo,             // $C07E
                             VideoCheckMode,     // $C07F
                             MemSetPaging,       // $C080
                             MemSetPaging,       // $C081
                             MemSetPaging,       // $C082
                             MemSetPaging,       // $C083
                             MemSetPaging,       // $C084
                             MemSetPaging,       // $C085
                             MemSetPaging,       // $C086
                             MemSetPaging,       // $C087
                             MemSetPaging,       // $C088
                             MemSetPaging,       // $C089
                             MemSetPaging,       // $C08A
                             MemSetPaging,       // $C08B
                             MemSetPaging,       // $C08C
                             MemSetPaging,       // $C08D
                             MemSetPaging,       // $C08E
                             MemSetPaging,       // $C08F
};

iofunction iowrite[0x100] = {MemSetPaging,       // $C000
                             MemSetPaging,       // $C001
                             MemSetPaging,       // $C002
                             MemSetPaging,       // $C003
                             MemSetPaging,       // $C004
                             MemSetPaging,       // $C005
                             MemSetPaging,       // $C006
                             MemSetPaging,       // $C007
                             MemSetPaging,       // $C008
                             MemSetPaging,       // $C009
                             MemSetPaging,       // $C00A
                             MemSetPaging,       // $C00B
                             VideoSetMode,       // $C00C
                             VideoSetMode,       // $C00D
                             VideoSetMode,       // $C00E
                             VideoSetMode,       // $C00F
                             KeybReadFlag,       // $C010
                             KeybReadFlag,       // $C011
                             KeybReadFlag,       // $C012
                             KeybReadFlag,       // $C013
                             KeybReadFlag,       // $C014
                             KeybReadFlag,       // $C015
                             KeybReadFlag,       // $C016
                             KeybReadFlag,       // $C017
                             KeybReadFlag,       // $C018
                             KeybReadFlag,       // $C019
                             KeybReadFlag,       // $C01A
                             KeybReadFlag,       // $C01B
                             KeybReadFlag,       // $C01C
                             KeybReadFlag,       // $C01D
                             KeybReadFlag,       // $C01E
                             KeybReadFlag,       // $C01F
                             TapeEscrita,        // $C020
                             NullIo,             // $C021
                             NullIo,             // $C022
                             NullIo,             // $C023
                             NullIo,             // $C024
                             NullIo,             // $C025
                             NullIo,             // $C026
                             NullIo,             // $C027
                             NullIo,             // $C028
                             NullIo,             // $C029
                             NullIo,             // $C02A
                             NullIo,             // $C02B
                             NullIo,             // $C02C
                             NullIo,             // $C02D
                             NullIo,             // $C02E
                             NullIo,             // $C02F
                             SpkrToggle,         // $C030
                             SpkrToggle,         // $C031
                             SpkrToggle,         // $C032
                             SpkrToggle,         // $C033
                             SpkrToggle,         // $C034
                             SpkrToggle,         // $C035
                             SpkrToggle,         // $C036
                             SpkrToggle,         // $C037
                             SpkrToggle,         // $C038
                             SpkrToggle,         // $C039
                             SpkrToggle,         // $C03A
                             SpkrToggle,         // $C03B
                             SpkrToggle,         // $C03C
                             SpkrToggle,         // $C03D
                             SpkrToggle,         // $C03E
                             SpkrToggle,         // $C03F
                             NullIo,             // $C040
                             NullIo,             // $C041
                             NullIo,             // $C042
                             NullIo,             // $C043
                             NullIo,             // $C044
                             NullIo,             // $C045
                             NullIo,             // $C046
                             NullIo,             // $C047
                             NullIo,             // $C048
                             NullIo,             // $C049
                             NullIo,             // $C04A
                             NullIo,             // $C04B
                             NullIo,             // $C04C
                             NullIo,             // $C04D
                             NullIo,             // $C04E
                             NullIo,             // $C04F
                             VideoSetMode,       // $C050
                             VideoSetMode,       // $C051
                             VideoSetMode,       // $C052
                             VideoSetMode,       // $C053
                             MemSetPaging,       // $C054
                             MemSetPaging,       // $C055
                             MemSetPaging,       // $C056
                             MemSetPaging,       // $C057
                             NullIo,             // $C058
                             NullIo,             // $C059
                             NullIo,             // $C05A
                             NullIo,             // $C05B
                             NullIo,             // $C05C
                             NullIo,             // $C05D
                             VideoSetMode,       // $C05E
                             VideoSetMode,       // $C05F
                             TapeLeitura,        // $C060
                             NullIo,             // $C061
                             NullIo,             // $C062
                             NullIo,             // $C063
                             NullIo,             // $C064
                             NullIo,             // $C065
                             NullIo,             // $C066
                             NullIo,             // $C067
                             NullIo,             // $C068
                             NullIo,             // $C069
                             NullIo,             // $C06A
                             NullIo,             // $C06B
                             NullIo,             // $C06C
                             NullIo,             // $C06D
                             NullIo,             // $C06E
                             NullIo,             // $C06F
                             JoyResetPosition,   // $C070
                             MemSetPaging,       // $C071 // memoria auxiliar TK WORKS
                             NullIo,             // $C072
                             MemSetPaging,       // $C073 // memoria auxiliar TK WORKS
                             NullIo,             // $C074
                             NullIo,             // $C075
                             NullIo,             // $C076
                             NullIo,             // $C077
                             NullIo,             // $C078
                             NullIo,             // $C079
                             NullIo,             // $C07A
                             NullIo,             // $C07B
                             NullIo,             // $C07C
                             NullIo,             // $C07D
                             NullIo,             // $C07E
                             NullIo,             // $C07F
                             MemSetPaging,       // $C080
                             MemSetPaging,       // $C081
                             MemSetPaging,       // $C082
                             MemSetPaging,       // $C083
                             MemSetPaging,       // $C084
                             MemSetPaging,       // $C085
                             MemSetPaging,       // $C086
                             MemSetPaging,       // $C087
                             MemSetPaging,       // $C088
                             MemSetPaging,       // $C089
                             MemSetPaging,       // $C08A
                             MemSetPaging,       // $C08B
                             MemSetPaging,       // $C08C
                             MemSetPaging,       // $C08D
                             MemSetPaging,       // $C08E
                             MemSetPaging,       // $C08F
};

/*
DWORD   imagemode[MAXIMAGES];
BYTE*   memshadow[MAXIMAGES][0x100];
BYTE*   memwrite[MAXIMAGES][0x100];
*/
BYTE*   memread[0x100];
BYTE*   memwrite[0x100];

BYTE*   RWpages[MAXEXPAGES];	// pointers to RW memory banks
regsrec regs;
BOOL    fastpaging   = 0;
DWORD   image        = 0;
DWORD   lastimage    = 0;
BOOL    lastwriteram = 0;
//BYTE*   mem          = NULL;
//BYTE*   memimage     = NULL;
int     RWatual      = 0;
BYTE*   memaux       = NULL;
BYTE*   memdirty     = NULL;
BYTE*   memmain      = NULL;
DWORD   memmode      = MF_BANK2 | MF_SLOTCXROM | MF_WRITERAM;
BYTE*   memrom       = NULL;
BYTE*   memslotext   = NULL;
BOOL    modechanging = 0;
DWORD   pages        = 0;
BYTE	slotext      = 0;
BYTE	Slots[7];
BYTE	NewSlots[7];

#ifdef  MEMDEBUG
FILE    *debugfile = NULL;
int     debugreadon = 0;
#endif

void	UpdatePaging (BOOL initialize, BOOL updatewriteonly);


#ifdef MEMDEBUG
//===========================================================================
void printdbg(const char *format, ...)
{
	char buf[4096];
	va_list args;

	va_start (args, format);
	vsprintf(buf, format, args);
	va_end(args);

	fwrite(buf, 1, strlen(buf), debugfile);
	fwrite("\r\n", 1, 2, debugfile);
	fflush(debugfile);
}
#endif

//===========================================================================
void BackMainImage()
{
//	FrameMostraMensagemAdvertencia("BackMainImage()");
/*
	int loop = 0;
	for (loop = 0; loop < 256; loop++)
	{
		if (memshadow[0][loop] &&
				((*(memdirty+loop) & 1) || (loop <= 1)))
			memcpy(memshadow[0][loop], memimage+(loop << 8),256);
		*(memdirty+loop) &= ~1;
	}
*/
}

//===========================================================================
BYTE __stdcall NullIo (WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if ((address & 0xF0) == 0xA0)
	{
		static const BYTE retval[16] = {0x58,0xFC,0x5B,0xFF,0x58,0xFC,0x5B,0xFF,
										0x0B,0x10,0x00,0x00,0xFF,0xFF,0xFF,0xFF};
		return retval[address & 15];
	}
	else if ((address >= 0xB0) && (address <= 0xCF))
	{
		BYTE r = (BYTE)(rand() & 0xFF);
		if (r >= 0x10)
			return 0xA0;
		else if (r >= 8)
			return (r > 0xC) ? 0xFF : 0x00;
		else
			return (address & 0xF7);
	}
	else if ((address & 0xF0) == 0xD0)
	{
		BYTE r = (BYTE)(rand() & 0xFF);
		if (r >= 0xC0)
			return 0xC0;
		else if (r >= 0x80)
			return 0x80;
		else if ((address == 0xD0) || (address == 0xDF))
			return 0;
		else if (r >= 0x40)
			return 0x40;
		else if (r >= 0x30)
			return 0x90;
		else
			return 0;
	}
	else
		return MemReturnRandomData(1);
}

//===========================================================================
void ResetPaging (BOOL initialize)
{
	if (!initialize)
		MemSetFastPaging(0);
	lastwriteram = 0;
	memmode      = MF_BANK2 | MF_SLOTCXROM | MF_WRITERAM;
	slotext      = 0;
	UpdatePaging(initialize, 0);
}


//===========================================================================
void UpdateFastPaging()
{
	FrameMostraMensagemAdvertencia("UpdateFastPaging()");
/*
	BOOL  found    = 0;
	DWORD imagenum = 0;
	do
		if ((imagemode[imagenum] == memmode) ||
				((lastimage >= 3) &&
				((imagemode[imagenum] & MF_IMAGEMASK) == (memmode & MF_IMAGEMASK))))
			found = 1;
		else
			++imagenum;
	while ((imagenum <= lastimage) && !found);
	if (found)
	{
		image = imagenum;
		mem   = memimage+(image << 16);
		if (imagemode[image] != memmode)
		{
			imagemode[image] = memmode;
			UpdatePaging(0,1);
		}
	}
	else
	{
		if (lastimage < MAXIMAGES-1)
		{
			imagenum = ++lastimage;
			//if (lastimage >= 3)
				//(memimage+lastimage*0x10000) = malloc(0x10000);
		}
		else
		{
			static DWORD nextimage = 0;
			if (nextimage > lastimage)
				nextimage = 0;
			imagenum = nextimage++;
		}
		imagemode[image = imagenum] = memmode;
		mem = memimage+(image << 16);
		UpdatePaging(1,0);
	}
	CpuReinitialize();
*/
}

//===========================================================================
void UpdatePaging (BOOL initialize, BOOL updatewriteonly)
{
	int loop;

	// UPDATE THE PAGING TABLES BASED ON THE NEW PAGING SWITCH VALUES
	if (initialize)
	{
		for (loop = 0x00; loop <= 0xBF; loop++)
			memwrite[loop] = memmain+(loop << 8);
		for (loop = 0xC0; loop <= 0xCF; loop++)
			memwrite[loop] = NULL;
	}
	if (!updatewriteonly)
		for (loop = 0x00; loop <= 0x01; loop++)
			memread[loop] = SW_ALTZP ? RWatual >= 0 ? RWpages[RWatual]+(loop << 8) 
												: NULL
									: memmain+(loop << 8);

	for (loop = 0x00; loop <= 0x01; loop++)
		memwrite[loop] = SW_ALTZP ? RWatual >= 0 ? RWpages[RWatual]+(loop << 8)
											: NULL
								: memmain+(loop << 8);
	for (loop = 0x02; loop <= 0xBF; loop++)
	{
		memread[loop]  = SW_AUXREAD ? RWatual >= 0 ? RWpages[RWatual]+(loop << 8)
											: NULL
									: memmain+(loop << 8);
		memwrite[loop] = SW_AUXWRITE ? RWatual >= 0 ? RWpages[RWatual]+(loop << 8)
											: NULL
									: memmain+(loop << 8);
	}
	if (!updatewriteonly)
	{
		for (loop = 0xC1; loop <= 0xC7; loop++)
			if (loop == 0xC3)
				memread[loop] = (SW_SLOTC3ROM && SW_SLOTCXROM) ? memrom+0x0300
																: memrom+0x1300;
			else
				memread[loop] = SW_SLOTCXROM ? Slots[loop-0xC0] == 0 ? NULL
																	: memrom+(loop << 8)-0xC000
											: memrom+(loop << 8)-0xB000;
		for (loop = 0xC8; loop <= 0xCF; loop++)
		{
			if (SW_SLOTCXROM && (slotext != 3))
			{
				memread[loop] = slotext ? memslotext+(loop << 8)-0xC800+((slotext-1) << 11)
										: NULL;
			} else
				memread[loop] = memrom+(loop << 8)-0xB000;
		}
	}
	for (loop = 0xD0; loop <= 0xDF; loop++)
	{
		int bankoffset = (SW_BANK2 ? 0 : 0x1000);
		memread[loop]  = SW_HIGHRAM ? SW_ALTZP ? RWatual >= 0 ? RWpages[RWatual]+(loop << 8)-bankoffset
														: NULL
											: memmain+(loop << 8)-bankoffset
									: memrom+(loop << 8)-0xB000;
		memwrite[loop] = SW_WRITERAM ? SW_ALTZP ? RWatual >= 0 ? RWpages[RWatual]+(loop << 8)-bankoffset
														: NULL
												: memmain+(loop << 8)-bankoffset
									: NULL;
	}
	for (loop = 0xE0; loop <= 0xFF; loop++)
	{
		memread[loop]  = SW_HIGHRAM ? SW_ALTZP ? RWatual >= 0 ? RWpages[RWatual]+(loop << 8)
														: NULL
												: memmain+(loop << 8)
									: memrom+(loop << 8)-0xB000;
		memwrite[loop] = SW_WRITERAM ? SW_ALTZP ? RWatual >= 0 ? RWpages[RWatual]+(loop << 8)
														: NULL
												: memmain+(loop << 8)
									: NULL;
	}
	if (SW_80STORE)
	{
		for (loop = 0x04; loop <= 0x07; loop++)
		{
			memread[loop] = SW_PAGE2 ? RWpages[0]+(loop << 8)
									: memmain+(loop << 8);
			memwrite[loop] = SW_PAGE2 ? RWpages[0]+(loop << 8)
									: memmain+(loop << 8);
		}
		if (SW_HIRES)
			for (loop = 0x20; loop <= 0x3F; loop++)
		{
			memread[loop] = SW_PAGE2 ? RWpages[0]+(loop << 8)
									: memmain+(loop << 8);
			memwrite[loop]  = SW_PAGE2 ? RWpages[0]+(loop << 8)
									: memmain+(loop << 8);
		}
	}
}

//
// ----- ALL GLOBALLY ACCESSIBLE FUNCTIONS ARE BELOW THIS LINE -----
//

//===========================================================================
int MemAtualizaSlots()
{
	int i, erro;

	for (i=1; i<8; i++)
	{
		// Se houve mudança em algum slot
		if (Slots[i] != NewSlots[i])
		{
			// Retira placa do slot
			switch (Slots[i])
			{
				case 0: // Vazio
					//
				break;

				case 1: // Impressora
					ImpressoraRetiraSlot();
				break;

				case 2: // Serial
					CommRetiraSlot();
				break;

				case 3: // AppleClock
					RelogioRetiraSlot();
				break;

				case 4: // TKCLOCK
					TKClockRetiraSlot();
				break;

				case 5: // Mouse
					MouseRetiraSlot();
				break;

				case 6: // CP/M Z80
					CPMZ80RetiraSlot();
				break;

				case 7: // DiskII
					DiscoRetiraSlot();
				break;

				case 8: // MassStorage
					HDRetiraSlot();
				break;

				case 9: // IDE Richard
					if (i == 7)
						IDERetiraSlot();
					else
						MBRetiraSlot();
				break;
			} // switch
		} // if (Slots[i] != NewSlots[i])
	} // for i 

	for (i=1; i<8; i++)
	{
		if (Slots[i] != NewSlots[i])
		{
			// Iguala slots
			Slots[i] = NewSlots[i];

			// Configura Slot
			switch (Slots[i])
			{
				case 0: // Vazio
					erro = MemRetiraSlot(i);
				break;

				case 1: // Impressora
					erro = ImpressoraConfiguraSlot(i);
				break;

				case 2: // Serial
					erro = CommConfiguraSlot(i);
				break;

				case 3: // AppleClock
					erro = RelogioConfiguraSlot(i);
				break;

				case 4: // TKCLOCK
					erro = TKClockConfiguraSlot(i);
				break;

				case 5: // Mouse
					erro = MouseConfiguraSlot(i);
				break;

				case 6: // CP/M Z80
					erro = CPMZ80ConfiguraSlot(i);
				break;

				case 7: // DiskII
					erro = DiscoConfiguraSlot(i);
				break;

				case 8: // MassStorage
					erro = HDConfiguraSlot(i);
				break;

				case 9: // IDE Richard
					if (i == 7)
						erro = IDEConfiguraSlot(i);
					else
						erro = MBConfiguraSlot(i);
				break;
			} // switch
			if (erro != ERR_OK)
			{
				Slots[i] = 0;
				return erro;
			}
		} // if
	} // for i
	return ERR_OK;
}

//===========================================================================
void MemRetiraTodosSlots()
{
	int i;

	for (i=1; i<8; i++)
	{
		// Retira placa do slot
		switch (Slots[i])
		{
			case 0: // Vazio
				//
			break;

			case 1: // Impressora
				ImpressoraRetiraSlot();
			break;

			case 2: // Serial
				CommRetiraSlot();
			break;

			case 3: // AppleClock
				RelogioRetiraSlot();
			break;

			case 4: // TKCLOCK
				TKClockRetiraSlot();
			break;

			case 5: // Mouse
				MouseRetiraSlot();
			break;

			case 6: // CP/M Z80
				CPMZ80RetiraSlot();
			break;

			case 7: // DiskII
				DiscoRetiraSlot();
			break;

			case 8: // MassStorage
				HDRetiraSlot();
			break;

			case 9: // IDE Richard
				if (i == 7)
					IDERetiraSlot();
				else
					MBRetiraSlot();
			break;
			} // switch
	} // for i
}

//===========================================================================
int MemRetiraSlot(int Slot)
{
	iofunction ioarrayread[0x10];
	iofunction ioarraywrite[0x10];

	ioarrayread[0x00] = NullIo;
	ioarrayread[0x01] = NullIo;
	ioarrayread[0x02] = NullIo;
	ioarrayread[0x03] = NullIo;
	ioarrayread[0x04] = NullIo;
	ioarrayread[0x05] = NullIo;
	ioarrayread[0x06] = NullIo;
	ioarrayread[0x07] = NullIo;
	ioarrayread[0x08] = NullIo;
	ioarrayread[0x09] = NullIo;
	ioarrayread[0x0A] = NullIo;
	ioarrayread[0x0B] = NullIo;
	ioarrayread[0x0C] = NullIo;
	ioarrayread[0x0D] = NullIo;
	ioarrayread[0x0E] = NullIo;
	ioarrayread[0x0F] = NullIo;

	ioarraywrite[0x00] = NullIo;
	ioarraywrite[0x01] = NullIo;
	ioarraywrite[0x02] = NullIo;
	ioarraywrite[0x03] = NullIo;
	ioarraywrite[0x04] = NullIo;
	ioarraywrite[0x05] = NullIo;
	ioarraywrite[0x06] = NullIo;
	ioarraywrite[0x07] = NullIo;
	ioarraywrite[0x08] = NullIo;
	ioarraywrite[0x09] = NullIo;
	ioarraywrite[0x0A] = NullIo;
	ioarraywrite[0x0B] = NullIo;
	ioarraywrite[0x0C] = NullIo;
	ioarraywrite[0x0D] = NullIo;
	ioarraywrite[0x0E] = NullIo;
	ioarraywrite[0x0F] = NullIo;

	return MemInsereSlot(Slot, ioarrayread, ioarraywrite, "NADA");
}

//===========================================================================
int MemInsereSlot(int Slot,
				   iofunction ioarrayread[0x10],
				   iofunction ioarraywrite[0x10],
				   char *NomeFirmware)
{
	char Caminho[MAX_PATH];
	char temp1[MAX_PATH];
	int  i;
	FILE *file;

	if ((Slot < 1) | (Slot > 7))
		return ERR_ERRO;
	if (_stricmp(NomeFirmware,"NADA"))
	{
		sprintf(Caminho,"%s%s_%s%s",progdir, "Firmware", NomeFirmware, ".rom");
		file = fopen(Caminho, "rb");
		if (file == NULL)
		{
			sprintf(temp1, "O firmware %s não foi achado.", NomeFirmware);
			FrameMostraMensagemErro(temp1);
			return ERR_ERRO;
		}
		fread(memrom + (Slot << 8), 256, 1, file);
		fread(memslotext + ((Slot-1) << 11), 0x0800, 1, file);
		fclose(file);
	}
	else
	{
		memset(memrom + (Slot << 8), 0xFF, 256);
		memset(memslotext + ((Slot-1) << 11), 0xFF, 0x0800);
	}
	for (i=0; i < 16; i++)
	{
		ioread[(Slot*0x10)+0x80+i] = ioarrayread[i];
		iowrite[(Slot*0x10)+0x80+i] = ioarraywrite[i];
	}

	UpdatePaging(0,0);
	return ERR_OK;
}

//===========================================================================
void MemAtualizaSlotExt(WORD endereco)
{
	if ((endereco >= 0xC100) && (endereco <= 0xC7FF) && SW_SLOTCXROM)
    {
		BYTE slotext2 = ((endereco >> 8) - 0xC0);
		if (slotext != slotext2)
		{
			slotext = slotext2;
			UpdatePaging(0,0);
		}
	}
	if ((endereco == 0xCFFF) && (slotext > 0))
	{
		slotext = 0;
		UpdatePaging(0,0);
	}
}

//===========================================================================
BOOL MemImportar(WORD EndInicial)
{
#ifdef _WINDOWS
	OPENFILENAME ofn;
#endif // _WINDOWS
	FILE  *Arquivo;
	char  *Buffer;
	DWORD TamMem;
	char  directory[MAX_PATH] = "";
	char  filename[MAX_PATH]  = "";

#ifdef _WINDOWS
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize     = sizeof(OPENFILENAME);
	ofn.hwndOwner       = (HWND)framewindow;
	ofn.hInstance       = (HINSTANCE)instance;
	ofn.lpstrFilter     = "Todos os Arquivos\0*.*\0";
	ofn.lpstrFile       = filename;
	ofn.nMaxFile        = MAX_PATH;
	ofn.lpstrInitialDir = directory;
	ofn.Flags           = OFN_PATHMUSTEXIST;
	ofn.lpstrTitle      = "Escolha o Arquivo";
	if (GetOpenFileName(&ofn))
#else
	if (0)
#endif
	{
		/*
		Arquivo = CreateFile(filename,
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								0,
								NULL);
		*/
		Arquivo = fopen(filename, "rb");
		if (!Arquivo)
		{
			FrameMostraMensagemErro("Erro ao abrir o arquivo!");
			return -1;
		}
		fseek(Arquivo, 0, SEEK_END);
		//TamMem = GetFileSize(Arquivo,NULL);
		TamMem = ftell(Arquivo);
		if ((EndInicial+TamMem) > 0xBFFF)
		{
			FrameMostraMensagemErro("O conteúdo do arquivo excede a capacidade de memória do TK3000 //e");
			/*
			MessageBox(framewindow,
						"O conteúdo do arquivo excede a capacidade "
						"de memória do TK3000 //e",
						"Erro",
						MB_OK | MB_ICONERROR
						);
			*/
			return -1;
		}
		Buffer = (char *)malloc(TamMem);
		fseek(Arquivo, 0, SEEK_SET);
		//ReadFile(Arquivo, (LPVOID)Buffer, TamMem, &TamMem, NULL);
		fread(Buffer, TamMem, 1, Arquivo);
		//memcpy((void *)(mem+EndInicial), (const void*)Buffer, (size_t)(TamMem));
		for (unsigned int c = 0; c < TamMem; c++)
			mem_writeb(EndInicial + c, Buffer[c]);
		fclose(Arquivo);
		return 0;
	}
	return -1;
}

//===========================================================================
BOOL MemExportar(WORD EndInicial, WORD EndFinal)
{
#ifdef _WINDOWS
	OPENFILENAME ofn;
#endif // _WINDOWS
	FILE  *Arquivo;
	char  *Buffer;
	DWORD TamMem;
	char  directory[MAX_PATH] = "";
	char  filename[MAX_PATH]  = "";

	TamMem = EndFinal - EndInicial + 1;
	Buffer = (char *)malloc(TamMem);
	for (unsigned int c = 0; c < TamMem; c++)
		Buffer[c] = mem_readb(EndInicial + c, 0);
	//memcpy((void *)Buffer, (const void*)(mem+EndInicial), (size_t)(TamMem));
#ifdef _WINDOWS
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize     = sizeof(OPENFILENAME);
	ofn.hwndOwner       = (HWND)framewindow;
	ofn.hInstance       = (HINSTANCE)instance;
	ofn.lpstrFilter     = "Todos os Arquivos\0*.*\0";
	ofn.lpstrFile       = filename;
	ofn.nMaxFile        = MAX_PATH;
	ofn.lpstrInitialDir = directory;
	ofn.Flags           = OFN_OVERWRITEPROMPT;
	ofn.lpstrTitle      = "Escolha o Arquivo";
	if (GetSaveFileName(&ofn))
#else // _WINDOWS
	if (0)
#endif // _WINDOWS
	{
		/*
		Arquivo = CreateFile(filename,
								GENERIC_WRITE,
								FILE_SHARE_READ,
								NULL,
								CREATE_ALWAYS,
								0,
								NULL);
		*/
		Arquivo = fopen(filename, "wb");
		if (!Arquivo)
		{
			FrameMostraMensagemErro("Erro ao criar arquivo!");
			return -1;
		}
		if (!fwrite(Buffer, TamMem, 1, Arquivo))
			/*
			WriteFile(Arquivo,
						(LPVOID)Buffer,
						TamMem,
						&BytesEscritos,
						NULL))
			*/
		{
			FrameMostraMensagemErro("Erro ao criar arquivo!");
			fclose(Arquivo);
			return -1;
		}
		fclose(Arquivo);
		return 0;
	}
	return -1;
}

//===========================================================================
BOOL MemImportarB()
{
#ifdef _WINDOWS
	OPENFILENAME ofn;
#endif // _WINDOWS
	FILE  *Arquivo;
	char  *Buffer;
	WORD TamMem, EndInicial;
	char  directory[MAX_PATH] = "";
	char  filename[MAX_PATH]  = "";

#ifdef _WINDOWS
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize     = sizeof(OPENFILENAME);
	ofn.hwndOwner       = (HWND)framewindow;
	ofn.hInstance       = (HINSTANCE)instance;
	ofn.lpstrFilter     = "Arquivos .B\0*.b\0Todos os Arquivos\0*.*\0";
	ofn.lpstrFile       = filename;
	ofn.nMaxFile        = MAX_PATH;
	ofn.lpstrInitialDir = directory;
	ofn.Flags           = OFN_PATHMUSTEXIST;
	ofn.lpstrTitle      = "Escolha o Arquivo";
	if (GetOpenFileName(&ofn))
#else
	if (0)
#endif
	{
		/*
		Arquivo = CreateFile(filename,
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								0,
								NULL);
		*/
		Arquivo = fopen(filename, "rb");
		if (!Arquivo)
		{
			FrameMostraMensagemErro("Erro ao abrir o arquivo!");
			return -1;
		}
		fseek(Arquivo, 0, SEEK_SET);
		fread(&EndInicial, sizeof(WORD), 1, Arquivo);
		fread(&TamMem, sizeof(WORD), 1, Arquivo);

		if ((EndInicial+TamMem) > 0xBFFF)
		{
			FrameMostraMensagemErro("O conteúdo do arquivo excede a capacidade de memória do TK3000 //e");
			/*
			MessageBox(framewindow,
						"O conteúdo do arquivo excede a capacidade "
						"de memória do TK3000 //e",
						"Erro",
						MB_OK | MB_ICONERROR
						);
			*/
			return -1;
		}
		Buffer = (char *)malloc(TamMem);
		//ReadFile(Arquivo, (LPVOID)Buffer, TamMem, &TamMem, NULL);
		fread(Buffer, TamMem, 1, Arquivo);
		//memcpy((void *)(mem+EndInicial), (const void*)Buffer, (size_t)(TamMem));
		for (unsigned int c = 0; c < TamMem; c++)
			mem_writeb(EndInicial + c, Buffer[c]);
		fclose(Arquivo);
		return 0;
	}
	return -1;
}

//===========================================================================
void MemSalvarEstado()
{
	//
}

//===========================================================================
void MemCarregarEstado()
{
	//
}

//===========================================================================
BYTE mem_readb(WORD endereco, BOOL AcessarIO)
{
	BYTE* page;
	BYTE  result;

	if (AcessarIO) MemAtualizaSlotExt(endereco);
	if ((endereco >= 0xC100) && (endereco <= 0xC7FF)) {
//		CPMZ80Verifica(endereco);
		MBVerifica(endereco, 0, 0);
	}

	if ((endereco & 0xFF00) == 0xC000) 
	{
		if (AcessarIO)
			result = ioread[endereco & 0xFF](regs.pc, (BYTE)endereco, false, 0);
		else
			result = MemReturnRandomData(1);
	}
	else
	{
		page = memread[endereco >> 8];
		if (page)
			result = *(page+(endereco & 0xFF));
		else
		{
			if (endereco >= 0xC100 && endereco <= 0xCFFF)
				result = MemReturnRandomData(1);
			else
				result = 0xFF;//MemReturnRandomData(1);
		}
	}
	return result;
}

//===========================================================================
WORD mem_readw(WORD endereco, BOOL AcessarIO)
{
	BYTE* page;
	WORD  result;

	if (AcessarIO) MemAtualizaSlotExt(endereco);
	if ((endereco >= 0xC100) && (endereco <= 0xC7FF)) {
//		CPMZ80Verifica(endereco);
		MBVerifica(endereco, 0, 0);
	}

	if ((endereco & 0xFF00) == 0xC000)
	{
		if (AcessarIO)
			result = (ioread[endereco & 0xFF](regs.pc,(BYTE)endereco, false, 0)) |
					 (ioread[++endereco & 0xFF](regs.pc,(BYTE)endereco, false, 0) << 8);
		else
			result = (MemReturnRandomData(1) << 8) | MemReturnRandomData(1);
	}
	else
	{
		page = memread[endereco >> 8];
		if (page)
			result = *(BYTE*)(page+(endereco & 0xFF));
		else
			result = 0xFF;//MemReturnRandomData(1);
		page = memread[++endereco >> 8];
		if (page)
			result |= (*(BYTE*)(page+(endereco & 0xFF))) << 8;
		else
			result |= 0xFF00;//MemReturnRandomData(1) << 8;
	}
	return result;
}

//===========================================================================
void mem_writeb(WORD endereco, BYTE valor)
{
	BYTE* page;
	
	memdirty[endereco >> 8] = 0xFF;
	if ((endereco >= 0xC100) && (endereco <= 0xC7FF)) {
		CPMZ80Verifica(endereco);
		MBVerifica(endereco, 1, valor);
	}
	if ((endereco & 0xFF00) == 0xC000)
	{
		iowrite[endereco & 0xFF](regs.pc, (BYTE)endereco, true, (BYTE)(valor));
	}
	else
	{
		page = memwrite[endereco >> 8];	
		if (page)
			*(page+(endereco & 0xFF)) = (BYTE)(valor);
	}
}

//===========================================================================
/*
void mem_writew(WORD endereco, WORD valor)
{
	BYTE* page;
	
	memdirty[endereco >> 8] = 0xFF;
	if ((endereco >= 0xC100) && (endereco <= 0xC7FF)) {
		CPMZ80Verifica(endereco);
		MBVerifica(endereco, 1, valor);	//FIXME: 2 escritas
	}
	if ((endereco & 0xFF00) == 0xC000)
	{
		iowrite[endereco & 0xFF](regs.pc,(BYTE)endereco, true, (BYTE)(valor));
		iowrite[++endereco & 0xFF](regs.pc,(BYTE)endereco, true, (BYTE)(valor >> 8));
	}
	else
	{
		page = memwrite[endereco >> 8];	
		if (page)
			*(BYTE*)(page+(endereco & 0xFF)) = valor & 0xFF;
		page = memwrite[++endereco >> 8];	
		if (page)
			*(BYTE*)(page+(endereco & 0xFF)) = valor >> 8;
	}
}
*/

//===========================================================================
void MemDestroy()
{
	int i;

	if (fastpaging)
		MemSetFastPaging(0);
	free(memaux);
	free(memdirty);
//	free(memimage);
	free(memmain);
	free(memrom);
	free(memslotext);
	memaux     = NULL;
	memdirty   = NULL;
//	memimage   = NULL;
	memmain    = NULL;
	memrom     = NULL;
	memslotext = NULL;
//	mem        = NULL;
	memset(memwrite, 0, sizeof(memwrite));
	memset(memread, 0, sizeof(memread));
	for (i=1; i<MAXEXPAGES; i++)
	{
		if (RWpages[i])
		{
			free(RWpages[i]);
			RWpages[i] = NULL;
		}
	}
	RWpages[0] = NULL;

#ifdef MEMDEBUG
	fclose(debugfile);
#endif
}

//===========================================================================
BYTE* MemGetAuxPtr (WORD offset)
{
	return RWpages[0]+offset;
}

//===========================================================================
BYTE* MemGetMainPtr (WORD offset)
{
	return memmain+offset;
}

//===========================================================================
void MemInitialize ()
{

	// ALLOCATE MEMORY FOR THE APPLE MEMORY IMAGE AND ASSOCIATED DATA
	// STRUCTURES
	//
	// THE MEMIMAGE BUFFER CAN CONTAIN EITHER MULTIPLE MEMORY IMAGES OR
	// ONE MEMORY IMAGE WITH COMPILER DATA
	int   i;
	char  filename[MAX_PATH];
	FILE  *file;
#ifdef MEMDEBUG
	char nomearq[MAX_PATH];

	strcpy(nomearq, progdir);
	strcat(nomearq, "memdebug.txt");
	debugfile = fopen(nomearq, "wb");
#endif

	memaux     = (BYTE*)malloc(0x010000);
	memdirty   = (BYTE*)malloc(0x000100);
	memmain    = (BYTE*)malloc(0x010000);
	memrom     = (BYTE*)malloc(0x005000);
	memslotext = (BYTE*)malloc(0x004800);
//	memimage   = (BYTE*)malloc(MAX(0x030000, MAXIMAGES*0x010000));
	if ((!memaux) || (!memdirty) || /*(!memimage) ||*/
			(!memmain) || (!memrom))
	{
		FrameMostraMensagemErro("Não foi possível alocar memória para o emulador.\n"
				"A execução não será possível.");
		exit(1);
	}
	RWpages[0] = memaux;
	i=1;
	while ((i<MAXEXPAGES) && (RWpages[i] = (unsigned char *)malloc(0x10000)))
		i++;

	// READ THE APPLE FIRMWARE ROMS INTO THE ROM IMAGE
	memset(memrom, 0xFF, 0x5000);
	memset(memslotext, 0xFF, 0x4800);
	strcpy(filename, progdir);
	strcat(filename, NOMEARQROM);
	file = fopen(filename, "rb");
	if (file == NULL)
	{
		FrameMostraMensagemErro("A imagem de ROM não foi achada: "NOMEARQROM".");
		exit(1);
	}
	fread(memrom+0x1000, 0x4000, 1, file);
	fclose(file);

	// Zera I/Os
	for (int c = 1; c < 8; c++)
		MemRetiraSlot(c);

	MemReset();
}

//===========================================================================
void MemReset ()
{

	// TURN OFF FAST PAGING IF IT IS CURRENTLY ACTIVE
	MemSetFastPaging(0);

	// INITIALIZE THE PAGING TABLES
	memset(memread,  0, 256*sizeof(BYTE*));
	memset(memwrite, 0, 256*sizeof(BYTE*));

	// INITIALIZE THE RAM IMAGES
	memset(memaux , 0, 0x10000);
	memset(memmain, 0, 0x10000);

	// SET UP THE MEMORY IMAGE
//	mem   = memimage;
	image = 0;

	// INITIALIZE THE CPU
	CpuInitialize();
	Z80_Reset();

	// INITIALIZE PAGING, FILLING IN THE 64K MEMORY IMAGE
	ResetPaging(1);
	regs.pc = mem_readw(0xFFFC, 1);
}

//===========================================================================
void MemResetPaging()
{
	ResetPaging(0);
}

//===========================================================================
BYTE MemReturnRandomData (BYTE highbit)
{
	static const BYTE retval[16] = {0x00,0x2D,0x2D,0x30,0x30,0x32,0x32,0x34,
									0x35,0x39,0x43,0x43,0x43,0x60,0x7F,0x7F};
	BYTE r = (BYTE)(rand() & 0xFF);
	if (r <= 0xAA)
		return 0x20 | (highbit ? 0x80 : 0);
	else
		return retval[r & 0x0F] | (highbit ? 0x80 : 0);
}

//===========================================================================
void MemSetFastPaging (BOOL on)
{
//	FrameMostraMensagemAdvertencia("MemSetFastPaging()");

	if (fastpaging && modechanging)
	{
		modechanging = 0;
		UpdateFastPaging();
	}
	else if (!fastpaging)
	{
		BackMainImage();
//		if (lastimage >= 3)
//			free(memimage+0x30000);
	}
	fastpaging   = on;
	image        = 0;
//	mem          = memimage;
	lastimage    = 0;
//	imagemode[0] = memmode;
	if (!fastpaging)
		UpdatePaging(1,0);
	cpuemtype = fastpaging ? CPU_FASTPAGING : CPU_COMPILING;
	CpuReinitialize();
	if (cpuemtype == CPU_COMPILING)
		CpuResetCompilerData();
}

//===========================================================================
void MemTrimImages ()
{
/*
	static DWORD trimnumber = 0;
	DWORD realimage;

	if (fastpaging && (lastimage > 2))
	{
		if (modechanging)
		{
			modechanging = 0;
			UpdateFastPaging();
		}
		if ((image != trimnumber) &&
				(image != lastimage) &&
				(trimnumber < lastimage))
		{
			//imagemode[trimnumber] = imagemode[lastimage];
			//VirtualFree(memimage+(lastimage-- << 16),0x10000,MEM_DECOMMIT);
			//free(memimage+(lastimage-- << 16));
			realimage = image;
			image   = trimnumber;
			//mem     = memimage+(image << 16);
			memmode = imagemode[image];
			UpdatePaging(1,0);
			image   = realimage;
			//mem     = memimage+(image << 16);
			memmode = imagemode[image];
			CpuReinitialize();
		}
		if (++trimnumber >= lastimage)
			trimnumber = 0;
	}
*/
}

// SoftSwitches
//===========================================================================
BYTE __stdcall MemCheckPaging (WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	BOOL result = 0;
	switch (address)
	{
		case 0x11: result = SW_BANK2;       break;
		case 0x12: result = SW_HIGHRAM;     break;
		case 0x13: result = SW_AUXREAD;     break;
		case 0x14: result = SW_AUXWRITE;    break;
		case 0x15: result = !SW_SLOTCXROM;  break;
		case 0x16: result = SW_ALTZP;       break;
		case 0x17: result = SW_SLOTC3ROM;   break;
		case 0x18: result = SW_80STORE;     break;
		case 0x1C: result = SW_PAGE2;       break;
		case 0x1D: result = SW_HIRES;       break;
	}
	return KeybGetKeycode() | (result ? 0x80 : 0);
}

//===========================================================================
BYTE __stdcall MemSetPaging (WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	DWORD lastmemmode = memmode;

	// DETERMINE THE NEW MEMORY PAGING MODE.
	if ((address >= 0x80) && (address <= 0x8F))
	{
		BOOL writeram = (address & 1);
		memmode &= ~(MF_BANK2 | MF_HIGHRAM | MF_WRITERAM);
		lastwriteram = 1; // note: because diags.do doesn't set switches twice!
		if (lastwriteram && writeram)
			memmode |= MF_WRITERAM;
		if (!(address & 8))
			memmode |= MF_BANK2;
		if (((address & 2) >> 1) == (address & 1))
			memmode |= MF_HIGHRAM;
		lastwriteram = writeram;
	}
	else
		switch (address)
		{
			case 0x00: memmode &= ~MF_80STORE;    break;
			case 0x01: memmode |=  MF_80STORE;    break;
			case 0x02: memmode &= ~MF_AUXREAD;    break;
			case 0x03: memmode |=  MF_AUXREAD;    break;
			case 0x04: memmode &= ~MF_AUXWRITE;   break;
			case 0x05: memmode |=  MF_AUXWRITE;   break;
			case 0x06: memmode |=  MF_SLOTCXROM;  break;
			case 0x07: memmode &= ~MF_SLOTCXROM;  break;
			case 0x08: memmode &= ~MF_ALTZP;      break;
			case 0x09: memmode |=  MF_ALTZP;      break;
			case 0x0A: memmode &= ~MF_SLOTC3ROM;  break;
			case 0x0B: memmode |=  MF_SLOTC3ROM;  break;
			case 0x54: memmode &= ~MF_PAGE2;      break;
			case 0x55: memmode |=  MF_PAGE2;      break;
			case 0x56: memmode &= ~MF_HIRES;      break;
			case 0x57: memmode |=  MF_HIRES;      break;
			case 0x71: // extended memory aux page number
			case 0x73: // Ramworks III set aux page number

	        			if (RWpages[value] && (value < numexpages))
						{
#ifdef MEMDEBUG
							debugreadon = 1;
#endif
							RWatual = value;
							if (fastpaging)
								UpdateFastPaging();
							else
							{
								UpdatePaging(0,0);
								if (cpuemtype == CPU_COMPILING)
									CpuResetCompilerData();
							}
						}
						else
						{
							RWatual = -1;
							UpdatePaging(0,0);
						}
			break;
		}

	// IF THE EMULATED PROGRAM HAS JUST UPDATE THE MEMORY WRITE MODE AND IS
	// ABOUT TO UPDATE THE MEMORY READ MODE, HOLD OFF ON ANY PROCESSING UNTIL
	// IT DOES SO.
	/*
	if ((address >= 4) && (address <= 5) &&
			((*(DWORD*)(mem+programcounter) & 0x00FFFEFF) == 0x00C0028D))
	{
		modechanging = 1;
		return write ? 0 : MemReturnRandomData(1);
	}
	if ((address >= 0x80) && (address <= 0x8F) && (programcounter < 0xC000) &&
			(((*(DWORD*)(mem+programcounter) & 0x00FFFEFF) == 0x00C0048D) ||
			((*(DWORD*)(mem+programcounter) & 0x00FFFEFF) == 0x00C0028D)))
	{
		modechanging = 1;
		return write ? 0 : MemReturnRandomData(1);
	}
	*/
	// IF THE MEMORY PAGING MODE HAS CHANGED, UPDATE OUR MEMORY IMAGES AND
	// WRITE TABLES.
	if ((lastmemmode != memmode) || modechanging)
	{
		modechanging = 0;
		++pages;

		// IF FAST PAGING IS ACTIVE, WE KEEP MULTIPLE COMPLETE MEMORY IMAGES
		// AND WRITE TABLES, AND SWITCH BETWEEN THEM.  THE FAST PAGING VERSION
		// OF THE CPU EMULATOR KEEPS ALL OF THE IMAGES COHERENT.
		if (fastpaging)
			UpdateFastPaging();

		// IF FAST PAGING IS NOT ACTIVE THEN WE KEEP ONLY ONE MEMORY IMAGE AND
		// WRITE TABLE, AND UPDATE THEM EVERY TIME PAGING IS CHANGED.
		else
		{
			UpdatePaging(0,0);
			if (cpuemtype == CPU_COMPILING)
				CpuResetCompilerData();
		}

	}

	if ((address <= 1) || ((address >= 0x54) && (address <= 0x57)))
		VideoSetMode(programcounter,address,write,value);
	return write ? 0
				: MemReturnRandomData(((address == 0x54) || (address == 0x55))
										? (SW_PAGE2 != 0) : 1);
}

// EOF