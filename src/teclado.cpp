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

// Emula o teclado do TK3000 //e

#include "stdhdr.h"
#include "teclado.h"
#include "cpu.h"
#include "memoria.h"
#include "impressora.h"
#include "disco.h"
#include "janela.h"
#include "video.h"
#include "mouse.h"
#include "Mockingboard.h"

#define MAX_BUFFER 100

BYTE asciicode[4] = {0x08,0x0B,0x15,0x0A};

BOOL  capslock        = 1;
BOOL  modekey	      = 0;
BOOL  keywaiting      = 0;
int   posKeycode	  = 0;
BYTE  keycode[MAX_BUFFER];
int   lastvirtkey[MAX_BUFFER];
static bool g_bPasteFromClipboard = false;
static bool g_bClipboardActive = false;
static HGLOBAL hglb = NULL;
static LPTSTR lptstr = NULL;
int ColarRapido = 0;

//===========================================================================
static void ClipboardDone()
{
	if (g_bClipboardActive)
	{
		g_bClipboardActive = false;
		GlobalUnlock(hglb);
		CloseClipboard();
	}
}

//===========================================================================
static void ClipboardInit()
{
	ClipboardDone();

	if (!IsClipboardFormatAvailable(CF_TEXT))
		return;

	if (!OpenClipboard((HWND)framewindow))
		return;

	hglb = GetClipboardData(CF_TEXT);
	if (hglb == NULL)
	{	
		CloseClipboard();
		return;
	}

	lptstr = (char*) GlobalLock(hglb);
	if (lptstr == NULL)
	{	
		CloseClipboard();
		return;
	}

	g_bPasteFromClipboard = false;
	g_bClipboardActive = true;
}

//===========================================================================
static char ClipboardCurrChar(bool bIncPtr)
{
	char nKey;
	int nInc = 1;

	if((lptstr[0] == 0x0D) && (lptstr[1] == 0x0A))
	{
		nKey = 0x0D;
		nInc = 2;
	}
	else
	{
		nKey = lptstr[0];
	}

	if(bIncPtr)
		lptstr += nInc;

	return nKey;
}


//
// ----- ALL GLOBALLY ACCESSIBLE FUNCTIONS ARE BELOW THIS LINE -----
//

//===========================================================================
void ClipboardInitiatePaste()
{
	if (g_bClipboardActive)
		return;

	g_bPasteFromClipboard = true;
}

//===========================================================================
void KeybGetModeStatus (BOOL *status)
{
	*status = modekey;  
}

//===========================================================================
void KeybGetCapsStatus (BOOL *status)
{
	*status = capslock;
}

//===========================================================================
BYTE KeybGetKeycode ()
{
	return keycode[0];
}

//===========================================================================
/*DWORD KeybGetNumQueries ()
{
DWORD result = keyboardqueries;
keyboardqueries = 0;
return result;
}*/

//===========================================================================
void KeybQueueKeypress (int key, BOOL ascii)
{
	if (ascii)
	{
		if (modekey)
		{
			if ((key >= 0xC0) && (key <= 0xDA)) key += 0x20;
			switch (key)
			{
			case 0xE0: key = '_';  break; // à
			case 0xE1: key = '@';  break; // á
			case 0xE2: key = '\\'; break; // â
			case 0xE3: key = '[';  break; // ã
			case 0xE7: key = ']';  break; // ç
			case 0xE9: key = '`';  break; // é
			case 0xEA: key = '&';  break; // ê
			case 0xED: key = '{';  break; // í
			case 0xF3: key = '~';  break; // ó
			case 0xF4: key = '}';  break; // ô
			case 0xF5: key = '#';  break; // õ
			case 0xFA: key = '|';  break; // ú
			}
		}
		if (key > 0x7F) return;
		if ((key >= 'a') && (key <= 'z') && (capslock))
			keycode[posKeycode] = key - ('a'-'A');
		else
			keycode[posKeycode] = key;
		lastvirtkey[posKeycode++] = LOBYTE(VkKeyScan(key));
	}
	else
	{
		if (key == VK_ESCAPE && g_bClipboardActive) {
			ClipboardDone();
		}
		if (((key == VK_CANCEL) || (key == VK_F12)) &&
			((GetKeyState(VK_CONTROL) < 0)))
		{
			// CTRL+RESET
			ClipboardDone();
			MemResetPaging();
			DiskReset();
			VideoResetState();
			ImpressoraReset();
			Z80_Reset();
			MouseReset();
			MB_Reset();
			CPUAtual = CPU65C02;
			regs.pc = mem_readw(0xFFFC, 0);
			return;
		}
		if (key == VK_F11)
		{
			KeybToggleModeKey();
			return;
		}
		if (key == VK_DELETE)
		{
			keycode[posKeycode] = 0x7F;
			lastvirtkey[posKeycode++] = key;
		}
		else if ((key >= VK_LEFT) && (key <= VK_DOWN))
		{
			keycode[posKeycode] = asciicode[key - VK_LEFT];
			lastvirtkey[posKeycode++] = key;
		}
		else if ((key == VK_INSERT) && (GetKeyState(VK_SHIFT) < 0))
		{
			// Shift+Insert
			ClipboardInitiatePaste();
			return;
		}
		else
			return;
	}
	keywaiting = 1;
}

//===========================================================================
void KeybAjustaCapsLock()
{
	capslock = (GetKeyState(VK_CAPITAL) & 1);
	FrameRefreshStatus(DRAW_LEDS);
}

//===========================================================================
void KeybToggleCapsLock ()
{
	capslock = !capslock;
	FrameRefreshStatus(DRAW_LEDS);
}

//===========================================================================
void KeybToggleModeKey()
{
	modekey = !modekey;
	FrameRefreshStatus(DRAW_LEDS);
	VideoRedrawScreen();
	ImpressoraMudaAcentuado();
}

bool KeybIsPasting()
{
	return g_bClipboardActive;
}

// Soft-Switches
//===========================================================================
BYTE __stdcall KeybReadData (WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if(g_bPasteFromClipboard)
		ClipboardInit();

	if(g_bClipboardActive)
	{
		if(*lptstr == 0)
			ClipboardDone();
		else
			return 0x80 | ClipboardCurrChar(false);
	}

	BYTE key = keycode[0];
	return key | (keywaiting ? 0x80 : 0);
}

//===========================================================================
BYTE __stdcall KeybReadFlag (WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if(g_bPasteFromClipboard)
		ClipboardInit();

	if(g_bClipboardActive)
	{
		if(*lptstr == 0)
			ClipboardDone();
		else
			return 0x80 | ClipboardCurrChar(true);
	}

	BYTE key     = keycode[0] | (keywaiting ? 0x80 : 0);
	int  virtkey = lastvirtkey[0];

	if (posKeycode) {
		if (posKeycode != 1) 
			memmove(keycode, keycode+1, posKeycode);
		posKeycode--;
	}
	if (!posKeycode)
		keywaiting = 0;
	return key;
}

// EOF