LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void initGL(HWND hwnd);
void DialogOpen(HWND);
void DropOpen(WPARAM);
void LoadFile(char * path);

void resetCPU();
void updateCPU(HWND);

void CreateBackground(unsigned char *, unsigned char);
void CreateSprites(unsigned char *);
void DrawScreen(unsigned char*, unsigned char*, unsigned char*, int*, int, int);

void DrawPixels(unsigned short, unsigned short);

void LoadFile(char * path);
void WindowSize(char, HWND);

int wx = 256, wy = 240;
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

unsigned char BADREAD = 0;
unsigned char SPRREAD = 0;
unsigned char SPRWRITE = 0;
unsigned char PALETTE = 0;
unsigned char W2003 = 0;
unsigned char W2000 = 0;
bool toggle;

unsigned char JOYREAD = 0;
unsigned char JOYWRITE = 0;
unsigned char currbit = 0;

void pushb(unsigned char);
void pushw(unsigned short);
unsigned char popb();
unsigned short popw();

unsigned char fetchbyte();
unsigned short fetchword();

//remember to remove these later
void CheckPortsW(unsigned short addr);

void N_FLAGS(unsigned char);
void Z_FLAGS(unsigned char);

int _cycles[256] =	
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define BRK		0x0
#define ORA_IX	0x1
#define ORA_Z	0x5
#define ASL_Z	0x6
#define PHP		0x8
#define ORA_I	0x9
#define ASL_AC	0xA
#define ORA_A	0xD
#define ASL_A	0xE

#define BPL		0x10
#define ORA_IY	0x11
#define ORA_ZX	0x15
#define ASL_ZX	0x17
#define CLC		0x18
#define ORA_AY	0x19
#define ORA_AX	0x1D
#define ASL_AX	0x1E

#define JSR		0x20
#define AND_IX	0x21
#define BIT_Z	0x24
#define AND_Z	0x25
#define ROL_Z	0x26
#define PLP		0x28
#define AND_I	0x29
#define ROL_AC	0x2A
#define BIT_A	0x2C
#define AND_A	0x2D
#define ROL_A	0x2E

#define BMI		0x30
#define AND_IY	0x31
#define AND_ZX	0x35
#define ROL_ZX	0x36
#define SEC		0x38
#define AND_AY	0x39
#define AND_AX	0x3D
#define ROL_AX	0x3E

#define RTI		0x40
#define EOR_IX	0x41
#define EOR_Z	0x45
#define LSR_Z	0x46
#define PHA		0x48
#define EOR_I	0x49
#define LSR_AC	0x4A
#define JMP_A	0x4C
#define EOR_A	0x4D
#define LSR_A	0x4E

#define BVC		0x50
#define EOR_IY	0x51
#define EOR_ZX	0x55
#define LSR_ZX	0x56
#define CLI		0x57
#define EOR_AY	0x59
#define EOR_AX	0x5D
#define LSR_AX	0x5E

#define RTS		0x60
#define ADC_IX	0x61
#define ADC_Z	0x65
#define ROR_Z	0x66
#define PLA		0x68
#define ADC_I	0x69
#define ROR_AC	0x6A
#define JMP_IN	0x6C
#define ADC_A	0x6D
#define ROR_A	0x6E

#define BVS		0x70
#define ADC_IY	0x71
#define ADC_ZX	0x75
#define ROR_ZX	0x77
#define SEI		0x78
#define ADC_AY	0x79
#define ADC_AX	0x7D
#define ROR_AX	0x7E

#define STA_IX	0x81
#define STY_Z	0x84
#define STA_Z	0x85
#define STX_Z	0x86
#define DEY		0x88
#define TXA		0x8A
#define STY_A	0x8C
#define STA_A	0x8D
#define STX_A	0x8E

#define BCC		0x90
#define STA_IY	0x91
#define STY_ZX	0x94
#define STA_ZX	0x95
#define STX_ZY	0x96
#define TYA		0x98
#define STA_AY	0x99
#define TXS		0x9A
#define STA_AX	0x9D

#define LDY_I	0xA0
#define LDA_IX	0xA1
#define LDX_I	0xA2
#define LDY_Z	0xA4
#define LDA_Z	0xA5
#define LDX_Z	0xA6
#define TAY		0xA8
#define LDA_I	0xA9
#define TAX		0xAA
#define LDY_A	0xAC
#define LDA_A	0xAD
#define LDX_A	0xAE

#define BCS		0xB0
#define LDA_IY	0xB1
#define LDY_ZX	0xB4
#define LDA_ZX	0xB5
#define LDX_ZY	0xB6
#define CLV		0xB8
#define LDA_AY	0xB9
#define TSX		0xBA
#define LDY_AX	0xBC
#define LDA_AX	0xBD
#define LDX_AY	0xBE

#define CPY_I	0xC0
#define CMP_IX	0xC1
#define CPY_Z	0xC4
#define CMP_Z	0xC5
#define DEC_Z	0xC6
#define INY		0xC8
#define CMP_I	0xC9
#define DEX		0xCA
#define CPY_A	0xCC
#define CMP_A	0xCD
#define DEC_A	0xCE

#define BNE		0xD0
#define CMP_IY	0xD1
#define CMP_ZX	0xD5
#define DEC_ZX	0xD6
#define CLD		0xD8
#define CMP_AY	0xD9
#define CMP_AX	0xDD
#define DEC_AX	0xDE

#define CPX_I	0xE0
#define SBC_IX	0xE1
#define CPX_Z	0xE4
#define SBC_Z	0xE5
#define INC_Z	0xE6
#define INX		0xE8
#define SBC_I	0xE9
#define NOP		0xEA
#define CPX_A	0xEC
#define SBC_A	0xED
#define INC_A	0xEE

#define BEQ		0xF0
#define SBC_IY	0xF1
#define SBC_ZX	0xF5
#define INC_ZX	0xF6
#define SED		0xF8
#define SBC_AY	0xF9
#define SBC_AX	0xFD
#define INC_AX	0xFE

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

//Old palette (faded?)
//unsigned char nes_palette[64][3] =
//{
//	{0x80,0x80,0x80}, {0x00,0x00,0xBB}, {0x37,0x00,0xBF}, {0x84,0x00,0xA6},
//	{0xBB,0x00,0x6A}, {0xB7,0x00,0x1E}, {0xB3,0x00,0x00}, {0x91,0x26,0x00},
//	{0x7B,0x2B,0x00}, {0x00,0x3E,0x00}, {0x00,0x48,0x0D}, {0x00,0x3C,0x22},
//	{0x00,0x2F,0x66}, {0x00,0x00,0x00}, {0x05,0x05,0x05}, {0x05,0x05,0x05},
//
//	{0xC8,0xC8,0xC8}, {0x00,0x59,0xFF}, {0x44,0x3C,0xFF}, {0xB7,0x33,0xCC},
//	{0xFF,0x33,0xAA}, {0xFF,0x37,0x5E}, {0xFF,0x37,0x1A}, {0xD5,0x4B,0x00},
//	{0xC4,0x62,0x00}, {0x3C,0x7B,0x00}, {0x1E,0x84,0x15}, {0x00,0x95,0x66},
//	{0x00,0x84,0xC4}, {0x11,0x11,0x11}, {0x09,0x09,0x09}, {0x09,0x09,0x09},
//
//	{0xFF,0xFF,0xFF}, {0x00,0x95,0xFF}, {0x6F,0x84,0xFF}, {0xD5,0x6F,0xFF},
//	{0xFF,0x77,0xCC}, {0xFF,0x6F,0x99}, {0xFF,0x7B,0x59}, {0xFF,0x91,0x5F},
//	{0xFF,0xA2,0x33}, {0xA6,0xBF,0x00}, {0x51,0xD9,0x6A}, {0x4D,0xD5,0xAE},
//	{0x00,0xD9,0xFF}, {0x66,0x66,0x66}, {0x0D,0x0D,0x0D}, {0x0D,0x0D,0x0D},
//
//	{0xFF,0xFF,0xFF}, {0x84,0xBF,0xFF}, {0xBB,0xBB,0xFF}, {0xD0,0xBB,0xFF},
//	{0xFF,0xBF,0xEA}, {0xFF,0xBF,0xCC}, {0xFF,0xC4,0xB7}, {0xFF,0xCC,0xAE},
//	{0xFF,0xD9,0xA2}, {0xCC,0xE1,0x99}, {0xAE,0xEE,0xB7}, {0xAA,0xF7,0xEE},
//	{0xB3,0xEE,0xFF}, {0xDD,0xDD,0xDD}, {0x11,0x11,0x11}, {0x11,0x11,0x11}
//};
