
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

#ifndef JANELA_H
#define JANELA_H

//#include <windows.h>

// Defini��es Gerais
#define	 DRAW_BACKGROUND	1
#define	 DRAW_LEDS			2
#define	 DRAW_TITLE			4

// Vari�veis Externas
extern int	  fullscreen;
extern int	  framewindow;
extern bool   g_bScrollLock_FullSpeed;

// Prot�tipos
void FrameCreateWindow();
int  FrameGetDC();
int	 FrameGetVideoDC(unsigned char* *,long *);
void FrameRefreshStatus(int);
void FrameRegisterClass();
void FrameReleaseDC();
void FrameReleaseVideoDC();
void FrameMenuDiskete(int);
void FrameMenuHD(int);
void FrameMenuIDE(int);
void FrameMostraMensagemAdvertencia(char *);
void FrameMostraMensagemErro(char *);
int  FramePerguntaInteiro(char *, int);
void SetNormalMode ();
void SetFullScreenMode ();


#endif // JANELA_H
// EOF