
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

#ifndef MOUSE_H
#define MOUSE_H

// Variáveis Externas
extern BOOL MouseUsando;
extern WORD MousePosX;
extern WORD MousePosY;
extern BYTE MouseBotao;

// Protótipos
void MouseReset();
int  MouseConfiguraSlot(int);
void MouseRetiraSlot();
void MouseVbl();

// Protótipos Softswitches
BYTE __stdcall MouseModo(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall MouseClamp(WORD,BYTE,BYTE,BYTE);	
BYTE __stdcall MouseComando(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall MouseByteComando(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall MouseObtemTexto(WORD,BYTE,BYTE,BYTE);

#endif
// EOF