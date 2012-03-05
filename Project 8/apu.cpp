#include "Project 8.h"
#include "APU.h"
#include "sound.h"

#include <math.h>

short SoundData[44100];

SOUNDBUFFER Pulse1Buf = 0;
SOUNDBUFFER Pulse2Buf = 0;
SOUNDBUFFER TriangleBuf = 0;

int interval = 0;

void StartAPU()
{
    ZeroMemory(SoundData, 44100);

    if(Pulse1Buf)
    {
        StopBuffer(Pulse1Buf);
        ReleaseBuffer(Pulse1Buf);
    }

    if(Pulse2Buf)
    {
        StopBuffer(Pulse2Buf);
        ReleaseBuffer(Pulse2Buf);
    }

    if(TriangleBuf)
    {
        StopBuffer(TriangleBuf);
        ReleaseBuffer(TriangleBuf);
    }

    Pulse1Buf = CreateBuffer();
    Pulse2Buf = CreateBuffer();
    TriangleBuf = CreateBuffer();

    PlayBuffer(Pulse1Buf);
    PlayBuffer(Pulse2Buf);
    //PlayBuffer(TriangleBuf);

    RAM[0x4000] = 0x30;
    RAM[0x4001] = 0x08;
    RAM[0x4002] = 0x00;
    RAM[0x4003] = 0x00;

    RAM[0x4004] = 0x30;
    RAM[0x4005] = 0x08;
    RAM[0x4006] = 0x00;
    RAM[0x4007] = 0x00;

    RAM[0x4008] = 0x30;
    RAM[0x4009] = 0x08;
    RAM[0x400A] = 0x00;
    RAM[0x400B] = 0x00;
}

void Pulse(char which)
{
    unsigned short addr;
    SOUNDBUFFER PulseBuf;

    if(which == 0)
    {
        addr = 0x4000;
        PulseBuf = Pulse1Buf;
    }

    else
    {
        addr = 0x4004;
        PulseBuf = Pulse2Buf;
    }

    short volume = RAM[addr] & 0x0F;
    volume = (volume * 20000) / 15;

    double duty = RAM[addr] >> 6;

    switch((int)duty)
    {
        case 0:
            duty = 0.125;
            break;

        case 1:
            duty = 0.25;
            break;

        case 2:
            duty = 0.5;
            break;

        case 3:
            duty = 0.75;
            break;
    }

    int period = RAM[addr+2] | ((RAM[addr+3] & 7) << 8);
    double frequency = 111860.78125 / (period + 1);
    double wavelength = 44100.0 / frequency;
        
    if(wavelength < 2)
    {
        memset(SoundData, 0, interval * 2);
        WriteBuffer(PulseBuf, SoundData, interval * 2);
        return;
    }

    if(interval % (int)wavelength)
        interval += (int)wavelength - (interval % (int)wavelength);

    int i;
    double pos;
    for(i = 0; i < interval; ++i)
    {
        pos = (double)i/wavelength;
        pos -= (int)pos;
        if(pos < duty)
            SoundData[i] = volume;
        else
            SoundData[i] = -volume;
    }

    WriteBuffer(PulseBuf, SoundData, interval*2);
}

void Triangle()
{
    unsigned short volume = 30000;

    int time = RAM[0x400A] | ((RAM[0x400B] & 7) << 8);
    if(!time) return;

    float frequency = 55930.4 / (time + 1);
    float wavelength = 44100 / frequency;

    unsigned int change = (volume * 2) / wavelength;
    int pos = -volume, toggle = 0;

    for(int i = 0; i < interval; ++i)
    {
        if(!toggle)
        {
            SoundData[i] = (unsigned char)pos;
            pos += change;

            if(pos >= volume)
            {
                pos = volume;
                toggle = 1;
            }
        }

        else
        {
            SoundData[i] = (unsigned char)pos;
            pos -= change;

            if(pos <= -volume)
            {
                pos = -volume;
                toggle = 0;
            }
        }
    }

    WriteBuffer(TriangleBuf, SoundData, interval*2);
}

void RunAPU(unsigned int cycles)
{
    if(status.sound == 0)
    {
        memset(SoundData, 0, 44100 * 2);
        WriteBuffer(Pulse1Buf, SoundData, 44100 * 2);
        WriteBuffer(Pulse2Buf, SoundData, 44100 * 2);
        return;
    }

    interval = (44100.0 * cycles) / 1789772.5;
    Pulse(0);

    interval = (44100.0 * cycles) / 1789772.5;
    Pulse(1);

    //Triangle();
}
