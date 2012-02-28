#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <windows.h>
#include <gl\gl.h>

#include "rsrc.h"
#include "defs.h"

#include "PPU.h"
#include "CPU.h"

#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"winmm.lib")

//THIS PARTICULAR FILE IS DEAD. RESTRUCTURING IS BEING DONE IN NEW PROJ. 
//I HAD SOME GOOD TIMES winmain.cpp !!! ;_;

/*
todo:
*split ppuupdate into multiple parts
*optimize instructions
*add clean up code for resources
*spu hur dur
*/

PPU ppu;
CPU cpu;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
			LPSTR lpCmdLine, int nShowCmd)
{
	MSG msg;
	WNDCLASSEX wc;
	HWND hwnd;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_SAVEBITS; //CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDR_ICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(0);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Win";
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	RECT ClientWindow;
	ClientWindow.bottom = 480;
	ClientWindow.left = 0;
	ClientWindow.right = 512;
	ClientWindow.top = 0;

	AdjustWindowRectEx(&ClientWindow,WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		WS_OVERLAPPEDWINDOW, true,
		WS_EX_ACCEPTFILES);

	hwnd = CreateWindowEx(
		WS_EX_ACCEPTFILES,
		"Win",
		"Project 8",
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		ClientWindow.right - ClientWindow.left,
		ClientWindow.bottom - ClientWindow.top,
		NULL,NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, nShowCmd);
	initGL(hwnd);
	UpdateWindow(hwnd);

	timeBeginPeriod(1);

	char run = 1;

	while(run)
	{
		if(GetMessage(&msg,NULL,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT) break;

		int instructions = 0; //

		while(play)
		{
			if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if(msg.message == WM_QUIT){ run = 0; break; }

			updateCPU(hwnd);
			instructions++;

			if(instructions == 3000)
			{
				//Sleep(1);
				instructions = 0;
			}

			glViewport(0, 0, wx, wy);
		}
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, 
						 WPARAM wParam, LPARAM lParam)
{
	static HMENU menu, submenu;
	static bool pause = false;
	static bool ctrl = false;

	switch(msg)
	{
		case WM_CREATE:
			menu = CreateMenu();

			submenu = CreatePopupMenu();
			AppendMenu(submenu, MF_STRING, ID_OPEN, "Open\tCtrl+O");
			AppendMenu(submenu, MF_STRING, ID_RESET, "Reset");
			AppendMenu(submenu, MF_STRING, ID_EXIT, "Exit\tAlt+F4");
			AppendMenu(menu,MF_STRING | MF_POPUP, (UINT)submenu, "File");

			submenu = CreatePopupMenu();
			AppendMenu(submenu, MF_STRING, ID_V1, "1x");
			AppendMenu(submenu, MF_STRING, ID_V2, "2x");
			AppendMenu(submenu, MF_STRING, ID_V3, "3x");
			AppendMenu(menu, MF_STRING | MF_POPUP, (UINT)submenu, "View");

			SetMenu(hwnd,menu);
			break;

		case WM_COMMAND:
			switch(wParam & 0xFFFF)
			{
				case ID_OPEN:
					DialogOpen(hwnd);
					break;

				case ID_RESET:
					resetCPU();
					ppu.resetPPU();
					break;

				case ID_EXIT:
					PostQuitMessage(0);
					return 0;

					//

				case ID_V1:
					WindowSize(1,hwnd);
					break;

				case ID_V2:
					WindowSize(2,hwnd);
					break;

				case ID_V3:
					WindowSize(3,hwnd);
					break;
			}
			break;

		case WM_SIZE:
			wx = lParam & 0xFFFF;
			wy = lParam >> 16;
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			timeEndPeriod(1);
			return 0;
			break;

		case WM_DROPFILES:
			DropOpen(wParam);
			break;


		//JOYPAD BITS
		// <-    ->    v   ^   enter  shift  z  x
		//right left down  up  start select  b  a
		//  7     6   5    4     3      2    1  0		
		case WM_KEYDOWN:
			switch(wParam)
			{
				case 0x41 + 23: //X
					JOYPAD |= 1;
					break;				
				case 0x41 + 25:	//Z
					JOYPAD |= 2;
					break;
				case VK_SHIFT:
					JOYPAD |= 4;
					break;
				case VK_RETURN:
					JOYPAD |= 8;
					break;
				case VK_UP:
					JOYPAD |= 16;
					break;
				case VK_DOWN:
					JOYPAD |= 32;
					break;
				case VK_LEFT:
					JOYPAD |= 64;
					break;
				case VK_RIGHT:
					JOYPAD |= 128;
					break;
				case VK_CONTROL:
					ctrl = true;
					break;
				case 0x41 + 14: //O
					if(ctrl) DialogOpen(hwnd);
					break;
			}
			break;

		case WM_KEYUP:
			switch(wParam)
			{				
				case 0x41 + 23:	//X
					JOYPAD &= 254;
					break;
				case 0x41 + 25:	//Z
					JOYPAD &= 253;
					break;
				case VK_SHIFT:
					JOYPAD &= 251;
					break;				
				case VK_RETURN:
					JOYPAD &= 247;
					break;
				case VK_UP:
					JOYPAD &= 239;
					break;
				case VK_DOWN:
					JOYPAD &= 223;
					break;
				case VK_LEFT:
					JOYPAD &= 191;
					break;
				case VK_RIGHT:
					JOYPAD &= 127;
					break;
				case VK_CONTROL:
					ctrl = false;
					break;
			}
			break;
	}

	return DefWindowProc(hwnd,msg,wParam,lParam);
}

void initGL(HWND hwnd)
{
	hDC = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		16,0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,
		16, 0, 0, PFD_MAIN_PLANE,
		0, 0, 0, 0
	};

	unsigned int PixelFormat = ChoosePixelFormat(hDC,&pfd);
	SetPixelFormat(hDC,PixelFormat,&pfd);

	hRC = wglCreateContext(hDC);

	wglMakeCurrent(hDC,hRC);

	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,256,240,0,0,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0,0,0,0);

	glClear(GL_COLOR_BUFFER_BIT);
	SwapBuffers(hDC);
}

void readError()
{
	MessageBox(NULL,"Could not succesfully copy ROM into memory",
	"Error!",MB_ICONEXCLAMATION);

	play = 0;
}

int HeaderCheck(FILE * ROM)
{
	char * buffer = (char *) malloc(8);
	unsigned char magic[4] = {'N','E','S',0x1A};
	fread(buffer,1,8,ROM);
	
	if(memcmp(buffer,magic,4))
	{
		free(buffer);
		return 0;
	}

	unsigned char prgcount = buffer[4];
	free(buffer);

	return prgcount;
}


void DialogOpen(HWND hwnd)
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = "NES Roms (.nes)\0*.NES";
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

	LoadFile(ofn.lpstrFile);
	free(ofn.lpstrFile);
}

void DropOpen(WPARAM hdrop)
{
	HDROP file = (HDROP)hdrop;

	int size = DragQueryFile(file,0,0,0);
	char * path = (char*)malloc(size+1);	//+1 for \0
	DragQueryFile(file,0,path,size+1);

	LoadFile(path);
	free(path);
}

void LoadFile(char * path)
{	
	unsigned char PRGcount;

	ppu.resetPPU();
	resetCPU();

	FILE * ROM = fopen(path, "rb");
	if(!ROM)
	{		
		MessageBox(NULL,"File could not be openned!",
						"Error",MB_ICONEXCLAMATION);
		
		play = 0;

		return;
	}

	if(!(PRGcount=HeaderCheck(ROM)))
	{		
		MessageBox(NULL,"Not a valid NES ROM.","Error",MB_ICONEXCLAMATION);

		play = 0;

		fclose(ROM);
		return;
	}

	fseek(ROM,16,SEEK_SET);

	if(PRGcount == 2)
	{
		if(fread(RAM + 0x8000, 1, 32768, ROM) != 32768) 
		{
			readError();
			fclose(ROM);
			return;
		}
	}
	
	else if(PRGcount == 1)
	{
		if(fread(RAM+0x8000, 1, 16384, ROM) != 16384)
		{
			readError();
			fclose(ROM);
			return;
		}

		fseek(ROM, 16, SEEK_SET);
		
		if(fread(RAM+0xC000, 1, 16384, ROM) != 16384)
		{
			readError();
			fclose(ROM);
			return;
		}
	}

	else
	{		
		MessageBox(NULL,"This type of ROM is not currently supported.",
			"Sorry!",MB_ICONEXCLAMATION);

		fclose(ROM);
		play = 0;
		return;
	}

	if(fread(VRAM,1,8192,ROM))
	{
		//MessageBox(NULL,"Could not copy pattern table into VRAM","Error",NULL);
	}

	resetCPU();
	ppu.resetPPU();

	play = 1;

	fclose(ROM);
}

void WindowSize(char factor, HWND hwnd)
{
	RECT window;
	window.bottom = 240 * factor;
	window.top = 0;
	window.right = 256 * factor;
	window.left = 0;

	AdjustWindowRectEx(&window, WS_OVERLAPPEDWINDOW, true, 0);

	int cx = window.right - window.left;
	int cy = window.bottom - window.top;

	WINDOWINFO info;
	info.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(hwnd, &info);

	if(info.dwStyle &= WS_MAXIMIZE)
		ShowWindow(hwnd, SW_RESTORE);

	SetWindowPos(hwnd, NULL, 0, 0, 
		cx, cy, SWP_NOMOVE);

	wx = 256 * factor;
	wy = 240 * factor;
}

void SpriteStuff(unsigned char * bits, char spritenum)
{
		unsigned char * sprdata = SPRRAM + ((spritenum - 1) * 4);

		for(int y = 0; y < 8; y++)
			for(int x = 0; x < 8; x++)
				if(bits[(y*8)+x])
					bits[(y*8)+x] |= (sprdata[2] & 3) << 2;

		if(sprdata[2] & 64) //Horizontal Flip//
		{
			unsigned char temparray[8][8];
			memcpy(temparray,bits,64);
			
			for(int i = 0; i < 8; i++)
			{
				for(int j = 0; j < 8; j++)
				{
					bits[(i * 8) + j] = temparray[i][7-j];
				}
			}
		}

		if(sprdata[2] & 128) //Vertical Flip//
		{
			unsigned char temparray[8][8];
			memcpy(temparray,bits,64);
			
			for(int i = 0; i < 8; i++)
			{
				for(int j = 0; j < 8; j++)
				{
					bits[(i * 8) + j] = temparray[7-i][j];
				}
			}
		}
		
		if(!(spritenum-1))	//for spr0 hit
			for(int y = 0; y < 8; y++)
				for(int x = 0; x < 8; x++)
					if(bits[(y*8)+x])
						bits[(y*8)+x] |= 128;
}

void PatternTableBits(unsigned char * tilearray, unsigned char * tileptr,
					  char sprite)
{
	unsigned char x = 0, y = 0;

	for(int j = 0; j < 16; j++)
	{
		unsigned char curline = tileptr[j];
		for(int k = 0; k < 8; k++)
		{
			if((curline << k) & 128) //each bit represents a color
			{
				if(j < 8)
				{
					if(sprite)
						tilearray[(y * 8) + x] |= 16;

					tilearray[(y * 8) + x] |= 1;
				}

				else
				{
					if(sprite)
						tilearray[(y * 8) + x] |= 16;

					tilearray[(y * 8) + x] |= 2;
				}
			}

			x++;
		}
		x = 0;
		y++;

		if(j == 7)
			y = 0;
	}
}

void CreateTile(unsigned char * buffer, unsigned char tile, 
				int x, int y, char sprite, char table)
{
	int start = x;
	int linestart = y;

	bool bigsprite = false;

	unsigned char * tileptr = VRAM + (tile * 16);

	unsigned char tilearray[8][8];
	unsigned char spritearray[8][8];
	ZeroMemory(tilearray, 64);
	ZeroMemory(spritearray, 64);

	if(!sprite)
	{
		if(RAM[0x2000] & 16) //data at v0x1000
				tileptr += 0x1000;
	}

	else if(sprite)
		if(RAM[0x2000] & 8)
			tileptr += 0x1000;
	
	x = 0;
	y = 0;

	PatternTableBits((unsigned char *)tilearray, tileptr, sprite);

	if(sprite && RAM[0x2000] & 32)
	{
		PatternTableBits((unsigned char *)spritearray, tileptr + 16, sprite);
		SpriteStuff((unsigned char *)spritearray, sprite);
		bigsprite = true;
	}

	if(sprite)
		SpriteStuff((unsigned char *)tilearray, sprite);

	else //Atrribute tables//
	{
		unsigned char bits;
		unsigned char attribute;
		unsigned char * attrtable = VRAM + 0x23C0 + (0x400 * table);

		unsigned char _y = linestart / 8;
		unsigned char _x = start / 8;	//tile coordinates

		attribute = attrtable[((_y/4) * 8) + (_x/4)];	//attr bytes are 4x4 tiles

		unsigned char tilex = 0;
		unsigned char tiley = 0;

		if((_x % 4) > 1)
			tilex = 1;
		if((_y % 4) > 1)
			tiley = 1;

		if(!tilex && !tiley) bits = attribute & 0x03;
		else if(tilex && !tiley) bits = (attribute >> 2) & 0x03;
		else if(!tilex && tiley) bits = (attribute >> 4) & 0x03;
		else if(tilex && tiley) bits = (attribute >> 6) & 0x03;

		bits <<= 2;

		for(y = 0; y < 8; y++)
			for(x = 0; x < 8; x++)
				if(tilearray[y][x])
					tilearray[y][x] |= bits;
	}

	x = 0;
	y = 0;

	for(int i = linestart; i < linestart + 8; i++)
	{
		for(int j = start; j < start + 8; j++)
		{
			if(tilearray[y][x])
			{
				if((i * 256) + j >= 61440)
					continue;

				buffer[(i * 256) + j] = tilearray[y][x];
			}
			x++;
		}
		x = 0;
		y++;
	}

	x = 0;
	y = 0;

	if(bigsprite)	//todo: clean/fix this
		for(int i = linestart+8; i < linestart + 16; i++)
		{
			for(int j = start; j < start + 8; j++)
			{
				if(spritearray[y][x])
				{
					buffer[(i * 256) + j] = spritearray[y][x];
				}
				x++;
			}
			x = 0;
			y++;
		}
}

void CreateBackground(unsigned char * screen, unsigned char table)
{
	int x = 0, y = 0;
	
	unsigned char * nametable = VRAM + 0x2000 + (0x400 * table);

	for(int i = 0; i < 960; i++)
	{
		CreateTile(screen, nametable[i], x, y, 0, table);

		x += 8;

		if(x == 256)
		{
			x = 0;
			y += 8;
		}
	}
}

void CreateSprites(unsigned char * table)
{
	for(int i = 63; i >= 0; i--)
	{
		unsigned char * sprite = SPRRAM + (i * 4);

		CreateTile(table, sprite[1], sprite[3], sprite[0]+1, i + 1, -1);
	}
}

void PutPixel(unsigned char color, int pos)
{
	glColor3f(
		nes_palette[color][0]/256.0,
		nes_palette[color][1]/256.0,
		nes_palette[color][2]/256.0);

	glVertex2f(pos % 256, pos / 256);
	glVertex2f(pos % 256, (pos / 256) - 1);
	glVertex2f((pos % 256) + 1, (pos / 256) - 1);
	glVertex2f((pos % 256) + 1, pos / 256);
}

void DrawScreen(unsigned char * table1, unsigned char * table2,
				unsigned char * sprites, int * pixel, int xscr, int yscr)
{
	unsigned char * maintable;
	unsigned char * secondtable;

	unsigned char * palette = VRAM + 0x3F00;

	if(!(RAM[0x2000] & 1))
	{
		maintable = table1;
		secondtable = table2;
	}

	else //does nothing for now
	{
		maintable = table1;
		secondtable = table2;
	}

	int scanline = *pixel / 256;
	int start = *pixel;

	for(int x = 0; x < 8; x++) 
	{
		unsigned short addr = (*pixel) + xscr + (yscr * 256);

		if(addr > (scanline * 256) + 255)
		{
			if(secondtable[addr])
				PutPixel(palette[secondtable[addr - 256]], *pixel);
		}

		else if(maintable[addr])
			PutPixel(palette[maintable[addr]], *pixel);

		if(sprites[*pixel])
		{
			if(sprites[*pixel]&128 && maintable[addr]
				&& !(RAM[0x2002] & 64))
			{
				RAM[0x2002] |= 64;
			}

			PutPixel(palette[sprites[*pixel] & 31], *pixel);
		}

		(*pixel)++;
	}
}

void DrawPixels(unsigned short howMany, unsigned short Pos)
{
	unsigned char * palette = VRAM + 0x3F00;
	unsigned char * nametable = VRAM + 0x2000;

	for(int i = 0; i < howMany; i++)
	{
		//lines of tiles + how far into new row of tiles
		unsigned short tilenum = ((Pos/2048)*32) + ((Pos%256)/8); 
		unsigned char * tile = VRAM + (nametable[tilenum] * 16);

		if(RAM[0x2000] & 16)
			tile += 0x1000;

		//current line of the tile
		tile += (Pos/256)%8;

		unsigned char color = 0;

		//choose correct bit for specific pixel
		if(((*tile)<<(Pos%8))&128) color |= 1;
		if(((*(tile+8))<<(Pos%8))&128) color |= 2;

		unsigned char attribute = *(VRAM + 0x23C0 + (Pos/2048));

		PutPixel(palette[color], Pos);
		Pos++;
	}
}

void resetCPU()
{
	A = 0;
	X = 0;
	Y = 0;

	for(int i = 0; i < 0x8000; i++)
		RAM[i] = 0;

	NMI = RAM[0xFFFA] | (RAM[0xFFFB] << 8);
	PC  = RAM[0xFFFC] | (RAM[0xFFFD] << 8);

	if(!PC) PC = 0x8000;
	
	S = 0xFF;
	P = 32;	//bit 5 always set
}

void updateCPU(HWND hwnd)
{
	char change;
	unsigned char byte;
	unsigned short addr;
	unsigned char result;

	unsigned char a = A;
	unsigned char x = X;
	unsigned char y = Y;

	int cycles = 1000;

	begin:
	switch(RAM[PC])		//instructions put in order of most used to least used
	{
		#include "instructions.h"

		end:
			ppu.update(hwnd, 0);
			cpu.joypad();
			cycles--;
			if(cycles) goto begin;

			A = a;
			X = x;
			Y = y;
			break;
	}
}

void pushw(unsigned short word)
{
	RAM[S+0x100] = (unsigned char) (word >> 8);
	S--;
	RAM[S+0x100] = (unsigned char) (word & 0xFF);
	S--;
}

unsigned short int popw()
{
	S++;
	unsigned char lo = RAM[S+0x100];
	S++;
	unsigned char hi = RAM[S+0x100];
	return ((hi << 8) | lo);
}

void pushb(unsigned char byte)
{
	RAM[S+0x100] = byte;
	S--;
}

unsigned char popb()
{
	S++;
	return RAM[S+0x100];
}

unsigned short fetchword()
{
	PC++;
	unsigned char lo = RAM[PC++];
	unsigned char hi = RAM[PC++];

	return (hi << 8) | lo;
}

unsigned char fetchbyte()
{
	PC++;
	return RAM[PC++];
}

void CheckPortsW(unsigned short addr)
{
	if (addr == 0x2000) W2000 = 1;
	else if(addr == 0x2003) W2003 = 1;
	else if(addr == 0x2004) SPRWRITE = 1;
	else if(addr == 0x2005) ppu.scroll(RAM[0x2005]);
	else if(addr == 0x2006) ppu.ppuaddress(RAM[0x2006]);
	else if(addr == 0x2007) ppu.ppuwrite();
	else if(addr == 0x4016) JOYWRITE = 1;
}

void N_FLAGS(unsigned char REG)
{
	if (REG & 128) P |= 128;
	else P &= 127;
}

void Z_FLAGS(unsigned char REG)
{
	if (!REG) P |= 2;
	else P &= 0xFD;
}
