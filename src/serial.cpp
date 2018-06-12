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

// Emula uma Super Serial Card da Apple

/*
  Configurações das Dip-Switches

  SW1:
  1 2 3 4 5 6 7
  | | | | | | |
  | | | | | | +--- Habilitar CTS
  | | | | | +-----\ Modo de
  | | | | +-------/ Trabalho
  | | | +---------\
  | | +----------- \ Baud
  | +------------- / Rate
  +---------------/

  SW2:
  1 2 3 4 5 6 7
  | | | | | | |
  | | | | | | +--- Habilitar CTS Secundario
  | | | | | +----- Habilitar Interrupcao
  | | | | +------- Gerar Line Feed Automaticamente
  | | | +--------- Habilitar Paridade (OFF=Habilitada, ON=Desabilitada)
  | | +----------- Paridade (OFF=Par, ON=Impar)
  | +------------- Numero de bits de Dados (OFF=7, ON=8)
  +--------------- Numero de Stop Bits (OFF=2, ON=1)
______________________________________________________________________________
  1-1 1-2 1-3 1-4    Default bit rate
  ON  ON  ON  ON     undefined
  ON  ON  ON  OFF    50
  ON  ON  OFF ON     75
  ON  ON  OFF OFF    109.92 (110)
  ON  OFF ON  ON     134.48 (134.5)
  ON  OFF ON  OFF    150
  ON  OFF OFF ON     300
  ON  OFF OFF OFF    600
  OFF ON  ON  ON     1200
  OFF ON  ON  OFF    1800
  OFF ON  OFF ON     2400
  OFF ON  OFF OFF    3600
  OFF OFF ON  ON     4800
  OFF OFF ON  OFF    7200
  OFF OFF OFF ON     9600
  OFF OFF OFF OFF    19200
*/

#include "stdhdr.h"
#include "tk3000e.h"
#include "serial.h"
#include "disco.h"
#include "memoria.h"
#include "janela.h"

#undef SERIAL_DEBUG

// Definições
#define	NOMEARQUIVO	 "serial.txt"
#define DEF_BAUDRATE CBR_19200
#define DEF_BYTESIZE 8
#define DEF_PARITY   NOPARITY
#define DEF_STOPBITS ONESTOPBIT

// Variáveis
#ifdef SERIAL_DEBUG
HANDLE arqdebug		  = INVALID_HANDLE_VALUE;
#endif

HANDLE commhandle     = INVALID_HANDLE_VALUE;
int    zz = 0;
BYTE   CommSlot;
DWORD  baudrate       = DEF_BAUDRATE;
BYTE   bytesize       = DEF_BYTESIZE;
BYTE   commandbyte    = 0x00;
DWORD  comminactivity = 0;
BYTE   controlbyte    = 0x1F;
BYTE   parity         = DEF_PARITY;
BYTE   recvbuffer[9];
DWORD  recvbytes      = 0;
DWORD  serialport     = 0;
BYTE   stopbits       = DEF_STOPBITS;

// Protótipos
void UpdateCommState();

//===========================================================================
BOOL CheckComm()
{
#ifdef SERIAL_DEBUG
	char arq[255];
#endif

	comminactivity = 0;
	if ((commhandle == INVALID_HANDLE_VALUE) && serialport)
	{
		char portname[255];

		if (serialport != 5)
			sprintf(portname,
#ifdef _WINDOWS
						"COM%u",
#else // _WINDOWS
						"/dev/ttyS%u",
#endif // _WINDOWS						
						serialport);
		else
			sprintf(portname, "%s%s",progdir,NOMEARQUIVO);

#ifdef SERIAL_DEBUG
		sprintf(arq, "%s%s", progdir, "serial_debug.txt");
		arqdebug  = CreateFile( arq,
								GENERIC_READ | GENERIC_WRITE,
								0,
								(LPSECURITY_ATTRIBUTES)NULL,
								OPEN_ALWAYS,
								0,
								NULL);
#endif
		commhandle = CreateFile(portname,
								GENERIC_READ | GENERIC_WRITE,
								0,
								(LPSECURITY_ATTRIBUTES)NULL,
								serialport != 5 ? OPEN_EXISTING : OPEN_ALWAYS,
								0,
								NULL);
		if (serialport == 5)
			SetFilePointer(commhandle, 0, NULL, FILE_END);
		else
		{
			if (commhandle != INVALID_HANDLE_VALUE)
			{
				COMMTIMEOUTS ct;

				UpdateCommState();
				ZeroMemory(&ct,sizeof(COMMTIMEOUTS));
				ct.ReadIntervalTimeout = MAXDWORD;
				SetCommTimeouts(commhandle,&ct);
			}
		}
	}
	return (commhandle != INVALID_HANDLE_VALUE);
}

//===========================================================================
void CheckReceive()
{
	if (recvbytes || (commhandle == INVALID_HANDLE_VALUE))
		return;
	ReadFile(commhandle, recvbuffer, 8, &recvbytes,NULL);
}

//===========================================================================
void CloseComm()
{
	if (commhandle != INVALID_HANDLE_VALUE)
		CloseHandle(commhandle);
#ifdef SERIAL_DEBUG
	CloseHandle(arqdebug);
#endif
	commhandle     = INVALID_HANDLE_VALUE;
	comminactivity = 0;
}

//===========================================================================
void UpdateCommState()
{
	DCB dcb;

	if (commhandle == INVALID_HANDLE_VALUE || serialport == 5)
		return;
	ZeroMemory(&dcb,sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	GetCommState(commhandle,&dcb);
	dcb.BaudRate = baudrate;
	dcb.ByteSize = bytesize;
	dcb.Parity   = parity;
	dcb.StopBits = stopbits;
	SetCommState(commhandle,&dcb);
}

//
// ----- ALL GLOBALLY ACCESSIBLE FUNCTIONS ARE BELOW THIS LINE -----
//

//===========================================================================
int CommConfiguraSlot(int Slot)
{
	int erro;
	iofunction ioarrayread[0x10];
	iofunction ioarraywrite[0x10];

	if (CommSlot)
	{
		FrameMostraMensagemErro("Super Serial Card já está configurado em outro slot");
		return ERR_EMUSO;
	}
	ioarrayread[0x00] = NullIo;
	ioarrayread[0x01] = CommDipSw;
	ioarrayread[0x02] = CommDipSw;
	ioarrayread[0x03] = NullIo;
	ioarrayread[0x04] = NullIo;
	ioarrayread[0x05] = NullIo;
	ioarrayread[0x06] = NullIo;
	ioarrayread[0x07] = NullIo;
	ioarrayread[0x08] = CommReceive;
	ioarrayread[0x09] = CommStatus;
	ioarrayread[0x0A] = CommCommand;
	ioarrayread[0x0B] = CommControl;
	ioarrayread[0x0C] = NullIo;
	ioarrayread[0x0D] = NullIo;
	ioarrayread[0x0E] = NullIo;
	ioarrayread[0x0F] = NullIo;

	ioarraywrite[0x00] = NullIo;
	ioarraywrite[0x01] = NullIo;
	ioarraywrite[0x02] = NullIo;
	ioarraywrite[0x03] = NullIo;
	ioarraywrite[0x04] = NullIo;
	ioarraywrite[0x05] = NullIo;
	ioarraywrite[0x06] = NullIo;
	ioarraywrite[0x07] = NullIo;
	ioarraywrite[0x08] = CommTransmit;
	ioarraywrite[0x09] = CommStatus;
	ioarraywrite[0x0A] = CommCommand;
	ioarraywrite[0x0B] = CommControl;
	ioarraywrite[0x0C] = NullIo;
	ioarraywrite[0x0D] = NullIo;
	ioarraywrite[0x0E] = NullIo;
	ioarraywrite[0x0F] = NullIo;

	erro = MemInsereSlot(Slot, ioarrayread, ioarraywrite, "Serial");
	if (erro == ERR_OK)
		CommSlot = Slot;
	return erro;
}

//===========================================================================
void CommRetiraSlot()
{
	if (CommSlot)
		CloseComm();
	CommSlot = 0;
}

//===========================================================================
void CommSetSerialPort (HWND window, DWORD newserialport)
{
	if (commhandle == INVALID_HANDLE_VALUE)
		serialport = newserialport;
	else
		FrameMostraMensagemAdvertencia("Você não pode mudar a porta serial enquanto ela estiver em uso.");
}

//===========================================================================
void CommUpdate (DWORD totalcycles)
{
	if (commhandle == INVALID_HANDLE_VALUE)
		return;
	if ((comminactivity += totalcycles) > 1000000)
	{
		static DWORD lastcheck = 0;

		if ((comminactivity > 2000000) || (comminactivity-lastcheck > 99950))
		{
			DWORD modemstatus = 0;
			if (serialport != 5)
			{
				GetCommModemStatus(commhandle,&modemstatus);
				if ((modemstatus & MS_RLSD_ON) || DiskIsSpinning())
					comminactivity = 0;
			} // if serialport
		} // if comminactivity
		if (comminactivity > 2000000)
			CloseComm();
	}
}

//===========================================================================
void CommReset()
{
  CloseComm();
  baudrate    = CBR_19200;
  bytesize    = 8;
  commandbyte = 0x00;
  controlbyte = 0x1F;
  parity      = NOPARITY;
  recvbytes   = 0;
  stopbits    = ONESTOPBIT;
}

//===========================================================================
void CommDestroy()
{
  if ((baudrate != CBR_19200) ||
      (bytesize != 8) ||
      (parity   != NOPARITY) ||
      (stopbits != ONESTOPBIT))
  {
    CommReset();
    CheckComm();
  }
  CloseComm();
}

// Softswitches

//===========================================================================
BYTE __stdcall CommCommand(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (!CheckComm())
		return 0;
	if (write && (value != commandbyte))
	{
		commandbyte = value;

		// UPDATE THE PARITY
		if (commandbyte & 0x20)
			switch (commandbyte & 0xC0)
			{
				case 0x00 : parity = ODDPARITY;    break;
				case 0x40 : parity = EVENPARITY;   break;
				case 0x80 : parity = MARKPARITY;   break;
				case 0xC0 : parity = SPACEPARITY;  break;
			} // switch
		else
			parity = NOPARITY;
		UpdateCommState();
	} // if write
	return commandbyte;
}

//===========================================================================
BYTE __stdcall CommControl(WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	if (!CheckComm())
		return 0;
	if (write && (value != controlbyte))
	{
		controlbyte = value;

		// UPDATE THE BAUD RATE
		switch (controlbyte & 0x0F)
		{
			case 0x00: // fall through
			case 0x01: // fall through
			case 0x02: // fall through
			case 0x03: // fall through
			case 0x04: // fall through
			case 0x05: baudrate = CBR_110;     break;
			case 0x06: baudrate = CBR_300;     break;
			case 0x07: baudrate = CBR_600;     break;
			case 0x08: baudrate = CBR_1200;    break;
			case 0x09: // fall through
			case 0x0A: baudrate = CBR_2400;    break;
			case 0x0B: // fall through
			case 0x0C: baudrate = CBR_4800;    break;
			case 0x0D: // fall through
			case 0x0E: baudrate = CBR_9600;    break;
			case 0x0F: baudrate = CBR_19200;   break;
		}

		// UPDATE THE BYTE SIZE
		switch (controlbyte & 0x60)
		{
			case 0x00: bytesize = 8;  break;
			case 0x20: bytesize = 7;  break;
			case 0x40: bytesize = 6;  break;
			case 0x60: bytesize = 5;  break;
		}

		// UPDATE THE NUMBER OF STOP BITS
		if (controlbyte & 0x80)
		{
		if ((bytesize == 8) && (parity == NOPARITY))
			stopbits = ONESTOPBIT;
		else if ((bytesize == 5) && (parity == NOPARITY))
			stopbits = ONE5STOPBITS;
		else
			stopbits = TWOSTOPBITS;
		}
		else
			stopbits = ONESTOPBIT;

		UpdateCommState();
	}
	return controlbyte;
}

//===========================================================================
BYTE __stdcall CommDipSw (WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	int result = 0;

	switch(address & 0x0F)
	{
		case 0x01:
			switch(DEF_BAUDRATE)
			{
				case CBR_110:
					result = 0x50;
				break;

				case CBR_300:
					result = 0x60;
				break;

				case CBR_600:
					result = 0x70;
				break;

				case CBR_1200:
					result = 0x80;
				break;

				case CBR_2400:
					result = 0xA0;
				break;

				case CBR_4800:
					result = 0xC0;
				break;

				case CBR_9600:
					result = 0xE0;
				break;

				case CBR_19200:
					result = 0xF0;
				break;
			} // switch baudrate
			//
		break; // case 0x01

		case 0x02:
			result |= (DEF_STOPBITS == ONESTOPBIT) ? 0x80 : 0x00;
			result |= (DEF_BYTESIZE == 8)          ? 0x40 : 0x00;
			result |= (DEF_PARITY   == EVENPARITY) ? 0x20 : 0x00;
			result |= (DEF_PARITY   == NOPARITY)   ? 0x10 : 0x00;
		break;
	}
	return result;
}

//===========================================================================
BYTE __stdcall CommReceive (WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	BYTE result = 0;

#ifdef SERIAL_DEBUG
	int  be;
	char c[12] = "Recebido: ";
#endif

	if (!CheckComm() || serialport == 5)
		return 0;
	if (!recvbytes)
		CheckReceive();
	if (recvbytes)
	{
		result = recvbuffer[0];
#ifdef SERIAL_DEBUG
		if (zz != 1)
		{
			WriteFile(arqdebug, c, 10, (unsigned long *)&be, NULL);
			zz = 1;
		}
		WriteFile(arqdebug, &result, 1, (unsigned long *)&be, NULL);
#endif
		if (--recvbytes)
			memmove(recvbuffer, recvbuffer+1, recvbytes);
	}
	return result;
}

//===========================================================================
BYTE __stdcall CommStatus (WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	DWORD modemstatus = 0;

	if (!CheckComm())
		return 0x70;
	if (serialport == 5)
		return 0x10;
	if (!recvbytes)
		CheckReceive();
	GetCommModemStatus(commhandle, &modemstatus);
	return 0x10 | (recvbytes                  ? 0x08 : 0x00)
				| ((modemstatus & MS_RLSD_ON) ? 0x00 : 0x20)
				| ((modemstatus & MS_DSR_ON)  ? 0x00 : 0x40);
}

//===========================================================================
BYTE __stdcall CommTransmit (WORD programcounter, BYTE address, BYTE write, BYTE value)
{
	DWORD byteswritten;

#ifdef SERIAL_DEBUG
	int be;
	char c[10] = "Enviado: ";
#endif

	if (!CheckComm())
		return 0;
	if (serialport == 5)
		value &= 0x7F;
	WriteFile(commhandle, &value, 1, &byteswritten, NULL);
#ifdef SERIAL_DEBUG
	if (zz != 2)
	{
		WriteFile(arqdebug, c, 9, (unsigned long *)&be, NULL);
		zz = 2;
	}
	WriteFile(arqdebug, &value, 1, (unsigned long *)&be, NULL);
#endif
	return 0;
}

// EOF