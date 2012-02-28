//wheelbarrow of dicks up my ass

class PPU
{
private:
	unsigned short ppuaddr, temp;
	unsigned char spraddr, xscroll, yscroll;
public:
	void sprdma(unsigned short);
	void ppuaddress(unsigned char);
	void scroll(unsigned char);
	void ppuread();
	void ppuwrite();

	void resetPPU();
	void update(HWND, unsigned char);
};

void PPU::ppuread()
{
	ppuaddr++;

	if(RAM[0x2000] & 4)
		ppuaddr += 31;
	
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

void PPU::ppuwrite()
{
	if(ppuaddr > 0x3F00 && !(ppuaddr % 0x10))
		VRAM[0x3F00] = RAM[0x2007];

	else VRAM[ppuaddr] = RAM[0x2007];

	ppuaddr++;

	if(RAM[0x2000] & 4)
		ppuaddr += 31;

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

void PPU::sprdma(unsigned short addr)
{
	addr *= 0x100;
	for(int i = 0; i < 256; i++)
	{
		SPRRAM[i] = RAM[addr + i];
	}
}

void PPU::ppuaddress(unsigned char byte)
{
	if(!toggle)
	{
		toggle = true;
		temp &= 0xFF;
		temp = (byte & 0x3F) << 8;
	}
	else
	{
		toggle = false;
		temp &= 0xFF00;
		temp |= byte;
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
}

void PPU::scroll(unsigned char byte)
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
}

void PPU::update(HWND hwnd, unsigned char command)	//ripped from winmain.cpp
{
	/*Instructions Per Second*/
	static unsigned int instr = 0;
	static int time = timeGetTime();
	static char ips[10];
	instr++;
	int timetemp = timeGetTime();
	if (timetemp - time >= 1000)
	{
		itoa(instr, ips, 10);
		time = timeGetTime();
		instr = 0;
	}

	if(W2000)
	{
		temp &= 0xF3FF;
		temp |= (RAM[0x2000] & 3) << 10; //Name table bits

		W2000 = 0;
	}

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

	if(ppuaddr >= 0x3F00) RAM[0x2007] = VRAM[ppuaddr];
	else RAM[0x2007] = VRAM[ppuaddr-1];

	/********************************************/

	/* Frames Per Second */

	static int time2 = timeGetTime();
	static int frames = 0;
	static char fps[4];
	int temptiem = timeGetTime();
	if(temptiem-time2 >= 1000)
	{
		itoa(frames, fps, 10);
		frames = 0;
		time2 = timeGetTime();
	}

	/*******************************************/

	int temptime = timeGetTime();

	static bool draw = true;
	if(temptime-ticks >= 17)	//This isn't exactly 1/60 of a second, 
								//but it is the highest speed that 
								//doesnt adversely affect stability
	{
		draw = true;
		ticks = timeGetTime();
	}

	static int pixel = 0; //pixel = scanline for now
	static int wastetime = 0;

	static unsigned char table1[0xF000];
	static unsigned char table2[0xF000];
	static unsigned char sprites[0xF000];

	if(draw)
	{
		if(!pixel)
		{
			RAM[0x2002] &= 63;
			ZeroMemory(table1, 0xF000);
			ZeroMemory(table2, 0xF000);
			ZeroMemory(sprites, 0xF000);

			if(RAM[0x2001] & 8)
			{
				CreateBackground(table1,0);
				CreateBackground(table2,1);
				
				if(RAM[0x2001] & 16)
					CreateSprites(sprites);
			}

			unsigned char * palette = VRAM + 0x3F00;
			unsigned char clearcol = palette[0];

			glClearColor(
				nes_palette[clearcol][0]/256.0,
				nes_palette[clearcol][1]/256.0,
				nes_palette[clearcol][2]/256.0, 1);

			glClear(GL_COLOR_BUFFER_BIT);

			glBegin(GL_QUADS);
		}

		
		DrawScreen(table1, table2, sprites, &pixel, xscroll, yscroll);
		//DrawPixels(8,pixel);
		//pixel+=8;

		if(pixel == 61440)
		{
			glEnd();
			SwapBuffers(hDC);

			RAM[0x2002] |= 128;
			draw = false;
			pixel = 0;

			frames++;
				
			char title[50] = "Project 8     IPS: ";
			strcat(title, ips);
			strcat(title, "     FPS: ");
			strcat(title, fps);
			SetWindowText(hwnd,title);

			if(!IN_INT && (RAM[0x2000] & 128))	//NMI
			{		
				pushw(PC);
				pushb(P);
				PC = NMI;

				IN_INT = 1;
			}
		}
	}
}

void PPU::resetPPU()
{
	ticks = timeGetTime();
	IN_INT = 0;

	for(int i = 0x2000; i < 0x4000; i++) VRAM[i] = 0;
	
	ZeroMemory(SPRRAM,0xFF);

	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	SwapBuffers(hDC);

	toggle = false;
}
