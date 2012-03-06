#include <Windows.h>
#include <gl/gl.h>
#pragma comment (lib,"opengl32.lib")

#include "graphics.h"

#include "Project 8.h"

#include "window.h"
#include "PPU.h"

//DISGUSTING GLOBALS
unsigned char screen[240][256][3];
unsigned int TexPtr;
extern Window window;

Graphics::Graphics()
{
    //Default constructor
}

void Graphics::initGraphics()
{
    hwnd = window.getWindowHwnd();
    hDC = GetDC(hwnd); //Device context of window

    PIXELFORMATDESCRIPTOR pfd=
    {
        //Typical graphics format
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL |
        PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        24,0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
        16, 0, 0, PFD_MAIN_PLANE,
        0, 0, 0, 0
    };

    int PixelFormat = ChoosePixelFormat(hDC,&pfd);
    if(PixelFormat == 0)
        MessageBox(NULL, "Failed to choose pixel format!", "Error", MB_OK);

    if(SetPixelFormat(hDC,1,&pfd) == 0)
        MessageBox(NULL, "Failed to set pixel format!", "Error", MB_OK);
    
    //Initialize OpenGL stuff
    this->hRC = wglCreateContext(hDC);
    if(hRC == NULL)
        MessageBox(NULL, "Failed to create OpenGL context!", "Error", MB_OK);
    if(wglMakeCurrent(hDC,hRC) == 0)
        MessageBox(NULL, "Failed to activate OpenGL context!", "Error", MB_OK);

    //Add a texture
    glGenTextures(1, &TexPtr);
    glBindTexture(GL_TEXTURE_2D, TexPtr);

    //Simple texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //Set up projection matrix for orthographic (2D) rendering
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 256, 240, 0, 0, 1); //True NES resolution is 256x240

    //Prep to add points
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //2D
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
}

void Graphics::startDraw()
{
    //Nothing here unless we switch from the texture method
}

void Graphics::clear(int r, int g, int b)
{
    for(int i = 0; i < 240; ++i)
        for(int j = 0; j < 256; ++j)
        {
            screen[i][j][0] = r;
            screen[i][j][1] = g;
            screen[i][j][2] = b;
        }
}

void Graphics::resizeViewport(int width, int height)
{
    glViewport(0,0,width,height);
}

void Graphics::putPixel(int x, int y, int r, int g, int b)
{
    screen[y][x][0] = r;
    screen[y][x][1] = g;
    screen[y][x][2] = b;
}

void Graphics::endDraw()
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 240, 0, GL_RGB, GL_UNSIGNED_BYTE, screen);
    glBegin(GL_QUADS);

    glTexCoord2i(0, 0); 
    glVertex2i(0, 0);

    glTexCoord2i(0, 1); 
    glVertex2i(0, 240);

    glTexCoord2i(1, 1); 
    glVertex2i(256, 240);

    glTexCoord2i(1, 0); 
    glVertex2i(256, 0);
    
    glEnd();
    SwapBuffers(hDC);
}

void Graphics::killGL()
{
    //Uninitialize OpenGL
    wglMakeCurrent(NULL, NULL);
    
    //Goodbye GL
    wglDeleteContext(hRC);

    //Goodbye device
    ReleaseDC(hwnd, hDC);
}
