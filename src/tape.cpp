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

// Emula um tape (ainda não implementado)

#include "stdhdr.h"
#include "tape.h"
#include "tk3000e.h"
#include "memoria.h"

// Definições
//#define DEBUGTAPE    /* Depurar atraves de um console */

#define CICLOMAXIMO  2000

#define CICLOSHEADER 655
#define CICLOSBIT1   500
#define CICLOSBIT0   250

// Variáveis
int   TapeNivel   = 0;
DWORD Ciclos      = 0;
DWORD UltimoCiclo = 0;
int   Header      = 0;
int   ContaBit    = 0;
BYTE  Byte        = 0;

// Protótipos

// Funções Internas

// Funções Globais

//===========================================================================
void TapeFinaliza()
{
#ifdef DEBUGTAPE
	FreeConsole();
#endif
}

//===========================================================================
void TapeInicializa()
{
	TapeNivel   = 0;
	Ciclos      = 0;
	UltimoCiclo = 0;
#ifdef DEBUGTAPE
	AllocConsole();
#endif
}

//===========================================================================
void TapeAtualiza(DWORD totalcycles)
{
	//
}

// Softswitches

//===========================================================================
BYTE __stdcall TapeLeitura(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	return MemReturnRandomData(1);
}

BYTE __stdcall TapeEscrita(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	return MemReturnRandomData(1);
}

// EOF