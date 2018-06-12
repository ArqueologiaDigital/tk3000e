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

// Emula o mouse

#include "stdhdr.h"
#include "mouse.h"
#include "tk3000e.h"
#include "cpu.h"
#include "memoria.h"
#include "janela.h"

// Definições
#define	MOUSE_LIGADO		0x01
#define	MOUSE_INT_MOVER		0x02
#define	MOUSE_INT_BOTAO		0x04
#define	MOUSE_INT_VERTICAL	0x08
#define	MOUSE_XY_MUDADO		0x20
#define	MOUSE_BOTAO_JA_CLIC	0x40
#define	MOUSE_BOTAO_CLICADO	0x80
#define	X_LOW				0x478
#define	Y_LOW				0x4F8
#define	X_HIGH				0x578
#define	Y_HIGH				0x5F8
#define RESERVADO			0x678
#define TEXTLENGTH			0x6F8
#define	STATUS_MOUSE		0x778
#define	MODO_MOUSE			0x7F8

#define MINIMO_X			0
#define MINIMO_Y			0
#define MAXIMO_X			1023
#define MAXIMO_Y			1023

// Variáveis
BOOL MouseUsando = 0;
BYTE MouseSlot;
BYTE ModoMouse;
BYTE StatusMouse, StatusInt;
WORD PosX;
WORD PosY;
WORD MousePosX;
WORD MousePosY;
WORD MousePosXAtual;
WORD MousePosYAtual;
BYTE MouseBotao;
short int LimiteSupX;
short int LimiteSupY;
short int LimiteInfX;
short int LimiteInfY;

// Protótipos
void ConvertePosMouse(void);

// Funções Internas
//===========================================================================
void ConvertePosMouse(void)
{
#define VIDEOWIDTH    560
#define VIDEOHEIGHT   384
	if (MousePosX > VIDEOWIDTH)
		MousePosX = VIDEOWIDTH;
	if (MousePosY > VIDEOHEIGHT)
		MousePosY = VIDEOHEIGHT;
	MousePosXAtual = (((LimiteInfX - LimiteSupX + 1) * MousePosX) / VIDEOWIDTH) + LimiteSupX;
	MousePosYAtual = (((LimiteInfY - LimiteSupY + 1) * MousePosY) / VIDEOHEIGHT) + LimiteSupY;
	if (MousePosXAtual < LimiteSupX)
		MousePosXAtual = LimiteSupX;
	if (MousePosYAtual < LimiteSupY)
		MousePosYAtual = LimiteSupY;
	if (MousePosXAtual > LimiteInfX)
		MousePosXAtual = LimiteInfX;
	if (MousePosYAtual > LimiteInfY)
		MousePosYAtual = LimiteInfY;

	if (PosX < LimiteSupX)
		PosX = LimiteSupX;
	if (PosY < LimiteSupY)
		PosY = LimiteSupY;
	if (PosX > LimiteInfX)
		PosX = LimiteInfX;
	if (PosY > LimiteInfY)
		PosY = LimiteInfY;
}

// Funções Globais
//===========================================================================
void MouseReset()
{
	if (MouseSlot)
	{
		ModoMouse   = 0;
		StatusMouse = 0;
		StatusInt   = 0;
		LimiteSupX  = MINIMO_X;
		LimiteSupY  = MINIMO_Y;
		LimiteInfX  = MAXIMO_X;
		LimiteInfY  = MAXIMO_Y;
		PosX        = LimiteSupX;
		PosY        = LimiteSupY;
	}
}

//===========================================================================
int MouseConfiguraSlot(int SLOT)
{
	int erro;
	iofunction ioarrayread[0x10];
	iofunction ioarraywrite[0x10];

	if (MouseSlot)
	{
		FrameMostraMensagemErro("Mouse já está configurado em outro slot");
		return ERR_EMUSO;
	}
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

	ioarraywrite[0x00] = MouseModo;
	ioarraywrite[0x01] = MouseClamp;
	ioarraywrite[0x02] = MouseComando;
	ioarraywrite[0x03] = MouseByteComando;
	ioarraywrite[0x04] = MouseObtemTexto;
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

	erro = MemInsereSlot(SLOT, ioarrayread, ioarraywrite, "Mouse");
	if (erro == ERR_OK)
		MouseSlot = SLOT;
	return erro;
}

//===========================================================================
void MouseRetiraSlot()
{
	MouseSlot = 0;
}

// Nota: Interrupções VBL podem acontecer sem o mouse estar
//       ligado.
//===========================================================================
void MouseVbl(void)
{

	if (StatusInt) return;

	if ((ModoMouse & MOUSE_INT_VERTICAL))
	{
		StatusInt |= MOUSE_INT_VERTICAL;
	}
	if (ModoMouse & MOUSE_LIGADO)
	{
		if ( (ModoMouse & MOUSE_INT_MOVER))
		{
			ConvertePosMouse();
			if ((PosX != MousePosXAtual) || (PosY != MousePosYAtual))
				StatusInt |= MOUSE_INT_MOVER;
		}

		if ( (ModoMouse & MOUSE_INT_BOTAO))
		{
			if ((ModoMouse & MOUSE_INT_BOTAO) && MouseBotao)
				StatusInt |= MOUSE_INT_BOTAO;
		}
	} // MOUSE_LIGADO

	// Se houve alguma interrupção, chamar CpuIRQ()
	if (StatusInt)
	{
		CpuIRQ();
	}
}

// Softswitches

//===========================================================================
BYTE __stdcall MouseModo(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write)
	{
		ModoMouse = value;
		mem_writeb(MODO_MOUSE + MouseSlot, value);
		if (ModoMouse & MOUSE_LIGADO)
		{
			StatusMouse |= MOUSE_LIGADO;
			MouseUsando = 1;
		}
		else
		{
			StatusMouse &= ~MOUSE_LIGADO;
			MouseUsando = 0;
		}
	}
	return 0;
}

//===========================================================================
BYTE __stdcall MouseClamp(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	short int Superior, Inferior;

	if (write)
	{
		Superior = (mem_readb(X_HIGH, 0)<<8) + mem_readb(X_LOW, 0);
		Inferior = (mem_readb(Y_HIGH, 0)<<8) + mem_readb(Y_LOW, 0);

		switch (value)
		{
			case 0:
				LimiteSupX = Superior;
				LimiteInfX = Inferior;
			break;

			case 1:
				LimiteSupY = Superior;
				LimiteInfY = Inferior;
			break;
		}
	}
	return 0;
}

//===========================================================================
BYTE __stdcall MouseComando(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	int StatusAnt;

	if (write)
	{
		switch (value & 0x7F)
		{
			case 1: // CMDSERVEMOUSE
				StatusMouse |= StatusInt;
				mem_writeb(STATUS_MOUSE + MouseSlot, StatusMouse);
				StatusInt = 0;
			break;

			case 2: // CMDREADMOUSE
				ConvertePosMouse();
				StatusAnt = StatusMouse;
				StatusMouse &= ~(MOUSE_XY_MUDADO | MOUSE_BOTAO_CLICADO | MOUSE_BOTAO_JA_CLIC);
				if (PosX != MousePosXAtual || PosY != MousePosYAtual)
					StatusMouse |= MOUSE_XY_MUDADO;
				PosX = MousePosXAtual;
				PosY = MousePosYAtual;
// TODO: Verificar MOUSE_BOTAO_JA_CLIC
				if (StatusAnt & MOUSE_BOTAO_CLICADO)
					StatusMouse |= MOUSE_BOTAO_JA_CLIC;
				if (MouseBotao)
					StatusMouse |= MOUSE_BOTAO_CLICADO;
				StatusMouse &= ~(MOUSE_INT_MOVER | MOUSE_INT_BOTAO | MOUSE_INT_VERTICAL);
				mem_writeb(STATUS_MOUSE + MouseSlot, StatusMouse);
				mem_writeb(X_LOW + MouseSlot, (byte)PosX);
				mem_writeb(X_HIGH + MouseSlot, (byte)(PosX >> 8));
				mem_writeb(Y_LOW + MouseSlot, (byte)PosY);
				mem_writeb(Y_HIGH + MouseSlot, (byte)(PosY >> 8));
			break;

			case 3: // CMDCLEARMOUSE
				StatusMouse &= ~(MOUSE_INT_MOVER | MOUSE_INT_BOTAO | MOUSE_INT_VERTICAL
								| MOUSE_XY_MUDADO | MOUSE_BOTAO_CLICADO | MOUSE_BOTAO_JA_CLIC);
				mem_writeb(STATUS_MOUSE + MouseSlot, StatusMouse);
				mem_writeb(X_LOW + MouseSlot, (byte)PosX);
				mem_writeb(X_HIGH + MouseSlot, (byte)(PosX >> 8));
				mem_writeb(Y_LOW + MouseSlot, (byte)PosY);
				mem_writeb(Y_HIGH + MouseSlot, (byte)(PosY >> 8));
			break;

			case 4: // CMDPOSMOUSE
				PosX = mem_readb(X_LOW + MouseSlot, 0) + (mem_readb(X_HIGH + MouseSlot, 0) << 8);
				PosY = mem_readb(Y_LOW + MouseSlot, 0) + (mem_readb(Y_HIGH + MouseSlot, 0) << 8);
				ConvertePosMouse();
				mem_writeb(X_LOW  + MouseSlot, (byte)PosX);
				mem_writeb(X_HIGH + MouseSlot, (byte)(PosX >> 8));
				mem_writeb(Y_LOW  + MouseSlot, (byte)PosY);
				mem_writeb(Y_HIGH + MouseSlot, (byte)(PosY >> 8));
			break;

			case 5: // CMDHOMEMOUSE
				PosX = LimiteSupX;
				PosY = LimiteSupY;
				mem_writeb(X_LOW  + MouseSlot, (byte)PosX);
				mem_writeb(X_HIGH + MouseSlot, (byte)(PosX >> 8));
				mem_writeb(Y_LOW  + MouseSlot, (byte)PosY);
				mem_writeb(Y_HIGH + MouseSlot, (byte)(PosY >> 8));
			break;

			case 6: // CMDINITMOUSE
				ModoMouse   = 0;
				StatusMouse = 0;
				StatusInt   = 0;
				LimiteSupX  = MINIMO_X;
				LimiteSupY  = MINIMO_Y;
				LimiteInfX  = MAXIMO_X;
				LimiteInfY  = MAXIMO_Y;
				PosX        = LimiteSupX;
				PosY        = LimiteSupY;
				mem_writeb(STATUS_MOUSE + MouseSlot, StatusMouse);
				mem_writeb(MODO_MOUSE   + MouseSlot, ModoMouse);
				mem_writeb(X_LOW        + MouseSlot, (byte)PosX);
				mem_writeb(X_HIGH       + MouseSlot, (byte)(PosX >> 8));
				mem_writeb(Y_LOW        + MouseSlot, (byte)PosY);
				mem_writeb(Y_HIGH       + MouseSlot, (byte)(PosY >> 8));
			break;
		}
	}
	return 0;
}

//===========================================================================
BYTE __stdcall MouseByteComando(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
//	value &= 0x7F;

//	if (value == 1)
//	{
//		MouseModo(0, 0, 1, 1);
//	}
	return 0;
}

//===========================================================================
BYTE __stdcall MouseObtemTexto(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	char   String[255];
	BYTE   Status2;
	int    StatusAnt;
	unsigned int len, i;

	ConvertePosMouse();
	StatusAnt = StatusMouse;
	StatusMouse &= ~(MOUSE_XY_MUDADO | MOUSE_BOTAO_CLICADO | MOUSE_BOTAO_JA_CLIC);
	if (PosX != MousePosXAtual || PosY != MousePosYAtual)
		StatusMouse |= MOUSE_XY_MUDADO;
	PosX = MousePosXAtual;
	PosY = MousePosYAtual;
	if (StatusAnt & MOUSE_BOTAO_CLICADO)
		StatusMouse |= MOUSE_BOTAO_JA_CLIC;
	if (MouseBotao)
		StatusMouse |= MOUSE_BOTAO_CLICADO;
	StatusMouse &= ~(MOUSE_INT_MOVER | MOUSE_INT_BOTAO | MOUSE_INT_VERTICAL);
	Status2 =	(StatusMouse & MOUSE_BOTAO_CLICADO ? 0x00 : 0x04) |
				(StatusMouse & MOUSE_BOTAO_JA_CLIC ? 0x02 : 0x00) |
				(StatusMouse & MOUSE_BOTAO_CLICADO ? 0x01 : 0x00);

	sprintf(String, "%d,%d,%d\x0D", PosX, PosY, Status2);
	len = strlen(String);
	for (i=0; i < len; i++)
	{
		mem_writeb(0x0200+i, String[i] | 0x80);
	}
	mem_writeb(TEXTLENGTH + MouseSlot, len-1);
	return 0;
}

// EOF