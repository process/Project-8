#include <windows.h>
#include "Project 8.h"

#include "CPU.h"
#include "PPU.h"
#include "APU.h"
#include "input.h"
#include "window.h"
#include "debug.h"

CPU* cpu;

LARGE_INTEGER Frequency;

LARGE_INTEGER CPUCurrentTicks;
LARGE_INTEGER CPUStartTicks;
long long CPUTicksNeeded;

#define UPFREQ 100
//The frequency of the program updates

void UpdateNES()
{
    QueryPerformanceCounter(&CPUCurrentTicks);
    if(CPUCurrentTicks.QuadPart - CPUStartTicks.QuadPart >= CPUTicksNeeded)
    {
        QueryPerformanceCounter(&CPUStartTicks);
        
        unsigned int cycles = 1789772.5 / UPFREQ;
        RunAPU(cycles);
        cpu->RunCPU(cycles);
    }

    //Sleep(1);
}

void StartNES()
{
    cpu = new CPU;

    ResetNES();
    QueryPerformanceFrequency(&Frequency);

    CPUTicksNeeded = Frequency.QuadPart / UPFREQ; 
    QueryPerformanceFrequency(&CPUStartTicks);

    cpu->StartCPU();
    StartPPU();
    StartInput();
    StartAPU();

    Project8::playing = true;
}

void* getCPU()
{
    return (void*)cpu;
}

void ResetNES()
{
    ZeroMemory(RAM, 0x8000); //Don't clear program!
    ZeroMemory(VRAM + 0x2000, 0x2000); //Don't clear pattern tables!
    ZeroMemory(SPRRAM, 0x100);
}

void PauseNES()
{
    if(Project8::playing)
    {
        Project8::paused = true;
        Project8::playing = false;
        Debug::getPauseInfo();
    }
}