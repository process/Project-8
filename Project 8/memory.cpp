#include <Windows.h>
#include "Project 8.h"

#include "mem.h"

extern unsigned char S;

//Clears the memory
void InitMemory()
{ ZeroMemory(RAM, 0x10000); }

void CopyPRG(char * data, char index)
{
    if(index == 1)
    {
        memcpy(RAM+0x8000, data, 0x4000);
        memcpy(RAM+0xC000, data, 0x4000);
    }
    else
    {
        memcpy(RAM+0xC000, data, 0x4000);
    }
}

//Push a word to the stack
void pushw(unsigned short word)
{
    RAM[S+0x100] = (unsigned char) (word >> 8);
    S--;
    RAM[S+0x100] = (unsigned char) (word & 0xFF);
    S--;
}

//Pop a word from the stack
unsigned short popw()
{
    S++;
    unsigned char lo = RAM[S+0x100];
    S++;
    unsigned char hi = RAM[S+0x100];
    return ((hi << 8) | lo);
}

//Push a byte to the stack
void pushb(unsigned char byte)
{
    RAM[S+0x100] = byte;
    S--;
}

//Pop a byte from the stack
unsigned char popb()
{
    S++;
    return RAM[S+0x100];
}
