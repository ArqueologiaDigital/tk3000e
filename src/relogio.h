
/*  Emulador do computador TK3000 //e (Microdigital)
 *  por F�bio Belavenuto - Copyright (C) 2004
 *
 *  Adaptado do emulador Applewin por Michael O'Brien
 *  Part of code is Copyright (C) 2003-2004 Tom Charlesworth
 *
 *  Este arquivo � distribuido pela Licen�a P�blica Geral GNU.
 *  Veja o arquivo Licenca.txt distribuido com este software.
 *
 *  ESTE SOFTWARE N�O OFERECE NENHUMA GARANTIA
 *
 */

#ifndef RELOGIO_H
#define RELOGIO_H

// Prot�tipos
int  RelogioConfiguraSlot(int);
void RelogioRetiraSlot();

// Prot�tipos Softswitches
BYTE __stdcall RelogioRead(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall RelogioWrite(WORD,BYTE,BYTE,BYTE);

#endif
// EOF