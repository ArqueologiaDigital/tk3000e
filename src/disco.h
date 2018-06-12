
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

#ifndef DISCO_H
#define DISCO_H

// Vari�veis Externas
extern BOOL	  enhancedisk;

// Prot�tipos
int  DiscoConfiguraSlot(int);
void DiscoRetiraSlot();
void DiskBoot ();
void DiskDestroy ();	
char *DiskGetFullName	(int);
void DiskGetLightStatus (int	*,int *);
char *DiskGetName (int);
void DiskInitialize ();
int	 DiskInsert (int, char*, int, int);
void DiskRemove (int);
void DiskEscolheVolume(int drive);
int	 DiskIsSpinning ();
void DiskNotifyInvalidImage (char* ,int);
void DiskReset ();
void DiskSelect (int);
void DiskUpdatePosition (DWORD);

// Prot�tipos Softswitches
BYTE __stdcall DiskControlMotor	(WORD,BYTE,BYTE,BYTE);
BYTE __stdcall DiskControlStepper (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall DiskEnable (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall DiskReadWrite (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall DiskSetLatchValue (WORD,BYTE,BYTE,BYTE);	
BYTE __stdcall DiskSetReadMode (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall DiskSetWriteMode	(WORD,BYTE,BYTE,BYTE);

#endif
// EOF