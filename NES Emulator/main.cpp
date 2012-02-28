#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 1

FILE * LoadRom();
int StartEmu();
int Project8();
int HeaderCheck(FILE *);

void info();

FILE * ROM;

unsigned char instruction;
unsigned char A = 0;
unsigned char X = 0;
unsigned char Y = 0;
unsigned char FLAGS = 0;

unsigned char RAM[65536];
unsigned short int STACKPTR = 0x01FF;
unsigned short int PC = 0;

void BPL();
void SEI();
void TXA();
void STA_A();
void TYA();
void TXS();
void LDX_I();
void TAY();
void LDA_I();
void TAX();
void LDA_A();
void TSX();
void CLD();

void N_FLAGS(unsigned char);
void Z_FLAGS(unsigned char);

void (*opcodes[256])() = 
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	BPL,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,SEI,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,TXA,0,0,STA_A,0,0,
	0,0,0,0,0,0,0,TYA,0,0,TXS,0,0,0,0,0,
	0,0,LDX_I,0,0,0,0,0,TAY,LDA_I,TAX,0,0,LDA_A,0,0,
	0,0,0,0,0,0,0,0,0,0,TSX,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,CLD,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};


int main(int argc, char * argv[])
{
	printf("Welcome to Project8 emulator Console Edition!\n");
	int result;
	while (1)
	{
		result = Project8();
		if(!result) break;
	}

	return 0;
}

int Project8()
{
	FILE * rom;

	rom = LoadRom();
	if(!rom) {printf("ROM file could not be openned!\n"); return 1;}

	if(HeaderCheck(rom))
	{
		ROM = rom;
		StartEmu();
	}
	else printf("Invalid ROM file!\n");

	return 1;
}

int HeaderCheck(FILE * rom)
{
	char * random = (char *) malloc(4);
	unsigned char magic[4] = {'N','E','S',0x1A};
	fread(random,1,4,rom);
	if(memcmp(random,magic,4)) return 0;

	return 1;
}

FILE * LoadRom()
{
	char location[256];

	printf("Location of ROM: ");
	gets(location);	

	if(location[0] == '"')
	{
		location[strlen(location)-1] = '\0';
		memcpy(location,location+1,strlen(location));
	}

	FILE * rom = fopen(location, "rb");

	return rom;
}

int StartEmu()
{
	fseek(ROM,16,SEEK_SET);
	bool play = true;

	while(play)
	{
		instruction = getc(ROM);
		opcodes[instruction]();
#if DEBUG
		info();
#endif
	}
		
	return 0;
}

void BPL()
{
	PC += 2;

	unsigned char byte = getc(ROM);

	if ((FLAGS & 128) == 0)
	{
		if ((byte & 128) == 128)
		{
			byte = (~byte + 1);
			PC -= byte;
		}
		
		else PC += byte;

		fseek(ROM,PC + 16,SEEK_SET); //16 must be added to skip header
	}
}

void SEI()
{
	PC++;
	
	FLAGS = FLAGS | 4;
}

void TXA()
{
	PC++;
	
	A = X;
	N_FLAGS(A);
	Z_FLAGS(A);
}

void STA_A()
{
	PC += 3;
	unsigned char lo = getc(ROM);
	unsigned char hi = getc(ROM);
	unsigned short int address = (hi << 4) | lo;

	RAM[address] = A;
}

void TYA()
{
	PC++;

	A = Y;
	N_FLAGS(A);
	Z_FLAGS(A);
}

void LDX_I()
{
	PC += 2;

	unsigned char byte = getc(ROM);
	X = byte;
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
	PC+=2;

	unsigned char byte = getc(ROM);
	A = byte;
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

void LDA_A()
{
	PC += 3;

	unsigned char lo = getc(ROM);
	unsigned char hi = getc(ROM);
	unsigned short int address = (hi << 4) | lo;

	A = RAM[address];
	N_FLAGS(A);
	Z_FLAGS(A);
}

void TSX()
{
	PC++;

	X = STACKPTR & 0xFF;
	N_FLAGS(X);
	Z_FLAGS(X);
}

void TXS()
{
	PC++;

	STACKPTR = X + 0x100;
}

void CLD()
{
	PC++;

	FLAGS = FLAGS | 8;
}

void N_FLAGS(unsigned char REG)
{
	if ((REG & 128) == 128) FLAGS = FLAGS | 128;
	else FLAGS = FLAGS & 127;
}

void Z_FLAGS(unsigned char REG)
{
	if (!REG) FLAGS = FLAGS | 2;
	else FLAGS = FLAGS & 0xFD;
}


void info()
{
	char flagstr[9];
	itoa(FLAGS,flagstr,2);
	printf("A = %.2x     X = %.2x     Y = %.2x     SP = %.2x     PC=%.4x     FLAGS = %s\n",A,X,Y,STACKPTR-0x100,PC,flagstr);
	system("pause");
}