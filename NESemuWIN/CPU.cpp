#include <stdlib.h>
#include <stdio.h>

#if DEBUG
void info();
#endif

unsigned char A = 0;
unsigned char X = 0;
unsigned char Y = 0;
unsigned char FLAGS = 0;
unsigned char RAM[65535];

unsigned short int STACKPTR = 0x01FF;
unsigned short int PC = 0;

void ORA_I();
void ASL_AC();

void BPL();
void CLC();

void JSR();
void AND_I();
void BIT_A();

void SEC();

void EOR_Z();
void PHA();
void LSR_AC();
void JMP_A();



void RTS();
void ADC_Z();
void PLA();
void ADC_I();
void JMP_IN();
void ROR_A();

void SEI();

void STY_Z();
void STA_Z();
void STX_Z();
void DEY();
void TXA();
void STA_A();
void STX_A();

void BCC();
void STA_IY();
void TYA();
void STA_AY();
void TXS();

void LDY_I();
void LDX_I();
void LDA_Z();
void LDX_Z();
void TAY();
void LDA_I();
void TAX();
void LDA_A();
void LDX_A();

void BCS();
void LDA_IY();
void LDA_ZX();
void TSX();
void LDA_AX();

void CPY_I();
void DEC_Z();
void INY();
void CMP_I();
void DEX();

void BNE();
void DEC_ZX();
void CLD();

void CPX_I();
void INC_A();

void BEQ();

void N_FLAGS(unsigned char);
void Z_FLAGS(unsigned char);

void (*opcodes[256])() = 
{
	0,0,0,0,0,0,0,0,0,ORA_I,ASL_AC,0,0,0,0,0,
	BPL,0,0,0,0,0,0,0,CLC,0,0,0,0,0,0,0,
	JSR,0,0,0,0,0,0,0,0,AND_I,0,0,BIT_A,0,0,0,
	0,0,0,0,0,0,0,0,SEC,0,0,0,0,0,0,0,
	0,0,0,0,0,EOR_Z,0,0,PHA,0,LSR_AC,0,JMP_A,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	RTS,0,0,0,0,ADC_Z,0,0,PLA,ADC_I,0,0,JMP_IN,0,ROR_A,0,
	0,0,0,0,0,0,0,0,SEI,0,0,0,0,0,0,0,
	0,0,0,0,STY_Z,STA_Z,STX_Z,0,DEY,0,TXA,0,0,STA_A,STX_A,0,
	BCC,STA_IY,0,0,0,0,0,0,TYA,STA_AY,TXS,0,0,0,0,0,
	LDY_I,0,LDX_I,0,0,LDA_Z,LDX_Z,0,TAY,LDA_I,TAX,0,0,LDA_A,LDX_A,0,
	BCS,LDA_IY,0,0,0,LDA_ZX,0,0,0,0,TSX,0,0,LDA_AX,0,0,
	CPY_I,0,0,0,0,0,DEC_Z,0,INY,CMP_I,DEX,0,0,0,0,0,
	BNE,0,0,0,0,0,DEC_ZX,0,CLD,0,0,0,0,0,0,0,
	CPX_I,0,0,0,0,0,0,0,0,0,0,0,0,0,INC_A,0,
	BEQ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

void resetCPU(unsigned char PRG)
{
	A = 0;
	X = 0;
	Y = 0;
	if(PRG == 2) PC = 0x8000;
	else if(PRG == 1) PC = 0xC000;
	STACKPTR = 0x1FF;
	FLAGS = 0;
}

void updateCPU()
{
	if(PC < 0x8000)
	{
		PC += 0x8000;
	}

	opcodes[RAM[PC]]();

#if DEBUG
	info();
#endif
}

void pushw(unsigned short word)
{
	RAM[STACKPTR] = (unsigned char) (word >> 8);
	STACKPTR--;
	RAM[STACKPTR] = (unsigned char) (word & 0xFF);
	STACKPTR--;
}

unsigned short int popw()
{
	STACKPTR++;
	unsigned char lo = RAM[STACKPTR];
	STACKPTR++;
	unsigned char hi = RAM[STACKPTR];
	return ((hi << 8) | lo);
}

void pushb(unsigned char byte)
{
	RAM[STACKPTR] = byte;
	STACKPTR--;
}

unsigned char popb()
{
	STACKPTR++;
	return RAM[STACKPTR];
}

void ORA_I()
{
	PC++;
	unsigned char byte = RAM[PC];
	PC++;

	A |= byte;

	N_FLAGS(A);
	Z_FLAGS(A);
}

void ASL_AC()
{
	PC++;

	FLAGS &= 254;
	FLAGS |= ((A >> 7) & 1);

	A <<= 1;

	N_FLAGS(A);
	Z_FLAGS(A);
}

void BPL()
{
	PC++;
	char byte = RAM[PC];
	PC++;

	if (!(FLAGS & 128))
	{
		PC += byte;
	}
}

void CLC()
{
	PC++;

	FLAGS &= 254;
}

void JSR()
{
	PC++;
	unsigned char lo = RAM[PC];
	PC++;
	unsigned char hi = RAM[PC];
	PC++;

	unsigned short int address = (hi << 8) | lo;

	pushw(PC - 1);

	PC = address;
}

void AND_I()
{
	PC++;
	unsigned char byte = RAM[PC];
	PC++;

	A &= byte;

	N_FLAGS(A);
	Z_FLAGS(A);
}

void BIT_A()
{
	PC++;
	unsigned char lo = RAM[PC];
	PC++;
	unsigned char hi = RAM[PC];
	PC++;

	unsigned short int address = (hi << 8) | lo;
	unsigned char memory = RAM[address];

	unsigned char temp = (memory & A);

	FLAGS |= (memory & 64);

	N_FLAGS(memory);
	Z_FLAGS(temp);
}

void SEC()
{
	PC++;
	FLAGS |= 1;
}

void EOR_Z()
{
	PC++;
	unsigned char address = RAM[PC];
	PC++;

	unsigned char byte = RAM[address];

	A ^= byte;

	N_FLAGS(A);
	Z_FLAGS(A);
}

void PHA()
{
	PC++;
	pushb(A);
}

void LSR_AC()
{
	PC++;
	
	FLAGS = FLAGS & 254;
	FLAGS = FLAGS | (A & 1);

	A <<= 1;
}

void JMP_A()
{
	PC++;
	unsigned char lo = RAM[PC];
	PC++;
	unsigned char hi = RAM[PC];
	PC++;

	PC = (hi << 8) | lo;
}

void RTS()
{
	PC = popw();
	PC++;
}

void ADC_Z()
{
	PC++;
	unsigned char byte = RAM[PC];
	PC++;

	unsigned char temp = A;
	A += byte + (FLAGS & 1);

	if(temp > A) FLAGS |= 1;

	if(((temp&128)&&(byte&128)&&(~A&128))||
		((~temp&128)&&(~byte&128)&&(A&128)))
		FLAGS |= 64;

	N_FLAGS(A);
	Z_FLAGS(A);
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
	PC++;
	unsigned char byte = RAM[PC];
	PC++;

	unsigned char temp = A;
	A = A + byte + (FLAGS & 1);

	if(temp > A)
	{
		FLAGS |= 1;
	}

	if(((temp & 128) && (byte & 128) && (~A & 128)) ||
		((~temp & 128) && (~byte & 128) && (A & 128)))
		FLAGS |= 64;

	N_FLAGS(A);
	Z_FLAGS(A);
}

void JMP_IN()
{
	PC++;
	unsigned char lo = RAM[PC];
	PC++;
	unsigned char hi = RAM[PC];
	PC++;

	unsigned short int address = (hi << 8) | lo;

	lo = RAM[address];
	hi = RAM[address + 1];

	PC = (hi << 8) | lo;
}

void ROR_A()
{
	PC++;
	unsigned char lo = RAM[PC];
	PC++;
	unsigned char hi = RAM[PC];
	PC++;

	unsigned short int address = (hi << 8) | lo;

	unsigned char byte = RAM[address];
	unsigned char temp = byte;

	byte >>= 1;
	byte |= (FLAGS & 1) << 7;

	FLAGS &= 254;
	FLAGS |= (temp & 1);

	RAM[address] = byte;

	N_FLAGS(byte);
	Z_FLAGS(byte);
}

void SEI()
{
	PC++;
	
	FLAGS |= 4;
}

void STY_Z()
{
	PC++;
	unsigned char address = RAM[PC];
	PC++;

	RAM[address] = Y;
}

void STA_Z()
{
	PC++;
	unsigned char address = RAM[PC];
	PC++;

	RAM[address] = A;
}

void STX_Z()
{
	PC++;
	unsigned char address = RAM[PC];
	PC++;

	RAM[address] = X;
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

void STA_A()
{
	PC++;
	unsigned char lo = RAM[PC];
	PC++;
	unsigned char hi = RAM[PC];
	PC++;

	unsigned short int address = (hi << 8) | lo;

	RAM[address] = A;
}

void STX_A()
{
	PC++;
	unsigned char lo = RAM[PC];
	PC++;
	unsigned char hi = RAM[PC];
	PC++;

	unsigned short int address = (hi << 8) | lo;

	RAM[address] = X;
}

void BCC()
{
	PC++;
	char change  = RAM[PC];
	PC++;

	if(!(FLAGS & 1))
	{
		PC += change;
	}
}

void STA_IY()
{
	PC++;
	unsigned char zp_address = RAM[PC];
	PC++;

	unsigned char lo = RAM[zp_address];
	unsigned char hi = RAM[zp_address];
	unsigned char address = (hi << 8) | lo;

	address += Y;
	RAM[address] = A;
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
	 PC++;
	 unsigned char lo = RAM[PC];
	 PC++;
	 unsigned char hi = RAM[PC];
	 PC++;

	 unsigned short int address = ((hi << 8) | lo) + Y;

	 RAM[address] = A;
}

void TXS()
{
	PC++;

	STACKPTR = X + 0x100;
}

void LDY_I()
{
	PC++;
	unsigned char byte = RAM[PC];
	PC++;

	Y = byte;

	Z_FLAGS(Y);
	N_FLAGS(Y);
}

void LDX_I()
{
	PC++;
	unsigned char byte = RAM[PC];
	PC++;

	X = byte;
	Z_FLAGS(X);
	N_FLAGS(X);
}

void LDA_Z()
{
	PC++;
	unsigned char address = RAM[PC];
	PC++;

	A = RAM[address];

	Z_FLAGS(A);
	N_FLAGS(A);
}

void LDX_Z()
{
	PC++;
	unsigned char address = RAM[PC];
	PC++;

	X = RAM[address];

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
	PC++;
	unsigned char byte = RAM[PC];
	PC++;

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
	PC++;
	unsigned char lo = RAM[PC];
	PC++;
	unsigned char hi = RAM[PC];
	PC++;

	unsigned short int address = (hi << 8) | lo;

	A = RAM[address];
	N_FLAGS(A);
	Z_FLAGS(A);
}

void LDX_A()
{
	PC++;
	unsigned char lo = RAM[PC];
	PC++;
	unsigned char hi = RAM[PC];
	PC++;

	unsigned short int address = (hi << 8) | lo;

	X = RAM[address];

	N_FLAGS(X);
	Z_FLAGS(X);
}

void BCS()
{
	PC++;
	char byte = RAM[PC];
	PC++;

	if(FLAGS & 1)
	{
		PC += byte;
	}
}

void LDA_IY()
{
	PC++;
	unsigned char zpaddress = RAM[PC];
	PC++;

	unsigned char lo = RAM[zpaddress];
	unsigned char hi = RAM[zpaddress + 1];
	unsigned short int address = (hi << 8) | lo;

	A = RAM[address];

	N_FLAGS(A);
	Z_FLAGS(A);
}

void LDA_ZX()
{
	PC++;
	unsigned char address = RAM[PC];
	PC++;

	address += X;

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

void LDA_AX()
{
	PC++;
	unsigned char lo = RAM[PC];
	PC++;
	unsigned char hi = RAM[PC];
	PC++;

	unsigned short address = (hi << 8) | lo;

	address += X;

	A = RAM[address];

	Z_FLAGS(A);
	N_FLAGS(A);
}

void CPY_I()
{
	PC++;
	unsigned char byte = RAM[PC];
	PC++;

	if(Y > byte) FLAGS = (FLAGS | 1) & 125;
	else if (Y == byte) FLAGS = (FLAGS | 3) & 127;
	else FLAGS = (FLAGS | 128) & 252;
}

void DEC_Z()
{
	PC++;
	unsigned char address = RAM[PC];
	PC++;

	unsigned char temp = RAM[address];
	temp--;
	RAM[address] = temp;

	Z_FLAGS(temp);
	N_FLAGS(temp);
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
	PC++;
	unsigned char byte = RAM[PC];
	PC++;

	if(A > byte) FLAGS = (FLAGS | 1) & 125;	//carry flag
	else if(A == byte) FLAGS = (FLAGS | 3) & 127;	//carry + zero
	else FLAGS = (FLAGS | 128) & 252;	//negative
}

void DEX()
{
	PC++;

	X--;

	N_FLAGS(X);
	Z_FLAGS(X);
}

void BNE()
{
	PC++;
	char byte = RAM[PC];
	PC++;

	if(!(FLAGS & 2))
	{
		PC += byte;
	}
}

void DEC_ZX()
{
	PC++;
	unsigned char address = RAM[PC];
	PC++;

	address += X;
	unsigned char byte = RAM[address];
	byte--;
	RAM[address] = byte;

	N_FLAGS(byte);
	Z_FLAGS(byte);
}

void CLD()
{
	PC++;

	FLAGS = FLAGS & 247;
}

void CPX_I()
{
	PC++;
	unsigned char byte = RAM[PC];
	PC++;

	if(X > byte) FLAGS = (FLAGS | 1) & 125;
	else if (X == byte) FLAGS = (FLAGS | 3) & 127;
	else FLAGS = (FLAGS | 128) & 252;
}

void INC_A()
{
	PC++;
	unsigned char lo = RAM[PC];
	PC++;
	unsigned char hi = RAM[PC];
	PC++;

	unsigned short int address = (hi << 8) | lo;

	unsigned char temp = RAM[address];
	temp++;
	RAM[address] = temp;

	N_FLAGS(temp);
	Z_FLAGS(temp);
}

void BEQ()
{
	PC++;
	char change = RAM[PC];
	PC++;

	if(FLAGS & 2) PC += change;
}

void N_FLAGS(unsigned char REG)
{
	if (REG & 128) FLAGS |= 128;
	else FLAGS &= 127;
}

void Z_FLAGS(unsigned char REG)
{
	if (!REG) FLAGS |= 2;
	else FLAGS &= 0xFD;
}

#if DEBUG
void info()
{
	char flagstr[9];
	char output[256];

	itoa(FLAGS,flagstr,2);
	sprintf(output,"NextOp = %.2x   A = %.2x   X = %.2x   Y = %.2x\
   SP = %.2x   PC=%.4x   FLAGS = %s\n",
				   RAM[PC],A,X,Y,STACKPTR-0x100,PC,flagstr,RAM[PC]);

	ConWrite(output);
}
#endif
