
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

#ifndef TAPE_H
#define TAPE_H

// Prot�tipos
void TapeFinaliza();
void TapeInicializa();
void TapeAtualiza(DWORD);

// Prot�tipos Softswitches
BYTE __stdcall TapeLeitura(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall TapeEscrita(WORD,BYTE,BYTE,BYTE);

#endif
// EOF