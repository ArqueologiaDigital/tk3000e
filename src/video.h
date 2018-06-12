
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

#ifndef VIDEO_H
#define VIDEO_H

// Defini��es
enum VIDEOTYPE
{
	VT_MONO=0,
	VT_COLOR_STANDARD,
	VT_COLOR_TEXT_OPTIMIZED,
	VT_COLOR_TVEMU,
	VT_NUM_MODES
};

// Vari�veis Externas
extern BOOL		behind;
extern DWORD	emulmsec;
extern BOOL		graphicsmode;	
extern DWORD	monochrome;
extern DWORD	videotype;
extern DWORD	vidmode;
extern BOOL		flash;
extern BOOL		behind;
extern DWORD	emulmsec;
extern DWORD	emulmsec_frac;
extern int		ScanLines;

// Prot�tipos
BOOL	VideoApparentlyDirty ();
void	VideoBenchmark ();
void	VideoCheckPage (BOOL);
void	VideoChooseColor ();
void	VideoDestroy ();
void	VideoDisplayLogo ();
BOOL	VideoHasRefreshed ();
void	VideoInitialize	();
void	VideoRealizePalette (HDC);
void	VideoRedrawScreen ();
void	VideoRefreshScreen ();
void	VideoReinitialize ();
void	VideoResetState	();
void	VideoUpdateVbl();
void	VideoUpdateFlash();
//BYTE	MixColors(BYTE c1, BYTE c2);
void	CreateColorMixMap();
//void __stdcall MixColorsVertical(int matx, int maty);


// Prot�tipos Softswitches
BYTE __stdcall VideoCheckMode (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall VideoCheckVbl (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall VideoSetMode (WORD,BYTE,BYTE,BYTE);

#endif
// EOF