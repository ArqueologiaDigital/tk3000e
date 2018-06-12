
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

#ifndef TK3000E_H
#define TK3000E_H

// Definição do	Modo do	Processador
#define	 MODE_LOGO		0
#define	 MODE_PAUSED	1
#define	 MODE_RUNNING	2
#define	 MODE_DEBUG		3
#define	 MODE_STEPPING	4

// Versão, Titulo e Arq de Rom
#define	VERSIONSTRING "2.4"
#define	TITULO        "Emulador TK3000//e"
#define	NOMEARQROM    "TK3000e.rom"
#define	NOMEARQSYM    "TK3000e.sym"
#define	NOMEARQBMP    "TK3000e.bmp"

// Definições Gerais
#define	 SPEED_NORMAL	10
#define	 SPEED_MAX		40
#define  ERR_OK          0  // Sem erros
#define  ERR_ERRO        1  // Erro geral
#define  ERR_EMUSO		 2  // Erro já está em uso

#define  SPKR_SAMPLE_RATE 44100

#define CLK_6502 (14.31818e6 / 14.0)

#define	 MAX(a,b)	(((a) > (b))	? (a) :	(b))
#define	 MIN(a,b)	(((a) < (b))	? (a) :	(b))

typedef	struct _IMAGE__	{
	int unused;
} *HIMAGE;

// Variáveis Externas
extern DWORD  cumulativecycles;
extern DWORD  cyclenum;
extern DWORD  extbench;
extern int    fullspeed;
extern DWORD  image;
extern int    instance;
extern DWORD  lastimage;
extern BYTE   slotext;
extern int    mode;	
extern char   progdir[MAX_PATH];
extern char   tempdir[MAX_PATH];
extern int    resettiming;
extern int    restart;
extern DWORD  speed;
extern double g_fCurrentCLK6502;
extern int    g_nCpuCyclesFeedback;
extern unsigned __int64 g_nCumulativeCycles;
extern double g_fMHz;
extern FILE*  g_fh;

// Prototipos
void Finalizar();
void SaveSlots();
void SetCurrentCLK6502();

#endif
// EOF