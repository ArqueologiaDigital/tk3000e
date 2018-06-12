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

// Emula um joystick

#include "stdhdr.h"
#include "tk3000e.h"
#include "cpu.h"
#include "joystick.h"
#include "memoria.h"

#define  BUTTONTIME       5000

#define  DEVICE_NONE      0
#define  DEVICE_JOYSTICK  1
#define  DEVICE_KEYBOARD  2
#define  DEVICE_MOUSE     3

#define  MODE_NONE        0
#define  MODE_STANDARD    1
#define  MODE_CENTERING   2
#define  MODE_SMOOTH      3

typedef struct _joyinforec
{
	int device;
	int mode;
} joyinforec, *joyinfoptr;

joyinforec joyinfo[5]   = {{DEVICE_NONE,MODE_NONE},
                           {DEVICE_JOYSTICK,MODE_STANDARD},
                           {DEVICE_KEYBOARD,MODE_STANDARD},
                           {DEVICE_KEYBOARD,MODE_CENTERING},
                           {DEVICE_MOUSE,MODE_STANDARD}};
BOOL       keydown[11]  = {0,0,0,0,0,0,0,0,0,0,0};
POINT      keyvalue[9]  = {{0,255},{127,255},{255,255},
                           {0,127},{127,127},{255,127},
                           {0,0  },{127,0  },{255,0  }};

// Variáveis:
DWORD buttonlatch[3] = {0,0,0};
BOOL  joybutton[3]   = {0,0,0};
int   joyshrx        = 8;
int   joyshry        = 8;
int   joysubx        = 0;
int   joysuby        = 0;
DWORD joytype        = 1;
BOOL  setbutton[2]   = {0,0};
int   xpos           = 127;
int   ypos           = 127;
static unsigned __int64 g_nJoyCntrResetCycle = 0;	// Abs cycle that joystick counters were reset

//===========================================================================
void CheckJoystick ()
{
	static DWORD lastcheck = 0;
	DWORD currtime = GetTickCount();

	if ((currtime-lastcheck >= 10) || joybutton[0] || joybutton[1])
	{
		JOYINFO info;

		lastcheck = currtime;
		if (joyGetPos(JOYSTICKID1, &info) == JOYERR_NOERROR)
		{
			if ((info.wButtons & JOY_BUTTON1) && !joybutton[0])
				buttonlatch[0] = BUTTONTIME;
			if ((info.wButtons & JOY_BUTTON2) && !joybutton[1])
				buttonlatch[1] = BUTTONTIME;
			if ((info.wButtons & JOY_BUTTON3) && !joybutton[2])
				buttonlatch[2] = BUTTONTIME;
			joybutton[0] = ((info.wButtons & JOY_BUTTON1) != 0);
			joybutton[1] = ((info.wButtons & JOY_BUTTON2) != 0);
			joybutton[2] = ((info.wButtons & JOY_BUTTON3) != 0);
			xpos = (info.wXpos-joysubx) >> joyshrx;
			ypos = (info.wYpos-joysuby) >> joyshry;
		}
	}
}

//
// ----- ALL GLOBALLY ACCESSIBLE FUNCTIONS ARE BELOW THIS LINE -----
//

//===========================================================================
void JoyInitialize ()
{
	UINT xrange;
	UINT yrange;

	if (joyinfo[joytype].device == DEVICE_JOYSTICK)
	{
		JOYCAPS caps;
		if (joyGetDevCaps(JOYSTICKID1,&caps,sizeof(JOYCAPS)) == JOYERR_NOERROR)
		{
			joyshrx = 0;
			joyshry = 0;
			joysubx = (int)caps.wXmin;
			joysuby = (int)caps.wYmin;
			xrange  = caps.wXmax-caps.wXmin;
			yrange  = caps.wYmax-caps.wYmin;
			while (xrange > 256)
			{
				xrange >>= 1;
				++joyshrx;
			}
			while (yrange > 256)
			{
				yrange >>= 1;
				++joyshry;
			}
		}
		else
			joytype = 3;
	}
}

//===========================================================================
void JoyDestroy()
{
	//
}

//===========================================================================
BOOL JoyProcessKey (int virtkey, BOOL extended, BOOL down, BOOL autorep)
{
	BOOL keychange;

	if ((joyinfo[joytype].device != DEVICE_KEYBOARD) && (virtkey != VK_MENU))
		return 0;
	keychange = !extended;
	if (virtkey == VK_MENU)
	{
		keychange = 1;
		keydown[9+(extended != 0)] = down;
	}
	else if (!extended)
	{
		if ((virtkey >= VK_NUMPAD1) && (virtkey <= VK_NUMPAD9))
			keydown[virtkey-VK_NUMPAD1] = down;
		else
			switch (virtkey)
		{
			case VK_END:     keydown[ 0] = down;  break;
			case VK_DOWN:    keydown[ 1] = down;  break;
			case VK_NEXT:    keydown[ 2] = down;  break;
			case VK_LEFT:    keydown[ 3] = down;  break;
			case VK_CLEAR:   keydown[ 4] = down;  break;
			case VK_RIGHT:   keydown[ 5] = down;  break;
			case VK_HOME:    keydown[ 6] = down;  break;
			case VK_UP:      keydown[ 7] = down;  break;
			case VK_PRIOR:   keydown[ 8] = down;  break;
			case VK_NUMPAD0: keydown[ 9] = down;  break;
			case VK_INSERT:  keydown[ 9] = down;  break;
			case VK_DECIMAL: keydown[10] = down;  break;
			case VK_DELETE:  keydown[10] = down;  break;
			default:         keychange = 0;       break;
		}
		if (keychange)
			if ((virtkey == VK_NUMPAD0) || (virtkey == VK_INSERT))
			{
				if (down)
					buttonlatch[0] = BUTTONTIME;
			}
			else if ((virtkey == VK_DECIMAL) || (virtkey == VK_DELETE))
			{
				if (down)
					buttonlatch[1] = BUTTONTIME;
			}
			else if ((down && !autorep) || (joyinfo[joytype].mode == MODE_CENTERING))
			{
				int xkeys  = 0;
				int ykeys  = 0;
				int xtotal = 0;
				int ytotal = 0;
				int keynum = 0;
				while (keynum < 9)
				{
					if (keydown[keynum])
					{
						if ((keynum % 3) != 1)
						{
							xkeys++;
							xtotal += keyvalue[keynum].x;
						}
						if ((keynum / 3) != 1)
						{
							ykeys++;
							ytotal += keyvalue[keynum].y;
						}
					}
					keynum++;
				}
				if (xkeys)
					xpos = xtotal / xkeys;
				else
					xpos = 127;
				if (ykeys)
					ypos = ytotal / ykeys;
				else
					ypos = 127;
		}
	}
	return keychange;
}

//===========================================================================
void JoyReset ()
{
	int loop = 0;
	while (loop < 11)
		keydown[loop++] = 0;
}

//===========================================================================
void JoySetButton (int number, BOOL down)
{
	if (number > 1)
		return;
	setbutton[number] = down;
	if (down)
		buttonlatch[number] = BUTTONTIME;
}

//===========================================================================
BOOL JoySetEmulationType (HWND window, DWORD newtype)
{
	if (joyinfo[newtype].device == DEVICE_JOYSTICK)
	{
		JOYCAPS caps;
		if (joyGetDevCaps(JOYSTICKID1,&caps,sizeof(JOYCAPS)) != JOYERR_NOERROR)
		{
			MessageBox(window,
					TEXT("O emulador não conseguiu ler o joystick do seu computador.\n")
					TEXT("Se o joystick estiver configurado corretamente,\n")
					TEXT("verifique se o cabo do joystick está encaixado corretamente\n")
					TEXT("e verifique se o driver do joystick está instalado."),
					TEXT("Configuração"),
					MB_ICONEXCLAMATION | MB_SETFOREGROUND);
			return 0;
		}
	}
	else if ((joyinfo[newtype].device == DEVICE_MOUSE) &&
			(joyinfo[joytype].device != DEVICE_MOUSE))
		MessageBox(window,
				TEXT("Para começar a emular o joystick com seu mouse, clique ")
				TEXT("na tela emulada com o botão esquerdo. Durante a emulação ")
				TEXT("o mouse não poderá ser usado para outros fins, para voltar ")
				TEXT("a usá-lo, clique com o botão esquerdo enquanto você segura ")
				TEXT("a tecla CTRL."),
				TEXT("Configuração"),
				MB_ICONINFORMATION | MB_SETFOREGROUND);
	joytype = newtype;
	JoyInitialize();
	JoyReset();
	return 1;
}

//===========================================================================
void JoySetPosition (int xvalue, int xrange, int yvalue, int yrange)
{
	xpos = (xvalue*255)/xrange;
	ypos = (yvalue*255)/yrange;
}
 
//===========================================================================
void JoyUpdatePosition (DWORD cycles)
{
	if (buttonlatch[0]) --buttonlatch[0];
	if (buttonlatch[1]) --buttonlatch[1];
	if (buttonlatch[2]) --buttonlatch[2];
}

//===========================================================================
BOOL JoyUsingMouse ()
{
	return (joyinfo[joytype].device == DEVICE_MOUSE);
}

// Soft-Switches
//===========================================================================
BYTE __stdcall JoyReadButton (WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	BOOL pressed = 0;

	if (joyinfo[joytype].device == DEVICE_JOYSTICK)
		CheckJoystick();
	switch (address)
	{
		case 0x61:
			pressed = (buttonlatch[0] || joybutton[0] || setbutton[0] || keydown[9]);
			buttonlatch[0] = 0;
		break;

		case 0x62:
			pressed = (buttonlatch[1] || joybutton[1] || setbutton[1] || keydown[10]);
			buttonlatch[1] = 0;
		break;

		case 0x63:
			pressed = (buttonlatch[2] || joybutton[2] || !(GetKeyState(VK_SHIFT) < 0));
			buttonlatch[2] = 0;
		break;
	}
	return MemReturnRandomData(pressed);
}

//===========================================================================

// PREAD:		; $FB1E
//  AD 70 C0 : (4) LDA $C070
//  A0 00    : (2) LDA #$00
//  EA       : (2) NOP
//  EA       : (2) NOP
// Lbl1:						; 11 cycles is the normal duration of the loop
//  BD 64 C0 : (4) LDA $C064,X
//  10 04    : (2) BPL Lbl2		; NB. 3 cycles if branch taken (not likely)
//  C8       : (2) INY
//  D0 F8    : (3) BNE Lbl1		; NB. 2 cycles if branck not taken (not likely)
//  88       : (2) DEY
// Lbl2:
//  60       : (6) RTS
//

static const double PDL_CNTR_INTERVAL = 2816.0 / 255.0;	// 11.04 (From KEGS)
//===========================================================================
BYTE __stdcall JoyReadPosition (WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	static ULONG g_nJoyCyclesExecuted = 0;
	ULONG  nPdlPos;
	BOOL   nPdlCntrActive;

	nPdlPos = (address & 1) ? ypos : xpos;

	// This is from KEGS. It helps games like Championship Lode Runner & Boulderdash
	if(nPdlPos >= 255)
		nPdlPos = 280;

	nPdlCntrActive = g_nCumulativeCycles <= (g_nJoyCntrResetCycle + (unsigned __int64) ((double)nPdlPos * PDL_CNTR_INTERVAL));

	return MemReturnRandomData(nPdlCntrActive);

}

//===========================================================================
BYTE __stdcall JoyResetPosition (WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	g_nJoyCntrResetCycle = g_nCumulativeCycles;

	if (joyinfo[joytype].device == DEVICE_JOYSTICK)
		CheckJoystick();

	return MemReturnRandomData(1);
}

// EOF