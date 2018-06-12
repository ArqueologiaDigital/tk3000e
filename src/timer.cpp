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

// Fun��es de Temporiza��o

#include "stdhdr.h"
#include "timer.h"
#include "tk3000e.h"
#include "janela.h"

//=============================================================================

static DWORD dwAdviseToken;
static IReferenceClock *pRefClock = NULL;
static HANDLE hSemaphore = NULL;
static bool g_bTimerActive = false;
static DWORD g_dwLastCyclePeriod = 0;


//=============================================================================
void Timer_InitTimer()
{
//	hSemaphore = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
	hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
	if (hSemaphore == NULL)
		fprintf(stderr, "Error creating semaphore\n");

	if (CoCreateInstance(CLSID_SystemClock, NULL, CLSCTX_INPROC,
                         IID_IReferenceClock, (LPVOID*)&pRefClock) != S_OK)
	{
		FrameMostraMensagemErro("Erro ao criar o temporizador");
	}
	if (!pRefClock)
	{
		FrameMostraMensagemErro("Erro ao criar o temporizador");
		ExitProcess(-1);
	}
}

//=============================================================================
void Timer_UninitTimer()
{
	Timer_StopTimer();
	pRefClock->Release();

	if (CloseHandle(hSemaphore) == 0)
	{
		FrameMostraMensagemErro("Erro ao terminar o temporizador");
	}
}

//=============================================================================
void Timer_WaitTimer()
{
	if(!g_bTimerActive)
		return;

	WaitForSingleObject(hSemaphore, INFINITE);
}

//=============================================================================
void Timer_StartTimer(DWORD dwCyclePeriod)
{
	if(g_bTimerActive && (g_dwLastCyclePeriod == dwCyclePeriod))
		return;

	Timer_StopTimer();

	REFERENCE_TIME rtPeriod = (REFERENCE_TIME) ((double)dwCyclePeriod / CLK_6502 * 1.e7);	// In units of 100ns
	REFERENCE_TIME rtNow;

	HRESULT hr = pRefClock->GetTime(&rtNow);
	// S_FALSE : Returned time is the same as the previous value

	if ((hr != S_OK) && (hr != S_FALSE))
	{
		FrameMostraMensagemErro("Erro ao iniciar o temporizador");
		return;
	}

	if (pRefClock->AdvisePeriodic(rtNow, rtPeriod, hSemaphore, &dwAdviseToken) != S_OK)
	{
		FrameMostraMensagemErro("Erro ao iniciar o temporizador");
		return;
	}

	g_dwLastCyclePeriod = dwCyclePeriod;
	g_bTimerActive = true;
}

//=============================================================================
void Timer_StopTimer()
{
	if(!g_bTimerActive)
		return;

	if (pRefClock->Unadvise(dwAdviseToken) != S_OK)
	{
		FrameMostraMensagemErro("Erro ao parar o temporizador");
		return;
	}

	g_bTimerActive = false;
}

// EOF