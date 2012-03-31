#include <Windows.h>
#include <stdio.h>

#include "Project 8.h"
#include "window.h"
#include "CPU.h"
#include "NES.h"

HANDLE LogFile;
char LogBuf[256];

//PIG DISGUSTING
extern Window window;

//Open the logging file and update the menus 
void StartLogging()
{
    int result = 
        MessageBoxEx(NULL, "Note: this will log all CPU info to a text file\
 named log.txt in the same directory as the program. This file will become\
 large very quickly. Are you sure you what to do this?", "Project 8", 
 MB_OKCANCEL | MB_ICONWARNING, 0);
    
    if(result == IDCANCEL)
        return;

    Project8::logging = true;
    HMENU menu = GetMenu(window.getWindowHwnd());
    ModifyMenu(menu, 9005 /*ID_LOG*/, MF_BYCOMMAND | MF_CHECKED, 9005 /*ID_LOG*/, "Enable Logging");
    SetMenu(window.getWindowHwnd(), menu);

    LogFile =  CreateFile("log.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

//Adds a new instruction to the log
void UpdateLog()
{
    CPU* cpu = (CPU*)getCPU();

    //sprintf(LogBuf,
    //    "PC:%.4X Opcode:%.2X\tA:%.2X X:%.2X Y:%.2X\tS:%.2X P:%.2X\n", 
    //    cpu->PC, RAM[cpu->PC], cpu->A, cpu->X, cpu->Y, cpu->S, cpu->P);
    int len = strlen(LogBuf);
    unsigned long d = 0;

    WriteFile(LogFile, LogBuf, len, &d, NULL);
}

//Close the log file and update the menus
void EndLogging()
{
    Project8::logging = false;

    HMENU menu = GetMenu(window.getWindowHwnd());
    ModifyMenu(menu, 9005 /*ID_LOG*/, MF_BYCOMMAND, 9005 /*ID_LOG*/, "Enable Logging");
    SetMenu(window.getWindowHwnd(), menu);

    CloseHandle(LogFile);
}