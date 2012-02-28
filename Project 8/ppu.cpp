#include "Project 8.h"

#include "PPU.h"
#include "CPU.h"
#include "graphics.h"
#include "debug.h"

unsigned short PPUaddr, PPULogicalAddr, PPUTempAddr;
unsigned char toggle, SPRaddr;
unsigned char spraddr = 0, xscroll, yscroll,xoffset;
unsigned char readBuffer = 0;

unsigned char drawing = 0;
unsigned int pixel, PPUcycles;
unsigned char * SCREEN1 = (unsigned char*)malloc(61440);
unsigned char * SCREEN2 = (unsigned char*)malloc(61440);
unsigned char * SPRITES = (unsigned char*)malloc(61440);

void StartPPU()
{
	toggle = 0;
	PPUcycles = 0;
}

void CheckPPUaddr()
{
	int temp;
	PPULogicalAddr = PPUaddr;

	if(PPUaddr >= 0x3000 && PPUaddr < 0x3F00)
	{
		PPULogicalAddr -= 0x1000;
	}

	else if(PPUaddr >= 0x3F20 && PPUaddr < 0x4000)
	{
		temp = (PPUaddr - 0x3F00) / 0x20;
		PPULogicalAddr -= temp * 0x20;
	}

	else if(PPUaddr >= 0x4000)
	{
		PPULogicalAddr &= 0x3FFF;
	}

	if(!(ROMINFO.info1 & 8))
	{
		if(ROMINFO.info1 & 1) //Vertical Mirroring
		{	
			if(PPULogicalAddr >= 0x2800 && PPULogicalAddr < 0x3F00)
			{
				PPULogicalAddr -= 0x800;
			}
		}

		else //Horizontal Mirroring
		{
			if((PPULogicalAddr >= 0x2400 && PPULogicalAddr < 0x2800) ||
				PPULogicalAddr >= 0x2C00 && PPULogicalAddr < 0x3F00)
			{
				PPULogicalAddr -= 0x400;
			}
				
		}
	}
}

void PPUaddress()
{
	if(!toggle)
	{
		toggle = 1;
		PPUTempAddr &= 0xFF;
		PPUTempAddr |= (RAM[0x2006] & 0x3F) << 8;
	}

	else
	{
		toggle = 0;
		PPUTempAddr &= 0xFF00;
		PPUTempAddr |= RAM[0x2006];
		PPUaddr = PPUTempAddr;
		//xscroll = (PPUTempAddr & 0x1F) << 3;
		//yscroll = (PPUTempAddr & 0x3E0) >> 2;
		//yscroll |= (PPUTempAddr & 0x7000) >> 12;
		CheckPPUaddr();
	}
}

void PPUsprDMA()
{
	for(int i = 0; i < 256; i++)
	{
		unsigned char * DMAbuf = RAM + (RAM[0x4014] * 0x100);
		SPRRAM[SPRaddr] = DMAbuf[i];
		SPRaddr++;
	}
} 

void PPUspraddr()
{
	SPRaddr = RAM[0x2003];
}

void PPUsprread()
{
	RAM[0x2004] = SPRRAM[SPRaddr];
}

void PPUsprwrite()
{
	SPRRAM[SPRaddr] = RAM[0x2004];
	SPRaddr++;
}

void PPUread()
{
	if(PPUaddr >= 0x3F00)
	{
		RAM[0x2007] = VRAM[PPULogicalAddr];
		unsigned short temp = PPUaddr;
		PPUaddr -= 0x1000;
		CheckPPUaddr();
		readBuffer = VRAM[PPULogicalAddr];
		PPUaddr = temp;
	}

	else 
	{
		RAM[0x2007] = readBuffer;
		readBuffer = VRAM[PPULogicalAddr];
	}

	PPUaddr++;
	if(RAM[0x2000] & 4) 
		PPUaddr += 31;
	CheckPPUaddr();
	toggle = 0;
}

void PPUwrite()
{
	VRAM[PPULogicalAddr] = RAM[0x2007];

	if(PPULogicalAddr >= 0x3F00 && !((PPUaddr - 0x3F00) % 4))
	{
		if(PPUaddr < 0x3F10) VRAM[PPULogicalAddr + 0x10] = RAM[0x2007];
		else VRAM[PPULogicalAddr - 0x10] = RAM[0x2007];
	}

	PPUaddr++;
	if(RAM[0x2000] & 4) 
		PPUaddr += 31; //Vertical Write
	CheckPPUaddr();
}

void PPU2000write()
{
	PPUTempAddr &= 0xF3FF;
	PPUTempAddr |= (RAM[0x2000] & 3) << 10;
}

void PPUscroll()
{
	if(!toggle)
	{
		PPUTempAddr &= 0xFFE0;
		PPUTempAddr |= RAM[0x2005] >> 3;
		xoffset = RAM[0x2005] & 7;
		xscroll = RAM[0x2005]; //Not technically how it works
		toggle = 1;
	}

	else
	{
		PPUTempAddr &= 0xC1F;
		PPUTempAddr |= (RAM[0x2005] & 0xF8) << 2;
		PPUTempAddr |= (RAM[0x2005] & 0x07) << 14;
		yscroll = RAM[0x2005];
		toggle = 0;
	}
}

void UpdatePPU(unsigned int pixels)
{
	if(PPUcycles < 341)
	{
		PPUcycles += pixels; //Dummy scanline
		RAM[0x2002] &= 0x3F;
	}

	else if(PPUcycles < 82181)
		ContinueDraw(pixels);

	//if(pixel >= 61400)
	//	RAM[0x2002] |= 128; //Some games expect flag to be set in middle of instruction

	else if(PPUcycles < 89342)
		PPUcycles += pixels;

	else
	{
		StartDraw();
		pixel = 0;
		PPUcycles = 0;
		drawing = 1;
	}
}

void CreateTile(unsigned char * buffer, unsigned char tile, 
				int x, int y, char sprite, char table)
{
	int start = x;
	int linestart = y;

	unsigned char * SprData = SPRRAM + ((sprite - 1) * 4);
	unsigned char * tileptr = VRAM + (tile * 16);

	unsigned char tilearray[8][8];
	ZeroMemory(tilearray, 64);

	if(!sprite)
	{
		if(RAM[0x2000] & 16) //data at v0x1000
				tileptr += 0x1000;
	}

	else
	{
		if(RAM[0x2000] & 8)
			tileptr += 0x1000;
	}
	
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
					tilearray[y][x] |= 1;

				else
					tilearray[y][x] |= 2;
			}

			x++;
		}
		x = 0;
		y++;

		if(j == 7)
			y = 0;
	}

//	if(sprite && RAM[0x2000] & 32)
//	{
//		PatternTableBits((unsigned char *)spritearray, tileptr + 16, sprite);
//		SpriteStuff((unsigned char *)spritearray, sprite);
//		bigsprite = true;
//	}

	if(sprite)
	{
		for(int y = 0; y < 8; y++)
			for(int x = 0; x < 8; x++)
				if(tilearray[y][x])
					tilearray[y][x] |= (SprData[2] & 3) << 2;

		if(SprData[2] & 64) //Horizontal Flip//
		{
			unsigned char temparray[8][8];
			memcpy(temparray,tilearray,64);
			
			for(int y = 0; y < 8; y++)
			{
				for(int x = 0; x < 8; x++)
				{
					tilearray[y][x] = temparray[y][7-x];
				}
			}
		}

		if(SprData[2] & 128) //Vertical Flip//
		{
			unsigned char temparray[8][8];
			memcpy(temparray,tilearray,64);
			
			for(int y = 0; y < 8; y++)
			{
				for(int x = 0; x < 8; x++)
				{
					tilearray[y][x] = temparray[7-y][x];
				}
			}
		}

		if(SprData[2] & 32) //Behind Background//
		{
			for(int y = 0; y < 8; y++)
			{
				for(int x = 0; x < 8; x++)
				{
					if(tilearray[y][x])
						tilearray[y][x] |= 64;
				}
			}
		}
		
	}

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
				int pix = (i * 256) + j;

				if(pix >= 61440 || j > 255 || i > 239)
					break;

				buffer[pix] = tilearray[y][x];
				if(sprite == 1) 
					buffer[pix] |= 128; //Sprite 0 ID
			}
			x++;
		}
		x = 0;
		y++;
	}
}

void CreateScreenFromNameTable(unsigned char * Screen, unsigned char NameTable)
{
	int x = 0, y = 0;
	
	unsigned char * pNameTable;
	
	if(!NameTable)
		pNameTable = VRAM + 0x2000;

	else
	{
		if(ROMINFO.info1 & 1)
			pNameTable = VRAM + 0x2400;

		else 
			pNameTable = VRAM + 0x2800;
	}

	for(int i = 0; i < 960; i++)
	{
		CreateTile(Screen, pNameTable[i], x, y, 0, NameTable);

		x += 8;

		if(x == 256)
		{
			x = 0;
			y += 8;
		}
	}
}

void CreateSprites(unsigned char * Screen)
{
	for(int i = 63; i > -1; i--)
	{
		unsigned char * sprite = SPRRAM + (i * 4);
		CreateTile(Screen, sprite[1], sprite[3], sprite[0]+1, i + 1, -1);
	}
}

void ContinueDraw(unsigned int pixels)
{
	if(!pixel)
	{
		ZeroMemory(SCREEN1, 61440);
		ZeroMemory(SCREEN2, 61440);
		ZeroMemory(SPRITES, 61440);
		CreateScreenFromNameTable(SCREEN1, 0);
		CreateScreenFromNameTable(SCREEN2, 1);
		CreateSprites(SPRITES);

		if(RAM[0x2001] & 8 && RAM[0x2001] & 16)
			PPUaddr = PPUTempAddr;
	}

	unsigned char * SCREEN1_;
	unsigned char * SCREEN2_;

	if(pixel % 256 && RAM[0x2001] & 8 && RAM[0x2001] & 16)
	{
		PPUaddr &= 0xFBE0;
		PPUaddr |= PPUTempAddr & 0x041F;
	}

	switch((PPUaddr >> 10) & 3)
	{
		case 0:
			SCREEN1_ = SCREEN1;
			break;

		case 1:
			if(ROMINFO.info1 & 1) //vertical mirror
				SCREEN1_ = SCREEN2;
			else SCREEN1_ = SCREEN1;
			break;

		case 2:
			if(ROMINFO.info1 & 1)
				SCREEN1_ = SCREEN1;
			else SCREEN1_ = SCREEN2;
			break;

		case 3:
			SCREEN1_ = SCREEN2;
			break;
	}

	if(SCREEN1_ == SCREEN2)
		SCREEN2_ = SCREEN1;
	else SCREEN2_ = SCREEN2;

	unsigned char * palette = VRAM + 0x3F00;
	unsigned char * SPRpalette = VRAM + 0x3F10;

	for(unsigned int temp = PPUcycles + pixels; PPUcycles < temp; pixel++, PPUcycles++)
	{
		if(PPUcycles % 341 > 255) //HBlank
		{
			--pixel;
			continue;
		}

		if(PPUcycles == 82181)
			break;

		int x = pixel % 256;
		int y = pixel / 256;
		int ScrPos = (y * 256) + x;
		char BackDrawn = 0;

		if(RAM[0x2001] & 8) //Background On
		{
			if(!((~RAM[0x2001] & 2) && (x < 8)))
			{
				if(x > 255 - xscroll)
				{
					if(ROMINFO.info1 & 1)
					{
						if(SCREEN2_[((pixel + xscroll) + yscroll*256) - 256])
						{
							PutPixel(palette[SCREEN2_[((pixel + xscroll) + yscroll*256) - 256]], pixel);
							BackDrawn = 1;
						}

						else BackDrawn = 0;
					}

					else
					{
						if(SCREEN1_[((pixel + xscroll) + yscroll*256) - 256])
						{
							PutPixel(palette[SCREEN1_[((pixel + xscroll) + yscroll*256) - 256]], pixel);
							BackDrawn = 1;
						}

						else BackDrawn = 1;
					}
				}

				else if(y > 239 - yscroll)
				{
					if(ROMINFO.info1 & 1)
					{
						if(SCREEN1_[((pixel + xscroll) + yscroll*256) - 61440])
						{
							PutPixel(palette[SCREEN1_[((pixel + xscroll) + yscroll*256) - 61440]], pixel);
							BackDrawn = 1;
						}

						else BackDrawn = 0;
					}

					else
					{
						if(SCREEN2_[((pixel + xscroll) + yscroll*256) - 61440])
						{
							PutPixel(palette[SCREEN2_[((pixel + xscroll) + yscroll*256) - 61440]], pixel);
							BackDrawn = 1;
						}

						else BackDrawn = 1;
					}
				}

				else if (SCREEN1_[pixel+xscroll+(yscroll*256)])
				{
					PutPixel(palette[SCREEN1_[pixel+xscroll+(yscroll*256)]], pixel);
					BackDrawn = 1;
				}

				else BackDrawn = 0;
			}
		}
	 
		if(RAM[0x2001] & 16) //Sprites On
		{
			if(!((~RAM[0x2001] & 4) && (x < 8)))
			{
				if(SPRITES[pixel])
				{
						if(BackDrawn && SPRITES[pixel] & 128
							&& RAM[0x2001] & 8 && !(~RAM[0x2001] & 2 && x < 8)
							&& x < 255 && y < 239)
							RAM[0x2002] |= 0x64;
						if(SPRITES[pixel] & 64 && BackDrawn) continue;
						PutPixel(SPRpalette[SPRITES[pixel] & 0x0F], pixel);
				}
			}
		}
	}

	if(PPUcycles == 82181)
	{
		EndDraw();
		RAM[0x2002] |= 128;

		if(!CPU.inNMI && (RAM[0x2000] & 128))
		{
			extern unsigned short PC;
			extern unsigned char P;
			P |= 32;
			pushw(PC);
			pushb(P);
			P |= 2;
			PC = RAM[0xFFFA + 1]<<8 | RAM[0xFFFA];	//Jumps the CPU into the NMI routine similar to a JSR
			CPU.inNMI = 1;

			UpdateFPS();
		}
	}
}

void PPUCleanUp()
{
}
