
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

#ifndef MEMORIA_H
#define MEMORIA_H

// Definições Gerais
#define MAXEXPAGES	   128
//#define MAXIMAGES	   16
BYTE __stdcall NullIo (WORD programcounter, BYTE address, BYTE write, BYTE value);

// Definições de Tipos
typedef	BYTE (__stdcall	*iofunction)(WORD,BYTE,BYTE,BYTE);

// Variáveis Externas
extern iofunction ioread[0x100];
extern iofunction iowrite[0x100];
//extern BYTE* mem;
extern BYTE* memaux;
extern BYTE* memdirty;
extern BYTE* memmain;
//extern BYTE* memshadow[MAXIMAGES][0x100];
//extern BYTE* memwrite[MAXIMAGES][0x100];
extern BYTE* memread[0x100];
extern BYTE* memwrite[0x100];
extern DWORD pages;
extern int   numexpages;
extern BYTE* RWpages[MAXEXPAGES];	// pointers to RW memory banks
extern BYTE  Slots[7];
extern BYTE  NewSlots[7];

// Protótipos
int   MemAtualizaSlots(void);
int   MemInsereSlot(int, iofunction[0x10], iofunction[0x10],char *);
int   MemRetiraSlot(int);
void  MemRetiraTodosSlots();
BYTE  mem_readb(WORD, BOOL);
void  mem_writeb(WORD, BYTE);
WORD  mem_readw(WORD, BOOL);
void  mem_writew(WORD, WORD);
void  MemDestroy();
BYTE* MemGetAuxPtr(WORD);
BYTE* MemGetMainPtr(WORD);
void  MemInitialize();
void  MemReset();
void  MemResetPaging();
BYTE  MemReturnRandomData(BYTE);
void  MemSetFastPaging(BOOL);
void  MemAtualizaSlotExt(WORD	endereco);
void  MemTrimImages();
BOOL  MemImportar(WORD);
BOOL  MemExportar(WORD,WORD);
BOOL  MemImportarB();
void  MemSalvarEstado();
void  MemCarregarEstado();

// Protótipos Softswitches
BYTE __stdcall MemCheckPaging (WORD,BYTE,BYTE,BYTE);
BYTE __stdcall MemSetPaging (WORD,BYTE,BYTE,BYTE);

#endif
// EOF