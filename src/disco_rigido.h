
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

#ifndef DISCORIGIDO_H
#define DISCORIGIDO_H

// Vari�veis Externas
extern int HDTamanho;

// Prot�tipos
int  HDConfiguraSlot(int);
void HDRetiraSlot();
void HDInicializa();
void HDFinaliza();
void HDEscolheImagem(int);
int	 HDFechaImagem(int);
int	 HDAbreImagem(char*, int);
void HDStatusLeds(int*, int*);
void HDAtualiza(DWORD);
char *HDNomeImagem(int);

// Prot�tipos Softswitches
BYTE __stdcall HDIOComando(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall HDIOBloco(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall HDIODado(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall HDIOStatus(WORD,BYTE,BYTE,BYTE);	
BYTE __stdcall HDIOUnidade(WORD,BYTE,BYTE,BYTE);

#endif
// EOF