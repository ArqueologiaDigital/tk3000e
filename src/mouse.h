
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

#ifndef MOUSE_H
#define MOUSE_H

// Vari�veis Externas
extern BOOL MouseUsando;
extern WORD MousePosX;
extern WORD MousePosY;
extern BYTE MouseBotao;

// Prot�tipos
void MouseReset();
int  MouseConfiguraSlot(int);
void MouseRetiraSlot();
void MouseVbl();

// Prot�tipos Softswitches
BYTE __stdcall MouseModo(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall MouseClamp(WORD,BYTE,BYTE,BYTE);	
BYTE __stdcall MouseComando(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall MouseByteComando(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall MouseObtemTexto(WORD,BYTE,BYTE,BYTE);

#endif
// EOF