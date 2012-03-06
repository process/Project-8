#include <Windows.h>

#include "Project 8.h"

unsigned char bit;
unsigned char strobe;

void StartInput()
{
    bit = 0;
    strobe = 0;
}

void ReadInput()
{
    switch(bit)
    {
        case 0:
            if(GetAsyncKeyState(0x58)) //X   A
                RAM[0x4016] = 0x41;
            else RAM[0x4016] = 0x40;
            break;

        case 1:
            if(GetAsyncKeyState(0x5A)) //Z   B
                RAM[0x4016] = 0x41;
            else RAM[0x4016] = 0x40;
            break;

        case 2:
            if(GetAsyncKeyState(VK_SHIFT)) //SELECT
                RAM[0x4016] = 0x41;
            else RAM[0x4016] = 0x40;
            break;

        case 3:
            if(GetAsyncKeyState(VK_RETURN)) //START
                RAM[0x4016] = 0x41;
            else RAM[0x4016] = 0x40;
            break;

        case 4:
            if(GetAsyncKeyState(VK_UP)) //UP
                RAM[0x4016] = 0x41;
            else RAM[0x4016] = 0x40;
            break;

        case 5:
            if(GetAsyncKeyState(VK_DOWN)) //DOWN
                RAM[0x4016] = 0x41;
            else RAM[0x4016] = 0x40;
            break;

        case 6:
            if(GetAsyncKeyState(VK_LEFT)) //LEFT
                RAM[0x4016] = 0x41;
            else RAM[0x4016] = 0x40;
            break;

        case 7:
            if(GetAsyncKeyState(VK_RIGHT)) //RIGHT
                RAM[0x4016] = 0x41;
            else RAM[0x4016] = 0x40;
            break;

        default:
            RAM[0x4016] = 0x40;
    }

    bit++;
}

void ReadInput2()
{
    RAM[0x4017] = 0x40;
}

void WriteInput()
{
    if(RAM[0x4016] == 1 && !strobe)
    {
        strobe = 1;
    }

    else if(RAM[0x4016] == 0 && strobe) 
    {
        bit = 0;
        strobe = 0;
    }

    else if(strobe)
    {
        strobe = 0;
    }
}
