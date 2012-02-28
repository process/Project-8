#include "Project 8.h"
#include "windows.h"

#include "window.h"
#include "resource.h"
#include "CPU.h"
#include "debug.h"

#include <cstdio>

#pragma comment (lib, "winmm.lib")

INT_PTR CALLBACK DebugWndProc(HWND hwnd, UINT msg, 
						 WPARAM wParam, LPARAM lParam);

HWND hwndDebug;
char debug = 0;
unsigned short BreakAddress = 0;

void GetPauseInfo()
{
	if(!debug) return;
	
	HWND TEXT_A = GetDlgItem(hwndDebug, REG_A);
	HWND TEXT_X = GetDlgItem(hwndDebug, REG_X);
	HWND TEXT_Y = GetDlgItem(hwndDebug, REG_Y);
	HWND TEXT_PC = GetDlgItem(hwndDebug, REG_PC);
	HWND TEXT_SP = GetDlgItem(hwndDebug, REG_SP);
	HWND TEXT_P = GetDlgItem(hwndDebug, REG_P);

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

void MakeDebugger(HINSTANCE hInst)
{
	if(debug) return;

	hwndDebug = CreateDialog(hInst, MAKEINTRESOURCE(DEBUGDIALOG), NULL, DebugWndProc);

	debug = 1;
}

void UpdateFPS()
{
	if(!debug) return;

	static int fpsTime = 0;
	static int fpsCount = 0;

	fpsCount++;

	if(timeGetTime() - fpsTime >= 1000)
	{
		char fpsString[4];
		_itoa(fpsCount, fpsString, 10);
		SetWindowText(GetDlgItem(hwndDebug, FPS), fpsString);
		fpsCount = 0;
		fpsTime = timeGetTime();
	}
}

void UpdateIPS()
{
	if(!debug) return;

	static int StartTime = 0;
	static int NumInstructions = 0;

	NumInstructions++;

	if(timeGetTime() - StartTime >= 1000)
	{
		char buffer[25];
		_itoa(NumInstructions, buffer, 10);
		SetWindowText(GetDlgItem(hwndDebug, IPS), buffer);
		NumInstructions = 0;
		StartTime = timeGetTime();
	}
}

char CheckBreakAddr(unsigned short addr)
{
	return (addr == BreakAddress);
}

void GetBreakAddr()
{
	char buffer[5];
	GetDlgItemText(hwndDebug, BREAKADDR, buffer, 5);
	int temp, temp2 = 1;

	BreakAddress = 0;

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

		BreakAddress += temp * temp2;
		temp = 0;
		temp2 = 1;
	}
}

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
			debug = 0;
			BreakAddress = 0;
			if(status.pause)
			{
				status.pause = 0;
				status.play = 1;
			}
			DestroyWindow(hwnd);
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
						if(status.pause)
							status.play = 1;
						status.pause = 0;
						break;

					case STEPBUTTON:
						if(status.play)
						{
							status.play = 0;
							status.pause = 1;
						}
						RunCPU(1);
						GetPauseInfo();
						break;

					case RESETBUTTON:
						if(status.play)
							StartNES();
						break;

					case BREAKCHK:
						if(SendMessage(GetDlgItem(hwnd, BREAKCHK), BM_GETCHECK, 0, 0) == BST_CHECKED)
							GetBreakAddr();
						else BreakAddress = 0;
						break;
				}
			}
			return 1;
	}

	return 0;
}
