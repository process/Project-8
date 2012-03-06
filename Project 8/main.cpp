/* Project 8
 * Copyright (C)  2012 Justin Chines
 */

#include <Windows.h>

#include "Project 8.h"
#include "resource.h"
#include "debug.h"
#include "logging.h"
#include "graphics.h"
#include "sound.h"
#include "window.h"

Window window;

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, 
    WPARAM wParam, LPARAM lParam);

//Define globals
bool Project8::running = true; //Continuing running program
bool Project8::playing = false; //Not emulating a game
bool Project8::logging = false; //No logging by default
bool Project8::paused = false; //Not paused of course!
bool Project8::soundActive = false; //Experimental sound support disabled

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
            LPSTR lpCmdLine, int nShowCmd )
{
    MSG msg;
    HWND hwnd;

    hwnd = window.makeWindow(512, 480);

    //Currently in PPU code. Should probably bring it back here.
    //initGL();

    InitDSound();
    timeBeginPeriod(1);

    while(Project8::running)
    {
        if(GetMessage(&msg,NULL,0,0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(msg.message == WM_QUIT) break;

        while(Project8::playing)
        {
            if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            UpdateNES();

            if(msg.message == WM_QUIT) { Project8::running = false; break; }
        }
    }

    //In PPU. BRING IT BACK!
    //killGL();
    KillDSound();
    timeEndPeriod(1);

    return msg.wParam;
}

