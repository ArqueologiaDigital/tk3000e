
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

#ifndef SERIAL_H
#define SERIAL_H

// Variáveis Externas
extern DWORD serialport;

// Protótipos
int  CommConfiguraSlot(int);
void CommRetiraSlot();
void CommDestroy ();	
void CommReset ();
void CommSetSerialPort (HWND,DWORD);	
void CommUpdate (DWORD);

// Protótipos Softswitches
BYTE __stdcall CommCommand  (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall CommControl  (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall CommDipSw    (WORD,BYTE,BYTE,BYTE);	
BYTE __stdcall CommReceive  (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall CommStatus   (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall CommTransmit (WORD,BYTE,BYTE,BYTE);

#endif
// EOF