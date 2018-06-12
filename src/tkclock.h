
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

#ifndef TKCLOCK_H
#define TKCLOCK_H

// Prototipos
int  TKClockConfiguraSlot(int);
void TKClockRetiraSlot();
void TKClockAtualiza(DWORD ciclos);

// Prot�tipos Softswitches
BYTE __stdcall TKClock1(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall TKClock2(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall TKClock3(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall TKClock4(WORD,BYTE,BYTE,BYTE);

#endif
// EOF