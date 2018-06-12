
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

#ifndef TECLADO_H
#define TECLADO_H

// Vari�veis Externas
//extern BOOL		modekey;
extern int ColarRapido;

// Prot�tipos
void	KeybGetCapsStatus (BOOL	*);
void	KeybGetModeStatus (BOOL *);
BYTE	KeybGetKeycode ();
//DWORD	KeybGetNumQueries ();
void	KeybQueueKeypress (int,BOOL);
void	KeybAjustaCapsLock();
void	KeybToggleCapsLock();
void	KeybToggleModeKey();
bool    KeybIsPasting();

// Prot�tipos Softswitches
BYTE __stdcall KeybReadData (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall KeybReadFlag (WORD,BYTE,BYTE,BYTE);

#endif
// EOF