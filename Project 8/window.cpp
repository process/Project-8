#include <Windows.h>

#include "Project 8.h"

#include "resource.h"

#include "file.h"
#include "window.h"
#include "graphics.h"
#include "CPU.h"
#include "logging.h"
#include "debug.h"

Window::Window()
{
    hInst = GetModuleHandle(0); //Get hInstance of this process.
}

Window::Window(int width, int height)
{
    hInst = GetModuleHandle(0);
    makeWindow(width, height);
}

HWND Window::makeWindow(int width, int height)
{
    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = windowProc;
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

    hwnd = CreateWindowEx(
        WS_EX_ACCEPTFILES,
        "Win", "Project 8",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        ClientWindow.right - ClientWindow.left,
        ClientWindow.bottom - ClientWindow.top,
        NULL, LoadMenu(hInst, MAKEINTRESOURCE(MYMENU)), 
        hInst, NULL);

    return hwnd;
}

HWND Window::getWindowHwnd()
{
    return hwnd;
}

void Window::openDialog()
{
    OPENFILENAME ofn;

    ZeroMemory(&ofn,sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
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

void Window::fileDropped(HDROP file)
{
    int length = DragQueryFile(file, 0, NULL, 0); 
    length++; //Accomodate for \0 at end of string
    char * filepath = (char*)malloc(length);
    DragQueryFile(file, 0, filepath, length);
    
    OpenFile(filepath);

    free(filepath);
}

void Window::changeSound()
{
    if(Project8::soundActive)
    {
        Project8::soundActive = false;
        CheckMenuItem(GetMenu(hwnd), ID_MUTE, MF_CHECKED);
        return;
    }

    Project8::soundActive = true;
    CheckMenuItem(GetMenu(hwnd), ID_MUTE, MF_CHECKED);
}

void Window::resizeWindow(int width, int height)
{
    RECT ClientWindow;
    ClientWindow.bottom = height;
    ClientWindow.left = 0;
    ClientWindow.right = width;
    ClientWindow.top = 0;

    AdjustWindowRectEx(&ClientWindow,WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
        WS_OVERLAPPEDWINDOW, true,
        WS_EX_ACCEPTFILES);

    SetWindowPos(hwnd, 0, 0, 0, ClientWindow.right - ClientWindow.left,
        ClientWindow.bottom - ClientWindow.top, SWP_NOMOVE);
}

LRESULT CALLBACK Window::windowProc(HWND hwnd, UINT msg, 
    WPARAM wParam, LPARAM lParam)
{
    extern Window window; //DISGUSTING
    switch(msg)
    {    
        case WM_CREATE:
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
            extern Graphics GFX;
            GFX.resizeViewport(lParam & 0xFFFF, (lParam>>16) & 0xFFFF);
            return 0;

        case WM_DROPFILES:
            window.fileDropped((HDROP) wParam);
            return 0;

        case WM_COMMAND:
            switch(wParam & 0xFFFF)
            {
                case ID_OPEN:
                    window.openDialog();
                    break;

                case ID_RESET:
                    StartNES();
                    break;

                case ID_1X:
                    window.resizeWindow(256, 240);
                    break;

                case ID_2X:
                    window.resizeWindow(512, 480);
                    break;

                case ID_3X:
                    window.resizeWindow(768, 720);
                    break;

                case ID_DEBUG:
                    Debug::makeDebugger();
                    break;
                
                case ID_MUTE:
                    window.changeSound();
                    break;

                case ID_LOG:
                    if(!Project8::logging)
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

//Delete these?
char logging = 0;
