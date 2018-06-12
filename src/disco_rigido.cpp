/****************************************************************************
*
*  APPLE //E EMULATOR FOR WINDOWS                    
*
*  Copyright (C) 1994-96, Michael O'Brien.  All rights reserved.
*
***/

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

// Emula um Disco Rígido ProDOS

#include "stdhdr.h"
#include "tk3000e.h"
#include "disco_rigido.h"
#include "janela.h"
#include "registro.h"
#include "memoria.h"

// Definições
#define PS_OK    0x00
#define PS_IOERR 0x27
#define PS_NODEV 0x28
#define PS_WRPRO 0x2B

#define NUMDEVS	2

// Variáveis
BYTE HDSlot    = 0;
int	 HDTamanho = 4;
BYTE HDUnidade = 0;
BYTE HDDado    = 0;
BYTE HDStatus  = PS_NODEV;
WORD HDPonteiro[NUMDEVS];
WORD HDBloco[NUMDEVS];
int	 HDTamanhos[NUMDEVS];
char NomeArqs[NUMDEVS][255];
char HDTitulos[NUMDEVS][255];
FILE *HandleArqs[NUMDEVS];
int	 HDProtegido[NUMDEVS];
int	 Led[NUMDEVS];
int	 Led1 = 0;
int	 Led2 = 0;

// Protótipos
void HDLeBloco(void);
void HDEscreveBloco(void);
void HDFormataVolume(void);

// Funções Internas
//===========================================================================
void HDLeBloco(void)
{
	int pos;

	if (HandleArqs[HDUnidade] == NULL)
	{
		Led[HDUnidade] = 0;
		HDStatus = PS_NODEV;
		return;
	}
	if (HDTamanhos[HDUnidade] == 0)
	{
		Led[HDUnidade] = 0;
		HDStatus = PS_IOERR;
		return;
	}
	pos = (HDBloco[HDUnidade] * 512) + HDPonteiro[HDUnidade]++;
	if (HDPonteiro[HDUnidade] == 512)
	{
		HDPonteiro[HDUnidade] = 0;
		Led[HDUnidade] = 0;
	}
	else
	{
		Led[HDUnidade] = 1;
	}
    if (fseek(HandleArqs[HDUnidade], pos, SEEK_SET) != 0)
	{
		Led[HDUnidade] = 0;
		HDStatus = PS_IOERR;
		return;
	}
	if (fread(&HDDado, 1, 1, HandleArqs[HDUnidade]) == 0)
	{
		Led[HDUnidade] = 0;
		HDStatus = PS_IOERR;
		return;
	}
	HDStatus = PS_OK;
}

//===========================================================================
void HDEscreveBloco(void)
{
	int pos;

	if (HDProtegido[HDUnidade])
	{
		Led[HDUnidade] = 0;
		HDStatus = PS_WRPRO;
		return;
	}
	if (HandleArqs[HDUnidade] == NULL)
	{
		Led[HDUnidade] = 0;
		HDStatus = PS_NODEV;
		return;
	}
	pos = (HDBloco[HDUnidade] * 512) + HDPonteiro[HDUnidade]++;
	if (HDPonteiro[HDUnidade] == 512)
	{
		HDPonteiro[HDUnidade] = 0;
		Led[HDUnidade] = 0;
	}
	else
	{
		Led[HDUnidade] = 2;
	}
    if (fseek(HandleArqs[HDUnidade], pos, SEEK_SET) != 0)
	{
		Led[HDUnidade] = 0;
		HDStatus = PS_IOERR;
		return;
	}
	if (fwrite(&HDDado, 1, 1, HandleArqs[HDUnidade]) != 1)
	{
		Led[HDUnidade] = 0;
		HDStatus = PS_IOERR;
		return;
	}
	HDStatus = PS_OK;
}

//===========================================================================
void HDFormataVolume(void)
{
	int pos;

	if (HDProtegido[HDUnidade])
	{
		Led[HDUnidade] = 0;
		HDStatus = PS_WRPRO;
		return;
	}
	Led[HDUnidade] = 2;
	pos = HDTamanhos[HDUnidade] == 0 ? (HDTamanho * 1048576) - 1 : 
									HDTamanhos[HDUnidade]-1;
    if (fseek(HandleArqs[HDUnidade], pos, SEEK_SET) != 0)
	{
		Led[HDUnidade] = 0;
		HDStatus = PS_IOERR;
		return;
	}
	if (fwrite(&HDDado, 1, 1, HandleArqs[HDUnidade]) != 1)
	{
		Led[HDUnidade] = 0;
		HDStatus = PS_IOERR;
		return;
	}
	Led[HDUnidade] = 0;
	HDStatus = PS_OK;
	fseek(HandleArqs[HDUnidade], 0, SEEK_END);
	HDTamanhos[HDUnidade] = ftell(HandleArqs[HDUnidade]);
}

// Funções Globais

//===========================================================================
int HDConfiguraSlot(int Slot)
{
	int erro;
	iofunction ioarrayread[0x10];
	iofunction ioarraywrite[0x10];

	if (HDSlot)
	{
		FrameMostraMensagemErro("Mass Storage (HD) já está configurado em outro slot");
		return ERR_EMUSO;
	}
	ioarrayread[0x00] = NullIo;
	ioarrayread[0x01] = HDIOUnidade;
	ioarrayread[0x02] = HDIOBloco;
	ioarrayread[0x03] = HDIOBloco;
	ioarrayread[0x04] = HDIODado;
	ioarrayread[0x05] = HDIOStatus;
	ioarrayread[0x06] = NullIo;
	ioarrayread[0x07] = NullIo;
	ioarrayread[0x08] = NullIo;
	ioarrayread[0x09] = NullIo;
	ioarrayread[0x0A] = NullIo;
	ioarrayread[0x0B] = NullIo;
	ioarrayread[0x0C] = NullIo;
	ioarrayread[0x0D] = NullIo;
	ioarrayread[0x0E] = NullIo;
	ioarrayread[0x0F] = NullIo;

	ioarraywrite[0x00] = HDIOComando;
	ioarraywrite[0x01] = HDIOUnidade;
	ioarraywrite[0x02] = HDIOBloco;
	ioarraywrite[0x03] = HDIOBloco;
	ioarraywrite[0x04] = HDIODado;
	ioarraywrite[0x05] = NullIo;
	ioarraywrite[0x06] = NullIo;
	ioarraywrite[0x07] = NullIo;
	ioarraywrite[0x08] = NullIo;
	ioarraywrite[0x09] = NullIo;
	ioarraywrite[0x0A] = NullIo;
	ioarraywrite[0x0B] = NullIo;
	ioarraywrite[0x0C] = NullIo;
	ioarraywrite[0x0D] = NullIo;
	ioarraywrite[0x0E] = NullIo;
	ioarraywrite[0x0F] = NullIo;

	erro = MemInsereSlot(Slot, ioarrayread, ioarraywrite, "HD");
	if (erro == ERR_OK)
	{
		HDSlot = Slot;
		FrameMenuHD(1);
	}
	return erro;
}

//===========================================================================
void HDRetiraSlot()
{
	if (HDSlot)
		HDFinaliza();
	HDSlot = 0;
	FrameMenuHD(0);
}

//===========================================================================
void HDStatusLeds(int *HD1, int *HD2)
{
  *HD1 = Led[0];
  *HD2 = Led[1];
}

//===========================================================================
void HDAtualiza(DWORD ciclos)
{
	if (Led[0] != Led1)
	{
		FrameRefreshStatus(DRAW_LEDS);
		Led1 = Led[0];
	}
	if (Led[1] != Led2)
	{
		FrameRefreshStatus(DRAW_LEDS);
		Led2 = Led[1];
	}
}

//===========================================================================
char* HDNomeImagem(int Disp)
{
	int I;
	
	strcpy(HDTitulos[Disp], NomeArqs[Disp]);
	I = strlen(HDTitulos[Disp]);
	while(I>0) {
		if (HDTitulos[Disp][I] == '.')
			HDTitulos[Disp][I] = '\0';
		if (HDTitulos[Disp][I--] == '\\')
		{
			return &HDTitulos[Disp][I+2];
		}
	}
	return "";
}

//===========================================================================
void HDInicializa(void)
{
	int I;
	for(I=0; I < NUMDEVS; I++)
	{
		HandleArqs[I]  = NULL;
		NomeArqs[I][0] = '\0';
		HDBloco[I]     = 0;
		HDPonteiro[I]  = 0;
		HDTamanhos[I]  = 0;
		HDProtegido[I] = 0;
		Led[I]         = 0;
	}
}

//===========================================================================
void HDFinaliza(void)
{
	int I;
	for(I=0; I < NUMDEVS; I++)
	{
		if (HandleArqs[I] != NULL)
			HDFechaImagem(I);
	}
}

#ifdef _WINDOWS
//===========================================================================
void HDEscolheImagem(int Disp)
{
  char directory[MAX_PATH] = "";
  char filename[MAX_PATH]  = "";
  char title[40];
  OPENFILENAME ofn;

  RegLoadString(PREFERENCIAS, DIRINIC, 1, directory, MAX_PATH);
  strcpy(title,"Selecione a imagem do HD ");
  strcat(title,Disp ? "2" : "1");  // TODO: Mudar
  ZeroMemory(&ofn,sizeof(OPENFILENAME));
  ofn.lStructSize     = sizeof(OPENFILENAME);
  ofn.hwndOwner       = (HWND)framewindow;
  ofn.hInstance       = (HINSTANCE)instance;
  ofn.lpstrFilter     = "Imagens de HD \0*.hdv\0"
                        "Todos os Arquivos\0*.*\0";
  ofn.lpstrFile       = filename;
  ofn.nMaxFile        = MAX_PATH;
  ofn.lpstrInitialDir = directory;
  ofn.Flags           = OFN_PATHMUSTEXIST;
  ofn.lpstrTitle      = title;
  if (GetOpenFileName(&ofn))
  {
    int error;

    if ((!ofn.nFileExtension) || !filename[ofn.nFileExtension])
      strcat(filename,".hdv");
    error = HDAbreImagem(filename, Disp);
    if (!error)
	{
      filename[ofn.nFileOffset] = 0;
      if (_stricmp(directory,filename))
        RegSaveString(PREFERENCIAS, DIRINIC, 1, filename);
    }
    else
		FrameMostraMensagemErro("Erro HD"); // TODO: mudar (imagem nao carregada)
  }

}
#else // _WINDOWS
//===========================================================================
void HDEscolheImagem(int Disp)
{

}
#endif // _WINDOWS

//===========================================================================
int HDFechaImagem(int Disp)
{
	if (HandleArqs[Disp] != NULL)
	{
		fclose(HandleArqs[Disp]);
		HandleArqs[Disp] = NULL;
		NomeArqs[Disp][0] = '\0';
	}
	return 0;
}

//===========================================================================
int HDAbreImagem(char *NomeArquivo, int Disp)
{
	int Tamanho;

	if (!HDSlot)
		return -1;
	if (HandleArqs[Disp] != NULL)
		HDFechaImagem(Disp);
	strcpy(NomeArqs[Disp], NomeArquivo);
	HDProtegido[Disp] = 0;
	HDTamanhos[Disp] = 0;
	HandleArqs[Disp] = fopen(NomeArquivo, "rb+");
	if (HandleArqs[Disp] == NULL)
	{
		HandleArqs[Disp] = fopen(NomeArquivo, "rb");
		if (HandleArqs[Disp] != NULL)
			HDProtegido[Disp] = 1;
	}
	if (HandleArqs[Disp] == NULL)
	{
		HandleArqs[Disp] = fopen(NomeArquivo, "wb+");
		if (HandleArqs[Disp] == NULL)
			return -1;
		HDTamanhos[Disp] = 0;
		return 0;
	}
	fseek(HandleArqs[Disp], 0, SEEK_END);
	Tamanho = ftell(HandleArqs[Disp]);
	if (Tamanho > 33554432)
	{
		fclose(HandleArqs[Disp]);
		HandleArqs[Disp] = NULL;
		return -1;
	}
	HDTamanhos[Disp] = Tamanho;
	return 0;
}

// Softswitches
//===========================================================================
BYTE __stdcall HDIOComando(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	switch (value)
	{
		case 0x00: // Status, determina número de blocos
			HDBloco[HDUnidade] = HDTamanhos[HDUnidade] == 0 ? ((HDTamanho * 1048576)-1) / 512 :
									(HDTamanhos[HDUnidade]-1) / 512;
		break;

		case 0x01: // Ler Bloco
			HDLeBloco();
		break;

		case 0x02: // Escrever Bloco
			HDEscreveBloco();
		break;
        
		case 0x03: // Formatar Volume
			HDFormataVolume();
		break;
	}
	return 0;
}

//===========================================================================
BYTE __stdcall HDIOUnidade(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write)
		HDUnidade = value;
	else
		return HDUnidade;
	return 0;
}

//===========================================================================
BYTE __stdcall HDIOBloco(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	switch(address & 0x0F)
	{
		case 0x02:
			if (write)
			{
				HDBloco[HDUnidade] = (HDBloco[HDUnidade] & 0xFF00) | value;
				HDPonteiro[HDUnidade] = 0;
			}
			else
				return HDBloco[HDUnidade] & 0xFF;
		break;

		case 0x03:
			if (write)
			{
				HDBloco[HDUnidade] = (HDBloco[HDUnidade] & 0x00FF) | (value << 8);
				HDPonteiro[HDUnidade] = 0;
			}
			else
				return HDBloco[HDUnidade] >> 8;
		break;
	}
	return 0;
}

//===========================================================================
BYTE __stdcall HDIODado(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write)
		HDDado = value;
	else
		return HDDado;
	return 0;
}

//===========================================================================
BYTE __stdcall HDIOStatus(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	return HDStatus;
}

// EOF