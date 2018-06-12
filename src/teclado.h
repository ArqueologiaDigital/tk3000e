
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

#ifndef TECLADO_H
#define TECLADO_H

// Variáveis Externas
//extern BOOL		modekey;
extern int ColarRapido;

// Protótipos
void	KeybGetCapsStatus (BOOL	*);
void	KeybGetModeStatus (BOOL *);
BYTE	KeybGetKeycode ();
//DWORD	KeybGetNumQueries ();
void	KeybQueueKeypress (int,BOOL);
void	KeybAjustaCapsLock();
void	KeybToggleCapsLock();
void	KeybToggleModeKey();
bool    KeybIsPasting();

// Protótipos Softswitches
BYTE __stdcall KeybReadData (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall KeybReadFlag (WORD,BYTE,BYTE,BYTE);

#endif
// EOF