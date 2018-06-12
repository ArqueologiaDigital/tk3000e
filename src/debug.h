
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

#ifndef DEBUG_H
#define DEBUG_H

// Protótipos
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