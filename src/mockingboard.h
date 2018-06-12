#pragma once

extern bool       g_bMBTimerIrqActive;
#ifdef _DEBUG
extern UINT32	g_uTimer1IrqCount;	// DEBUG
#endif

#define MAX_SAMPLES (16*1024)

typedef struct
{
	union
	{
		struct
		{
			BYTE l;
			BYTE h;
		};
		USHORT w;
	};
} IWORD;

typedef struct
{
	BYTE ORB;				// $00 - Port B
	BYTE ORA;				// $01 - Port A (with handshaking)
	BYTE DDRB;				// $02 - Data Direction Register B
	BYTE DDRA;				// $03 - Data Direction Register A
	//
	// $04 - Read counter (L) / Write latch (L)
	// $05 - Read / Write & initiate count (H)
	// $06 - Read / Write & latch (L)
	// $07 - Read / Write & latch (H)
	// $08 - Read counter (L) / Write latch (L)
	// $09 - Read counter (H) / Write latch (H)
	IWORD TIMER1_COUNTER;
	IWORD TIMER1_LATCH;
	IWORD TIMER2_COUNTER;
	IWORD TIMER2_LATCH;
	//
	BYTE SERIAL_SHIFT;		// $0A
	BYTE ACR;				// $0B - Auxiliary Control Register
	BYTE PCR;				// $0C - Peripheral Control Register
	BYTE IFR;				// $0D - Interrupt Flag Register
	BYTE IER;				// $0E - Interrupt Enable Register
	BYTE ORA_NO_HS;			// $0F - Port A (without handshaking)
} SY6522;

typedef struct
{
	BYTE DurationPhonome;
	BYTE Inflection;		// I10..I3
	BYTE RateInflection;
	BYTE CtrlArtAmp;
	BYTE FilterFreq;
	//
	BYTE CurrentMode;		// b7:6=Mode; b0=D7 pin (for IRQ)
} SSI263A;

typedef struct
{
	SY6522		RegsSY6522;
	BYTE		RegsAY8910[16];
	SSI263A		RegsSSI263;
	BYTE		nAYCurrentRegister;
	bool		bTimer1IrqPending;
	bool		bTimer2IrqPending;
	bool		bSpeechIrqPending;
} MB_Unit;

const UINT MB_UNITS_PER_CARD = 2;

enum eSOUNDCARDTYPE {SC_UNINIT=0, SC_NONE, SC_MOCKINGBOARD, SC_PHASOR};	// Apple soundcard type

void	MB_Initialize();
void	MB_Reinitialize();
void	MB_Destroy();
void    MB_Reset();
//void    MB_InitializeIO(LPBYTE pCxRomPeripheral, UINT uSlot4, UINT uSlot5);
void    MB_Mute();
void    MB_Demute();
void    MB_StartOfCpuExecute();
void    MB_EndOfVideoFrame();
void    MB_CheckIRQ();
void    MB_UpdateCycles(ULONG uExecutedCycles);
eSOUNDCARDTYPE MB_GetSoundcardType();
void    MB_SetSoundcardType(eSOUNDCARDTYPE NewSoundcardType);
double  MB_GetFramePeriod();
bool    MB_IsActive();
DWORD   MB_GetVolume();
void    MB_SetVolume(DWORD dwVolume, DWORD dwVolumeMax);

BYTE __stdcall MBLeitura(WORD programcounter, BYTE address, BYTE write, BYTE value);
BYTE __stdcall MBEscrita(WORD programcounter, BYTE address, BYTE write, BYTE value);
int MBConfiguraSlot(int Slot);
void MBRetiraSlot();
void MBVerifica(WORD Endereco, BOOL write, BYTE value);
