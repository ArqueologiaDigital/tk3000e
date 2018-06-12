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

// Emula uma TKCLOCK ( Incompleto )

#include "stdhdr.h"
#include "tk3000e.h"
#include "tkclock.h"
#include "memoria.h"
#include "janela.h"

// Definições
#define DEZENA(a)  ((int)(a / 10))
#define UNIDADE(a) ((int)(a % 10))

// Variáveis
BYTE TKClockSlot;
BYTE modo1  = 0;
BYTE modo2  = 0;
BYTE portaA = 0;
BYTE portaB = 0;
BYTE registros[16];
BYTE numreg = 0;

// Protótipos
void LerRelogio();

// Funções Internas
//===========================================================================
void LerRelogio()
{
	SYSTEMTIME horas;

	GetLocalTime(&horas);
	registros[0x00] = UNIDADE(horas.wSecond);
	registros[0x01] = DEZENA(horas.wSecond);
	registros[0x02] = UNIDADE(horas.wMinute);
	registros[0x03] = DEZENA(horas.wMinute);
	registros[0x04] = UNIDADE(horas.wHour);
	registros[0x05] = DEZENA(horas.wHour);
	registros[0x06] = UNIDADE(horas.wDayOfWeek);
	registros[0x07] = UNIDADE(horas.wDay);
	registros[0x08] = DEZENA(horas.wDay);
	registros[0x09] = UNIDADE(horas.wMonth);
	registros[0x0A] = DEZENA(horas.wMonth);
	registros[0x0B] = UNIDADE((horas.wYear % 100));
	registros[0x0C] = DEZENA((horas.wYear % 100));
}

// Funções Globais
//===========================================================================
int TKClockConfiguraSlot(int Slot)
{
	int erro;
	iofunction ioarrayread[0x10];
	iofunction ioarraywrite[0x10];

	if (TKClockSlot)
	{
		FrameMostraMensagemErro("TK CLOCK já está configurado em outro slot");
		return ERR_EMUSO;
	}
	ioarrayread[0x00] = TKClock1;
	ioarrayread[0x01] = TKClock2;
	ioarrayread[0x02] = TKClock3;
	ioarrayread[0x03] = TKClock4;
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

	ioarraywrite[0x00] = TKClock1;
	ioarraywrite[0x01] = TKClock2;
	ioarraywrite[0x02] = TKClock3;
	ioarraywrite[0x03] = TKClock4;
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

	erro = MemInsereSlot(Slot, ioarrayread, ioarraywrite, "TKClock");
	if (erro == ERR_OK)
		TKClockSlot = Slot;
	return erro;
}

//===========================================================================
void TKClockRetiraSlot()
{
	TKClockSlot = 0;
}

//===========================================================================
void TKClockAtualiza(DWORD ciclos)
{
	// TODO: Implementar geração de interrupção
}

// Softswitches
//===========================================================================
BYTE __stdcall TKClock1(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	LerRelogio();
	if (write)
	{
		if (modo1 & 4)
		{
			portaA = value;
		}
	}
	else
	{
		if (modo1 & 4)
		{
			portaA = registros[numreg] & 0x0F;
			return portaA;
		}
	}
	return 0;
}

//===========================================================================
BYTE __stdcall TKClock2(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write)
	{
		modo1 = value;
	}
	return modo1;
}

//===========================================================================
BYTE __stdcall TKClock3(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write)
	{
		if (modo1 & 4)
		{
			portaB = value;
			numreg = (portaB & 0x0F);
		}
	}
	else
	{
		if (modo1 & 4)
			return portaB;
	}
	return 0;
}

//===========================================================================
BYTE __stdcall TKClock4(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write)
	{
		modo2 = value;
	}
	return modo2;
}

// EOF