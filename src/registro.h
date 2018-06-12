
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

#ifndef REGISTRO_H
#define REGISTRO_H

// Definições do registro
#define POSX           "Posicao X Janela"
#define POSY           "Posicao Y Janela"
#define PREFERENCIAS   "Preferencias"
#define DIRINIC        "Diretorio Inicial"
#define VERSAO         "Versao"
#define CONFIG         "Configuracao"
#define RUNOS          "Sistema Operacional"
#define EMUVIDEO       "Emulacao Video"
#define EMUVIDEOM      "Video Monocromatico"
#define EMUJOY         "Emulacao Joystick"
#define EMUSOM         "Emulacao Som"
#define EMUSOMVOL      "Volume Som"
#define EMUSERIAL      "Porta Serial"
#define EMUVEL         "Velocidade"
#define EMUVELCPU      "Velocidade Emulacao"
#define EMUDISCO       "Velocidade Disco Rapida"
#define EMUBANCOS      "Bancos TKWorks"
#define EMUIMPRESSORA  "Impressora"
#define EMUDISCORIG    "Disco Rigido"
#define EMUSCANLINES   "Scan Lines" // Scan Lines
#define EMUCOLARRAPIDO "Colar Rapido"
#define ULTIMPEND      "Ultimo End Importacao"
#define ULTEXPENDI     "Ultimo End Inic Exportacao"
#define ULTEXPENDF     "Ultimo End Final Exportacao"
#define SLOT1	  	   "Slot 1"
#define SLOT2		   "Slot 2"
#define SLOT3		   "Slot 3"
#define SLOT4		   "Slot 4"
#define SLOT5		   "Slot 5"
#define SLOT6		   "Slot 6"
#define SLOT7		   "Slot 7"

// Protótipos
int  RegLoadString (char*, char*, int, char*, DWORD);
int  RegLoadValue  (char*, char*, int, DWORD *);
void RegSaveString (char*, char*, int, char*);
void RegSaveValue  (char*, char*, int, DWORD);

#endif
// EOF