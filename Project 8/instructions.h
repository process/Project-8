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
#define ASL_ZX	0x16
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
#define CLI		0x58
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
#define ROR_ZX	0x76
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

/* Undocumented Opcodes */
#define HLT_02 0x02
#define HLT_12 0x12
#define HLT_22 0x22
#define HLT_32 0x32
#define HLT_42 0x42
#define HLT_52 0x52
#define HLT_62 0x62
#define HLT_72 0x72
#define HLT_92 0x92
#define HLT_B2 0xB2
#define HLT_D2 0xD2
#define HLT_F2 0xF2

#define NOP_1A 0x1A
#define NOP_3A 0x3A
#define NOP_5A 0x5A
#define NOP_7A 0x7A
#define NOP_DA 0xDA
#define NOP_FA 0xFA

#define SKB_04 0x04
#define SKB_14 0x14
#define SKB_34 0x34
#define SKB_44 0x44
#define SKB_54 0x54
#define SKB_64 0x64
#define SKB_74 0x74
#define SKB_80 0x80
#define SKB_82 0x82
#define SKB_89 0x89
#define SKB_C2 0xC2
#define SKB_D4 0xD4
#define SKB_E2 0xE2
#define SKB_F4 0xF4

#define SKW_0C 0x0C
#define SKW_1C 0x1C
#define SKW_3C 0x3C
#define SKW_5C 0x5C
#define SKW_7C 0x7C
#define SKW_DC 0xDC
#define SKW_FC 0xFC

#define LAX_Z  0xA7
#define LAX_ZY 0xB7
#define LAX_A  0xAF
#define LAX_AY 0xBF
#define LAX_IX 0xA3
#define LAX_IY 0xB3

#define SAX_Z  0x87
#define SAX_ZY 0x97
#define SAX_IX 0x83
#define SAX_A  0x8F

#define SBC_EB 0xEB

#define DCP_Z  0xC7
#define DCP_ZX 0xD7
#define DCP_A  0xCF
#define DCP_AX 0xDF
#define DCP_AY 0xDB
#define DCP_IX 0xC3
#define DCP_IY 0xD3

#define ISB_Z  0xE7
#define ISB_ZX 0xF7
#define ISB_A  0xEF
#define ISB_AX 0xFF
#define ISB_AY 0xFB
#define ISB_IX 0xE3
#define ISB_IY 0xF3

#define SLO_Z  0x07
#define SLO_ZX 0x17
#define SLO_A  0x0F
#define SLO_AX 0x1F
#define SLO_AY 0x1B
#define SLO_IX 0x03
#define SLO_IY 0x13

#define RLA_Z  0x27
#define RLA_ZX 0x37
#define RLA_A  0x2F
#define RLA_AX 0x3F
#define RLA_AY 0x3B
#define RLA_IX 0x23
#define RLA_IY 0x33

#define SRE_Z  0x47
#define SRE_ZX 0x57
#define SRE_A  0x4F
#define SRE_AX 0x5F
#define SRE_AY 0x5B
#define SRE_IX 0x43
#define SRE_IY 0x53

#define RRA_Z  0x67
#define RRA_ZX 0x77
#define RRA_A  0x6F
#define RRA_AX 0x7F
#define RRA_AY 0x7B
#define RRA_IX 0x63
#define RRA_IY 0x73

#define AAC_0B 0x0B
#define AAC_2B 0x2B

#define ARR    0x6B
#define ASR    0x4B
#define ATX	   0xAB

#define AXA_AY 0x9F
#define AXA_IY 0x93

#define AXS    0xCB
#define LAR_AY 0xBB
#define SXA_AY 0x9E
#define SYA_AX 0x9C
#define XAA    0x8B
#define XAS_AY 0x9B

unsigned char OPcycles[256] = 
{ 
/*
0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
7, 6, 0, 8, 3, 3, 5, 5, 8, 9, 2, 2, 4, 4, 6, 6, 
2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 
6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6, 
2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 

6, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6, 
2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 
6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6, 
2, 5, 0, 8, 4, 4, 6, 6, 2, 3, 2, 7, 4, 3, 7, 7, 

2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, 
2, 6, 0, 6, 4, 4, 4, 4, 2, 5, 2, 5, 0, 5, 5, 5, 
2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, 
2, 5, 0, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4, 

2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 5, 4, 6, 6, 
2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 
2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, 
2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7
};

//Note: The following instructions are supposed to have extra
//		cycles under certain conitions:
//			*BPL (0x10) - +1 if branch succeeds; +2 if to new page
//			*ORA_IY (0x11) - +1 if page crossed
//			*ORA_AY (0x19) - +1 if page crossed
//			*ORA_AX (0x1D) - +1 if page crossed
//			*BMI (0x30) - +1 if branch succeeds; +2 if to new page
//			*AND_IY (0x31) - +1 if page crossed
//			*AND_AY (0x39) - +1 if page crossed
//			*AND_AX (0x3A) - +1 if page crossed
//			*BVC (0x50) - +1 if branch succeeds; +2 if to new page
//			*EOR_IY (0x51) - +1 if page crossed
//			*EOR_AY (0x59) - +1 if page crossed
//			*EOR_AX (0x5D) - +1 if page crossed
//			*BVS (0x70) - +1 if branch succeeds; +2 if to new page
//			*ADC_IY (0x71) - +1 if page crossed
//			*ADC_AY (0x79) - +1 if page crossed
//			*ADC_AX (0x7D) - +1 if page crossed
//			*BCC (0x90) - +1 if branch succeeds; +2 if to new page
//			*BCS (0xB0) - +1 if branch succeeds; +2 if to new page
//			*LDA_IY (0xB1) - +1 if page crossed
//			*LDA_AY (0xB9) - +1 if page crossed
//			*LDY_AX (0xBC) - +1 if page crossed
//			*LDA_AX (0xBD) - +1 if page crossed
//			*LDX_AY (0xBE) - +1 if page crossed
//			*BNE (0xD0) - +1 if branch succeeds; +2 if to new page
//			*CMP_IY (0xD1) - +1 if page crossed
//			*CMP_AY (0xD9) - +1 if page crossed
//			*CMP_AX (0xDD) - +1 if page crossed
//			*BEQ (0xF0) - +1 if branch succeeds; +2 if to new page
//			*SBC_IY (0xF1) - +1 if page crossed
//			*SBC_AY (0xF9) - +1 if page crossed
//			*SBC_AX (0xFD) - +1 if page crossed
//
//			*SKW_1C (0x1C) - +1 if page crossed
//			*SKW_3C (0x3C) - +1 if page crossed
//			*SKW_5C (0x5C) - +1 if page crossed
//			*SKW_7C (0x7C) - +1 if page crossed
//			*SKW_DC (0xDC) - +1 if page crossed
//			*SKW_FC (0xFC) - +1 if page crossed
//			*LAX_AY (0xBF) - +1 if page crossed
//			*LAX_IY (0xB3) - +1 if page crossed
//			*LAR_AY (0xBB) - +1 if page crossed