#include <Windows.h>

#include "Project 8.h"

#include "NES.h"
#include "window.h"
#include "resource.h"
#include "CPU.h"
#include "Debug.h"

#include <stdio.h>

#pragma comment (lib, "winmm.lib")

INT_PTR CALLBACK DebugWndProc(HWND hwnd, UINT msg, 
                         WPARAM wParam, LPARAM lParam);

//Initialize variables
HWND Debug::debugHWND = NULL;
bool Debug::debugging = false;
unsigned short Debug::breakAddr = 0;

void Debug::updateFPS()
{
    if(!Debug::debugging) return;

    static int fpsTime = 0;
    static int fpsCount = 0;

    fpsCount++;

    if(timeGetTime() - fpsTime >= 1000)
    {
        char fpsString[4];
        _itoa(fpsCount, fpsString, 10);
        SetWindowText(GetDlgItem(Debug::debugHWND, FPS), fpsString);
        fpsCount = 0;
        fpsTime = timeGetTime();
    }
}

void Debug::updateIPS()
{
    if(!Debug::debugging) return;

    static int StartTime = 0;
    static int NumInstructions = 0;

    NumInstructions++;

    if(timeGetTime() - StartTime >= 1000)
    {
        char buffer[25];
        _itoa(NumInstructions, buffer, 10);
        SetWindowText(GetDlgItem(Debug::debugHWND, IPS), buffer);
        NumInstructions = 0;
        StartTime = timeGetTime();
    }
}

void Debug::getBreakAddr()
{
    if(SendMessage(GetDlgItem(Debug::debugHWND, BREAKCHK), BM_GETCHECK, 0, 0) != BST_CHECKED)
        Debug::breakAddr = 0;

    char buffer[5];
    GetDlgItemText(Debug::debugHWND, BREAKADDR, buffer, 5);
    int temp, temp2 = 1;

    Debug::breakAddr = 0;

    for(int i = 3; i > -1; i--)
    {
        if(buffer[i] == 'a' || buffer[i] == 'A')
            temp = 10;
        else if(buffer[i] == 'b' || buffer[i] == 'B')
            temp = 11;
        else if(buffer[i] == 'c' || buffer[i] == 'C')
            temp = 12;
        else if(buffer[i] == 'd' || buffer[i] == 'D')
            temp = 13;
        else if(buffer[i] == 'e' || buffer[i] == 'E')
            temp = 14;
        else if(buffer[i] == 'f' || buffer[i] == 'F')
            temp = 15;
        else temp = buffer[i] - 48;

        for(int j = (i - 3) * -1; j > 0; j--)
        {
            temp2 *= 16;
        }

        Debug::breakAddr += temp * temp2;
        temp = 0;
        temp2 = 1;
    }
}

void Debug::makeDebugger()
{
    if(Debug::debugging) return;

    Debug::debugHWND = CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(DEBUGDIALOG), NULL, DebugWndProc);

    Debug::debugging = true;
    Debug::breakAddr = 0;
}

bool Debug::checkBreakAddr(unsigned short addr)
{
    return (Debug::breakAddr == addr);
}

void Debug::getPauseInfo()
{
    if(!Debug::debugging) return;
    
    HWND TEXT_A = GetDlgItem(Debug::debugHWND, REG_A);
    HWND TEXT_X = GetDlgItem(Debug::debugHWND, REG_X);
    HWND TEXT_Y = GetDlgItem(Debug::debugHWND, REG_Y);
    HWND TEXT_PC = GetDlgItem(Debug::debugHWND, REG_PC);
    HWND TEXT_SP = GetDlgItem(Debug::debugHWND, REG_SP);
    HWND TEXT_P = GetDlgItem(Debug::debugHWND, REG_P);

    extern unsigned char A;
    extern unsigned char X;
    extern unsigned char Y;
    extern unsigned short PC;
    extern unsigned char S;
    extern unsigned char P;

    char buffer[5];

    sprintf_s(buffer, "%X", A);
    SetWindowText(TEXT_A, buffer);
    sprintf_s(buffer, "%X", X);
    SetWindowText(TEXT_X, buffer);
    sprintf_s(buffer, "%X", Y);
    SetWindowText(TEXT_Y, buffer);
    sprintf_s(buffer, "%X", PC);
    SetWindowText(TEXT_PC, buffer);
    sprintf_s(buffer, "%X", S);
    SetWindowText(TEXT_SP, buffer);
    sprintf_s(buffer, "%X", P);
    SetWindowText(TEXT_P, buffer);
}

void Debug::destroyDebugger()
{
    Debug::debugging = false;
    Debug::breakAddr = 0;

    if(Project8::paused)
    {
        Project8::paused = false;
        Project8::playing = true;
    }

    DestroyWindow(Debug::debugHWND);
    Debug::debugHWND = NULL;
}

//-------------------------------

INT_PTR CALLBACK DebugWndProc(HWND hwnd, UINT msg, 
                         WPARAM wParam, LPARAM lParam)
{
    HWND ComBox;

    switch(msg)
    {
        case WM_INITDIALOG:
            ComBox = GetDlgItem(hwnd, RAMCHOOSE);
            SendMessage(ComBox, CB_INSERTSTRING, -1, (LPARAM)"RAM");
            SendMessage(ComBox, CB_INSERTSTRING, -1, (LPARAM)"VRAM");
            SendMessage(ComBox, CB_INSERTSTRING, -1, (LPARAM)"SPRRAM");
            break;
        case WM_DESTROY:
        case WM_CLOSE:
        case WM_QUIT:
            Debug::destroyDebugger();
            return 1;

        case WM_COMMAND:
            if((wParam >> 16) == BN_CLICKED)
            {
                switch(wParam & 0xFFFF)
                {
                    case PAUSEBUTTON:
                        PauseNES();
                        break;

                    case RUNBUTTON:
                        if(Project8::paused)
                            Project8::playing = true;
                        Project8::paused = 0;
                        break;

                    case STEPBUTTON:
                        if(Project8::playing)
                        {
                            Project8::playing = false;
                            Project8::paused = true;
                        }
                        ((CPU*)getCPU())->RunCPU(1);
                        Debug::getPauseInfo();
                        break;

                    case RESETBUTTON:
                        if(Project8::playing)
                            StartNES();
                        break;

                    case BREAKCHK:
                        Debug::getBreakAddr();
                        break;
                }
            }
            return 1;
    }

    return 0;
}
