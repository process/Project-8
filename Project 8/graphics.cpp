#include "Project 8.h"

#include "window.h"
#include "PPU.h"

#include <gl/gl.h>
#pragma comment (lib,"opengl32.lib")

HDC hDC;
HGLRC hRC;

unsigned int TexPtr;

unsigned char screen[240][256][3];

unsigned char nes_palette[64][3] =
{
    {117, 117, 117}, { 39,  27, 143}, {  0,   0, 171}, { 71,   0, 159}, 
    {143,   0, 119}, {171,   0,  19}, {167,   0,   0}, {127,  11,   0}, 
    { 67,  47,   0}, {  0,  71,   0}, {  0,  81,   0}, {  0,  63,  23}, 
    { 27,  63,  95}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, 

    {188, 188, 188}, {  0, 115, 239}, { 35,  59, 239}, {131,   0, 243}, 
    {191,   0, 191}, {231,   0,  91}, {219,  43,   0}, {203,  79,  15}, 
    {139, 115,   0}, {  0, 151,   0}, {  0, 171,   0}, {  0, 147,  59}, 
    {  0, 131, 139}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, 

    {255, 255, 255}, { 63, 191, 255}, { 95, 151, 255}, {167, 139, 253}, 
    {247, 123, 255}, {255, 119, 183}, {255, 119,  99}, {255, 155,  59}, 
    {243, 191,  63}, {131, 211,  19}, { 79, 223,  75}, { 88, 248, 152}, 
    {  0, 235, 219}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, 

    {255, 255, 255}, {171, 231, 255}, {199, 215, 255}, {215, 203, 255}, 
    {255, 199, 255}, {255, 199, 219}, {255, 191, 179}, {255, 219, 171}, 
    {255, 231, 163}, {227, 255, 163}, {171, 243, 191}, {179, 255, 207}, 
    {159, 255, 243}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}
};

void PutPixel(unsigned char color, int pos)
{
    int x = pos % 256, y = pos / 256;

    screen[y][x][0] = nes_palette[color][0];
    screen[y][x][1] = nes_palette[color][1];
    screen[y][x][2] = nes_palette[color][2];
}

void ResizeViewport(int width, int height)
{
    glViewport(0,0,width,height);
}

void initGL()
{
    hDC = GetDC(GetWindowHwnd());

    PIXELFORMATDESCRIPTOR pfd=
    {
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
    hRC = wglCreateContext(hDC);
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
    glOrtho(0, 256, 240, 0, 0, 1);

    //Prep to add points
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //2D
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
}

void killGL()
{
    wglMakeCurrent(NULL,NULL);
    wglDeleteContext(hRC);
    ReleaseDC(GetWindowHwnd(),hDC);
}

void StartDraw()
{
    unsigned char color = VRAM[0x3F00];
    
    unsigned char r = nes_palette[color][0];
    unsigned char g = nes_palette[color][1];
    unsigned char b = nes_palette[color][2];

    int i, j;
    for(i = 0; i < 240; ++i)
    {
        for(j = 0; j < 256; ++j)
        {
            screen[i][j][0] = r;
            screen[i][j][1] = g;
            screen[i][j][2] = b;
        }
    }
}

void EndDraw()
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
