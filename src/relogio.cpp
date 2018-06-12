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

// Emula uma Realtime Clock

#include "stdhdr.h"
#include "tk3000e.h"
#include "relogio.h"
#include "memoria.h"
#include "janela.h"

// Definições
#define TOBCD(a) (((((a)/10) % 10) << 4) | ((a) % 10))

// Variáveis
BYTE	RelogioSlot;
BYTE	DadosRelogio[16] = 
						{
							0,0,0,0,
							0,0,0,0,
							0xBA,19,0,0,
							0,0,0,0
						};
/* O significado dos valores da variável DadosRelogio segue abaixo:
 * 
 * Todos estão em formato BCD
 *
 * Índice:      Significado:
 * 0x00         Ano (Parte Baixa)
 * 0x01         Ano (Parte Alta)
 * 0x02         Mes
 * 0x03         Dia
 * 0x04         Dia da Semana (0=Domingo, 1=Segunda, etc)
 * 0x05         Hora
 * 0x06         Minuto
 * 0x07         Segundo
 * 0x08         Modo (ver abaixo)
 * 0x09         Comprimento da String
 * 0x0A         Posição do Caractere
 * 0x0B         Caractere
 * 
 * Os caracteres da String de formato são:
 * S   - Dia da semana (número)  (1 caractere)
 * SSS - Dia da semana (texto)   (3 caracteres)
 * dd  - Dia do mês              (2 caracteres)
 * mm  - Mês (número)            (2 caracteres)
 * mmm - Mês (texto)             (3 caracteres)
 * aa  - Ano                     (2 caracteres)
 * HH  - Hora (24 horas)         (2 caracteres)
 * hh  - Hora (12 horas)         (2 caracteres)
 * mi  - Minuto	                 (2 caracteres)
 * ss  - Segundos                (2 caracteres)
 * X   - A ou P (AM ou PM)       (1 caractere)
 * 
 * Modos de Trabalho:
 * 
 * $A3 = '#' : Formato ProDOS           "S,dd,mm,HH,mi,ss"        -> 16 carac
 * $BE = '>' : Formato Interger BASIC   "mm/dd HH;mi;ss.Saa"      -> 18 carac
 * $A0 = ' ' : Formato AppleClock       "mm/dd HH;mi;ss.Saa"      -> 18 carac
 * $A5 = '%' : Formato Applesoft BASIC  "SSS mmm dd hh:mi:ss XM"  -> 22 carac
 * $A6 = '&' : Formato Applesoft BASIC  "SSS mmm dd HH:mi:ss"     -> 19 carac
 * $BA = ':' : Formato TKCLOCK          "S mm/dd/aa HH:mi:ss"     -> 19 carac
 *  
 */

char NomeMeses[12][5] = 
{
	"JAN\0","FEV\0","MAR\0","ABR\0","MAI\0","JUN\0",
	"JUL\0","AGO\0","SET\0","OUT\0","NOV\0","DEZ\0"
};

char NomeDiaSemana[7][5] = 
{
	"DOM\0","SEG\0","TER\0","QUA\0","QUI\0","SEX\0","SAB\0"
};

// Funções Globais
//===========================================================================
int RelogioConfiguraSlot(int Slot)
{
	int erro;
	iofunction ioarrayread[0x10];
	iofunction ioarraywrite[0x10];

	if (RelogioSlot)
	{
		FrameMostraMensagemErro("AppleClock já está configurado em outro slot");
		return ERR_EMUSO;
	}
	ioarrayread[0x00] = RelogioRead;
	ioarrayread[0x01] = RelogioRead;
	ioarrayread[0x02] = RelogioRead;
	ioarrayread[0x03] = RelogioRead;
	ioarrayread[0x04] = RelogioRead;
	ioarrayread[0x05] = RelogioRead;
	ioarrayread[0x06] = RelogioRead;
	ioarrayread[0x07] = RelogioRead;
	ioarrayread[0x08] = RelogioRead;
	ioarrayread[0x09] = RelogioRead;
	ioarrayread[0x0A] = RelogioRead;
	ioarrayread[0x0B] = RelogioRead;
	ioarrayread[0x0C] = RelogioRead;
	ioarrayread[0x0D] = RelogioRead;
	ioarrayread[0x0E] = RelogioRead;
	ioarrayread[0x0F] = RelogioRead;

	ioarraywrite[0x00] = RelogioWrite;
	ioarraywrite[0x01] = RelogioWrite;
	ioarraywrite[0x02] = RelogioWrite;
	ioarraywrite[0x03] = RelogioWrite;
	ioarraywrite[0x04] = RelogioWrite;
	ioarraywrite[0x05] = RelogioWrite;
	ioarraywrite[0x06] = RelogioWrite;
	ioarraywrite[0x07] = RelogioWrite;
	ioarraywrite[0x08] = RelogioWrite;
	ioarraywrite[0x09] = RelogioWrite;
	ioarraywrite[0x0A] = RelogioWrite;
	ioarraywrite[0x0B] = RelogioWrite;
	ioarraywrite[0x0C] = RelogioWrite;
	ioarraywrite[0x0D] = RelogioWrite;
	ioarraywrite[0x0E] = RelogioWrite;
	ioarraywrite[0x0F] = RelogioWrite;

	erro = MemInsereSlot(Slot, ioarrayread, ioarraywrite, "Relogio");
	if (erro == ERR_OK)
		RelogioSlot = Slot;
	return erro;
}

//===========================================================================
void RelogioRetiraSlot()
{
	RelogioSlot = 0;
}

// Softswitches
//===========================================================================
BYTE __stdcall RelogioRead(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	SYSTEMTIME horas;
	char temp[256];

	GetLocalTime(&horas);
	DadosRelogio[0x00] = TOBCD(horas.wYear);
	DadosRelogio[0x01] = TOBCD(horas.wYear / 100);
	DadosRelogio[0x02] = TOBCD(horas.wMonth);
	DadosRelogio[0x03] = TOBCD(horas.wDay);
	DadosRelogio[0x04] = TOBCD(horas.wDayOfWeek);
	DadosRelogio[0x05] = TOBCD(horas.wHour);
	DadosRelogio[0x06] = TOBCD(horas.wMinute);
	DadosRelogio[0x07] = TOBCD(horas.wSecond);

	switch(DadosRelogio[0x08])
	{
		unsigned char c;
		
		case 0xA3: /* Formato ProDOS */
			sprintf(temp,
					"%02d,%02d,%02d,%02d,%02d,%02d",
				    horas.wMonth,
					horas.wDayOfWeek,
					horas.wDay,
					horas.wHour,
					horas.wMinute,
					horas.wSecond);
		break;

		case 0xBE:
		case 0xA0: /* Formato AppleClock */
			sprintf(temp,
					"%02d/%02d %02d;%02d;%02d.%01d%02d",
					horas.wMonth,
					horas.wDay,
					horas.wHour,
					horas.wMinute,
					horas.wSecond,
					horas.wDayOfWeek,
					horas.wYear % 100);
		break;

		case 0xA5: /* Formato Applesoft Basic 12 horas */
			if (horas.wHour == 0) horas.wHour = 24;
			if (horas.wHour > 12) {
				horas.wHour -= 12;
				c = 'P';
			} else {
				c = 'A';
			}
			sprintf(temp,
					"%s %s %02d %02d;%02d;%02d %cM",
					NomeDiaSemana[horas.wDayOfWeek],
					NomeMeses[horas.wMonth],
					horas.wDay,
					horas.wHour,
					horas.wMinute,
					horas.wSecond,
					c);
		break;

		case 0xA6: /* Formato Applesoft Basic 24 horas */
			sprintf(temp,
					"%s %s %02d %02d;%02d;%02d",
					NomeDiaSemana[horas.wDayOfWeek],
					NomeMeses[horas.wMonth],
					horas.wDay,
					horas.wHour,
					horas.wMinute,
					horas.wSecond);
		break;

		case 0xBA: /* Formato TKCLOCK */
			sprintf(temp,
					"%01d %02d/%02d/%02d %02d;%02d;%02d",
					horas.wDayOfWeek,
					horas.wMonth,
					horas.wDay,
					horas.wYear % 100,
					horas.wHour,
					horas.wMinute,
					horas.wSecond);
		break;
	}
    /* Seta Bit 7 */
    DadosRelogio[0x0B] = temp[DadosRelogio[0x0A]] |= 0x80;
    /* Retorna Valor */
    return DadosRelogio[address & 0x0F];
}

//===========================================================================
BYTE __stdcall RelogioWrite(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	switch(address & 0x0f)
	{
		case 0x08:
			/* Determina Modo */
			switch (value)
			{
				case 0xA0:
				case 0xA3:
				case 0xA5:
				case 0xA6:
				case 0xBA:
				case 0xBE:
					DadosRelogio[0x08] = value;
				break;
			}
			/* Determina Comprimento */
			switch (DadosRelogio[0x08])
			{
				case 0xA0:
				case 0xBE:
					DadosRelogio[0x09] = 18;
				break;

				case 0xA3:
					DadosRelogio[0x09] = 16;
				break;

				case 0xA5:
					DadosRelogio[0x09] = 22;
				break;

				case 0xA6:
				case 0xBA:
					DadosRelogio[0x09] = 19;
				break;

			}
        break;

		case 0x0A:
			/* Determina Posicão do Caractere */
			if ( value < DadosRelogio[0x09])
			{
				DadosRelogio[0x0A] = value;
			}
		break;
	}
	/* Atualiza dados do relógio */
	return RelogioRead(programcounter, address, 0, 0);
}

// EOF