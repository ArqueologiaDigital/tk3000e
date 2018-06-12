
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

#ifndef IDE_H
#define IDE_H

// Protótipos
int  IDEConfiguraSlot(int);
void IDERetiraSlot();
void IDEInicializa();
void IDEFinaliza();
void IDEEscolheImagem();
int	 IDEFechaImagem();
int	 IDEAbreImagem(char *);
void IDEAtualiza(DWORD);
char *IDENomeImagem();
void IDEStatusLed(int *);

// Protótipos Softswitches
BYTE __stdcall IDEATADataHigh(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall IDESetCSMask(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall IDEClearCSMask(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall IDEATAAltStatus(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall IDEATADataLow(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall IDEATAError(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall IDEATASectorCnt(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall IDEATASector(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall IDEATACylinder(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall IDEATACylinderH(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall IDEATAHead(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall IDEATAStatus(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall IDEATADevCtrl(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall IDEATACommand(WORD programcounter, BYTE address, BYTE write, BYTE value);

#endif

// EOF