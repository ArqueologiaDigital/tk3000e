
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

#ifndef DEBUG_H
#define DEBUG_H

// Prot�tipos
void DebugBegin ();
void DebugContinueStepping ();
void DebugDestroy ();
void DebugDisplay (BOOL);
void DebugEnd ();
void DebugInitialize	();
void DebugProcessChar (TCHAR);
void DebugProcessCommand (int);

#endif
// EOF