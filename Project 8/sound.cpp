#include "Project 8.h"
#include "sound.h"
#include "window.h"

#include <dsound.h>

#pragma comment (lib,"dsound.lib")

LPDIRECTSOUND8 ds8;
DSBUFFERDESC bufdesc;
WAVEFORMATEX waveFormat;

void InitDSound()
{
	int seconds = 1;

	DirectSoundCreate8(NULL, &ds8, NULL);
	ds8->SetCooperativeLevel(GetWindowHwnd(), DSSCL_NORMAL);

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = 1;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample/8)*waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

    memset(&bufdesc, 0, sizeof(DSBUFFERDESC)); 
    bufdesc.dwSize = sizeof(DSBUFFERDESC); 
    bufdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS;
	bufdesc.dwBufferBytes = waveFormat.nAvgBytesPerSec * seconds;
	bufdesc.guid3DAlgorithm = GUID_NULL;
	bufdesc.lpwfxFormat = &waveFormat;
}

SOUNDBUFFER CreateBuffer()
{
	LPDIRECTSOUNDBUFFER ret;

	ds8->CreateSoundBuffer(&bufdesc, &ret, NULL);
	ret->SetVolume(1000);

	return ((int*)ret);
}

void KillDSound()
{
	ds8->Release();
}

void WriteBuffer(int * DestBuf, short * SrcBuf, int datalen)
{
	void * writePtr1;
	void * writePtr2;
	unsigned long length1;
	unsigned long length2;

	LPDIRECTSOUNDBUFFER dsBuffer = (LPDIRECTSOUNDBUFFER) DestBuf;

	unsigned long WriteCur;

	dsBuffer->GetCurrentPosition(NULL, &WriteCur);

	dsBuffer->Lock(WriteCur, datalen, &writePtr1, &length1, &writePtr2, &length2, 0);
	memcpy(writePtr1, SrcBuf, length1);
	memcpy(writePtr2, SrcBuf + length1, length2);
	dsBuffer->Unlock(writePtr1, length1, writePtr2, length2);

	dsBuffer->SetCurrentPosition(WriteCur);
}

void PlayBuffer(int * Buffer)
{
	LPDIRECTSOUNDBUFFER dsBuffer = (LPDIRECTSOUNDBUFFER) Buffer;

	dsBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

void StopBuffer(int * Buffer)
{
	LPDIRECTSOUNDBUFFER dsBuffer = (LPDIRECTSOUNDBUFFER) Buffer;

	dsBuffer->Stop();
}

void ReleaseBuffer(int * Buffer)
{
	LPDIRECTSOUNDBUFFER dsBuffer = (LPDIRECTSOUNDBUFFER) Buffer;

	dsBuffer->Release();
}
