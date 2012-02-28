//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include <time.h>
//
//#include <windows.h>
//#include <gl\gl.h>
//
//#include "rsrc.h"
//#include "defs.h"
//
//#pragma comment (lib,"opengl32.lib")
//#pragma comment (lib,"winmm.lib")
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
//			LPSTR lpCmdLine, int nShowCmd)
//{	
//	MSG msg;
//	WNDCLASSEX wc;
//	HWND hwnd;
//
//	wc.cbSize = sizeof(WNDCLASSEX);
//	wc.style = 0;
//	wc.lpfnWndProc = WndProc;
//	wc.cbClsExtra = 0;
//	wc.cbWndExtra = 0;
//	wc.hInstance = hInstance;
//    wc.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDR_ICON));
//    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wc.hbrBackground = CreateSolidBrush(0);
//	wc.lpszMenuName = NULL;
//	wc.lpszClassName = "Win";
//	wc.hIconSm = NULL;
//
//	RegisterClassEx(&wc);
//
//	RECT ClientWindow;
//	ClientWindow.bottom = 480;
//	ClientWindow.left = 0;
//	ClientWindow.right = 512;
//	ClientWindow.top = 0;
//
//	AdjustWindowRectEx(&ClientWindow,WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
//		WS_OVERLAPPEDWINDOW, true,
//		WS_EX_ACCEPTFILES);
//
//	hwnd = CreateWindowEx(
//		WS_EX_ACCEPTFILES,
//		"Win",
//		"Project 8",
//		WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
//		WS_OVERLAPPEDWINDOW,
//		CW_USEDEFAULT,
//		CW_USEDEFAULT,
//		ClientWindow.right - ClientWindow.left,
//		ClientWindow.bottom - ClientWindow.top,
//		NULL,NULL,
//		hInstance,
//		NULL);
//
//	ShowWindow(hwnd, nShowCmd);
//	initGL(hwnd);
//	UpdateWindow(hwnd);
//
//	timeBeginPeriod(1);
//
//	char run = 1;
//
//	while(run)
//	{
//		if(GetMessage(&msg,NULL,0,0))
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//
//		if(msg.message == WM_QUIT) break;
//
//		while(play)
//		{
//			if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
//			{
//				TranslateMessage(&msg);
//				DispatchMessage(&msg);
//			}
//
//			if(msg.message == WM_QUIT){ run = 0; break; }
//
//			updateCPU();
//			updatePPU(hwnd);
//
//			glViewport(0, 0, wx, wy);
//		}
//	}
//
//	return msg.wParam;
//}
//
//LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, 
//						 WPARAM wParam, LPARAM lParam)
//{
//	static HMENU menu, submenu;
//	static bool pause = false;
//	static bool ctrl = false;
//
//	switch(msg)
//	{
//		case WM_CREATE:
//			menu = CreateMenu();
//
//			submenu = CreatePopupMenu();
//			AppendMenu(submenu, MF_STRING, ID_OPEN, "Open\tCtrl+O");
//			AppendMenu(submenu, MF_STRING, ID_RESET, "Reset");
//			AppendMenu(submenu, MF_STRING, ID_EXIT, "Exit\tAlt+F4");
//			AppendMenu(menu,MF_STRING | MF_POPUP, (UINT)submenu, "File");
//
//			submenu = CreatePopupMenu();
//			AppendMenu(submenu, MF_STRING, ID_V1, "1x");
//			AppendMenu(submenu, MF_STRING, ID_V2, "2x");
//			AppendMenu(submenu, MF_STRING, ID_V3, "3x");
//			AppendMenu(menu, MF_STRING | MF_POPUP, (UINT)submenu, "View");
//
//			SetMenu(hwnd,menu);
//			break;
//
//		case WM_COMMAND:
//			switch(wParam & 0xFFFF)
//			{
//				case ID_OPEN:
//					DialogOpen(hwnd);
//					break;
//
//				case ID_RESET:
//					resetCPU();
//					resetPPU();
//					break;
//
//				case ID_EXIT:
//					PostQuitMessage(0);
//					return 0;
//
//					//
//
//				case ID_V1:
//					WindowSize(1,hwnd);
//					break;
//
//				case ID_V2:
//					WindowSize(2,hwnd);
//					break;
//
//				case ID_V3:
//					WindowSize(3,hwnd);
//					break;
//			}
//			break;
//
//		case WM_SIZE:
//			wx = lParam & 0xFFFF;
//			wy = lParam >> 16;
//			break;
//
//		case WM_DESTROY:
//			PostQuitMessage(0);
//			timeEndPeriod(1);
//			return 0;
//			break;
//
//		case WM_DROPFILES:
//			DropOpen(wParam);
//			break;
//
//
//		//JOYPAD BITS
//		// <-    ->    v   ^   enter  shift  z  x
//		//right left down  up  start select  b  a
//		//  7     6   5    4     3      2    1  0		
//		case WM_KEYDOWN:
//			switch(wParam)
//			{
//				case 0x41 + 23: //X
//					JOYPAD |= 1;
//					break;				
//				case 0x41 + 25:	//Z
//					JOYPAD |= 2;
//					break;
//				case VK_SHIFT:
//					JOYPAD |= 4;
//					break;
//				case VK_RETURN:
//					JOYPAD |= 8;
//					break;
//				case VK_UP:
//					JOYPAD |= 16;
//					break;
//				case VK_DOWN:
//					JOYPAD |= 32;
//					break;
//				case VK_LEFT:
//					JOYPAD |= 64;
//					break;
//				case VK_RIGHT:
//					JOYPAD |= 128;
//					break;
//				case VK_CONTROL:
//					ctrl = true;
//					break;
//				case 0x41 + 14: //O
//					if(ctrl) DialogOpen(hwnd);
//					break;
//			}
//			break;
//
//		case WM_KEYUP:
//			switch(wParam)
//			{				
//				case 0x41 + 23:	//X
//					JOYPAD &= 254;
//					break;
//				case 0x41 + 25:	//Z
//					JOYPAD &= 253;
//					break;
//				case VK_SHIFT:
//					JOYPAD &= 251;
//					break;				
//				case VK_RETURN:
//					JOYPAD &= 247;
//					break;
//				case VK_UP:
//					JOYPAD &= 239;
//					break;
//				case VK_DOWN:
//					JOYPAD &= 223;
//					break;
//				case VK_LEFT:
//					JOYPAD &= 191;
//					break;
//				case VK_RIGHT:
//					JOYPAD &= 127;
//					break;
//				case VK_CONTROL:
//					ctrl = false;
//					break;
//			}
//			break;
//	}
//
//	return DefWindowProc(hwnd,msg,wParam,lParam);
//}
//
//void initGL(HWND hwnd)
//{
//	hDC = GetDC(hwnd);
//
//	PIXELFORMATDESCRIPTOR pfd=
//	{
//		sizeof(PIXELFORMATDESCRIPTOR),
//		1,
//		PFD_DRAW_TO_WINDOW |
//		PFD_SUPPORT_OPENGL |
//		PFD_DOUBLEBUFFER,
//		PFD_TYPE_RGBA,
//		16,0, 0, 0, 0, 0, 0,
//		0, 0, 0, 0, 0, 0, 0,
//		16, 0, 0, PFD_MAIN_PLANE,
//		0, 0, 0, 0
//	};
//
//	unsigned int PixelFormat = ChoosePixelFormat(hDC,&pfd);
//	SetPixelFormat(hDC,PixelFormat,&pfd);
//
//	hRC = wglCreateContext(hDC);
//
//	wglMakeCurrent(hDC,hRC);
//
//	glDisable(GL_DEPTH_TEST);
//
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrtho(0,256,240,0,0,1);
//
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	glClearColor(0,0,0,0);
//
//	glClear(GL_COLOR_BUFFER_BIT);
//	SwapBuffers(hDC);
//}
//
//void readError()
//{
//	MessageBox(NULL,"Could not succesfully copy ROM into memory",
//	"Error!",MB_ICONEXCLAMATION);
//
//	play = 0;
//}
//
//void DialogOpen(HWND hwnd)
//{
//	OPENFILENAME ofn;
//
//	ZeroMemory(&ofn,sizeof(ofn));
//	ofn.lStructSize = sizeof(ofn);
//	ofn.hwndOwner = hwnd;
//	ofn.hInstance = NULL;
//	ofn.lpstrFilter = "NES Roms (.nes)\0*.NES";
//	ofn.nFilterIndex = 1;
//	ofn.lpstrFile = (char*)malloc(512);
//	ofn.lpstrFile[0] = '\0';
//	ofn.nMaxFile = 256;
//	ofn.lpstrFileTitle = NULL;
//	ofn.nMaxFileTitle = 0;
//	ofn.lpstrInitialDir = NULL;
//	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
//	ofn.lpstrDefExt = "NES";
//
//	GetOpenFileName(&ofn);
//
//	if(ofn.lpstrFile[0] == '\0')
//	{
//		free(ofn.lpstrFile);
//		return;
//	}
//
//	LoadFile(ofn.lpstrFile);
//	free(ofn.lpstrFile);
//}
//
//void DropOpen(WPARAM hdrop)
//{
//	HDROP file = (HDROP)hdrop;
//
//	int size = DragQueryFile(file,0,0,0);
//	char * path = (char*)malloc(size+1);	//+1 for \0
//	DragQueryFile(file,0,path,size+1);
//
//	LoadFile(path);
//	free(path);
//}
//
//void LoadFile(char * path)
//{	
//	unsigned char PRGcount;
//
//	resetPPU();
//	resetCPU();
//
//	FILE * ROM = fopen(path, "rb");
//	if(!ROM)
//	{		
//		MessageBox(NULL,"File could not be openned!",
//						"Error",MB_ICONEXCLAMATION);
//		
//		play = 0;
//
//		return;
//	}
//
//	if(!(PRGcount=HeaderCheck(ROM)))
//	{		
//		MessageBox(NULL,"Not a valid NES ROM.","ERROR",MB_ICONEXCLAMATION);
//
//		play = 0;
//
//		fclose(ROM);
//		return;
//	}
//
//	fseek(ROM,16,SEEK_SET);
//
//	if(PRGcount == 2)
//	{
//		if(fread(RAM + 0x8000, 1, 32768, ROM) != 32768) 
//		{
//			readError();
//			fclose(ROM);
//			return;
//		}
//	}
//	
//	else if(PRGcount == 1)
//	{
//		if(fread(RAM+0x8000, 1, 16384, ROM) != 16384)
//		{
//			readError();
//			fclose(ROM);
//			return;
//		}
//
//		fseek(ROM, 16, SEEK_SET);
//		
//		if(fread(RAM+0xC000, 1, 16384, ROM) != 16384)
//		{
//			readError();
//			fclose(ROM);
//			return;
//		}
//	}
//
//	else
//	{		
//		MessageBox(NULL,"This type of ROM is not currently supported.",
//			"Sorry!",MB_ICONEXCLAMATION);
//
//		fclose(ROM);
//		play = 0;
//		return;
//	}
//
//	if(fread(VRAM,1,8192,ROM))
//	{
//		//MessageBox(NULL,"Could not copy pattern table into VRAM","Error",NULL);
//	}
//
//	resetCPU();
//	resetPPU();
//
//	play = 1;
//
//	fclose(ROM);
//}
//
//int HeaderCheck(FILE * ROM)
//{
//	char * buffer = (char *) malloc(8);
//	unsigned char magic[4] = {'N','E','S',0x1A};
//	fread(buffer,1,8,ROM);
//	
//	if(memcmp(buffer,magic,4))
//	{
//		free(buffer);
//		return 0;
//	}
//
//	unsigned char prgcount = buffer[4];
//	free(buffer);
//
//	return prgcount;
//}
//
//void WindowSize(char factor, HWND hwnd)
//{
//	RECT window;
//	window.bottom = 240 * factor;
//	window.top = 0;
//	window.right = 256 * factor;
//	window.left = 0;
//
//	AdjustWindowRectEx(&window, WS_OVERLAPPEDWINDOW, true, 0);
//
//	int cx = window.right - window.left;
//	int cy = window.bottom - window.top;
//
//	WINDOWINFO info;
//	info.cbSize = sizeof(WINDOWINFO);
//	GetWindowInfo(hwnd, &info);
//
//	if(info.dwStyle &= WS_MAXIMIZE)
//		ShowWindow(hwnd, SW_RESTORE);
//
//	SetWindowPos(hwnd, NULL, 0, 0, 
//		cx, cy, SWP_NOMOVE);
//
//	wx = 256 * factor;
//	wy = 240 * factor;
//}
//
//void SpriteStuff(unsigned char * bits, char spritenum)
//{
//		unsigned char * sprdata = SPRRAM + ((spritenum - 1) * 4);
//
//		for(int y = 0; y < 8; y++)
//			for(int x = 0; x < 8; x++)
//				if(bits[(y*8)+x])
//					bits[(y*8)+x] |= (sprdata[2] & 3) << 2;
//
//		if(sprdata[2] & 64) //Horizontal Flip//
//		{
//			unsigned char temparray[8][8];
//			memcpy(temparray,bits,64);
//			
//			for(int i = 0; i < 8; i++)
//			{
//				for(int j = 0; j < 8; j++)
//				{
//					bits[(i * 8) + j] = temparray[i][7-j];
//				}
//			}
//		}
//
//		if(sprdata[2] & 128) //Vertical Flip//
//		{
//			unsigned char temparray[8][8];
//			memcpy(temparray,bits,64);
//			
//			for(int i = 0; i < 8; i++)
//			{
//				for(int j = 0; j < 8; j++)
//				{
//					bits[(i * 8) + j] = temparray[7-i][j];
//				}
//			}
//		}
//		
//		if(!(spritenum-1))
//			for(int y = 0; y < 8; y++)
//				for(int x = 0; x < 8; x++)
//					if(bits[(y*8)+x])
//						bits[(y*8)+x] |= 128;
//}
//
//void PatternTableBits(unsigned char * tilearray, unsigned char * tileptr,
//					  char sprite)
//{
//	unsigned char x = 0, y = 0;
//
//	for(int j = 0; j < 16; j++)
//	{
//		unsigned char curline = tileptr[j];
//		for(int k = 0; k < 8; k++)
//		{
//			if((curline << k) & 128) //each bit represents a color
//			{
//				if(j < 8)
//				{
//					if(sprite)
//						tilearray[(y * 8) + x] |= 16;
//
//					tilearray[(y * 8) + x] |= 1;
//				}
//
//				else
//				{
//					if(sprite)
//						tilearray[(y * 8) + x] |= 16;
//
//					tilearray[(y * 8) + x] |= 2;
//				}
//			}
//
//			x++;
//		}
//		x = 0;
//		y++;
//
//		if(j == 7)
//			y = 0;
//	}
//}
//
//void CreateTile(unsigned char * buffer, unsigned char tile, 
//				int x, int y, char sprite, char table)
//{
//	int start = x;
//	int linestart = y;
//
//	bool bigsprite = false;
//
//	unsigned char * tileptr = VRAM + (tile * 16);
//
//	unsigned char tilearray[8][8];
//	unsigned char spritearray[8][8];
//	ZeroMemory(tilearray, 64);
//	ZeroMemory(spritearray, 64);
//
//	if(!sprite)
//	{
//		if(RAM[0x2000] & 16) //data at v0x1000
//				tileptr += 0x1000;
//	}
//
//	else if(sprite)
//		if(RAM[0x2000] & 8)
//			tileptr += 0x1000;
//	
//	x = 0;
//	y = 0;
//
//	PatternTableBits((unsigned char *)tilearray, tileptr, sprite);
//
//	if(sprite && RAM[0x2000] & 32)
//	{
//		PatternTableBits((unsigned char *)spritearray, tileptr + 16, sprite);
//		SpriteStuff((unsigned char *)spritearray, sprite);
//		bigsprite = true;
//	}
//
//	if(sprite)
//		SpriteStuff((unsigned char *)tilearray, sprite);
//
//	else //Atrribute tables//
//	{
//		unsigned char bits;
//		unsigned char attribute;
//		unsigned char * attrtable = VRAM + 0x23C0 + (0x400 * table);
//
//		unsigned char _y = linestart / 8;
//		unsigned char _x = start / 8;	//tile coordinates
//
//		attribute = attrtable[((_y/4) * 8) + (_x/4)];	//attr bytes are 4x4 tiles
//
//		unsigned char tilex = 0;
//		unsigned char tiley = 0;
//
//		if((_x % 4) > 1)
//			tilex = 1;
//		if((_y % 4) > 1)
//			tiley = 1;
//
//		if(!tilex && !tiley) bits = attribute & 0x03;
//		else if(tilex && !tiley) bits = (attribute >> 2) & 0x03;
//		else if(!tilex && tiley) bits = (attribute >> 4) & 0x03;
//		else if(tilex && tiley) bits = (attribute >> 6) & 0x03;
//
//		bits <<= 2;
//
//		for(y = 0; y < 8; y++)
//			for(x = 0; x < 8; x++)
//				if(tilearray[y][x])
//					tilearray[y][x] |= bits;
//	}
//
//	x = 0;
//	y = 0;
//
//	for(int i = linestart; i < linestart + 8; i++)
//	{
//		for(int j = start; j < start + 8; j++)
//		{
//			if(tilearray[y][x])
//			{
//				buffer[(i * 256) + j] = tilearray[y][x];
//			}
//			x++;
//		}
//		x = 0;
//		y++;
//	}
//
//	if(bigsprite)	//todo: clean/fix this
//		for(int i = linestart+8; i < linestart + 8; i++)
//		{
//			for(int j = start; j < start + 8; j++)
//			{
//				if(tilearray[y][x])
//				{
//					buffer[(i * 256) + j] = spritearray[y][x];
//				}
//				x++;
//			}
//			x = 0;
//			y++;
//		}
//}
//
//void CreateBackground(unsigned char * screen, unsigned char table)
//{
//	int x = 0, y = 0;
//	
//	unsigned char * nametable = VRAM + 0x2000 + (0x400 * table);
//
//	for(int i = 0; i < 960; i++)
//	{
//		CreateTile(screen, nametable[i], x, y, 0, table);
//
//		x += 8;
//
//		if(x == 256)
//		{
//			x = 0;
//			y += 8;
//		}
//	}
//}
//
//void CreateSprites(unsigned char * table)
//{
//	for(int i = 63; i >= 0; i--)
//	{
//		unsigned char * sprite = SPRRAM + (i * 4);
//
//		CreateTile(table, sprite[1], sprite[3], sprite[0]+1, i + 1, -1);
//	}
//}
//
//void PutPixel(unsigned char color, int pos)
//{
//	glColor3f(
//		nes_palette[color][0]/256.0,
//		nes_palette[color][1]/256.0,
//		nes_palette[color][2]/256.0);
//
//	glVertex2f(pos % 256, pos / 256);
//	glVertex2f(pos % 256, (pos / 256) - 1);
//	glVertex2f((pos % 256) + 1, (pos / 256) - 1);
//	glVertex2f((pos % 256) + 1, pos / 256);
//}
//
//void DrawScreen(unsigned char * table1, unsigned char * table2,
//				unsigned char * sprites, int * pixel, int xscr, int yscr)
//{
//	unsigned char * maintable;
//	unsigned char * secondtable;
//
//	unsigned char * palette = VRAM + 0x3F00;
//
//	if(*pixel == 0)
//	{
//		unsigned char clearcol = palette[0];
//
//		glClearColor(
//			nes_palette[clearcol][0]/256.0,
//			nes_palette[clearcol][1]/256.0,
//			nes_palette[clearcol][2]/256.0, 1);
//
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		glBegin(GL_QUADS);
//	}
//
//	if(!(RAM[0x2000] & 1))
//	{
//		maintable = table1;
//		secondtable = table2;
//	}
//
//	else
//	{
//		maintable = table1;
//		secondtable = table2;
//	}
//
//	int scanline = *pixel / 256;
//	int start = *pixel;
//
//	for(int x = 0; x < 8; x++) 
//	{
//		unsigned short addr = (*pixel) + xscr + (yscr * 256);
//
//		if(addr > (scanline * 256) + 255)
//		{
//			if(secondtable[addr])
//				PutPixel(palette[secondtable[addr - 256]], *pixel);
//		}
//
//		else if(maintable[addr])
//			PutPixel(palette[maintable[addr]], *pixel);
//
//		if(sprites[*pixel])
//		{
//			if(sprites[*pixel] & 128 && maintable[addr]
//				&& !(RAM[0x2002] & 64))
//			{
//				RAM[0x2002] |= 64;
//				*pixel = start;
//				return;
//			}
//
//			PutPixel(palette[sprites[*pixel] & 31], *pixel);
//		}
//
//		(*pixel)++;
//	}
//
//	if(*pixel == 61440)
//	{
//		glEnd();
//		SwapBuffers(hDC);
//	}
//}
//
//void resetCPU()
//{
//	A = 0;
//	X = 0;
//	Y = 0;
//
//	for(int i = 0; i < 0x8000; i++)
//		RAM[i] = 0;
//
//	NMI = RAM[0xFFFA] | (RAM[0xFFFB] << 8);
//	PC  = RAM[0xFFFC] | (RAM[0xFFFD] << 8);
//
//	if(!PC) PC = 0x8000;
//	
//	S = 0xFF;
//	P = 32;	//bit 5 always set
//}
//
//void resetPPU()
//{
//	ticks = timeGetTime();
//	IN_INT = 0;
//
//	for(int i = 0x2000; i < 0x4000; i++) VRAM[i] = 0;
//	
//	ZeroMemory(SPRRAM,0xFF);
//
//	glClearColor(0,0,0,0);
//	glClear(GL_COLOR_BUFFER_BIT);
//	SwapBuffers(hDC);
//
//	toggle = false;
//}
//
//void updateCPU()
//{
//	static bool strobe = false;
//	static unsigned int instructions = 0;
//
//	if(opcodes[RAM[PC]]) opcodes[RAM[PC]]();
//	else PC++;
//
//	instructions++;
//
//	if(instructions == 1300)
//	{
//		//Sleep(1);	//Waste time to avoid CPU overwork
//		instructions = 0;
//	}
//
//	if(JOYREAD)
//	{
//		currbit++;
//		JOYREAD = 0;
//	}
//
//	if(JOYWRITE)
//	{
//		if(strobe)
//		{
//			if(RAM[0x4016] == 0)
//			{
//				currbit = 0;
//				strobe = false;
//			}
//		}
//
//		else if(RAM[0x4016] == 1)
//			strobe = true;
//
//		else
//			strobe = false;
//
//		JOYWRITE = 0;
//	}
//
//	if(currbit < 8)
//		RAM[0x4016] = 0x40 | ((JOYPAD >> currbit) & 1);
//	else 
//		RAM[0x4016] = 0x41;
//}
//
//void updatePPU(HWND hwnd)
//{
//	static unsigned short ppuaddr;
//	static unsigned short temp;
//	static unsigned char spraddr;
//	static unsigned char xscroll = 0, yscroll = 0;
//
//	/*Instructions Per Second*/
//
//	static unsigned int instr = 0;
//	static int time = timeGetTime();
//	static char ips[10];
//	instr++;
//	int timetemp = timeGetTime();
//	if (timetemp - time >= 1000)
//	{
//		itoa(instr, ips, 10);
//		time = timeGetTime();
//		instr = 0;
//	}
//
//	/*********************************************/
//
//	else if(RAM[0x4014])	//SPR-RAM DMA
//	{
//		unsigned short RAMaddr = RAM[0x4014] * 0x100;
//		for(int i = 0; i < 256; i++)
//		{
//			SPRRAM[i] = RAM[RAMaddr + i];
//		}
//		RAM[0x4014] = 0;
//	}
//
//	/********************************************/
//	//General PPU address//
//	
//	else if(W2006)
//	{
//		if(!toggle)
//		{
//			toggle = true;
//			temp &= 0xFF;
//			temp = (RAM[0x2006] & 0x3F) << 8;
//		}
//		else
//		{
//			toggle = false;
//			temp &= 0xFF00;
//			temp |= RAM[0x2006];
//			ppuaddr = temp;
//			BADREAD = 0;
//		}
//
//		if(ppuaddr >= 0x3000 && ppuaddr < 0x3F00)
//			ppuaddr -= 0x1000;
//
//		if(ppuaddr >= 0x3F20)
//		{
//			char div = (ppuaddr - 0x3F00) / 0x20;
//			ppuaddr -= div * 0x20;
//		}
//
//		if(ppuaddr >= 0x3F00)
//			PALETTE = 1;
//		else PALETTE = 0;
//
//		if(ppuaddr > 0x3F10 && !(ppuaddr % 4))
//			ppuaddr -= 0x10;
//
//		W2006 = 0;
//	}
//
//	else if(W2000)
//	{
//		temp &= 0xF3FF;
//		temp |= (RAM[0x2000] & 3) << 10; //Name table bits
//
//		W2000 = 0;
//	}
//	
//	/********************************************/
//	//Scrolling offsets//
//
//	else if(W2005)
//	{
//		if(!toggle)
//		{
//			temp &= 0xFFE0;
//			temp |= RAM[0x2005] >> 3;
//			xscroll = RAM[0x2005];
//			toggle = 1;
//		}
//
//		else
//		{
//			temp &= 0xC1F;
//			temp |= (RAM[0x2005] & 0xF8) << 2;
//			temp |= RAM[0x2005] << 12;
//			yscroll = RAM[0x2005];
//			toggle = 0;
//		}
//
//		W2005 = 0;
//	}
//
//	/********************************************/
//
//	else if(PPUREAD)
//	{
//		ppuaddr++;
//
//		if(RAM[0x2000] & 4)
//			ppuaddr += 31;
//
//		PPUREAD = 0;
//		
//		if(ppuaddr >= 0x3000 && ppuaddr < 0x3F00)
//			ppuaddr -= 0x1000;
//
//		if(ppuaddr >= 0x3F20)
//		{
//			char div = (ppuaddr - 0x3F00) / 0x20;
//			ppuaddr -= div * 0x20;
//		}
//
//		if(ppuaddr >= 0x3F00)
//			PALETTE = 1;
//	}
//
//	/********************************************/
//
//	else if(PPUWRITE)
//	{
//		if(ppuaddr > 0x3F00 && !(ppuaddr % 0x10))
//			VRAM[0x3F00] = RAM[0x2007];
//
//		else VRAM[ppuaddr] = RAM[0x2007];
//
//		ppuaddr++;
//
//		if(RAM[0x2000] & 4)
//			ppuaddr += 31;
//
//		PPUWRITE = 0;
//
//		if(ppuaddr >= 0x3000 && ppuaddr < 0x3F00)
//			ppuaddr -= 0x1000;
//
//		if(ppuaddr >= 0x3F20)
//		{
//			char div = (ppuaddr - 0x3F00) / 0x20;
//			ppuaddr -= div * 0x20;
//		}
//
//		if(ppuaddr >= 0x3F00)
//			PALETTE = 1;
//	}
//
//	/*********************************************/
//
//	if(W2003)
//	{
//		spraddr = RAM[0x2003];
//		W2003 = 0;
//	}
//
//	else if(SPRREAD)
//	{
//		spraddr++;
//		SPRREAD = 0;
//	}
//
//	else if(SPRWRITE)
//	{
//		SPRRAM[spraddr] = RAM[0x2004];
//		spraddr++;
//		SPRWRITE = 0;
//	}
//
//	RAM[0x2004] = SPRRAM[spraddr];
//
//	/********************************************/
//
//	if(ppuaddr >= 0x3F00) RAM[0x2007] = VRAM[ppuaddr];
//	else RAM[0x2007] = VRAM[ppuaddr-1];
//
//	/********************************************/
//
//	/* Frames Per Second */
//
//	static int time2 = timeGetTime();
//	static int frames = 0;
//	static char fps[4];
//	int temptiem = timeGetTime();
//	if(temptiem-time2 >= 1000)
//	{
//		itoa(frames, fps, 10);
//		frames = 0;
//		time2 = timeGetTime();
//	}
//
//	/*******************************************/
//
//	int temptime = timeGetTime();
//
//	static bool draw = true;
//	if(temptime-ticks >= 17)	//I know this isn't exactly 1/60 of a second, 
//								//but it is the highest speed that 
//								//doesnt compromise stability
//	{
//		draw = true;
//		ticks = timeGetTime();
//	}
//
//	static int pixel = 0; //pixel = scanline for now
//	static int wastetime = 0;
//
//	static unsigned char table1[0xF000];
//	static unsigned char table2[0xF000];
//	static unsigned char sprites[0xF000];
//
//	if(draw)
//	{
//		if(!pixel)
//		{
//			RAM[0x2002] &= 63;
//			ZeroMemory(table1, 0xF000);
//			ZeroMemory(table2, 0xF000);
//			ZeroMemory(sprites, 0xF000);
//
//			if(RAM[0x2001] & 8)
//			{
//				CreateBackground(table1,0);
//				CreateBackground(table2,1);
//				
//				if(RAM[0x2001] & 16)
//					CreateSprites(sprites);
//			}
//		}
//
//		DrawScreen(table1, table2, sprites, &pixel, xscroll, yscroll);
//
//		if(pixel == 61440)
//		{
//			RAM[0x2002] |= 128;
//			draw = false;
//			pixel = 0;
//
//			frames++;
//				
//			char title[50] = "Project 8     IPS: ";
//			strcat(title, ips);
//			strcat(title, "     FPS: ");
//			strcat(title, fps);
//			SetWindowText(hwnd,title);
//
//			if(!IN_INT && (RAM[0x2000] & 128))	//NMI
//			{		
//				pushw(PC);
//				pushb(P);
//				PC = NMI;
//
//				IN_INT = 1;
//			}
//		}
//	}
//
//	
//}
//
//void pushw(unsigned short word)
//{
//	RAM[S+0x100] = (unsigned char) (word >> 8);
//	S--;
//	RAM[S+0x100] = (unsigned char) (word & 0xFF);
//	S--;
//}
//
//unsigned short int popw()
//{
//	S++;
//	unsigned char lo = RAM[S+0x100];
//	S++;
//	unsigned char hi = RAM[S+0x100];
//	return ((hi << 8) | lo);
//}
//
//void pushb(unsigned char byte)
//{
//	RAM[S+0x100] = byte;
//	S--;
//}
//
//unsigned char popb()
//{
//	S++;
//	return RAM[S+0x100];
//}
//
//unsigned short fetchword()
//{
//	PC++;
//	unsigned char lo = RAM[PC];
//	PC++;
//	unsigned char hi = RAM[PC];
//	PC++;
//
//	return (hi << 8) | lo;
//}
//
//unsigned char fetchbyte()
//{
//	PC++;
//	unsigned char byte = RAM[PC];
//	PC++;
//
//	return byte;
//}
//
//void CheckPortsW(unsigned short addr)
//{
//	if (addr == 0x2000) W2000 = 1;
//	else if(addr == 0x2003) W2003 = 1;
//	else if(addr == 0x2004) SPRWRITE = 1;
//	else if(addr == 0x2005) W2005 = 1;
//	else if(addr == 0x2006) W2006 = 1;
//	else if(addr == 0x2007) PPUWRITE = 1;
//	else if(addr == 0x4016) JOYWRITE = 1;
//}
//
//void ADC(unsigned char byte)
//{
//	unsigned char temp = A;
//	A += byte + (P & 1);
//
//	if(temp > A) P |= 1;
//	else P &= 254;
//
//	if(((temp&128)&&(byte&128)&&(~A&128))||
//		((~temp&128)&&(~byte&128)&&(A&128)))
//		P |= 64;
//	else P &= 0xBF;
//}
//
//void SBC(unsigned char byte)
//{
//	unsigned char temp = A;
//	A = A - byte - (~P&1);
//
//	if(A < temp) P |= 1;
//	else P &= 254;
//
//	if(((temp&128)&&(~byte&128)&&(~A&128))||
//		((~temp&128)&&(byte&128)&&(A&128)))
//		P |= 64;
//	else P &= 0xBF;
//}
//
//void CMP(unsigned char byte)
//{
//	unsigned char result = A - byte;
//
//	if(A >= byte) P |= 1;
//	else P &= 254;
//
//	N_FLAGS(result);
//	Z_FLAGS(result);
//}
//
//void BRK()
//{
//	PC++;
//
//	P |= 16;
//	
//	pushw(PC+1);
//	pushb(P);
//
//	PC = (RAM[0xFFFF] << 8) | RAM[0xFFFE];
//}
//
//void ORA_IX()
//{
//	unsigned char zp = fetchbyte() + X;
//
//	A |= RAM[(RAM[zp+1] << 8) | RAM[zp]];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ORA_Z()
//{
//	A |= RAM[fetchbyte()];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ASL_Z()
//{
//	unsigned char addr = fetchbyte();
//
//	P &= 254;
//	P |= RAM[addr] >> 7;
//
//	RAM[addr] <<= 1;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void PHP()
//{
//	PC++;
//	
//	P |= 16;	//Set BRK bit. It's a bug.
//	pushb(P);
//}
//
//void ORA_I()
//{
//	A |= fetchbyte();
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ASL_AC()
//{
//	PC++;
//
//	P &= 254;
//	P |= A >> 7;
//
//	A <<= 1;
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ORA_A()
//{
//	A |= RAM[fetchword()];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ASL_A()
//{
//	unsigned short addr = fetchword();
//
//	P &= 254;
//	P |= RAM[addr] >> 7;
//
//	RAM[addr] <<= 1;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void BPL()
//{
//	char byte = (char)fetchbyte();
//
//	if (!(P & 128)) PC += byte;
//}
//
//void ORA_IY()
//{
//	unsigned char zp = fetchbyte();
//
//	A |= RAM[((RAM[zp+1] << 8) | RAM[zp]) + Y];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ORA_ZX()
//{
//	A |= RAM[(fetchbyte() + X) & 0xFF];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ASL_ZX()
//{
//	unsigned char addr = fetchbyte() + X;
//
//	P &= 254;
//	P |= RAM[addr] >> 7;
//
//	RAM[addr] <<= 1;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void CLC()
//{
//	PC++;
//
//	P &= 254;
//}
//
//void ORA_AY()
//{
//	A |= RAM[(fetchword() + Y) & 0xFFFF];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ORA_AX()
//{
//	A |= RAM[(fetchword() + X) & 0xFFFF];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ASL_AX()
//{
//	unsigned short addr = fetchword() + X;
//
//	P &= 254;
//	P |= RAM[addr] >> 7;
//
//	RAM[addr] <<= 1;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void JSR()
//{
//	unsigned short address = fetchword();
//	if(address < 0x8000) return;
//
//	pushw(PC - 1);
//
//	PC = address;
//}
//
//void AND_IX()
//{
//	unsigned char zp = fetchbyte() + X;
//
//	A &= RAM[(RAM[zp+1] << 8) | RAM[zp]];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void BIT_Z()
//{
//	unsigned char byte = RAM[fetchbyte()];
//	unsigned char result = A & byte;
//
//	P &= 63;
//	P |= (byte & 192);
//
//	Z_FLAGS(result);
//}
//
//void AND_Z()
//{
//	A &= RAM[fetchbyte()];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ROL_Z()
//{
//	unsigned short addr = fetchbyte();
//	unsigned char temp = RAM[addr];
//
//	RAM[addr] <<= 1;
//	RAM[addr] |= P & 1;
//
//	P &= 254;
//	P |= temp >> 7;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void PLP()
//{
//	PC++;
//	P = popb();
//	P |= 32; //bit 5 is ALWAYS set
//}
//
//void AND_I()
//{
//	A &= fetchbyte();
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ROL_AC()
//{
//	PC++;
//
//	unsigned char temp = A;
//
//	A <<= 1;
//	A |= P & 1;
//
//	P &= 254;
//	P |= temp >> 7;
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void BIT_A()
//{
//	unsigned char byte = RAM[fetchword()];
//
//	P &= 63;
//	P |= byte & 192;
//
//	Z_FLAGS(byte & A);
//}
//
//void AND_A()
//{
//	A &= RAM[fetchword()];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ROL_A()
//{
//	unsigned short addr = fetchword();
//	unsigned char temp = RAM[addr];
//
//	RAM[addr] <<= 1;
//	RAM[addr] |= P & 1;
//
//	P &= 254;
//	P |= temp >> 7;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void BMI()
//{
//	char byte = (char)fetchbyte();
//
//	if(P & 128) PC += byte;
//}
//
//void AND_IY()
//{
//	unsigned char zp = fetchbyte();
//
//	A &= RAM[((RAM[zp+1] << 8) | RAM[zp]) + Y];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void AND_ZX()
//{
//	A &= RAM[(fetchbyte() + X) & 0xFF];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ROL_ZX()
//{
//	unsigned char addr = fetchbyte() + X;
//	unsigned char temp = RAM[addr];
//
//	RAM[addr] <<= 1;
//	RAM[addr] |= P & 1;
//
//	P &= 254;
//	P |= temp >> 7;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void SEC()
//{
//	PC++;
//	P |= 1;
//}
//
//void AND_AY()
//{
//	A &= RAM[(fetchword() + Y) & 0xFFFF];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void AND_AX()
//{
//	A &= RAM[(fetchword() + X) & 0xFFFF];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ROL_AX()
//{
//	unsigned short addr = fetchword() + X;
//	unsigned char temp = RAM[addr];
//
//	RAM[addr] <<= 1;
//	RAM[addr] |= P & 1;
//
//	P &= 254;
//	P |= (temp >> 7);
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void RTI()
//{
//	IN_INT = 0;
//
//	P = popb();
//	PC = popw();
//
//	P &= 0xEF; //break flag
//}
//
//void EOR_IX()
//{
//	unsigned char zp = fetchbyte() + X;
//
//	A ^= RAM[(RAM[zp+1] << 8) | RAM[zp]];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void EOR_Z()
//{
//	A ^= RAM[fetchbyte()];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void LSR_Z()
//{
//	unsigned char addr = fetchbyte();
//
//	P &= 254;
//	P |= RAM[addr] & 1;
//
//	RAM[addr] >>= 1;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void PHA()
//{
//	PC++;
//	pushb(A);
//}
//
//void EOR_I()
//{
//	A ^= fetchbyte();
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void LSR_AC()
//{
//	PC++;
//	
//	P &= 254;
//	P |= A & 1;
//
//	A >>= 1;
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void JMP_A()
//{
//	PC = fetchword();
//}
//
//void EOR_A()
//{
//	A ^= RAM[fetchword()];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void LSR_A()
//{
//	unsigned short addr = fetchword();
//
//	P &= 254;
//	P |= RAM[addr] & 1;
//
//	RAM[addr] >>= 1;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void BVC()
//{
//	char byte = (char)fetchbyte();
//
//	if(!(P & 64))
//		PC += byte;
//}
//
//void EOR_IY()
//{
//	unsigned char zp = fetchbyte();
//
//	A ^= RAM[((RAM[zp+1] << 8) | RAM[zp]) + Y];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void EOR_ZX()
//{
//	A ^= RAM[(fetchbyte() + X) & 0xFF];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void LSR_ZX()
//{
//	unsigned char addr = fetchbyte() + X;
//
//	P &= 254;
//	P |= RAM[addr] & 1;
//
//	RAM[addr] >>= 1;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void CLI()
//{
//	PC++;
//
//	P &= 251;
//}
//
//void EOR_AY()
//{
//	A ^= RAM[(fetchword() + Y) & 0xFFFF];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void EOR_AX()
//{
//	A ^= RAM[(fetchword() + X) & 0xFFFF];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void LSR_AX()
//{
//	unsigned short addr = fetchword() + X;
//
//	P &= 254;
//	P |= RAM[addr] & 1;
//
//	RAM[addr] >>= 1;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void RTS()
//{
//	PC = popw();
//	PC++;
//}
//
//void ADC_IX()
//{
//	unsigned char zp = fetchbyte() + X;
//
//	ADC(RAM[(RAM[zp+1] << 8) | RAM[zp]]);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ADC_Z()
//{
//	ADC(RAM[fetchbyte()]);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ROR_Z()
//{
//	unsigned char addr = fetchbyte();
//	unsigned char temp = RAM[addr];
//
//	RAM[addr] >>= 1;
//
//	RAM[addr] |= P << 7;
//
//	P &= 254;
//	P |= temp & 1;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void PLA()
//{
//	PC++;
//	A = popb();
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ADC_I()
//{
//	ADC(fetchbyte());
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ROR_AC()
//{
//	PC++;
//
//	unsigned char temp = A;
//	A >>= 1;
//	A |= P << 7;
//
//	P &= 254;
//	P |= temp & 1;
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void JMP_IN()
//{
//	unsigned short addr = fetchword();
//
//	PC = (RAM[((addr + 1) & 0xFF) | (addr & 0xFF00)] << 8) | RAM[addr];
//}
//
//void ADC_A()
//{
//	ADC(RAM[fetchword()]);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}	
//
//void ROR_A()
//{
//	unsigned short int addr = fetchword();
//	unsigned char temp = RAM[addr];
//
//	RAM[addr] >>= 1;
//	RAM[addr] |= P << 7;
//
//	P &= 254;
//	P |= (temp & 1);
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void BVS()
//{
//	char byte = (char)fetchbyte();
//
//	if(P & 64)
//		PC += byte;
//}
//
//void ADC_IY()
//{
//	unsigned char zp = fetchbyte();
//
//	ADC(((RAM[zp+1] << 8) | RAM[zp]) + Y);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ADC_ZX()
//{
//	ADC(RAM[(fetchbyte() + X) & 0xFF]);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ROR_ZX()
//{
//	unsigned char addr = fetchbyte() + X;
//	unsigned char temp = RAM[addr];
//
//	RAM[addr] >>= 1;
//
//	RAM[addr] |= P << 7;
//
//	P &= 254;
//	P |= temp & 1;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void SEI()
//{
//	PC++;
//	
//	P |= 4;
//}
//
//void ADC_AY()
//{
//	ADC(RAM[(fetchword() + Y) & 0xFFFF]);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ADC_AX()
//{
//	ADC(RAM[(fetchword() + X) & 0xFFFF]);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void ROR_AX()
//{
//	unsigned short addr = fetchword() + X;
//	unsigned char temp = RAM[addr];
//
//	RAM[addr] >>= 1;
//	RAM[addr] |= P << 7;
//
//	P &= 254;
//	P |= temp & 1;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void STA_IX()
//{
//	unsigned char zp = fetchbyte() + X;
//
//	RAM[(RAM[zp+1] << 8) | RAM[zp]] = A;
//}
//
//void STY_Z()
//{
//	RAM[fetchbyte()] = Y;
//}
//
//void STA_Z()
//{
//	RAM[fetchbyte()] = A;
//}
//
//void STX_Z()
//{
//	RAM[fetchbyte()] = X;
//}
//
//void DEY()
//{
//	PC++;
//	Y--;
//
//	Z_FLAGS(Y);
//	N_FLAGS(Y);
//}
//
//void TXA()
//{
//	PC++;
//	
//	A = X;
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void STY_A()
//{
//	unsigned short addr = fetchword();
//	
//	if(addr >= 0x800 && addr < 0x2000)
//	{
//		char div = addr / 0x800;
//		addr -= (0x800 * div);
//	}
//
//	CheckPortsW(addr);
//
//	RAM[addr] = Y;
//}
//
//void STA_A()
//{
//	unsigned short addr = fetchword();
//
//	if(addr >= 0x800 && addr < 0x2000)
//	{
//		char div = addr / 0x800;
//		addr -= (0x800 * div);
//	}
//
//	CheckPortsW(addr);
//
//	RAM[addr] = A;
//}
//
//void STX_A()
//{
//	unsigned short addr = fetchword();
//
//	if(addr >= 0x800 && addr < 0x2000)
//	{
//		char div = addr / 0x800;
//		addr -= (0x800 * div);
//	}
//
//	CheckPortsW(addr);
//
//	RAM[addr] = X;
//}
//
//void BCC()
//{
//	char change  = (char)fetchbyte();
//
//	if(!(P & 1))
//		PC += change;
//}
//
//void STA_IY()
//{
//	unsigned char zp = fetchbyte();
//
//	RAM[((RAM[zp+1] << 8) | RAM[zp]) + Y] = A;
//}
//
//void STY_ZX()
//{
//	RAM[(fetchbyte() + X) & 0xFF] = Y;
//}
//
//void STA_ZX()
//{
//	RAM[(fetchbyte() + X) & 0xFF] = A;
//}
//
//void STX_ZY()
//{
//	RAM[(fetchbyte() + Y) & 0xFF] = X;
//}
//
//void TYA()
//{
//	PC++;
//
//	A = Y;
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void STA_AY()
//{
//	unsigned short addr = fetchword() + Y;
//
//	CheckPortsW(addr);
//	
//	RAM[addr] = A;
//}
//
//void TXS()
//{
//	PC++;
//
//	S = X;
//}
//
//void STA_AX()
//{
//	unsigned short addr = fetchword() + X;
//
//	CheckPortsW(addr);
//
//	RAM[addr] = A;
//}
//
//void LDY_I()
//{
//	Y = fetchbyte();
//
//	Z_FLAGS(Y);
//	N_FLAGS(Y);
//}
//
//void LDA_IX()
//{
//	unsigned char zp = fetchbyte() + X;
//
//	A = RAM[(RAM[zp+1] << 8) | RAM[zp]];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void LDX_I()
//{
//	X = fetchbyte();
//
//	Z_FLAGS(X);
//	N_FLAGS(X);
//}
//
//void LDY_Z()
//{
//	Y = RAM[fetchbyte()];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void LDA_Z()
//{
//	A = RAM[fetchbyte()];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void LDX_Z()
//{
//	X = RAM[fetchbyte()];
//
//	N_FLAGS(X);
//	Z_FLAGS(X);
//}
//
//void TAY()
//{
//	PC++;
//
//	Y = A;
//	N_FLAGS(Y);
//	Z_FLAGS(Y);
//}
//
//void LDA_I()
//{
//	A = fetchbyte();
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void TAX()
//{
//	PC++;
//
//	X = A;
//	N_FLAGS(X);
//	Z_FLAGS(X);
//}
//
//void LDY_A()
//{
//	unsigned short addr = fetchword();
//
//	if(addr >= 0x800 && addr < 0x2000)
//	{
//		char div = addr / 0x800;
//		addr -= (0x800 * div);
//	}
//
//	if(addr == 0x2007 && !BADREAD && !PALETTE)
//	{
//		PPUREAD = 1;
//		BADREAD = 1;
//		N_FLAGS(Y);
//		Z_FLAGS(Y);
//		return;
//	}
//
//	Y = RAM[addr];
//
//	if(addr == 0x2002)
//	{
//		RAM[0x2002] &= 127;
//		toggle = false;
//	}
//	if(addr == 0x2004) SPRREAD = 1;
//	if(addr == 0x2007) PPUREAD = 1;
//	if(addr == 0x4016) JOYREAD = 1;
//
//	N_FLAGS(Y);
//	Z_FLAGS(Y);
//}
//
//void LDA_A()
//{
//	unsigned short addr = fetchword();
//
//	if(addr >= 0x800 && addr < 0x2000)
//	{
//		char div = addr / 0x800;
//		addr -= (0x800 * div);
//	}
//
//	if(addr == 0x2007 && !BADREAD && !PALETTE)
//	{
//		PPUREAD = 1;
//		BADREAD = 1;
//		N_FLAGS(A);
//		Z_FLAGS(A);
//		return;
//	}
//
//	A = RAM[addr];
//
//	if(addr == 0x2002)
//	{
//		RAM[addr] &= 127;
//		toggle = false;
//	}
//
//	else if(addr == 0x2004) SPRREAD = 1;
//	else if(addr == 0x2007) PPUREAD = 1;
//	else if(addr == 0x4016) JOYREAD = 1;
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void LDX_A()
//{
//	unsigned short addr = fetchword();
//
//	if(addr >= 0x800 && addr < 0x2000)
//	{
//		char div = addr / 0x800;
//		addr -= (0x800 * div);
//	}
//
//	if(addr == 0x2007 && !BADREAD && !PALETTE)
//	{
//		PPUREAD = 1;
//		BADREAD = 1;
//		N_FLAGS(X);
//		Z_FLAGS(X);
//		return;
//	}
//
//	X = RAM[addr];
//
//	if(addr == 0x2002)
//	{
//		RAM[0x2002] &= 127;
//		toggle = false;
//	}
//
//	if(addr == 0x2004) SPRREAD = 1;
//	if(addr == 0x2007) PPUREAD = 1;
//	if(addr == 0x4016) JOYREAD = 1;
//
//	N_FLAGS(X);
//	Z_FLAGS(X);
//}
//
//void BCS()
//{
//	char byte = (char)fetchbyte();
//
//	if(P & 1)
//		PC += byte;
//}
//
//void LDA_IY()
//{
//	unsigned char zp = fetchbyte();
//
//	A = RAM[((RAM[zp+1] << 8) | RAM[zp]) + Y];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void LDY_ZX()
//{
//	Y = RAM[(fetchbyte() + X) & 0xFF];
//
//	N_FLAGS(Y);
//	Z_FLAGS(Y);
//}
//
//void LDA_ZX()
//{
//	A = RAM[(fetchbyte() + X) & 0xFF];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void LDX_ZY()
//{
//	X = RAM[(fetchbyte() + Y) & 0xFF];
//
//	N_FLAGS(X);
//	Z_FLAGS(X);
//}
//
//void CLV()
//{
//	PC++;
//	P &= 191;
//}
//
//void LDA_AY()
//{
//	A = RAM[(fetchword() + Y) & 0xFFFF];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void TSX()
//{
//	PC++;
//
//	X = S;
//
//	N_FLAGS(X);
//	Z_FLAGS(X);
//}
//
//void LDY_AX()
//{
//	Y = RAM[(fetchword() + X) & 0xFFFF];
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void LDA_AX()
//{
//	unsigned short addr = fetchword() + X;
//
//	A = RAM[addr];
//
//	if(addr == 0x4016) JOYREAD = 1;
//
//	Z_FLAGS(A);
//	N_FLAGS(A);
//}
//
//void LDX_AY()
//{
//	X = RAM[(fetchword() + Y) & 0xFFFF];
//
//	N_FLAGS(X);
//	Z_FLAGS(X);
//}
//
//void CPY_I()
//{
//	unsigned char byte = fetchbyte();
//
//	unsigned char result = Y - byte;
//
//	if(Y >= byte) P |= 1;
//	else P &= 254;
//
//	N_FLAGS(result);
//	Z_FLAGS(result);
//}
//
//void CMP_IX()
//{
//	unsigned char zp = fetchbyte() + X;
//
//	CMP(RAM[(RAM[zp+1] << 8) | RAM[zp]]);
//}
//
//void CPY_Z()
//{
//	unsigned char byte = RAM[fetchbyte()];
//
//	unsigned char result = Y - byte;
//
//	if(Y >= byte) P |= 1;
//	else P &= 254;
//
//	N_FLAGS(result);
//	Z_FLAGS(result);
//}
//
//void CMP_Z()
//{
//	CMP(RAM[fetchbyte()]);
//}
//
//void DEC_Z()
//{
//	unsigned char addr = fetchbyte();
//
//	RAM[addr]--;
//
//	Z_FLAGS(RAM[addr]);
//	N_FLAGS(RAM[addr]);
//}
//
//void INY()
//{
//	PC++;
//
//	Y++;
//
//	N_FLAGS(Y);
//	Z_FLAGS(Y);
//}
//
//void CMP_I()
//{
//	unsigned char byte = fetchbyte();
//
//	CMP(byte);
//}
//
//void DEX()
//{
//	PC++;
//
//	X--;
//
//	N_FLAGS(X);
//	Z_FLAGS(X);
//}
//
//void CPY_A()
//{
//	unsigned char byte = RAM[fetchword()];
//
//	unsigned char result = Y - byte;
//
//	if(Y >= byte) P |= 1;
//	else P &= 254;
//
//	N_FLAGS(result);
//	Z_FLAGS(result);
//}
//
//void CMP_A()
//{
//	CMP(RAM[fetchword()]);
//}
//
//void DEC_A()
//{
//	unsigned short addr = fetchword();
//	RAM[addr]--;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void BNE()
//{
//	char byte = (char)fetchbyte();
//
//	if(!(P & 2))
//		PC += byte;
//}
//
//void CMP_IY()
//{
//	unsigned char zp = fetchbyte();
//
//	CMP(RAM[(RAM[zp+1] << 8) | RAM[zp]]);
//}
//
//void CMP_ZX()
//{
//	CMP(RAM[(fetchbyte() + X) & 0xFF]);
//}
//
//void DEC_ZX()
//{
//	unsigned char addr = fetchbyte() + X;
//
//	RAM[addr]--;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void CLD()
//{
//	PC++;
//
//	P = P & 247;
//}
//
//void CMP_AY()
//{
//	CMP(RAM[(fetchword() + Y) & 0xFFFF]);
//}
//
//void CMP_AX()
//{
//	CMP(RAM[(fetchword() + X) & 0xFFFF]);
//}
//
//void DEC_AX()
//{
//	unsigned short addr = fetchword() + X;
//
//	RAM[addr]--;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void CPX_I()
//{
//	unsigned char byte = fetchbyte();
//
//	unsigned char result = X - byte;
//
//	if(X >= byte) P |= 1;
//	else P &= 254;
//
//	N_FLAGS(result);
//	Z_FLAGS(result);
//}
//
//void SBC_IX()
//{
//	unsigned char zp = fetchbyte() + X;
//
//
//	SBC(RAM[(RAM[zp+1] << 8) | RAM[zp]]);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void CPX_Z()
//{
//	unsigned char byte = RAM[fetchbyte()];
//
//	unsigned char result = X - byte;
//
//	if(X >= byte) P |= 1;
//	else P &= 254;
//
//	N_FLAGS(result);
//	Z_FLAGS(result);
//}
//
//void SBC_Z()
//{
//	SBC(RAM[fetchbyte()]);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void INC_Z()
//{
//	unsigned char addr = fetchbyte();
//
//	RAM[addr]++;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void INX()
//{
//	PC++;
//
//	X++;
//
//	N_FLAGS(X);
//	Z_FLAGS(X);
//}
//
//void SBC_I()
//{
//	SBC(fetchbyte());
//	
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void NOP()
//{
//	PC++;
//}
//
//void CPX_A()
//{
//	unsigned char byte = RAM[fetchword()];
//
//	unsigned char result = X - byte;
//
//	if(X >= byte) P |= 1;
//	else P &= 254;
//
//	N_FLAGS(result);
//	Z_FLAGS(result);
//}
//
//void SBC_A()
//{
//	SBC(RAM[fetchword()]);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void INC_A()
//{
//	unsigned short int addr = fetchword();
//
//	RAM[addr]++;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void BEQ()
//{
//	PC++;
//	char change = RAM[PC];
//	PC++;
//
//	if(P & 2) PC += change;
//}
//
//void SBC_IY()
//{
//	unsigned char zp = fetchbyte();
//
//	SBC(RAM[(RAM[zp+1] << 8) | RAM[zp]]);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void SBC_ZX()
//{
//	SBC(RAM[(fetchbyte() + X) & 0xFF]);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void INC_ZX()
//{
//	unsigned char addr = fetchbyte() + X;
//	
//	RAM[addr]++;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void SED()
//{
//	PC++;
//	P |= 8;
//}
//
//void SBC_AY()
//{
//	SBC(RAM[(fetchword() + Y) & 0xFFFF]);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void SBC_AX()
//{
//	SBC(RAM[(fetchword() + X) & 0xFFFF]);
//
//	N_FLAGS(A);
//	Z_FLAGS(A);
//}
//
//void INC_AX()
//{
//	unsigned short addr = fetchword() + X;
//	
//	RAM[addr]++;
//
//	N_FLAGS(RAM[addr]);
//	Z_FLAGS(RAM[addr]);
//}
//
//void N_FLAGS(unsigned char REG)
//{
//	if (REG & 128) P |= 128;
//	else P &= 127;
//}
//
//void Z_FLAGS(unsigned char REG)
//{
//	if (!REG) P |= 2;
//	else P &= 0xFD;
//}
