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

// Emula a CPU Z80

#include "stdhdr.h"
#include "tk3000e.h"
#include "memoria.h"
#include "cpu.h"
#include "z80emu.h"
#include "janela.h"

// Variaveis
int CPMZ80Slot	= 0;
int Z80_IRQ		= 0;
int CPMZ80Led	= 0;

// Funções Globais
//===========================================================================
int CPMZ80ConfiguraSlot(int Slot)
{
	int erro;
	iofunction ioarrayread[0x10];
	iofunction ioarraywrite[0x10];

	if (CPMZ80Slot)
	{
		FrameMostraMensagemErro("CP/M Z80 já está configurado em outro slot");
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

	erro = MemInsereSlot(Slot, ioarrayread, ioarraywrite, "NADA");
	if (erro == ERR_OK)
		CPMZ80Slot = Slot;
	return erro;
}

//===========================================================================
void CPMZ80RetiraSlot()
{
	CPMZ80Slot = 0;
}

//===========================================================================
void CPMZ80Verifica(WORD Endereco)
{
	if (CPMZ80Slot == 0) 
	{
		CPMZ80Led = 0;
		return;
	}
	if ((Endereco & 0xFF00) == (0xC000 + (CPMZ80Slot << 8)))
	{
		CPUAtual = (CPUAtual == CPU65C02) ? CPUZ80 : CPU65C02;
	}
	CPMZ80Led = (CPUAtual == CPUZ80) ? 1 : 0;
}

//===========================================================================
void CPMZ80StatusLed(int *Led)
{
	*Led = CPMZ80Led;
}

// EOF