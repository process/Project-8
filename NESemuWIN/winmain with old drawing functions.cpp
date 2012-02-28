#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <windows.h>
#include <gl\gl.h>

#include "rsrc.h"

#pragma comment (lib,"opengl32.lib")

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void initGL(HWND hwnd);
void DialogOpen(HWND);
void DropOpen(WPARAM);
void LoadFile(char * path);

bool debug = false;
HANDLE ConOut;
HANDLE ConIn;
void ConWrite(char *);
void CPUinfo();

void resetCPU();
void resetPPU();
void updateCPU();
void updatePPU();

int HeaderCheck(FILE *);
void LoadFile(char * path);

char play = 0;

HDC hDC;
HGLRC hRC;

unsigned char A = 0;
unsigned char X = 0;
unsigned char Y = 0;
unsigned char P = 0;

unsigned char RAM[65536];
unsigned char VRAM[16384];
unsigned char SPRRAM[256];
unsigned char S = 0xFF;
unsigned short int PC = 0;

int ticks;
unsigned short NMI;
unsigned short RETURN;
unsigned char JOYPAD;
unsigned char IN_INT = 0;

unsigned char PPUREAD = 0;
unsigned char BADREAD = 0;
unsigned char PPUWRITE = 0;
unsigned char SPRREAD = 0;
unsigned char SPRWRITE = 0;
unsigned char PALETTE = 0;
unsigned char W2005 = 0;
unsigned char W2006 = 0;
unsigned char W2003 = 0;
unsigned char W2000 = 0;
bool toggle;
int hit;

unsigned char JOYREAD = 0;
unsigned char JOYWRITE = 0;
unsigned char currbit = 0;


void pushb(unsigned char);
void pushw(unsigned short);
unsigned char popb();
unsigned short popw();

void BRK();
void ORA_IX();
void ORA_Z();
void ASL_Z();
void PHP();
void ORA_I();
void ASL_AC();
void ORA_A();
void ASL_A();

void BPL();
void ORA_IY();
void ORA_ZX();
void ASL_ZX();
void CLC();
void ORA_AY();
void ORA_AX();
void ASL_AX();

void JSR();
void AND_IX();
void BIT_Z();
void AND_Z();
void ROL_Z();
void PLP();
void AND_I();
void ROL_AC();
void BIT_A();
void AND_A();
void ROL_A();

void BMI();
void AND_IY();
void AND_ZX();
void ROL_ZX();
void SEC();
void AND_AY();
void AND_AX();
void ROL_AX();

void RTI();
void EOR_IX();
void EOR_Z();
void LSR_Z();
void PHA();
void EOR_I();
void LSR_AC();
void JMP_A();
void EOR_A();
void LSR_A();

void BVC();
void EOR_IY();
void EOR_ZX();
void LSR_ZX();
void CLI();
void EOR_AY();
void EOR_AX();
void LSR_AX();

void RTS();
void ADC_IX();
void ADC_Z();
void ROR_Z();
void PLA();
void ADC_I();
void ROR_AC();
void JMP_IN();
void ADC_A();
void ROR_A();

void BVS();
void ADC_IY();
void ADC_ZX();
void ROR_ZX();
void SEI();
void ADC_AY();
void ADC_AX();
void ROR_AX();

void STA_IX();
void STY_Z();
void STA_Z();
void STX_Z();
void DEY();
void TXA();
void STY_A();
void STA_A();
void STX_A();

void BCC();
void STA_IY();
void STY_ZX();
void STA_ZX();
void STX_ZY();
void TYA();
void STA_AY();
void TXS();
void STA_AX();

void LDY_I();
void LDA_IX();
void LDX_I();
void LDY_Z();
void LDA_Z();
void LDX_Z();
void TAY();
void LDA_I();
void TAX();
void LDY_A();
void LDA_A();
void LDX_A();

void BCS();
void LDA_IY();
void LDY_ZX();
void LDA_ZX();
void LDX_ZY();
void CLV();
void LDA_AY();
void TSX();
void LDY_AX();
void LDA_AX();
void LDX_AY();

void CPY_I();
void CMP_IX();
void CPY_Z();
void CMP_Z();
void DEC_Z();
void INY();
void CMP_I();
void DEX();
void CPY_A();
void CMP_A();
void DEC_A();

void BNE();
void CMP_IY();
void CMP_ZX();
void DEC_ZX();
void CLD();
void CMP_AY();
void CMP_AX();
void DEC_AX();

void CPX_I();
void SBC_IX();
void CPX_Z();
void SBC_Z();
void INC_Z();
void INX();
void SBC_I();
void NOP();
void CPX_A();
void SBC_A();
void INC_A();

void BEQ();
void SBC_IY();
void SBC_ZX();
void INC_ZX();
void SED();
void SBC_AY();
void SBC_AX();
void INC_AX();

void N_FLAGS(unsigned char);
void Z_FLAGS(unsigned char);

void (*opcodes[256])() = 
{
	BRK,ORA_IX,0,0,0,ORA_Z,ASL_Z,0,PHP,ORA_I,ASL_AC,0,0,ORA_A,ASL_A,0,
	BPL,ORA_IY,0,0,0,ORA_ZX,0,ASL_ZX,CLC,ORA_AY,0,0,0,ORA_AX,ASL_AX,0,
	JSR,AND_IX,0,0,BIT_Z,AND_Z,ROL_Z,0,PLP,AND_I,ROL_AC,0,BIT_A,AND_A,ROL_A,0,
	BMI,AND_IY,0,0,0,AND_ZX,ROL_ZX,0,SEC,AND_AY,0,0,0,AND_AX,ROL_AX,0,
	RTI,EOR_IX,0,0,0,EOR_Z,LSR_Z,0,PHA,EOR_I,LSR_AC,0,JMP_A,EOR_A,LSR_A,0,
	BVC,EOR_IY,0,0,0,EOR_ZX,LSR_ZX,CLI,0,EOR_AY,0,0,0,EOR_AX,LSR_AX,0,
	RTS,ADC_IX,0,0,0,ADC_Z,ROR_Z,0,PLA,ADC_I,ROR_AC,0,JMP_IN,ADC_A,ROR_A,0,
	BVS,ADC_IY,0,0,0,ADC_ZX,0,ROR_ZX,SEI,ADC_AY,0,0,0,ADC_AX,ROR_AX,0,
	0,STA_IX,0,0,STY_Z,STA_Z,STX_Z,0,DEY,0,TXA,0,STY_A,STA_A,STX_A,0,
	BCC,STA_IY,0,0,STY_ZX,STA_ZX,STX_ZY,0,TYA,STA_AY,TXS,0,0,STA_AX,0,0,
	LDY_I,LDA_IX,LDX_I,0,LDY_Z,LDA_Z,LDX_Z,0,TAY,LDA_I,TAX,0,LDY_A,LDA_A,LDX_A,0,
	BCS,LDA_IY,0,0,LDY_ZX,LDA_ZX,LDX_ZY,0,CLV,LDA_AY,TSX,0,LDY_AX,LDA_AX,LDX_AY,0,
	CPY_I,CMP_IX,0,0,CPY_Z,CMP_Z,DEC_Z,0,INY,CMP_I,DEX,0,CPY_A,CMP_A,DEC_A,0,
	BNE,CMP_IY,0,0,0,CMP_ZX,DEC_ZX,0,CLD,CMP_AY,0,0,0,CMP_AX,DEC_AX,0,
	CPX_I,SBC_IX,0,0,CPX_Z,SBC_Z,INC_Z,0,INX,SBC_I,NOP,0,CPX_A,SBC_A,INC_A,0,
	BEQ,SBC_IY,0,0,0,SBC_ZX,INC_ZX,0,SED,SBC_AY,0,0,0,SBC_AX,INC_AX,0
};


unsigned char nes_palette[64][3] =
{
	{0x80,0x80,0x80}, {0x00,0x00,0xBB}, {0x37,0x00,0xBF}, {0x84,0x00,0xA6},
	{0xBB,0x00,0x6A}, {0xB7,0x00,0x1E}, {0xB3,0x00,0x00}, {0x91,0x26,0x00},
	{0x7B,0x2B,0x00}, {0x00,0x3E,0x00}, {0x00,0x48,0x0D}, {0x00,0x3C,0x22},
	{0x00,0x2F,0x66}, {0x00,0x00,0x00}, {0x05,0x05,0x05}, {0x05,0x05,0x05},

	{0xC8,0xC8,0xC8}, {0x00,0x59,0xFF}, {0x44,0x3C,0xFF}, {0xB7,0x33,0xCC},
	{0xFF,0x33,0xAA}, {0xFF,0x37,0x5E}, {0xFF,0x37,0x1A}, {0xD5,0x4B,0x00},
	{0xC4,0x62,0x00}, {0x3C,0x7B,0x00}, {0x1E,0x84,0x15}, {0x00,0x95,0x66},
	{0x00,0x84,0xC4}, {0x11,0x11,0x11}, {0x09,0x09,0x09}, {0x09,0x09,0x09},

	{0xFF,0xFF,0xFF}, {0x00,0x95,0xFF}, {0x6F,0x84,0xFF}, {0xD5,0x6F,0xFF},
	{0xFF,0x77,0xCC}, {0xFF,0x6F,0x99}, {0xFF,0x7B,0x59}, {0xFF,0x91,0x5F},
	{0xFF,0xA2,0x33}, {0xA6,0xBF,0x00}, {0x51,0xD9,0x6A}, {0x4D,0xD5,0xAE},
	{0x00,0xD9,0xFF}, {0x66,0x66,0x66}, {0x0D,0x0D,0x0D}, {0x0D,0x0D,0x0D},

	{0xFF,0xFF,0xFF}, {0x84,0xBF,0xFF}, {0xBB,0xBB,0xFF}, {0xD0,0xBB,0xFF},
	{0xFF,0xBF,0xEA}, {0xFF,0xBF,0xCC}, {0xFF,0xC4,0xB7}, {0xFF,0xCC,0xAE},
	{0xFF,0xD9,0xA2}, {0xCC,0xE1,0x99}, {0xAE,0xEE,0xB7}, {0xAA,0xF7,0xEE},
	{0xB3,0xEE,0xFF}, {0xDD,0xDD,0xDD}, {0x11,0x11,0x11}, {0x11,0x11,0x11}
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
			LPSTR lpCmdLine, int nShowCmd)
{	
	MSG msg;
	WNDCLASSEX wc;
	HWND hwnd;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
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
	ClientWindow.bottom = 240;
	ClientWindow.left = 0;
	ClientWindow.right = 256;
	ClientWindow.top = 0;

	AdjustWindowRectEx(&ClientWindow,WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX), true,
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

	char run = 1;

	while(run)
	{
		if(GetMessage(&msg,NULL,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT) break;

		while(play)
		{
			if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if(!play) break;
			if(msg.message == WM_QUIT){ run = 0; break; }

			updateCPU();
			updatePPU();

			if(debug) CPUinfo();
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
			AppendMenu(submenu, MF_STRING, ID_DEBUG, "Debug Console");
			AppendMenu(submenu, MF_SEPARATOR, 0, 0);
			AppendMenu(submenu, MF_STRING | MF_GRAYED, 0, "Pause");
			AppendMenu(submenu, MF_STRING | MF_GRAYED, 0, "Step\tF11");


			AppendMenu(menu, MF_STRING | MF_POPUP, (UINT)submenu, "Debug");

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
					resetPPU();
					break;

				case ID_DEBUG:
					if(!debug)
					{
						debug = true;
						AllocConsole();
						ConOut = GetStdHandle(STD_OUTPUT_HANDLE);
						ConIn = GetStdHandle(STD_INPUT_HANDLE);
						SetConsoleTitle("Debug");
						SetConsoleMode(ConIn, 0);
						ConWrite("Project 8 Debug Output\n----------------------\n");

						submenu = CreatePopupMenu();
						DeleteMenu(menu, 1, MF_BYPOSITION);
						AppendMenu(submenu, MF_STRING | MF_CHECKED, ID_DEBUG, "Debug Console");
						AppendMenu(submenu, MF_SEPARATOR, 0, 0);
						AppendMenu(submenu, MF_STRING, ID_PAUSE, "Pause");
						AppendMenu(submenu, MF_STRING, ID_STEP, "Step\tF11");
						AppendMenu(menu, MF_STRING | MF_POPUP, (UINT) submenu, "Debug");
					}

					else
					{
						FreeConsole();
						debug = false;

						submenu = CreatePopupMenu();
						DeleteMenu(menu, 1, MF_BYPOSITION);
						AppendMenu(submenu, MF_STRING, ID_DEBUG, "Debug Console");
						AppendMenu(submenu, MF_SEPARATOR, 0, 0);
						AppendMenu(submenu, MF_STRING | MF_GRAYED, 0, "Pause");
						AppendMenu(submenu, MF_STRING | MF_GRAYED, 0, "Step\tF11");
						AppendMenu(menu, MF_STRING | MF_POPUP, (UINT) submenu, "Debug");
					}
					break;

				case ID_PAUSE:
					if(play)
					{
						play = 0;
						pause = true;
					}

					else if(pause)
					{
						pause = 0;
						play = 1;
					}

					break;

				case ID_STEP:
					pause = true;
					play = 0;

					updateCPU();
					updatePPU();
					CPUinfo();
					break;

				case ID_EXIT:
					PostQuitMessage(0);
					return 0;
			}
			break;

		case WM_SIZE:
			glViewport(0,0,lParam & 0xFFFF, lParam >> 16);;
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
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
				case VK_F11:
					if(debug && (play || pause))
						PostMessage(hwnd,WM_COMMAND,ID_STEP,0);
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


void ConWrite(char * string)
{
	unsigned long x;
	WriteConsole(ConOut,string,strlen(string),&x,NULL);
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

	glTranslatef(0.375,0.375,0);
}

void readError()
{
	MessageBox(NULL,"Could not succesfully copy ROM into memory",
	"Error!",MB_ICONEXCLAMATION);

	if(debug)
		ConWrite("Could not copy ROM (32k) into RAM");

	play = 0;
}

void DialogOpen(HWND hwnd)
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = "NES Roms (.nes)\0*.NES\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = (char*)malloc(256);
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = "NES";

	GetOpenFileName(&ofn);

	if(ofn.lpstrFile[0] == '\0') return;

	LoadFile(ofn.lpstrFile);
	free(ofn.lpstrFile);
}

void DropOpen(WPARAM hdrop)
{
	HDROP file = (HDROP)hdrop;

	int size = DragQueryFile(file,0,0,0);
	char * path = (char*)malloc(size+1);	//+1 for \0
	DragQueryFile(file,0,path,size+1);

	if(debug)
	{
		ConWrite("File dragged into window: ");
		ConWrite(path);
		ConWrite("\n");
	}

	LoadFile(path);
	free(path);
}

void LoadFile(char * path)
{	
	unsigned char PRGcount;

	resetPPU();
	resetCPU();

	FILE * ROM = fopen(path, "rb");
	if(!ROM)
	{		
		if(debug)
			ConWrite("Error openning file!\n");

		MessageBox(NULL,"File could not be openned!",
						"Error",MB_ICONEXCLAMATION);
		
		play = 0;

		return;
	}

	if(debug)
		ConWrite("File Successfully openned!\n");

	if(!(PRGcount=HeaderCheck(ROM)))
	{		
		MessageBox(NULL,"Not a valid NES ROM.","ERROR",MB_ICONEXCLAMATION);

		if(debug)
			ConWrite("Invalid ROM file.\n");

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
		if(debug)
			ConWrite("ROM not supported\n(Invalid number of PRG banks)\n");

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
	resetPPU();

	play = 1;

	fclose(ROM);
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
	
	if(debug)
	{
		ConWrite("NES magic number present\n");

		buffer[0] = buffer[4] + 48;
		buffer[1] = '\0';

		ConWrite("Number of 16k PRG-ROM pages: ");
		ConWrite(buffer);

		buffer[0] = buffer[5] + 48;
		ConWrite("\nNumber of 8k CHR-ROM pages: ");
		ConWrite(buffer);
		ConWrite("\n");
	}

	unsigned char prgcount = buffer[4];
	free(buffer);

	return prgcount;
}

void DrawTile(unsigned short tile,int x, int y, unsigned char isSprite)
{
	if(!tile) return;

	unsigned char tilearray[8][8];
	ZeroMemory(tilearray, 64);	//array starts as 0xCC

	unsigned char * palette = VRAM + 0x3F00;

	int start = x;
	int linestart = y;

	unsigned char * tileptr = VRAM + (tile * 16);

	if(!isSprite)
	{
		if(RAM[0x2000] & 16) //background pattern data at v0x1000
			tileptr += 0x1000;
	}
	
	if(isSprite)
	{
		if(RAM[0x2000] & 8) //sprite pattern data at v0x1000
			tileptr += 0x1000;
		palette += 0x10;
	}

	int tx = 0;
	int ty = 0;

	for(int i = 0; i < 16; i++)
	{
		unsigned char curline = tileptr[i];
		for(int j = 0; j < 8; j++)
		{
			if((curline << j) & 128)
			{
				if(i < 8) tilearray[ty][tx]++;
				else tilearray[ty][tx] += 2;
			}

			tx++;
		}
		tx = 0;
		ty++;

		if(i == 7)
			ty = 0;
	}

	unsigned char bits;

	if(!isSprite) //Atrribute tables//
	{
		//attr tables can leaves the bounds of the screen?

		unsigned char attribute;
		unsigned char * attrtable = VRAM + (0x400 * (RAM[2000] & 3)) + 0x23C0;

		attribute = attrtable[((y/4) * 8) + (x/4)];	//attr bytes are 4x4 tiles

		unsigned char tilex = 0;
		unsigned char tiley = 0;

		if((x % 4) > 1)
			tilex = 1;
		if((y % 4) > 1)
			tiley = 1;

		if(!tilex && !tiley) bits = attribute & 0x03;
		else if(tilex && !tiley) bits = (attribute >> 2) & 0x03;
		else if(!tilex && tiley) bits = (attribute >> 4) & 0x03;
		else if(tilex && tiley) bits = (attribute >> 6) & 0x03;


		bits = 0; ////////////////////////FIX ME PLOX////////////////////
	}

	if(isSprite) //Sprite Color Data//
	{
		unsigned char * sprdata = SPRRAM + ((isSprite - 1) * 4);
		bits = sprdata[2] & 0x3;
	}

	bits <<= 2;

	for(int i = 0; i < 8; i++)
		for(int j = 0; j < 8; j++)
			tilearray[i][j] |= bits;

	if(isSprite)
	{
		unsigned char * sprdata = SPRRAM + ((isSprite - 1) * 4);
		if(sprdata[2] & 64) //Horizontal Flip//
		{
			unsigned char temparray[8][8];
			memcpy(temparray,tilearray,64);
			
			for(int i = 0; i < 8; i++)
			{
				for(int j = 0; j < 8; j++)
				{
					tilearray[i][j] = temparray[i][7-j];
				}
			}
		}

		if(sprdata[2] & 128) //Vertical Flip//
		{
			unsigned char temparray[8][8];
			memcpy(temparray,tilearray,64);
			
			for(int i = 0; i < 8; i++)
			{
				for(int j = 0; j < 8; j++)
				{
					tilearray[i][j] = temparray[7-i][j];
				}
			}
		}

	}

	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if(tilearray[i][j])
			{
				unsigned char color = palette[tilearray[i][j]];

				glColor3f(
					nes_palette[color][0]/256.0,
					nes_palette[color][1]/256.0,
					nes_palette[color][2]/256.0);

				glVertex2f(x,y);
			}
			x++;
		}
		x = start;
		y++;
	}
}

void DrawSprites()
{
	for(int i = 63; i >= 0; i--)
	{
		unsigned char * sprite = SPRRAM + (i * 4);

		DrawTile(sprite[1],sprite[3],sprite[0],i+1);
	}
}

void DrawFrame(unsigned char xscr, unsigned char yscr)
{
	if(RAM[0x2001] & 8) //Draw Screen//
	{
		unsigned char * palette = VRAM + 0x3F00;
		unsigned char color = palette[0];

		glClearColor(
			nes_palette[color][0]/256.0,	//r
			nes_palette[color][1]/256.0,	//g
			nes_palette[color][2]/256.0,	//b
			1);
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_POINTS);
		
		int x = 0;
		int y = 0;

		unsigned short tableaddr = 0x2000 + (0x400 * (RAM[2000] & 3));
		if(tableaddr >= 0x2800) tableaddr -= 0x800;

		unsigned char * nametable = VRAM + tableaddr;

		for(int i = 0; i < 960; i++)
		{
			//scrolling1?!@??@!?@?!!?!@?
			if(y == yscr)
				DrawTile(nametable[i],x+xscr,y,0);

			else
				DrawTile(nametable[i],x,y,0);

			x += 8;
			if(x > 248)
			{
				y += 8;
				x = 0;
			}
		}

		if(RAM[0x2001] & 16) //Draw 
			DrawSprites();

		glEnd();
		SwapBuffers(hDC);
	}

	else //Don't draw screen//
	{
		glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT);
		SwapBuffers(hDC);
	}
}

void CreateTile(unsigned char * screen, unsigned char tile, int x, int y, char sprite)
{
	int start = x;
	int linestart = y;

	unsigned char * tileptr = VRAM + (tile * 16);

	unsigned char tilearray[8][8];
	ZeroMemory(tilearray, 64);

	if(!sprite)
	{
		if(RAM[0x2000] & 16) //data at v0x1000
				tileptr += 0x1000;
	}	//needs braces for visual studio to recognize end of "if"

	else if(sprite)
		if(RAM[0x2000] & 8)
			tileptr += 0x1000;
	
	x = 0;
	y = 0;

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
						tilearray[y][x] = 16;

					tilearray[y][x] |= 1;
				}

				else
				{
					if(!(tilearray[y][x] & 16) && sprite)
						tilearray[y][x] = 16;

					tilearray[y][x] |= 2;
				}
			}

			x++;
		}
		x = 0;
		y++;

		if(j == 7)
			y = 0;
	}

	if(sprite)
	{
		unsigned char * sprdata = SPRRAM + ((sprite - 1) * 4);

		for(y = 0; y < 8; y++)
			for(x = 0; x < 8; x++)
				if(tilearray[y][x])
					tilearray[y][x] |= (sprdata[2] & 3) << 2;
	
		if(sprdata[2] & 64) //Horizontal Flip//
		{
			unsigned char temparray[8][8];
			memcpy(temparray,tilearray,64);
			
			for(int i = 0; i < 8; i++)
			{
				for(int j = 0; j < 8; j++)
				{
					tilearray[i][j] = temparray[i][7-j];
				}
			}
		}

		if(sprdata[2] & 128) //Vertical Flip//
		{
			unsigned char temparray[8][8];
			memcpy(temparray,tilearray,64);
			
			for(int i = 0; i < 8; i++)
			{
				for(int j = 0; j < 8; j++)
				{
					tilearray[i][j] = temparray[7-i][j];
				}
			}
		}
	}

	//////Sporked from old drawing functions

	//if(!isSprite) //Atrribute tables//
	//{
	//	//attr tables can leaves the bounds of the screen?

	//	unsigned char attribute;
	//	unsigned char * attrtable = VRAM + (0x400 * (RAM[2000] & 3)) + 0x23C0;

	//	attribute = attrtable[((y/4) * 8) + (x/4)];	//attr bytes are 4x4 tiles

	//	unsigned char tilex = 0;
	//	unsigned char tiley = 0;

	//	if((x % 4) > 1)
	//		tilex = 1;
	//	if((y % 4) > 1)
	//		tiley = 1;

	//	if(!tilex && !tiley) bits = attribute & 0x03;
	//	else if(tilex && !tiley) bits = (attribute >> 2) & 0x03;
	//	else if(!tilex && tiley) bits = (attribute >> 4) & 0x03;
	//	else if(tilex && tiley) bits = (attribute >> 6) & 0x03;


	//	bits = 0; ////////////////////////FIX ME PLOX////////////////////
	//}


	x = 0;
	y = 0;

	hit = -1;

	for(int i = linestart; i < linestart + 8; i++)
	{
		for(int j = start; j < start + 8; j++)
		{
			if(tilearray[y][x])
			{
				if(!(sprite-1) && screen[(i * 256) + j] && !(hit + 1))
				{
					hit = i;
				}

				screen[(i * 256) + j] = tilearray[y][x];
			}
			x++;
		}
		x = 0;
		y++;
	}
}

void CreateBackground(unsigned char * screen)
{
	int x = 0, y = 0;
	
	unsigned char * nametable = VRAM + 0x2000 + (0x400 * (RAM[0x2000] & 3));

	for(int i = 0; i < 960; i++)
	{
		CreateTile(screen, nametable[i], x, y, 0);

		x += 8;

		if(x > 248)
		{
			x = 0;
			y += 8;
		}
	}
}

void CreateSprites(unsigned char * screen)
{
	for(int i = 63; i >= 0; i--)
	{
		unsigned char * sprite = SPRRAM + (i * 4);

		CreateTile(screen, sprite[1], sprite[3], sprite[0] + 1, i + 1);
	}
}

void DrawScreen(unsigned char * screen, int scanline)
{
	unsigned char * palette = VRAM + 0x3F00;

	if(scanline == 1)
	{
		unsigned char clearcol = palette[0];

		glClearColor(
			nes_palette[clearcol][0]/256.0,
			nes_palette[clearcol][1]/256.0,
			nes_palette[clearcol][2]/256.0, 1);

		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_POINTS);
	}

	if(scanline == hit)
		RAM[0x2002] |= 64;

	for(int x = 0; x < 256; x++)
	{
		unsigned short addr = ((scanline - 1) * 256) + x;
		if(screen[addr])
		{
			unsigned char color = palette[screen[addr]];

			glColor3f(
				nes_palette[color][0]/256.0,
				nes_palette[color][1]/256.0,
				nes_palette[color][2]/256.0);

			glVertex2f(x,scanline - 1);
		}
	}

	if(scanline == 240)
	{
		glEnd();
		SwapBuffers(hDC);
	}
}

void resetCPU()
{
	A = 0;
	X = 0;
	Y = 0;

	for(int i = 0; i < 0x8000; i++)
		RAM[i] = 0;

	PC = 0x8000;

	NMI = RAM[0xFFFA] | (RAM[0xFFFB] << 8);

	S = 0xFF;
	P = 32;	//bit 5 always set
}

void resetPPU()
{
	ticks = clock();
	IN_INT = 0;

	for(int i = 0x2000; i < 0x4000; i++) VRAM[i] = 0;
	
	ZeroMemory(SPRRAM,0xFF);

	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	SwapBuffers(hDC);

	toggle = false;
}

void updateCPU()
{
	static bool strobe = false;
	static unsigned int instructions = 0;

	if(opcodes[RAM[PC]]) opcodes[RAM[PC]]();
	else PC++;

	instructions++;

	if(instructions == 900)
	{
		Sleep(1);	//Waste time to avoid CPU overwork
		instructions = 0;
	}

	if(JOYREAD)
	{
		currbit++;
		JOYREAD = 0;
	}

	if(JOYWRITE)
	{
		if(strobe)
		{
			if(RAM[0x4016] == 0)
			{
				currbit = 0;
				strobe = false;
			}
		}

		else if(RAM[0x4016] == 1)
			strobe = true;

		else
			strobe = false;

		JOYWRITE = 0;
	}

	if(currbit < 8)
		RAM[0x4016] = 0x40 | ((JOYPAD >> currbit) & 1);
	else 
		RAM[0x4016] = 0x41;
}

void updatePPU()
{
	static unsigned short ppuaddr;
	static unsigned short temp;
	static unsigned char spraddr;
	static unsigned char xscroll = 0, yscroll = 0;

	/*Instructions Per Second*/

	//static unsigned int instr = 0;
	//static int time = clock();
	//instr++;
	//int timetemp = clock();
	//if (timetemp - time >= CLOCKS_PER_SEC)
	//{
	//	char buf[25];
	//	itoa(instr, buf, 10);
	//	MessageBox(0,buf,"IPS",0);
	//	time = clock();
	//	instr = 0;
	//}

	if(RAM[0x4014])	//SPR-RAM DMA
	{
		unsigned char multiplier = RAM[0x4014];
		unsigned short RAMaddr = multiplier * 0x100;
		for(int i = 0; i < 256; i++)
		{
			SPRRAM[i] = RAM[RAMaddr + i];
		}
		RAM[0x4014] = 0;
	}

	/********************************************/
	//General PPU address//
	
	else if(W2006)
	{
		if(!toggle)
		{
			toggle = true;
			temp &= 0xFF;
			temp = (RAM[0x2006] & 0x3F) << 8;
		}
		else
		{
			toggle = false;
			temp &= 0xFF00;
			temp |= RAM[0x2006];
			ppuaddr = temp;
			BADREAD = 0;
		}

		if(ppuaddr >= 0x3000 && ppuaddr < 0x3F00)
			ppuaddr -= 0x1000;

		if(ppuaddr >= 0x3F20)
		{
			char div = (ppuaddr - 0x3F00) / 0x20;
			ppuaddr -= div * 0x20;
		}

		if(ppuaddr >= 0x3F00)
			PALETTE = 1;
		else PALETTE = 0;

		if(ppuaddr > 0x3F10 && !(ppuaddr % 4))
			ppuaddr -= 0x10;

		//if(ppuaddr < 0x3F20 && ppuaddr > 0x3F00  && !(ppuaddr % 4))
		//	ppuaddr = 0x3FFF;

		W2006 = 0;
	}

	else if(W2000)
	{
		temp &= 0xF3FF;
		temp |= (RAM[0x2000] & 3) << 10; //Name table bit

		W2000 = 0;
	}
	
	/********************************************/
	//Scrolling offsets//

	else if(W2005)
	{
		if(!toggle)
		{
			temp &= 0xFFE0;
			temp |= RAM[0x2005] >> 3;
			xscroll = RAM[0x2005];
			toggle = 1;
		}

		else
		{
			temp &= 0xC1F;
			temp |= (RAM[0x2005] & 0xF8) << 2;
			temp |= RAM[0x2005] << 12;
			yscroll = RAM[0x2005];
			toggle = 0;
		}

		W2005 = 0;
	}

	/********************************************/

	else if(PPUREAD)
	{
		ppuaddr++;

		if(RAM[0x2000] & 4)
			ppuaddr += 31;

		PPUREAD = 0;
		
		if(ppuaddr >= 0x3000 && ppuaddr < 0x3F00)
			ppuaddr -= 0x1000;

		if(ppuaddr >= 0x3F20)
		{
			char div = (ppuaddr - 0x3F00) / 0x20;
			ppuaddr -= div * 0x20;
		}

		if(ppuaddr >= 0x3F00)
			PALETTE = 1;
	}

	/********************************************/

	else if(PPUWRITE)
	{
		if(ppuaddr > 0x3F00 && !(ppuaddr % 0x10))
			VRAM[0x3F00] = RAM[0x2007];

		else VRAM[ppuaddr] = RAM[0x2007];

		ppuaddr++;

		if(RAM[0x2000] & 4)
			ppuaddr += 31;

		PPUWRITE = 0;

		if(ppuaddr >= 0x3000 && ppuaddr < 0x3F00)
			ppuaddr -= 0x1000;

		if(ppuaddr >= 0x3F20)
		{
			char div = (ppuaddr - 0x3F00) / 0x20;
			ppuaddr -= div * 0x20;
		}

		if(ppuaddr >= 0x3F00)
			PALETTE = 1;
	}
	
	if(ppuaddr >= 0x3F00) RAM[0x2007] = VRAM[ppuaddr];
	else RAM[0x2007] = VRAM[ppuaddr-1];

	/*********************************************/

	if(W2003)
	{
		spraddr = RAM[0x2003];
		W2003 = 0;
	}

	else if(SPRREAD)
	{
		spraddr++;
		SPRREAD = 0;
	}

	else if(SPRWRITE)
	{
		SPRRAM[spraddr] = RAM[0x2004];
		spraddr++;
		SPRWRITE = 0;
	}

	RAM[0x2004] = SPRRAM[spraddr];

	/********************************************/

	int temptime = clock();

	static bool draw = false;
	if((temptime-ticks) >= (float)(CLOCKS_PER_SEC/60))
	{
		draw = true;
		ticks = clock();
	}

	static int scanline = 0;
	static int wastetime = 0;

	static unsigned char screen[0xF000];

	if(draw)
	{		
		if(!wastetime)
		{
			if(!scanline)
			{
				RAM[0x2002] &= 63;
				wastetime = 25;	//H-BLANK
				ZeroMemory(screen, 0xF000);

				if(RAM[0x2001] & 8)
				{
					CreateBackground(screen);
					
					if(RAM[0x2001] & 16)
						CreateSprites(screen);
				}
			}
			
			else if(scanline)
			{
				DrawScreen(screen, scanline);
				wastetime = 25;
				//if(scanline == 1) DrawFrame(xscroll, yscroll);

				if(scanline == 240)
				{
					RAM[0x2002] |= 128;
					draw = false;
					scanline = -1;

					if(!IN_INT && (RAM[0x2000] & 128))	//NMI
					{		
						pushw(PC);
						pushb(P);
						PC = NMI;

						IN_INT = 1;
					}
				}
			}
			scanline++;
		}

		else wastetime--;
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
	unsigned char lo = RAM[PC];
	PC++;
	unsigned char hi = RAM[PC];
	PC++;

	return (hi << 8) | lo;
}

unsigned char fetchbyte()
{
	PC++;
	unsigned char byte = RAM[PC];
	PC++;

	return byte;
}

void ADC(unsigned char byte)
{
	unsigned char temp = A;
	A += byte + (P & 1);

	if(temp > A) P |= 1;
	else P &= 254;

	if(((temp&128)&&(byte&128)&&(~A&128))||
		((~temp&128)&&(~byte&128)&&(A&128)))
		P |= 64;
	else P &= 0xBF;
}

void SBC(unsigned char byte)
{
	unsigned char temp = A;
	A = A - byte - (~P&1);

	if(A < temp) P |= 1;
	else P &= 254;

	if(((temp&128)&&(~byte&128)&&(~A&128))||
		((~temp&128)&&(byte&128)&&(A&128)))
		P |= 64;
	else P &= 0xBF;
}

void CMP(unsigned char byte)
{
	unsigned char result = A - byte;

	if(A >= byte) P |= 1;
	else P &= 254;

	N_FLAGS(result);
	Z_FLAGS(result);
}

void BRK()
{
	PC++;

	P |= 16;
	
	pushw(PC+1);
	pushb(P);

	PC = (RAM[0xFFFF] << 8) | RAM[0xFFFE];
}

void ORA_IX()
{
	unsigned char zp = fetchbyte() + X;

	A |= RAM[(RAM[zp+1] << 8) | RAM[zp]];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ORA_Z()
{
	A |= RAM[fetchbyte()];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ASL_Z()
{
	unsigned char addr = fetchbyte();

	P &= 254;
	P |= RAM[addr] >> 7;

	RAM[addr] <<= 1;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void PHP()
{
	PC++;
	
	P |= 16;	//Set BRK bit. It's a bug.
	pushb(P);
}

void ORA_I()
{
	A |= fetchbyte();

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ASL_AC()
{
	PC++;

	P &= 254;
	P |= A >> 7;

	A <<= 1;

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ORA_A()
{
	A |= RAM[fetchword()];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ASL_A()
{
	unsigned short addr = fetchword();

	P &= 254;
	P |= RAM[addr] >> 7;

	RAM[addr] <<= 1;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void BPL()
{
	char byte = (char)fetchbyte();

	if (!(P & 128)) PC += byte;
}

void ORA_IY()
{
	unsigned char zp = fetchbyte();

	A |= RAM[((RAM[zp+1] << 8) | RAM[zp]) + Y];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ORA_ZX()
{
	A |= RAM[(fetchbyte() + X) & 0xFF];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ASL_ZX()
{
	unsigned char addr = fetchbyte() + X;

	P &= 254;
	P |= RAM[addr] >> 7;

	RAM[addr] <<= 1;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void CLC()
{
	PC++;

	P &= 254;
}

void ORA_AY()
{
	A |= RAM[(fetchword() + Y) & 0xFFFF];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ORA_AX()
{
	A |= RAM[(fetchword() + X) & 0xFFFF];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ASL_AX()
{
	unsigned short addr = fetchword() + X;

	P &= 254;
	P |= RAM[addr] >> 7;

	RAM[addr] <<= 1;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void JSR()
{
	unsigned short address = fetchword();
	if(address < 0x8000) return;

	pushw(PC - 1);

	PC = address;
}

void AND_IX()
{
	unsigned char zp = fetchbyte() + X;

	A &= RAM[(RAM[zp] << 8) | RAM[zp]];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void BIT_Z()
{
	unsigned char byte = RAM[fetchbyte()];
	unsigned char result = A & byte;

	P &= 63;
	P |= (byte & 192);

	Z_FLAGS(result);
}

void AND_Z()
{
	A &= RAM[fetchbyte()];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ROL_Z()
{
	unsigned short addr = fetchbyte();
	unsigned char temp = RAM[addr];

	RAM[addr] <<= 1;
	RAM[addr] |= P & 1;

	P &= 254;
	P |= temp >> 7;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void PLP()
{
	PC++;
	P = popb();
	P |= 32; //bit 5 is ALWAYS set
}

void AND_I()
{
	A &= fetchbyte();

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ROL_AC()
{
	PC++;

	unsigned char temp = A;

	A <<= 1;
	A |= P & 1;

	P &= 254;
	P |= temp >> 7;

	N_FLAGS(A);
	Z_FLAGS(A);
}

void BIT_A()
{
	unsigned char byte = RAM[fetchword()];
	unsigned char result = byte & A;

	P &= 63;
	P |= byte & 192;

	Z_FLAGS(result);
}

void AND_A()
{
	A &= RAM[fetchword()];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ROL_A()
{
	unsigned short addr = fetchword();
	unsigned char temp = RAM[addr];

	RAM[addr] <<= 1;
	RAM[addr] |= P & 1;

	P &= 254;
	P |= temp >> 7;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void BMI()
{
	char byte = (char)fetchbyte();

	if(P & 128) PC += byte;
}

void AND_IY()
{
	unsigned char zp = fetchbyte();

	A &= RAM[((RAM[zp+1] << 8) | RAM[zp]) + Y];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void AND_ZX()
{
	A &= RAM[(fetchbyte() + X) & 0xFF];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ROL_ZX()
{
	unsigned char addr = fetchbyte() + X;
	unsigned char temp = RAM[addr];

	RAM[addr] <<= 1;
	RAM[addr] |= P & 1;

	P &= 254;
	P |= temp >> 7;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void SEC()
{
	PC++;
	P |= 1;
}

void AND_AY()
{
	A &= RAM[(fetchword() + Y) & 0xFFFF];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void AND_AX()
{
	A &= RAM[(fetchword() + X) & 0xFFFF];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ROL_AX()
{
	unsigned short addr = fetchword() + X;
	unsigned char temp = RAM[addr];

	RAM[addr] <<= 1;
	RAM[addr] |= P & 1;

	P &= 254;
	P |= (temp >> 7);

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void RTI()
{
	IN_INT = 0;

	P = popb();
	PC = popw();

	P &= 0xEF; //break flag
}

void EOR_IX()
{
	unsigned char zp = fetchbyte() + X;

	A ^= RAM[(RAM[zp+1] << 8) | RAM[zp]];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void EOR_Z()
{
	A ^= RAM[fetchbyte()];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void LSR_Z()
{
	unsigned char addr = fetchbyte();

	P &= 254;
	P |= RAM[addr] & 1;

	RAM[addr] >>= 1;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void PHA()
{
	PC++;
	pushb(A);
}

void EOR_I()
{
	A ^= fetchbyte();

	N_FLAGS(A);
	Z_FLAGS(A);
}

void LSR_AC()
{
	PC++;
	
	P &= 254;
	P |= A & 1;

	A >>= 1;

	N_FLAGS(A);
	Z_FLAGS(A);
}

void JMP_A()
{
	PC = fetchword();
}

void EOR_A()
{
	A ^= RAM[fetchword()];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void LSR_A()
{
	unsigned short addr = fetchword();

	P &= 254;
	P |= RAM[addr] & 1;

	RAM[addr] >>= 1;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void BVC()
{
	char byte = (char)fetchbyte();

	if(!(P & 64))
		PC += byte;
}

void EOR_IY()
{
	unsigned char zp = fetchbyte();

	A ^= RAM[((RAM[zp+1] << 8) | RAM[zp]) + Y];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void EOR_ZX()
{
	A ^= RAM[(fetchbyte() + X) & 0xFF];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void LSR_ZX()
{
	unsigned char addr = fetchbyte() + X;

	P &= 254;
	P |= RAM[addr] & 1;

	RAM[addr] >>= 1;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void CLI()
{
	PC++;

	P &= 251;
}

void EOR_AY()
{
	A ^= RAM[(fetchword() + Y) & 0xFFFF];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void EOR_AX()
{
	A ^= RAM[(fetchword() + X) & 0xFFFF];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void LSR_AX()
{
	unsigned short addr = fetchword() + X;

	P &= 254;
	P |= RAM[addr] & 1;

	RAM[addr] >>= 1;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void RTS()
{
	PC = popw();
	PC++;
}

void ADC_IX()
{
	unsigned char zp = fetchbyte() + X;

	ADC(RAM[(RAM[zp+1] << 8) | RAM[zp]]);

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ADC_Z()
{
	ADC(RAM[fetchbyte()]);

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ROR_Z()
{
	unsigned char addr = fetchbyte();
	unsigned char temp = RAM[addr];

	RAM[addr] >>= 1;

	RAM[addr]|= P << 7;

	P &= 254;
	P |= temp & 1;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void PLA()
{
	PC++;
	A = popb();

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ADC_I()
{
	ADC(fetchbyte());

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ROR_AC()
{
	PC++;

	unsigned char temp = A;
	A >>= 1;
	A |= P << 7;

	P &= 254;
	P |= temp & 1;

	N_FLAGS(A);
	Z_FLAGS(A);
}

void JMP_IN()
{
	unsigned short addr = fetchword();

	PC = (RAM[((addr + 1) & 0xFF) | (addr & 0xFF00)] << 8) | RAM[addr];
}

void ADC_A()
{
	ADC(RAM[fetchword()]);

	N_FLAGS(A);
	Z_FLAGS(A);
}	

void ROR_A()
{
	unsigned short int addr = fetchword();
	unsigned char temp = RAM[addr];

	RAM[addr] >>= 1;
	RAM[addr] |= P << 7;

	P &= 254;
	P |= (temp & 1);

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void BVS()
{
	char byte = (char)fetchbyte();

	if(P & 64)
		PC += byte;
}

void ADC_IY()
{
	unsigned char zp = fetchbyte();

	ADC(((RAM[zp+1] << 8) | RAM[zp]) + Y);

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ADC_ZX()
{
	ADC(RAM[(fetchbyte() + X) & 0xFF]);

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ROR_ZX()
{
	unsigned char addr = fetchbyte() + X;
	unsigned char temp = RAM[addr];

	RAM[addr] >>= 1;

	RAM[addr] |= P << 7;

	P &= 254;
	P |= temp & 1;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void SEI()
{
	PC++;
	
	P |= 4;
}

void ADC_AY()
{
	ADC(RAM[(fetchword() + Y) & 0xFFFF]);

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ADC_AX()
{
	ADC(RAM[(fetchword() + X) & 0xFFFF]);

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ROR_AX()
{
	unsigned short addr = fetchword() + X;
	unsigned char temp = RAM[addr];

	RAM[addr] >>= 1;
	RAM[addr] |= P << 7;

	P &= 254;
	P |= temp & 1;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void STA_IX()
{
	unsigned char zp = fetchbyte() + X;

	RAM[(RAM[zp+1] << 8) | RAM[zp]] = A;
}

void STY_Z()
{
	RAM[fetchbyte()] = Y;
}

void STA_Z()
{
	RAM[fetchbyte()] = A;
}

void STX_Z()
{
	RAM[fetchbyte()] = X;
}

void DEY()
{
	PC++;
	Y--;

	Z_FLAGS(Y);
	N_FLAGS(Y);
}

void TXA()
{
	PC++;
	
	A = X;
	N_FLAGS(A);
	Z_FLAGS(A);
}

void STY_A()
{
	unsigned short addr = fetchword();
	
	if(addr >= 0x800 && addr < 0x2000)
	{
		char div = addr / 0x800;
		addr -= (0x800 * div);
	}

	RAM[addr] = Y;

	if (addr == 0x2000) W2000 = 1;
	else if(addr == 0x2003) W2003 = 1;
	else if(addr == 0x2004) SPRWRITE = 1;
	else if(addr == 0x2005) W2005 = 1;
	else if(addr == 0x2006) W2006 = 1;
	else if(addr == 0x2007) PPUWRITE = 1;
	else if(addr == 0x4016) JOYWRITE = 1;
}

void STA_A()
{
	unsigned short addr = fetchword();

	if(addr >= 0x800 && addr < 0x2000)
	{
		char div = addr / 0x800;
		addr -= (0x800 * div);
	}

	RAM[addr] = A;

	if (addr == 0x2000) W2000 = 1;
	else if(addr == 0x2003) W2003 = 1;
	else if(addr == 0x2004) SPRWRITE = 1;
	else if(addr == 0x2005) W2005 = 1;
	else if(addr == 0x2006) W2006 = 1;
	else if(addr == 0x2007) PPUWRITE = 1;
	else if(addr == 0x4016) JOYWRITE = 1;
}

void STX_A()
{
	unsigned short addr = fetchword();

	if(addr >= 0x800 && addr < 0x2000)
	{
		char div = addr / 0x800;
		addr -= (0x800 * div);
	}

	RAM[addr] = X;

	if (addr == 0x2000) W2000 = 1;
	else if(addr == 0x2003) W2003 = 1;
	else if(addr == 0x2004) SPRWRITE = 1;
	else if(addr == 0x2005) W2005 = 1;
	else if(addr == 0x2006) W2006 = 1;
	else if(addr == 0x2007) PPUWRITE = 1;
	else if(addr == 0x4016) JOYWRITE = 1;
}

void BCC()
{
	char change  = (char)fetchbyte();

	if(!(P & 1))
		PC += change;
}

void STA_IY()
{
	unsigned char zp = fetchbyte();

	RAM[((RAM[zp+1] << 8) | RAM[zp]) + Y] = A;
}

void STY_ZX()
{
	RAM[(fetchbyte() + X) & 0xFF] = Y;
}

void STA_ZX()
{
	RAM[(fetchbyte() + X) & 0xFF] = A;
}

void STX_ZY()
{
	RAM[(fetchbyte() + Y) & 0xFF] = X;
}

void TYA()
{
	PC++;

	A = Y;
	N_FLAGS(A);
	Z_FLAGS(A);
}

void STA_AY()
{
	 RAM[(fetchword() + Y) & 0xFFFF] = A;
}

void TXS()
{
	PC++;

	S = X;
}

void STA_AX()
{
	RAM[(fetchword() + X) & 0xFFFF] = A;
}

void LDY_I()
{
	Y = fetchbyte();

	Z_FLAGS(Y);
	N_FLAGS(Y);
}

void LDA_IX()
{
	unsigned char zp = fetchbyte() + X;

	A = RAM[(RAM[zp+1] << 8) | RAM[zp]];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void LDX_I()
{
	X = fetchbyte();

	Z_FLAGS(X);
	N_FLAGS(X);
}

void LDY_Z()
{
	Y = RAM[fetchbyte()];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void LDA_Z()
{
	A = RAM[fetchbyte()];

	Z_FLAGS(A);
	N_FLAGS(A);
}

void LDX_Z()
{
	X = RAM[fetchbyte()];

	Z_FLAGS(X);
	N_FLAGS(X);
}

void TAY()
{
	PC++;

	Y = A;
	N_FLAGS(Y);
	Z_FLAGS(Y);
}

void LDA_I()
{
	A = fetchbyte();

	Z_FLAGS(A);
	N_FLAGS(A);
}

void TAX()
{
	PC++;

	X = A;
	N_FLAGS(X);
	Z_FLAGS(X);
}

void LDY_A()
{
	unsigned short addr = fetchword();

	if(addr >= 0x800 && addr < 0x2000)
	{
		char div = addr / 0x800;
		addr -= (0x800 * div);
	}

	if(addr == 0x2007 && !BADREAD && !PALETTE)
	{
		PPUREAD = 1;
		BADREAD = 1;
		N_FLAGS(Y);
		Z_FLAGS(Y);
		return;
	}

	Y = RAM[addr];

	if(addr == 0x2002)
	{
		RAM[0x2002] &= 127;
		toggle = false;
	}
	if(addr == 0x2004) SPRREAD = 1;
	if(addr == 0x2007) PPUREAD = 1;
	if(addr == 0x4016) JOYREAD = 1;

	N_FLAGS(Y);
	Z_FLAGS(Y);
}

void LDA_A()
{
	unsigned short addr = fetchword();

	if(addr >= 0x800 && addr < 0x2000)
	{
		char div = addr / 0x800;
		addr -= (0x800 * div);
	}

	if(addr == 0x2007 && !BADREAD && !PALETTE)
	{
		PPUREAD = 1;
		BADREAD = 1;
		N_FLAGS(A);
		Z_FLAGS(A);
		return;
	}

	A = RAM[addr];

	if(addr == 0x2002)
	{
		RAM[addr] &= 127;
		toggle = false;
	}

	else if(addr == 0x2004) SPRREAD = 1;
	else if(addr == 0x2007) PPUREAD = 1;
	else if(addr == 0x4016) JOYREAD = 1;

	N_FLAGS(A);
	Z_FLAGS(A);
}

void LDX_A()
{
	unsigned short addr = fetchword();

	if(addr >= 0x800 && addr < 0x2000)
	{
		char div = addr / 0x800;
		addr -= (0x800 * div);
	}

	if(addr == 0x2007 && !BADREAD && !PALETTE)
	{
		PPUREAD = 1;
		BADREAD = 1;
		N_FLAGS(X);
		Z_FLAGS(X);
		return;
	}

	X = RAM[addr];

	if(addr == 0x2002)
	{
		RAM[0x2002] &= 127;
		toggle = false;
	}

	if(addr == 0x2004) SPRREAD = 1;
	if(addr == 0x2007) PPUREAD = 1;
	if(addr == 0x4016) JOYREAD = 1;

	N_FLAGS(X);
	Z_FLAGS(X);
}

void BCS()
{
	char byte = (char)fetchbyte();

	if(P & 1)
		PC += byte;
}

void LDA_IY()
{
	unsigned char zp = fetchbyte();

	A = RAM[((RAM[zp+1] << 8) | RAM[zp]) + Y];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void LDY_ZX()
{
	Y = RAM[(fetchbyte() + X) & 0xFF];

	N_FLAGS(Y);
	Z_FLAGS(Y);
}

void LDA_ZX()
{
	A = RAM[(fetchbyte() + X) & 0xFF];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void LDX_ZY()
{
	X = RAM[(fetchbyte() + Y) & 0xFF];

	N_FLAGS(X);
	Z_FLAGS(X);
}

void CLV()
{
	PC++;
	P &= 191;
}

void LDA_AY()
{
	A = RAM[(fetchword() + Y) & 0xFFFF];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void TSX()
{
	PC++;

	X = S;

	N_FLAGS(X);
	Z_FLAGS(X);
}

void LDY_AX()
{
	Y = RAM[(fetchword() + X) & 0xFFFF];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void LDA_AX()
{
	A = RAM[(fetchword() + X) & 0xFFFF];

	Z_FLAGS(A);
	N_FLAGS(A);
}

void LDX_AY()
{
	X = RAM[(fetchword() + Y) & 0xFFFF];

	N_FLAGS(X);
	Z_FLAGS(X);
}

void CPY_I()
{
	unsigned char byte = fetchbyte();

	unsigned char result = Y - byte;

	if(Y >= byte) P |= 1;
	else P &= 254;

	N_FLAGS(result);
	Z_FLAGS(result);
}

void CMP_IX()
{
	unsigned char zp = fetchbyte() + X;

	CMP(RAM[(RAM[zp+1] << 8) | RAM[zp]]);
}

void CPY_Z()
{
	unsigned char byte = RAM[fetchbyte()];

	unsigned char result = Y - byte;

	if(Y >= byte) P |= 1;
	else P &= 254;

	N_FLAGS(result);
	Z_FLAGS(result);
}

void CMP_Z()
{
	CMP(RAM[fetchbyte()]);
}

void DEC_Z()
{
	unsigned char addr = fetchbyte();

	RAM[addr]--;

	Z_FLAGS(RAM[addr]);
	N_FLAGS(RAM[addr]);
}

void INY()
{
	PC++;

	Y++;

	N_FLAGS(Y);
	Z_FLAGS(Y);
}

void CMP_I()
{
	unsigned char byte = fetchbyte();

	CMP(byte);
}

void DEX()
{
	PC++;

	X--;

	N_FLAGS(X);
	Z_FLAGS(X);
}

void CPY_A()
{
	unsigned char byte = RAM[fetchword()];

	unsigned char result = Y - byte;

	if(Y >= byte) P |= 1;
	else P &= 254;

	N_FLAGS(result);
	Z_FLAGS(result);
}

void CMP_A()
{
	CMP(RAM[fetchword()]);
}

void DEC_A()
{
	unsigned short addr = fetchword();
	RAM[addr]--;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void BNE()
{
	char byte = (char)fetchbyte();

	if(!(P & 2))
		PC += byte;
}

void CMP_IY()
{
	unsigned char zp = fetchbyte();

	CMP(RAM[(RAM[zp+1] << 8) | RAM[zp]]);
}

void CMP_ZX()
{
	CMP(RAM[(fetchbyte() + X) & 0xFF]);
}

void DEC_ZX()
{
	unsigned char addr = fetchbyte() + X;

	RAM[addr]--;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void CLD()
{
	PC++;

	P = P & 247;
}

void CMP_AY()
{
	CMP(RAM[(fetchword() + Y) & 0xFFFF]);
}

void CMP_AX()
{
	CMP(RAM[(fetchword() + X) & 0xFFFF]);
}

void DEC_AX()
{
	unsigned short addr = fetchword() + X;

	RAM[addr]--;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void CPX_I()
{
	unsigned char byte = fetchbyte();

	unsigned char result = X - byte;

	if(X >= byte) P |= 1;
	else P &= 254;

	N_FLAGS(result);
	Z_FLAGS(result);
}

void SBC_IX()
{
	unsigned char zp = fetchbyte() + X;


	SBC(RAM[(RAM[zp+1] << 8) | RAM[zp]]);

	N_FLAGS(A);
	Z_FLAGS(A);
}

void CPX_Z()
{
	unsigned char byte = RAM[fetchbyte()];

	unsigned char result = X - byte;

	if(X >= byte) P |= 1;
	else P &= 254;

	N_FLAGS(result);
	Z_FLAGS(result);
}

void SBC_Z()
{
	SBC(RAM[fetchbyte()]);

	N_FLAGS(A);
	Z_FLAGS(A);
}

void INC_Z()
{
	unsigned char addr = fetchbyte();

	RAM[addr]++;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void INX()
{
	PC++;

	X++;

	N_FLAGS(X);
	Z_FLAGS(X);
}

void SBC_I()
{
	SBC(fetchbyte());
	
	N_FLAGS(A);
	Z_FLAGS(A);
}

void NOP()
{
	PC++;
}

void CPX_A()
{
	unsigned char byte = RAM[fetchword()];

	unsigned char result = X - byte;

	if(X >= byte) P |= 1;
	else P &= 254;

	N_FLAGS(result);
	Z_FLAGS(result);
}

void SBC_A()
{
	SBC(RAM[fetchword()]);

	N_FLAGS(A);
	Z_FLAGS(A);
}

void INC_A()
{
	unsigned short int addr = fetchword();

	RAM[addr]++;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void BEQ()
{
	PC++;
	char change = RAM[PC];
	PC++;

	if(P & 2) PC += change;
}

void SBC_IY()
{
	unsigned char zp = fetchbyte();

	SBC(RAM[(RAM[zp+1] << 8) | RAM[zp]]);

	N_FLAGS(A);
	Z_FLAGS(A);
}

void SBC_ZX()
{
	SBC(RAM[(fetchbyte() + X) & 0xFF]);

	N_FLAGS(A);
	Z_FLAGS(A);
}

void INC_ZX()
{
	unsigned char addr = fetchbyte() + X;
	
	RAM[addr]++;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
}

void SED()
{
	PC++;
	P |= 8;
}

void SBC_AY()
{
	SBC(RAM[(fetchword() + Y) & 0xFFFF]);

	N_FLAGS(A);
	Z_FLAGS(A);
}

void SBC_AX()
{
	SBC(RAM[(fetchword() + X) & 0xFFFF]);

	N_FLAGS(A);
	Z_FLAGS(A);
}

void INC_AX()
{
	unsigned short addr = fetchword() + X;
	
	RAM[addr]++;

	N_FLAGS(RAM[addr]);
	Z_FLAGS(RAM[addr]);
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

char * neumonic[256] =
{
	"BRK","ORA_IX","0","0","0","ORA_Z","ASL_Z","0","PHP","ORA_I","ASL_AC","0","0","ORA_A","ASL_A","0",
	"BPL","ORA_IY","0","0","0","ORA_ZX","0","ASL_ZX","CLC","ORA_AY","0","0","0","ORA_AX","ASL_AX","0",
	"JSR","AND_IX","0","0","BIT_Z","AND_Z","ROL_Z","0","PLP","AND_I","ROL_AC","0","BIT_A","AND_A","ROL_A","0",
	"BMI","AND_IY","0","0","0","AND_ZX","ROL_ZX","0","SEC","AND_AY","0","0","0","AND_AX","ROL_AX","0",
	"RTI","EOR_IX","0","0","0","EOR_Z","LSR_Z","0","PHA","EOR_I","LSR_AC","0","JMP_A","EOR_A","LSR_A","0",
	"BVC","EOR_IY","0","0","0","EOR_ZX","LSR_ZX","CLI","0","EOR_AY","0","0","0","EOR_AX","LSR_AX","0",
	"RTS","ADC_IX","0","0","0","ADC_Z","ROR_Z","0","PLA","ADC_I","ROR_AC","0","JMP_IN","ADC_A","ROR_A","0",
	"BVS","ADC_IY","0","0","0","ADC_ZX","0","ROR_ZX","SEI","ADC_AY","0","0","0","ADC_AX","ROR_AX","0",
	"0","STA_IX","0","0","STY_Z","STA_Z","STX_Z","0","DEY","0","TXA","0","STY_A","STA_A","STX_A","0",
	"BCC","STA_IY","0","0","STY_ZX","STA_ZX","STX_ZY","0","TYA","STA_AY","TXS","0","0","STA_AX","0","0",
	"LDY_I","LDA_IX","LDX_I","0","LDY_Z","LDA_Z","LDX_Z","0","TAY","LDA_I","TAX","0","LDY_A","LDA_A","LDX_A","0",
	"BCS","LDA_IY","0","0","LDY_ZX","LDA_ZX","LDX_ZY","0","CLV","LDA_AY","TSX","0","LDY_AX","LDA_AX","LDX_AY","0",
	"CPY_I","CMP_IX","0","0","CPY_Z","CMP_Z","DEC_Z","0","INY","CMP_I","DEX","0","CPY_A","CMP_A","DEC_A","0",
	"BNE","CMP_IY","0","0","0","CMP_ZX","DEC_ZX","0","CLD","CMP_AY","0","0","0","CMP_AX","DEC_AX","0",
	"CPX_I","SBC_IX","0","0","CPX_Z","SBC_Z","INC_Z","0","INX","SBC_I","NOP","0","CPX_A","SBC_A","INC_A","0",
	"BEQ","SBC_IY","0","0","0","SBC_ZX","INC_ZX","0","SED","SBC_AY","0","0","0","SBC_AX","INC_AX","0"
};

void Pause()
{
	char buf = 0;
	unsigned long x;
	ReadConsole(ConIn, &buf, 1, &x, NULL);
	ConWrite("Press any key to continue\n");
}

void  CPUinfo()
{
	char buf1[9];
	char buf2[9];
	char output[256];

	itoa(P,buf1,2);
	itoa(JOYPAD,buf2,2);

	sprintf(output,
"Next Instruction: %s\n\
Opcode = %.2x\n\
A = %.2x   X = %.2x   Y = %.2x\n\
SP = %.2x   PC=%.4x   P = %s\n\
Joypad status = %s\n",
				   neumonic[RAM[PC]],RAM[PC],A,X,Y,S,PC,buf1,buf2);

	ConWrite(output);

	if(neumonic[RAM[PC]][0] == '0')
	{
		ConWrite("*WARNING*: Unrecognized Opcode\n");
		Pause();
	}

	ConWrite("-----------------------------------\n");
}