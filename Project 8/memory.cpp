#include "Project 8.h"

#include "mem.h"

void InitMemory(char prg)
{
    ZeroMemory(RAM, 0x10000);
}

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

unsigned char MemoryRead(int address)
{
    unsigned char ret;
    
    ret = 0;
    
    return ret;
}

void MemoryWrite(int address, unsigned char val)
{

}
