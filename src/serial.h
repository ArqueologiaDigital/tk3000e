
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

#ifndef SERIAL_H
#define SERIAL_H

// Vari�veis Externas
extern DWORD serialport;

// Prot�tipos
int  CommConfiguraSlot(int);
void CommRetiraSlot();
void CommDestroy ();	
void CommReset ();
void CommSetSerialPort (HWND,DWORD);	
void CommUpdate (DWORD);

// Prot�tipos Softswitches
BYTE __stdcall CommCommand  (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall CommControl  (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall CommDipSw    (WORD,BYTE,BYTE,BYTE);	
BYTE __stdcall CommReceive  (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall CommStatus   (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall CommTransmit (WORD,BYTE,BYTE,BYTE);

#endif
// EOF