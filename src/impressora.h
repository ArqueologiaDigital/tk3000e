
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

#ifndef IMPRESSORA_H
#define IMPRESSORA_H

// Vari�veis Externas
extern DWORD ImpressoraPorta;

// Prot�tipos
int  ImpressoraConfiguraSlot(int);
void ImpressoraRetiraSlot();
void ImpressoraInicializa();
void ImpressoraFinaliza();
void ImpressoraAtualiza(DWORD);
void ImpressoraDefinePorta(int);
void ImpressoraReset();
void ImpressoraMudaAcentuado();

// Prot�tipos Softswitches
BYTE __stdcall ImpressoraIO(WORD,BYTE,BYTE,BYTE);	

#endif
// EOF