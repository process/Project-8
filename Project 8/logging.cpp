#include "Project 8.h"

#include <cstdio>

HANDLE LogFile;
extern HWND hwndWindow;
char LogBuf[256];

void StartLogging()
{
	int result = 
		MessageBoxEx(NULL, "Note: this will log all CPU info to a text file\
 named log.txt in the same directory as the program. This file will become\
 large very quickly. Are you sure you what to do this?", "Project 8", 
 MB_OKCANCEL | MB_ICONWARNING, 0);
	
	if(result == IDCANCEL)
		return;

	status.logging = 1;
	HMENU menu = GetMenu(hwndWindow);
	ModifyMenu(menu, 9005 /*ID_LOG*/, MF_BYCOMMAND | MF_CHECKED, 9005 /*ID_LOG*/, "Enable Logging");
	SetMenu(hwndWindow, menu);

	LogFile =  CreateFile("log.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

void UpdateLog()
{
	extern unsigned char A;
	extern unsigned char X;
	extern unsigned char Y;
	extern unsigned char S;
	extern unsigned char P;
	extern unsigned short PC;

	sprintf(LogBuf,"PC:%.4X Opcode:%.2X\tA:%.2X X:%.2X Y:%.2X\tS:%.2X P:%.2X\n", PC, RAM[PC], A, X, Y, S, P);
	int len = strlen(LogBuf);
	unsigned long d = 0;

	WriteFile(LogFile, LogBuf, len, &d, NULL);
}

void EndLogging()
{
	status.logging = 0;

	HMENU menu = GetMenu(hwndWindow);
	ModifyMenu(menu, 9005 /*ID_LOG*/, MF_BYCOMMAND, 9005 /*ID_LOG*/, "Enable Logging");
	SetMenu(hwndWindow, menu);

	CloseHandle(LogFile);
}