//Some code was taken from the PlayerPRO project, but just the DirectSound loading, clean-up, and playback
//#include "stdafx.h"
#include <windows.h>
#include <dsound.h>
//This is needed to get M_PI
#define _USE_MATH_DEFINES
#include <math.h>
#include "SoundHandler.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")

#include <cguid.h>

#define BUFSIZE 22050

BOOL LC3Sound::LoadSamp(LPDIRECTSOUND lpDirectSound,
              LPDIRECTSOUNDBUFFER *lplpDsb,
              LPBYTE samp, UINT length, UINT flags)
{
    DSBUFFERDESC dsbdesc;
    HRESULT hr;
    WAVEFORMATEX pcmwf;

    // Set up wave format structure.
    memset(&pcmwf, 0, sizeof(WAVEFORMATEX));
    pcmwf.wFormatTag			= WAVE_FORMAT_PCM;
    pcmwf.nChannels = 1;
    pcmwf.nSamplesPerSec		= 22050;
    pcmwf.wBitsPerSample		= 8;
    pcmwf.nBlockAlign			= pcmwf.nChannels * (pcmwf.wBitsPerSample/8);
    pcmwf.nAvgBytesPerSec		= pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
    
    // Set up DSBUFFERDESC structure.
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out.
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
//    dsbdesc.dwFlags =	DSBCAPS_STATIC | DSBCAPS_CTRLDEFAULT | DSBCAPS_GETCURRENTPOSITION2;	//;	//;	// | flags;	//DSBCAPS_STATIC | DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY| DSBCAPS_GLOBALFOCUS|DSBCAPS_GETCURRENTPOSITION2|flags;
  
//  	dsbdesc.dwFlags =	DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|flags;
  	
  	dsbdesc.dwFlags =  DSBCAPS_GLOBALFOCUS;
    dsbdesc.dwBufferBytes = length;
    dsbdesc.dwReserved = 0;
    dsbdesc.lpwfxFormat = &pcmwf;
    hr = lpDirectSound->CreateSoundBuffer( &dsbdesc, lplpDsb, NULL);
    if(hr == DS_OK)
    {
//        lpDirectSound->lpVtbl->SetCooperativeLevel(
//                lpDirectSound,hwnd, DSSCL_EXCLUSIVE);
        // Succeeded! Valid interface is in *lplpDsb.
  //      WriteDataToBuffer(*lplpDsb, 0, samp,length);
//        lpDirectSound->lpVtbl->SetCooperativeLevel(
//                lpDirectSound,hwnd, DSSCL_NORMAL);
    }
    else
    {
    	//DEBUG debugger( TranslateDSError( hr));
    	
      *lplpDsb=NULL;
      return 0;
    }
    return 1;
}

BOOL LC3Sound::WriteDataToBuffer(
    LPDIRECTSOUNDBUFFER lpDsb,
    DWORD dwOffset,
    LPBYTE lpbSoundData,
    DWORD dwSoundBytes)
{
    LPVOID lpvPtr1;
    DWORD dwBytes1;
    LPVOID lpvPtr2;
    DWORD dwBytes2;
    HRESULT hr;
    
	//	lpDirectSound->lpVtbl->SetCooperativeLevel( lpDirectSound, hwnd, DSSCL_EXCLUSIVE);
		
    // Obtain write pointer.
    hr = lpDsb->Lock(dwOffset, dwSoundBytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);

    // If we got DSERR_BUFFERLOST, restore and retry lock.
    if(DSERR_BUFFERLOST == hr) {
        lpDsb->Restore();
        hr = lpDsb->Lock(dwOffset, dwSoundBytes, &lpvPtr1,
             &dwBytes1, &lpvPtr2, &dwBytes2, 0);
    }
    if(DS_OK == hr) {
        // Write to pointers.
        memcpy(lpvPtr1, lpbSoundData, dwBytes1);
        if(NULL != lpvPtr2) {
            memcpy(lpvPtr2, lpbSoundData+dwBytes1, dwBytes2);
        }
        // Release the data back to DirectSound.
        hr = lpDsb->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);
        if(DS_OK == hr) {
            // Success!
            return TRUE;
        }
    }
    
//		lpDirectSound->lpVtbl->SetCooperativeLevel( lpDirectSound, hwnd, DSSCL_NORMAL);
    
    // If we got here, then we failed Lock, Unlock, or Restore.
    return FALSE;
}

bool LC3Sound::DirectSave( char* sndData)
{
	if(tableIndex != 0)
	{
		timeLeft -= 10;
		if(timeLeft <= 0)
		{
			PopTable();
		}
		for(int i = 0; i < BUFSIZE; i++)
		{
			sndData[i] = 2 * sin(2 * M_PI * i * table[0].tone / 22050.0);
		}
		return true;
	}
	return false;
}

void LC3Sound::PopTable()
{
	for(int i = 0; i < 9; i++)
	{
		table[i] = table[i + 1];
	}
	table[9].length = 0;
	table[9].tone = 0;
	timeLeft = table[0].length;
	tableIndex--;
}

void LC3Sound::TimeProc(
	 UINT  IDEvent,			/* identifies timer event */
	 UINT  uReserved,		/* not used */
	 DWORD_PTR  dwUser,			/* application-defined instance data */
	 DWORD_PTR  dwReserved1,	/* not used */
	 DWORD_PTR  dwReserved2		/* not used */
)
{
	DWORD 		pos, posp;
	static volatile int timersema=0;

	/* use semaphore to prevent entering the mixing routines twice.. do we need this ? */

	if(++timersema==1)
	{
		//MADDriverRec	*WinMADDriver = (MADDriverRec*) dwUser;
		
		lpSwSamp->GetCurrentPosition( &pos, &posp);
		
		//if(pos > BUFSIZE )
		{
			//WinMADDriver->OnOff = false;
			
			if( !DirectSave( soundData))
			{
				memset(soundData, 0x80, BUFSIZE);
				//memset(soundData, 0, BUFSIZE);
			}
			
			if( !WriteDataToBuffer( lpSwSamp, 0, (unsigned char*) soundData, BUFSIZE))
			{
				//DEBUG 	debugger("ERR");
			}
		}
		
	}
	timersema--;
}

BOOL LC3Sound::AppCreateWritePrimaryBuffer(
    LPDIRECTSOUND lpDirectSound,
    LPDIRECTSOUNDBUFFER *lplpDsb,
    HWND hwnd)
{
    DSBUFFERDESC dsbdesc;
    HRESULT hr;
    WAVEFORMATEX pcmwf;

    // Set up wave format structure.
    memset(&pcmwf, 0, sizeof(WAVEFORMATEX));
    pcmwf.wFormatTag 		= WAVE_FORMAT_PCM;
    
    pcmwf.nChannels = 1;
    pcmwf.nSamplesPerSec		= 22050;
    pcmwf.wBitsPerSample		= 8;
    pcmwf.nBlockAlign			= pcmwf.nChannels * (pcmwf.wBitsPerSample/8);
    pcmwf.nAvgBytesPerSec		= pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
    
    // Set up DSBUFFERDESC structure.
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out.
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;	//;	//DSBCAPS_GLOBALFOCUS;	//;
    dsbdesc.dwBufferBytes = 0; // Buffer size is determined
                               // by sound hardware.
    dsbdesc.lpwfxFormat = NULL; // Must be NULL for primary buffers.
    // Obtain write-primary cooperative level.
    hr = lpDirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
    if(DS_OK == hr)
    {
        // Succeeded! Try to create buffer.
        hr = lpDirectSound->CreateSoundBuffer(
            &dsbdesc, lplpDsb, NULL);
        if(DS_OK == hr) {
            // Succeeded! Set primary buffer to desired format.
            hr = (*lplpDsb)->SetFormat(&pcmwf);
            (*lplpDsb)->Play(0, 0, DSBPLAY_LOOPING);
            return TRUE;
        }
        //DEBUG else debugger( TranslateDSError( hr));
    }
    // If we got here, then we failed SetCooperativeLevel.
    // CreateSoundBuffer, or SetFormat.
    *lplpDsb = NULL;
    return FALSE;
}

void CALLBACK TimeProcC(
	 UINT  IDEvent,			/* identifies timer event */
	 UINT  uReserved,		/* not used */
	 DWORD_PTR  dwUser,			/* application-defined instance data */
	 DWORD_PTR  dwReserved1,	/* not used */
	 DWORD_PTR  dwReserved2		/* not used */
)
{
	LC3Sound *sound = (LC3Sound*)dwUser;
	sound->TimeProc(IDEvent, uReserved, NULL, dwReserved1, dwReserved2);
}

LC3Sound::LC3Sound()
{
	memset(table, 0, sizeof(table));
	timeLeft = 0;
	tableIndex = 0;
	hwnd = GetForegroundWindow();
	DirectSoundCreate(NULL, &lpDirectSound, NULL);
	AppCreateWritePrimaryBuffer( lpDirectSound, &lpDirectSoundBuffer, hwnd);
	LoadSamp(lpDirectSound, &lpSwSamp, NULL, BUFSIZE, DSBCAPS_LOCSOFTWARE);
	lpDirectSound->SetCooperativeLevel(hwnd, DSSCL_NORMAL);
	//lpDirectSound->Initialize(&soundGUID);
	lpSwSamp->Play(0, 0, DSBPLAY_LOOPING);
	soundData = (char*)malloc(BUFSIZE);
	gwid = timeSetEvent( 40, 40, TimeProcC, (unsigned long) this, TIME_PERIODIC);
}

LC3Sound::~LC3Sound()
{
	/* stop the timer */
	timeEndPeriod( 20);
	timeKillEvent( gwid);

	
	lpSwSamp->Stop();
	lpSwSamp->Release();
	lpSwSamp = NULL;

	lpDirectSoundBuffer->Stop();
  	lpDirectSoundBuffer->Release();
  
	lpDirectSound->Release();
	lpDirectSound = NULL;
	free(soundData);
}

int LC3Sound::AddTimeTable(LC3TimeTable theTable)
{
	if(theTable.length && theTable.tone)
	{
		if(tableIndex < 9)
		{
			LC3TimeTable *aTable = &table[tableIndex];
			*aTable = theTable;
			if(tableIndex == 0)
			{
				timeLeft = aTable->length;
			}
			tableIndex++;
			return 1;
		} else return -1;
	} else {
		if (timeLeft == 0)
			return 0;
		else
			return 1;
	}
}