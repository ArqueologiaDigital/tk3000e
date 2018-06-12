
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

#ifndef JOYSTICK_H
#define JOYSTICK_H

// Variáveis Externas
extern DWORD joytype;

// Protótipos
void JoyInitialize ();
void JoyDestroy();
BOOL JoyProcessKey (int,BOOL,BOOL,BOOL);
void JoyReset ();
void JoySetButton (int,BOOL);
BOOL JoySetEmulationType (HWND,DWORD);
void JoySetPosition (int,int,int,int);
void JoyUpdatePosition (DWORD);
BOOL JoyUsingMouse ();

// Protótipos Softswitches
BYTE __stdcall JoyReadButton (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall JoyReadPosition (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall JoyResetPosition	(WORD,BYTE,BYTE,BYTE);

#endif
// EOF