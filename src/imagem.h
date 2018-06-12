
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

#ifndef IMAGEM_H
#define IMAGEM_H

#include "TK3000e.h"

// Defini��es Gerais
#define	 TRACKS		35
#define	 VOLUME		254

typedef	struct _imageinforec
{
	char  filename[MAX_PATH];
	BYTE  Volume;				// Volume support
	DWORD format;
//	FILE  *file;
	DWORD offset;
	int	  writeprotected;
	DWORD headersize;
	BYTE* header;
	int	  validtrack[TRACKS];
	BYTE  *imagem;
	int   tamanho;
	int   Mudado;				// Flag file changed
} imageinforec,	*imageinfoptr;

// Prot�tipos
int	 ImageBoot(HIMAGE);
void ImageClose(HIMAGE);
void ImageDestroy();
void ImageInitialize();
int	 ImageOpen(char*, BYTE *, HIMAGE *, int *, int);
void ImageReadTrack(HIMAGE, int, int, BYTE*, int *);
void ImageWriteTrack(HIMAGE, int, int, BYTE*, int);

#endif
// EOF