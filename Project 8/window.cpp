#include "Project 8.h"

#include "resource.h"

#include "file.h"
#include "window.h"
#include "graphics.h"
#include "CPU.h"
#include "logging.h"
#include "debug.h"

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, 
						 WPARAM wParam, LPARAM lParam);
void MakeMenu(HWND hwnd);
char logging = 0;

HWND hwndWindow;
HINSTANCE hInst;
WNDCLASSEX wc;

HWND MakeWindow(int width, int height,
				HINSTANCE hInstance)
{
	hInst = hInstance;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
    wc.hIcon = NULL; //LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(0);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Win";
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	RECT ClientWindow;
	ClientWindow.bottom = height;
	ClientWindow.left = 0;
	ClientWindow.right = width;
	ClientWindow.top = 0;

	AdjustWindowRectEx(&ClientWindow,WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		WS_OVERLAPPEDWINDOW, true,
		WS_EX_ACCEPTFILES);

	hwndWindow = CreateWindowEx(
		WS_EX_ACCEPTFILES,
		"Win", "Project 8",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		ClientWindow.right - ClientWindow.left,
		ClientWindow.bottom - ClientWindow.top,
		NULL, LoadMenu(hInst, MAKEINTRESOURCE(MYMENU)), 
		hInst, NULL);

	return hwndWindow;
}

HWND GetWindowHwnd()
{
	return hwndWindow;
}

void OpenDialog()
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwndWindow;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = "NES Roms (.nes)\0*.NES\0All Files (*.*)\0*.*";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = (char*)malloc(512);
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = "NES";

	GetOpenFileName(&ofn);

	if(ofn.lpstrFile[0] == '\0')
	{
		free(ofn.lpstrFile);
		return;
	}

	OpenFile(ofn.lpstrFile);
	free(ofn.lpstrFile);
}

void FileDropped(HDROP file)
{
	int length = DragQueryFile(file, 0, NULL, 0); 
	length++; //Accomodate for \0 at end of string
	char * filepath = (char*)malloc(length);
	DragQueryFile(file, 0, filepath, length);
	
	OpenFile(filepath);

	free(filepath);
}

void ChangeSound()
{
	if(status.sound == 1)
	{
		status.sound = 0;
		CheckMenuItem(GetMenu(hwndWindow), ID_MUTE, MF_CHECKED);
		return;
	}

	status.sound = 1;
	CheckMenuItem(GetMenu(hwndWindow), ID_MUTE, MF_CHECKED);
}

void ResizeWindow(int width, int height)
{
	RECT ClientWindow;
	ClientWindow.bottom = height;
	ClientWindow.left = 0;
	ClientWindow.right = width;
	ClientWindow.top = 0;

	AdjustWindowRectEx(&ClientWindow,WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		WS_OVERLAPPEDWINDOW, true,
		WS_EX_ACCEPTFILES);

	SetWindowPos(hwndWindow, 0, 0, 0, ClientWindow.right - ClientWindow.left,
		ClientWindow.bottom - ClientWindow.top, SWP_NOMOVE);
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, 
						 WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{	
		case WM_CREATE:
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_SIZE:
			ResizeViewport(lParam & 0xFFFF, (lParam>>16) & 0xFFFF);
			return 0;

		case WM_DROPFILES:
			FileDropped((HDROP) wParam);
			return 0;

		case WM_COMMAND:
			switch(wParam & 0xFFFF)
			{
				case ID_OPEN:
					OpenDialog();
					break;

				case ID_RESET:
					StartNES();
					break;

				case ID_1X:
					ResizeWindow(256, 240);
					break;

				case ID_2X:
					ResizeWindow(512, 480);
					break;

				case ID_3X:
					ResizeWindow(768, 720);
					break;

				case ID_DEBUG:
					MakeDebugger(hInst);
					break;
				
				case ID_MUTE:
					ChangeSound();
					break;

				case ID_LOG:
					if(status.logging == 0)
						StartLogging();
					else
						EndLogging();
					break;

				case ID_EXIT:
					PostQuitMessage(0);
					return 0;

				
			}
			return 0;
	}

	return DefWindowProc(hwnd,msg,wParam,lParam);
}
