#include "Project 8.h"
#include "graphics.h"
#include "sound.h"
#include "window.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
            LPSTR lpCmdLine, int nShowCmd )
{
    MSG msg;
    HWND hwnd;

    hwnd = MakeWindow(512, 480, hInstance);
    initGL();
    InitDSound();
    timeBeginPeriod(1);

    status.run = 1;
    status.sound = 0;
    status.play = 0;
    status.pause = 0;

    while(status.run)
    {
        if(GetMessage(&msg,NULL,0,0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(msg.message == WM_QUIT) break;

        while(status.play)
        {
            if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            UpdateNES();

            if(msg.message == WM_QUIT) { status.run = 0; break; }
        }
    }

    killGL();
    KillDSound();
    timeEndPeriod(1);

    return msg.wParam;
}
