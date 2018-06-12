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

// Emula a interface IDE do Richard Dreher

#include "stdhdr.h"
#include "tk3000e.h"
#include "ide.h"
#include "janela.h"
#include "registro.h"
#include "memoria.h"

// Definições
#define	ATASTATUS_ERR	0x01	// Error
#define	ATASTATUS_IDX	0x02	// Index Vendor
#define	ATASTATUS_CORR	0x04	// Corrected Data Error
#define	ATASTATUS_DRQ	0x08	// Data Request
#define	ATASTATUS_DSC	0x10	// Device Seek Complete
#define	ATASTATUS_DF	0x20	// Device Fault
#define	ATASTATUS_DRDY	0x40	// Device Ready
#define	ATASTATUS_BSY	0x80	// Busy

#define ATAERROR_AMNF	0x01	// Address Mark Not Found
#define ATAERROR_TK0NF	0x02	// Track 0 Not Found
#define ATAERROR_ABRT	0x04	// Aborted Command
#define ATAERROR_MCR	0x08	// Media Change Requested
#define ATAERROR_IDNF	0x10	// ID Not Found
#define ATAERROR_MC		0x20	// Media Changed
#define ATAERROR_UNC	0x40	// Uncorrectable Data Error

#define ATADEVCTRL_nIEN	0x02
#define ATADEVCTRL_SRST	0x04

#define ATAHEAD_DEV		0x10
#define ATAHEAD_LBA		0x20

#define ATACMD_IDLE				0x00
#define ATACMD_READSECTOR		0x01
#define ATACMD_WRITESECTOR		0x02
#define ATACMD_IDENTIFYDEVICE	0x03

typedef struct
{
	WORD  TipoDisp;					//  0 -   1
	WORD  TotalCilindros;			//  2 -   3
	WORD  Reservado01;				//  4 -   5
	WORD  TotalCabecas;				//  6 -   7
	DWORD Obsoleto01;				//  8 -  11
	WORD  TotalSetoresTrilha;		// 12 -  13
	char  VendorSpecific01[6];		// 14 -  19
	char  NumeroSerial[20];			// 20 -  39
	DWORD Obsoleto02;				// 40 -  43
	WORD  NumBytesComandosLongos;	// 44 -  45
	char  RevisaoFirmware[8];		// 46 -  53
	char  Modelo[40];				// 54 -  93
	WORD  VendorSpecific02;			// 94 -  95
	WORD  Reservado02;				// 96 -  97
	WORD  Capacidades01;			// 98 -  99
	WORD  Reservado03;				//100 - 101
	WORD  CicloPIO;					//102 - 103
	WORD  Obsoleto03;				//104 - 105
	WORD  CamposValidos;			//106 - 107
	WORD  TotalCilindros2;			//108 - 109
	WORD  TotalCabecas2;			//110 - 111
	WORD  TotalSetoresTrilha2;		//112 - 113
	DWORD TotalSetores;				//114 - 117
	WORD  Capacidades02;			//118 - 119
	DWORD TotalLBA;					//120 - 123
	WORD  Obsoleto04;				//124 - 125
	WORD  CapacidadesDMA;			//126 - 127
	WORD  CapacidadePIO;			//128 - 129
	WORD  CicloDMA;					//130 - 131
	WORD  CicloRecomendadoDMA;		//132 - 133
	WORD  CicloMinimoPIO;			//134 - 135
	WORD  CicloMinimoPIOComIORDY;	//136 - 137
	char  Reservado04[22];			//138 - 159
	WORD  NumeroVersaoMaior;		//160 - 161
	WORD  NumeroVersaoMenor;		//162 - 163
	WORD  ComandosSuportados;		//164 - 165
	WORD  ComandosSuportados2;		//166 - 167
	char  Reservado05[88];			//168 - 255
	WORD  StatusSeguranca;			//256 - 257
	char  VendorSpecific03[62];		//258 - 319
	char  Reservado06[192];			//320 - 511
} IdDisp;

// Variáveis
BYTE   IDESlot       = 0;
FILE   *IDEHandle    = NULL;
int	   IDECmdAtual   = ATACMD_IDLE;
int	   IDEProtegido  = 0;
int	   IDEPonteiro   = 0;
int	   IDELed        = 0;
int	   IDELedA       = 0;
char   IDETitulo[255];
char   IDENomeArq[255];
IdDisp IDEIdentificacao;

BYTE  ATAStatus = ATASTATUS_DSC | ATASTATUS_DRDY;
BYTE  ATAError;
BYTE  ATAHead;
BYTE  ATASector;
BYTE  ATASectorCnt;
BYTE  ATADevCtrl;
WORD  ATACylinder;
WORD  ATAData;
DWORD ATALBA;

// Protótipos
void ATADeviceReset();
void ATAReadSector();
void ATAWriteSector();
void ATAIdentifyDevice();

// Funções Internas
// ============================================================================
void ATADeviceReset()
{
	ATAError	= 0;
	ATAStatus	= ATASTATUS_DSC | ATASTATUS_DRDY;
	ATAHead		= 0;
	ATASector	= 0;
	ATASectorCnt= 0;
	ATACylinder	= 0;
	ATAData		= 0;
	ATALBA		= 0;
	IDECmdAtual = ATACMD_IDLE;
	IDELed = 0;
	FrameRefreshStatus(DRAW_LEDS);
}

// ============================================================================
void ATAReadSector()
{
	int pos;

	if (!IDEHandle)
	{
		ATAStatus = (ATAStatus & ~ATASTATUS_DRQ) | ATASTATUS_ERR;
		ATAError = ATAERROR_UNC;
		IDECmdAtual = ATACMD_IDLE;
		IDELed = 0;
		FrameRefreshStatus(DRAW_LEDS);
		return;
	}
	pos = (ATALBA * 512) + IDEPonteiro++;
	if (++IDEPonteiro == 512)
	{
		IDEPonteiro = 0;
		if (--ATASectorCnt)
			ATALBA++;
		else
		{
			ATAStatus &= ~ATASTATUS_DRQ;
			IDECmdAtual = ATACMD_IDLE;
			IDELed = 0;
			FrameRefreshStatus(DRAW_LEDS);
		}
	}
	else
	{
		ATAStatus |= ATASTATUS_DRQ;
	}
    if (fseek(IDEHandle, pos, SEEK_SET))
	{
		ATAStatus = (ATAStatus & ~ATASTATUS_DRQ) | ATASTATUS_ERR;
		ATAError = ATAERROR_UNC;
		IDECmdAtual = ATACMD_IDLE;
		IDELed = 0;
		FrameRefreshStatus(DRAW_LEDS);
		return;
	}
	if (fread(&ATAData, 1, 2, IDEHandle) != 2)
	{
		ATAStatus = (ATAStatus & ~ATASTATUS_DRQ) | ATASTATUS_ERR;
		ATAError = ATAERROR_UNC;
		IDECmdAtual = ATACMD_IDLE;
		IDELed = 0;
		FrameRefreshStatus(DRAW_LEDS);
		return;
	}
	ATAStatus &= ~ATASTATUS_ERR;
}

// ============================================================================
void ATAWriteSector()
{
	int pos;

	if (!IDEHandle)
	{
		ATAStatus = (ATAStatus & ~ATASTATUS_DRQ) | ATASTATUS_ERR;
		ATAError = ATAERROR_UNC;
		IDECmdAtual = ATACMD_IDLE;
		IDELed = 0;
		FrameRefreshStatus(DRAW_LEDS);
		return;
	}
	pos = (ATALBA * 512) + IDEPonteiro++;
	if (++IDEPonteiro == 512)
	{
		IDEPonteiro = 0;
		if (--ATASectorCnt)
			ATALBA++;
		else
		{
			ATAStatus &= ~ATASTATUS_DRQ;
			IDECmdAtual = ATACMD_IDLE;
			IDELed = 0;
			FrameRefreshStatus(DRAW_LEDS);
		}
	}
	else
	{
		ATAStatus |= ATASTATUS_DRQ;
	}
    if (fseek(IDEHandle, pos, SEEK_SET))
	{
		ATAStatus = (ATAStatus & ~ATASTATUS_DRQ) | ATASTATUS_ERR;
		ATAError = ATAERROR_UNC;
		IDECmdAtual = ATACMD_IDLE;
		IDELed = 0;
		FrameRefreshStatus(DRAW_LEDS);
		return;
	}
	if (fwrite(&ATAData, 1, 2, IDEHandle) != 2)
	{
		ATAStatus = (ATAStatus & ~ATASTATUS_DRQ) | ATASTATUS_ERR;
		ATAError = ATAERROR_UNC;
		IDECmdAtual = ATACMD_IDLE;
		IDELed = 0;
		FrameRefreshStatus(DRAW_LEDS);
		return;
	}
	ATAStatus &= ~ATASTATUS_ERR;
}

// ============================================================================
void ATAIdentifyDevice()
{
	ATAData = *((WORD *)((&IDEIdentificacao.TipoDisp) + IDEPonteiro));
	IDEPonteiro += 1;
	if (IDEPonteiro == 256)
	{
		ATAStatus &= ~ATASTATUS_DRQ;
		IDECmdAtual = ATACMD_IDLE;
		IDELed = 0;
		FrameRefreshStatus(DRAW_LEDS);
	}
}


// Funções Globais

//===========================================================================
int IDEConfiguraSlot(int Slot)
{
	int erro;
	iofunction ioarrayread[0x10];
	iofunction ioarraywrite[0x10];

	if (IDESlot)
	{
		FrameMostraMensagemErro("Interface IDE já está configurado em outro slot");
		return ERR_EMUSO;
	}
	ioarrayread[0x00] = IDEATADataHigh;
	ioarrayread[0x01] = IDESetCSMask;
	ioarrayread[0x02] = IDEClearCSMask;
	ioarrayread[0x03] = NullIo;
	ioarrayread[0x04] = NullIo;
	ioarrayread[0x05] = NullIo;
	ioarrayread[0x06] = IDEATAAltStatus;
	ioarrayread[0x07] = NullIo;
	ioarrayread[0x08] = IDEATADataLow;
	ioarrayread[0x09] = IDEATAError;
	ioarrayread[0x0A] = IDEATASectorCnt;
	ioarrayread[0x0B] = IDEATASector;
	ioarrayread[0x0C] = IDEATACylinder;
	ioarrayread[0x0D] = IDEATACylinderH;
	ioarrayread[0x0E] = IDEATAHead;
	ioarrayread[0x0F] = IDEATAStatus;

	ioarraywrite[0x00] = IDEATADataHigh;
	ioarraywrite[0x01] = IDESetCSMask;
	ioarraywrite[0x02] = IDEClearCSMask;
	ioarraywrite[0x03] = NullIo;
	ioarraywrite[0x04] = NullIo;
	ioarraywrite[0x05] = NullIo;
	ioarraywrite[0x06] = IDEATADevCtrl;
	ioarraywrite[0x07] = NullIo;
	ioarraywrite[0x08] = IDEATADataLow;
	ioarraywrite[0x09] = IDEATAError;
	ioarraywrite[0x0A] = IDEATASectorCnt;
	ioarraywrite[0x0B] = IDEATASector;
	ioarraywrite[0x0C] = IDEATACylinder;
	ioarraywrite[0x0D] = IDEATACylinderH;
	ioarraywrite[0x0E] = IDEATAHead;
	ioarraywrite[0x0F] = IDEATACommand;

	erro = MemInsereSlot(Slot, ioarrayread, ioarraywrite, "IDE");
	if (erro == ERR_OK)
	{
		IDESlot = Slot;
		FrameMenuIDE(1);
	}
	return erro;
}

// ============================================================================
void IDERetiraSlot()
{
	if (IDESlot)
		IDEFinaliza();
	IDESlot = 0;
	FrameMenuIDE(0);
}

// ============================================================================
void IDEAtualiza(DWORD ciclos)
{
	if (IDELed != IDELedA)
	{
		FrameRefreshStatus(DRAW_LEDS);
		IDELedA = IDELed;
	}
}

// ============================================================================
void IDEInicializa(void)
{
	memset(&IDEIdentificacao, 0, sizeof(IdDisp));
	IDEIdentificacao.TipoDisp				= 0x40;
	IDEIdentificacao.TotalCilindros			= 1000;
	IDEIdentificacao.TotalCabecas			= 10;
	IDEIdentificacao.TotalSetoresTrilha		= 53;
	strcpy(IDEIdentificacao.NumeroSerial, "12345678901234567890");
	IDEIdentificacao.NumBytesComandosLongos	= 512;
	strcpy(IDEIdentificacao.RevisaoFirmware, "12345678");
	strcpy(IDEIdentificacao.Modelo, "EMULADOR TK3000//E  EMULADOR DE DISP ATA");
	IDEIdentificacao.CamposValidos			= 1;
	IDEIdentificacao.TotalCilindros2		= 1000;
	IDEIdentificacao.TotalCabecas2			= 10;
	IDEIdentificacao.TotalSetoresTrilha2	= 53;
	IDEIdentificacao.TotalSetores			= 530000;
	IDEIdentificacao.TotalLBA				= 530000;

}

// ============================================================================
void IDEFinaliza(void)
{
	IDEFechaImagem();
}

#ifdef _WINDOWS
// ============================================================================
void IDEEscolheImagem(void)
{
	char directory[MAX_PATH] = "";
	char filename[MAX_PATH]  = "";
	char title[40];
	OPENFILENAME ofn;

	RegLoadString(PREFERENCIAS, DIRINIC, 1, directory, MAX_PATH);
	strcpy(title,"Selecione a imagem da IDE");
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize     = sizeof(OPENFILENAME);
	ofn.hwndOwner       = (HWND)framewindow;
	ofn.hInstance       = (HINSTANCE)instance;
	ofn.lpstrFilter     = "Imagens de IDE \0*.IDE\0"
							"Todos os Arquivos\0*.*\0";
	ofn.lpstrFile       = filename;
	ofn.nMaxFile        = MAX_PATH;
	ofn.lpstrInitialDir = directory;
	ofn.Flags           = OFN_PATHMUSTEXIST;
	ofn.lpstrTitle      = title;
	if (GetOpenFileName(&ofn))
	{
		int erro;

		if ((!ofn.nFileExtension) || !filename[ofn.nFileExtension])
			strcat(filename,".ide");
		erro = IDEAbreImagem(filename);
		if (!erro)
		{
			filename[ofn.nFileOffset] = 0;
			if (_stricmp(directory,filename))
				RegSaveString(PREFERENCIAS, DIRINIC, 1, filename);
		}
		else
			MessageBox(0,"Erro IDE","Erro IDE",0);  // mudar (imagem nao carregada)
	}
}
#else // _WINDOWS
// ============================================================================
void IDEEscolheImagem(void)
{
}
#endif // _WINDOWS

// ============================================================================
int IDEFechaImagem(void)
{
	if (IDEHandle)
	{
		fclose(IDEHandle);
		IDEHandle = NULL;
		IDENomeArq[0] = '\0';
	}
	return 0;
}

// ============================================================================
int IDEAbreImagem(char *NomeArquivo)
{
	if (!IDESlot)
		return -1;
	if (IDEHandle != NULL)
		IDEFechaImagem();
	strcpy(IDENomeArq, NomeArquivo);
	IDEHandle = fopen(NomeArquivo, "rb+");
	if (IDEHandle == NULL)
	{
		IDEHandle = fopen(NomeArquivo, "rb");
		if (IDEHandle != NULL)
			IDEProtegido = 1;
	}
	if (IDEHandle == NULL)
	{
		IDEHandle = fopen(NomeArquivo, "wb+");
		if (IDEHandle == NULL)
			return -1;
		return 0;
	}
	return 0;
}

// ============================================================================
char *IDENomeImagem()
{
	int I;
	
	strcpy(IDETitulo, IDENomeArq);
	I = strlen(IDETitulo);
	while(I>0)
	{
		if (IDETitulo[I] == '.')
			IDETitulo[I] = '\0';
		if (IDETitulo[I--] == '\\') 
		{
			return &IDETitulo[I+2];
		}
	}
	return "";
}

// ============================================================================
void IDEStatusLed(int *Led)
{
	*Led = IDELed;
}



// Softswitches
// ============================================================================
BYTE __stdcall IDEATADataHigh(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write) 
		ATAData = (ATAData & 0x00FF) | (value << 8);

	return ((ATAData & 0xFF00) >> 8);
}

// ============================================================================
BYTE __stdcall IDESetCSMask(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	return 0;
}

// ============================================================================
BYTE __stdcall IDEClearCSMask(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	return 0;
}

// ============================================================================
BYTE __stdcall IDEATAAltStatus(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	return ATAStatus;
}

// ============================================================================
BYTE __stdcall IDEATADataLow(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write)
		ATAData = (ATAData & 0xFF00) | value;

	if (ATAStatus & ATASTATUS_DRQ)
	{
		if (IDECmdAtual == ATACMD_READSECTOR) ATAReadSector();
		if (IDECmdAtual == ATACMD_WRITESECTOR) ATAWriteSector();
		if (IDECmdAtual == ATACMD_IDENTIFYDEVICE) ATAIdentifyDevice();
	}
	return (ATAData & 0xFF);
}

// ============================================================================
BYTE __stdcall IDEATAError(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	return ATAError;
}

// ============================================================================
BYTE __stdcall IDEATASectorCnt(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write) ATASectorCnt = value;
	return ATASectorCnt;
}

// ============================================================================
BYTE __stdcall IDEATASector(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write) ATASector = value;
	ATALBA = (ATALBA & 0xFFFFFF00) | ATASector;
	return ATASector;
}

// ============================================================================
BYTE __stdcall IDEATACylinder(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write) ATACylinder = (ATACylinder & 0xFF00) | value;
	ATALBA = (ATALBA & 0xFF0000FF) | (ATACylinder << 8);
	return (ATACylinder & 0xFF);
}

// ============================================================================
BYTE __stdcall IDEATACylinderH(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write) ATACylinder = (ATACylinder & 0x00FF) | (value << 8);
	ATALBA = (ATALBA & 0xFF0000FF) | (ATACylinder << 8);
	return ((ATACylinder & 0xFF00) >> 8);
}

// ============================================================================
BYTE __stdcall IDEATAHead(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write) ATAHead = value;
	ATALBA = (ATALBA & 0x00FFFFFF) | ((ATAHead & 0x0F) << 24);
	return ATAHead;
}

// ============================================================================
BYTE __stdcall IDEATAStatus(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	return ATAStatus;
}

// ============================================================================
BYTE __stdcall IDEATADevCtrl(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (write) ATADevCtrl = value;
	return ATADevCtrl;
}

// ============================================================================
BYTE __stdcall IDEATACommand(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	switch(value)
	{
		case 0x08:
			ATADeviceReset();
		break;

		case 0x20:
			IDEPonteiro = 0;
			IDECmdAtual = ATACMD_READSECTOR;
			ATAStatus |= ATASTATUS_DRQ;
			IDELed = 1;
			FrameRefreshStatus(DRAW_LEDS);
		break;

		case 0x30:
			IDEPonteiro = 0;
			IDECmdAtual = ATACMD_WRITESECTOR;
			ATAStatus |= ATASTATUS_DRQ;
			IDELed = 2;
			FrameRefreshStatus(DRAW_LEDS);
		break;

		case 0xEC:
 			IDEPonteiro = 0;
			IDECmdAtual = ATACMD_IDENTIFYDEVICE;
			ATAStatus |= ATASTATUS_DRQ;
			IDELed = 1;
			FrameRefreshStatus(DRAW_LEDS);
		break;

		default:
			{
				//char tmp[255];
				
				//sprintf(tmp, "IDE: Comando %X ainda não implementado.", value);
				//MessageBox(0, tmp, "IDE", 0);
			}
		break;
	}
	return 0;
}

// EOF