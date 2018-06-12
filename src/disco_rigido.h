
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

#ifndef DISCORIGIDO_H
#define DISCORIGIDO_H

// Variáveis Externas
extern int HDTamanho;

// Protótipos
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

// Protótipos Softswitches
BYTE __stdcall HDIOComando(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall HDIOBloco(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall HDIODado(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall HDIOStatus(WORD,BYTE,BYTE,BYTE);	
BYTE __stdcall HDIOUnidade(WORD,BYTE,BYTE,BYTE);

#endif
// EOF