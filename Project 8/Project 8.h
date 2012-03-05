#include "windows.h"

void UpdateNES();
void StartNES();
void ResetNES();
void PauseNES();

void InitDevice(HWND hwnd);

extern unsigned char RAM[0x10000];
extern unsigned char VRAM[0x4000]; 
extern unsigned char SPRRAM[0x100];

extern struct status
{
    char run;
    char play;
    char ready;
    char pause;
    unsigned char logging;
    unsigned char sound;
}status;

extern struct CPU
{
    unsigned short NMI;
    unsigned char inNMI;
}CPU;

extern struct ROMINFO
{
    char id[4];
    char prg;
    char vram;
    char info1;
    char info2;
    char ram; //possibly unused?
    char info3;
    char reserved[6];
} ROMINFO;
