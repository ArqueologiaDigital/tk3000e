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

// Emula uma Super Parallel Card da Microdigital

#include "stdhdr.h"
#include "impressora.h"
#include "tk3000e.h"
#include "teclado.h"
#include "memoria.h"
#include "janela.h"

// Definições
#define NOMEARQUIVO	"impressora.txt"

// Variáveis
BYTE	ImpressoraSlot;
DWORD	ImpressoraPorta			= 0;
FILE	*ArqImpressora			= NULL;
DWORD	InatividadeImpressora	= 0;
BYTE	bit7					= 0x7F;
BYTE	CaracCtrl				= 0x09;
BOOL	Comando					= 0;
BOOL	LineFeed				= 1;
BOOL    Abicomp					= 0;
BOOL	Acentuado				= 0;
BOOL    Maiuscula				= 0;
BOOL	Minuscula				= 0;
int		MaxH					= 40;

// Protótipos
BOOL ChecarImpressora();

// Funções Internas
//===========================================================================
BOOL ChecarImpressora()
{
	InatividadeImpressora = 0;
	if ((ArqImpressora == NULL) && ImpressoraPorta)
	{
		char nomeporta[255];

		if (ImpressoraPorta != 3)
			sprintf(nomeporta,
#ifdef _WINDOWS
					"LPT%u",
#else // _WINDOWS
					"/dev/lpt%u",
#endif // _WINDOWS
					ImpressoraPorta);
		else
		{
			sprintf(nomeporta,"%s%s",progdir,NOMEARQUIVO);
		}
		/*
		ArqImpressora = CreateFile(nomeporta,
									GENERIC_WRITE,
							        0,
									(LPSECURITY_ATTRIBUTES)NULL,
									ImpressoraPorta != 3 ? OPEN_EXISTING : OPEN_ALWAYS,
									0,
									NULL);
		*/
		ArqImpressora = fopen(nomeporta, ImpressoraPorta != 3 ? "ab+" : "wb+");
//		if (ImpressoraPorta == 3)
//			SetFilePointer(ArqImpressora,0,NULL,FILE_END);
	}

	return (ArqImpressora != NULL);
}

// Funções Globais

//===========================================================================
int ImpressoraConfiguraSlot(int Slot)
{
	int erro;
	iofunction ioarrayread[0x10];
	iofunction ioarraywrite[0x10];

	if (ImpressoraSlot)
	{
		FrameMostraMensagemErro("Super Parallel Card já está configurado em outro slot");
		return ERR_EMUSO;
	}
	ioarrayread[0x00] = NullIo;
	ioarrayread[0x01] = NullIo;
	ioarrayread[0x02] = NullIo;
	ioarrayread[0x03] = NullIo;
	ioarrayread[0x04] = NullIo;
	ioarrayread[0x05] = NullIo;
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

	ioarraywrite[0x00] = ImpressoraIO;
	ioarraywrite[0x01] = NullIo;
	ioarraywrite[0x02] = NullIo;
	ioarraywrite[0x03] = NullIo;
	ioarraywrite[0x04] = NullIo;
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

	erro = MemInsereSlot(Slot, ioarrayread, ioarraywrite, "Impresso");
	if (erro == ERR_OK)
		ImpressoraSlot = Slot;
	return erro;
}

//===========================================================================
void ImpressoraRetiraSlot()
{
	if (ImpressoraSlot)
		ImpressoraFinaliza();
	ImpressoraSlot = 0;
}

//===========================================================================
void ImpressoraInicializa()
{
	ImpressoraMudaAcentuado();
}

//===========================================================================
void ImpressoraFinaliza(void)
{
	if (ArqImpressora != NULL)
	{
		fclose(ArqImpressora);
	}
	ArqImpressora = NULL;
	InatividadeImpressora = 0;
}

//===========================================================================
void ImpressoraReset()
{
	CaracCtrl = 0x09;
	KeybGetModeStatus(&Acentuado);
	Abicomp   = 0;
	bit7      = 0x7F;
	LineFeed  = 1;
}

//===========================================================================
void ImpressoraMudaAcentuado()
{
	KeybGetModeStatus(&Acentuado);
}

//===========================================================================
void ImpressoraAtualiza(DWORD totalcycles)
{
	if (ArqImpressora == NULL)
		return;
	InatividadeImpressora += totalcycles;
	if (InatividadeImpressora > 2000000)
		ImpressoraFinaliza();
	}

//===========================================================================
void ImpressoraDefinePorta(int Porta)
{
	if (ArqImpressora == NULL)
		ImpressoraPorta = Porta;
	else
		/*
		MessageBox(window,
					"Você não pode mudar a porta da impressora "
					"enquanto ela estiver em uso.",
					"Configuração",
					MB_ICONEXCLAMATION | MB_SETFOREGROUND);
		*/
		FrameMostraMensagemAdvertencia("Você não pode mudar a porta da impressora "
				"enquanto ela estiver em uso.");
}

// SoftSwitches
BYTE __stdcall ImpressoraIO(WORD   programcounter,
							BYTE address,
							BYTE write,
							BYTE value)
{
	if (write)
	{
		if (ChecarImpressora())
		{
			value &= bit7;
			if (((value & 0x7F) == CaracCtrl) && CaracCtrl)
			{
				Comando = 1;
			}
			else
			{
				if (Comando)
				{
					static int NumTemp;

					value &= 0x7F;
					if (value >= '0' && value <= '9')
					{
						NumTemp = (NumTemp * 10) + (value - '0');
					} else if ((value < 32) && (value != 13))
					{
						CaracCtrl = value;
						Comando = 0;
					}
					else switch(value)
					{
						case 'H':
							bit7    = 0xFF;
							Comando = 0;
						break;

						case 'I':
							MaxH     = 40;
							LineFeed = 1;
							Comando  = 0;
						break;

						case 'K':
							LineFeed = 0;
							Comando  = 0;
						break;

						case 'N':
							MaxH    = NumTemp;
							NumTemp = 0;
							Comando = 0;
						break;

						case 'P':
							Abicomp = 1;
							Comando = 0;
						break;

						case 'X':
							bit7    = 0x7F;
							Abicomp = 0;
							Comando = 0;
						break;

						case 'R':
							Comando = 0;
						break;

						case 'M':
							Acentuado = !Acentuado;
							Abicomp   = 0;
							Comando   = 0;
						break;

						case '@':
							bit7      = 0x7F;
							LineFeed  = 1;
							Abicomp   = 0;
							Comando   = 0;
							KeybGetModeStatus(&Acentuado);
						break;

						case 'Z':
							CaracCtrl = 0;
							Comando   = 0;
						break;

						default:
							Comando   = 0;
						break;

					} // switch
				}
				else
				{
					char buffer[10];
					int  TamBuf = 1;

					memset(buffer, 0, 10);
					buffer[0] = value;

					if (Acentuado && !Minuscula)
					{
						if (Abicomp)
						{
							switch (value & 0x7F)
							{
								case '_':  buffer[0] = (char)0xC1; break; // à
								case '@':  buffer[0] = (char)0xC2; break; // á
								case '\\': buffer[0] = (char)0xC3; break; // â
								case '[':  buffer[0] = (char)0xC4; break; // ã
								case ']':  buffer[0] = (char)0xC6; break; // ç
								case '`':  buffer[0] = (char)0xC8; break; // é
								case '&':  buffer[0] = (char)0xC9; break; // ê
								case '{':  buffer[0] = (char)0xCC; break; // í
								case '~':  buffer[0] = (char)0xD1; break; // ó
								case '}':  buffer[0] = (char)0xD2; break; // ô
								case '#':  buffer[0] = (char)0xD3; break; // õ
								case '|':  buffer[0] = (char)0xD7; break; // ú
							} // switch
							if ((value & 0x7F) == '<')
							{
								Minuscula = 1;
								buffer[0] = '\0';
								TamBuf = 0;
							}
							else if ((value & 0x7F) == '>')
							{
								Maiuscula = 1;
								buffer[0] = '\0';
								TamBuf = 0;
							}
							else if (Maiuscula)
							{
								Maiuscula = 0;
								buffer[0] -= 0x20;
							}// if Maiuscula
						}
						else
						{
							switch (value & 0x7F)
							{
								case '_':  strcpy(buffer,"a\b`"); break; // à
								case '@':  strcpy(buffer,"a\b'"); break; // á
								case '\\': strcpy(buffer,"a\b^"); break; // â
								case '[':  strcpy(buffer,"a\b~"); break; // ã
								case ']':  strcpy(buffer,"c\b,"); break; // ç
								case '`':  strcpy(buffer,"e\b'"); break; // é
								case '&':  strcpy(buffer,"e\b^"); break; // ê
								case '{':  strcpy(buffer,"i\b'"); break; // í
								case '~':  strcpy(buffer,"o\b'"); break; // ó
								case '}':  strcpy(buffer,"o\b^"); break; // ô
								case '#':  strcpy(buffer,"o\b~"); break; // õ
								case '|':  strcpy(buffer,"u\b'"); break; // ú
							} // switch
							if (strlen(buffer) == 3)
								TamBuf = 3;
							if ((value & 0x7F) == '<')
							{
								Minuscula = 1;
								buffer[0] = '\0';
								TamBuf = 0;
							}
							else if ((value & 0x7F) == '>')
							{
								Maiuscula = 1;
								buffer[0] = '\0';
								TamBuf = 0;
							}
							else if (Maiuscula)
							{
								Maiuscula = 0;
								buffer[0] -= 0x20;
							}// if Maiuscula
						} // if Abicomp
					}
					else if (Minuscula)
					{
						Minuscula = 0;
					}// if Minuscula

					if ((value & 0x7F) == 13 && LineFeed)
					{
						buffer[1] = (char)(10 | (bit7 & 0x80));
						TamBuf = 2;
					}
					/*
					WriteFile(ArqImpressora,
								buffer,
								TamBuf,
								&BytesEscritos,
								NULL);
					*/
					fwrite(buffer, 1, TamBuf, ArqImpressora);
				} // if Comando
			} // if value = CaracCtrl
		} // if Checar
	} // if write

	return MemReturnRandomData(1);
}

// EOF