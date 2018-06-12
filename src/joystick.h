
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

#ifndef JOYSTICK_H
#define JOYSTICK_H

// Vari�veis Externas
extern DWORD joytype;

// Prot�tipos
void JoyInitialize ();
void JoyDestroy();
BOOL JoyProcessKey (int,BOOL,BOOL,BOOL);
void JoyReset ();
void JoySetButton (int,BOOL);
BOOL JoySetEmulationType (HWND,DWORD);
void JoySetPosition (int,int,int,int);
void JoyUpdatePosition (DWORD);
BOOL JoyUsingMouse ();

// Prot�tipos Softswitches
BYTE __stdcall JoyReadButton (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall JoyReadPosition (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall JoyResetPosition	(WORD,BYTE,BYTE,BYTE);

#endif
// EOF