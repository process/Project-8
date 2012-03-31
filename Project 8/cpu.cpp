#include <windows.h>

#include "Project 8.h"

#include "CPU.h"
#include "PPU.h"
#include "input.h"
#include "Mem.h"
#include "instructions.h"
#include "logging.h"
#include "debug.h"

unsigned short int PC;
unsigned char P;
unsigned char S;
unsigned char A;
unsigned char X;
unsigned char Y;

//Load an absolute+Y address
#define __AY__ \
    addr = fetchword(); \
    if((addr & 0xFF00) != ((addr + Y) & 0xFF00)) \
    { \
        ++instrCycles; \
        --cycles; \
    } \
    addr += Y;

//Load an absolute+X address
#define __AX__ \
    addr = fetchword(); \
    if((addr & 0xFF00) != ((addr + X) & 0xFF00)) \
    { \
        ++instrCycles; \
        --cycles; \
    } \
    addr += X;

//Load an indirect+Y address
#define __IY__ \
    byte = fetchbyte(); \
    addr = (RAM[(byte+1) & 0xFF] << 8) | RAM[byte]; \
    if((addr & 0xFF00) != ((addr + Y) & 0xFF00)) \
    { \
        ++instrCycles; \
        --cycles; \
    } \
    addr += Y;

//Load a word from the next memory addresses
//Used for 3-byte instructions
unsigned short inline fetchword()
{
    PC++;
    unsigned char lo = RAM[PC++];
    unsigned char hi = RAM[PC++];

    return (hi << 8) | lo;
}

//Fetch the next byte in memory
//Used in 2-byte instructions
unsigned char inline fetchbyte()
{
    PC++;
    return RAM[PC++];
}

//Set N and Z flags based on register, REG
void inline NZ_FLAGS(unsigned char REG)
{
    if (REG & 128) P |= 128;   else P &= 127;
    if (!REG) P |= 2;          else P &= 0xFD;
}

//Initialize the CPU
void CPU::StartCPU()
{
    PC = RAM[0xFFFC+1]<<8|RAM[0xFFFC]; //The "RESET" address
    S = 0xFF;
    P = 32;
    this->inNMI = 0;
}

//Run the CPU for 'cycles' cycles.
//This function will call upon the PPU to run after every instruction.
//It also checks to see if a breakpoint has been reached, which is
//used for debugging.
//If logging is enabled, this will send data to the logging functions
void CPU::RunCPU(int cycles)
{ 
    char change;
    unsigned char byte;
    unsigned short addr;
    unsigned char result;

    unsigned char OP;
    unsigned char instrCycles;

start:
    OP = RAM[PC];

    instrCycles = OPcycles[OP];
    cycles -= instrCycles;

    //Check to see if a breakpoint has been hit
    if(Debug::checkBreakAddr(PC))
    {
        PauseNES();
        MessageBoxEx(NULL, "Breakpoint hit!", "Debug", MB_OK | MB_SETFOREGROUND, 0);
        cycles = -1;
    }

    //Logging
    if(Project8::logging)
        UpdateLog();

    //Instructions per second
    Debug::updateIPS();

    //This giant switch statement gets compiled into an optimized jumplist.
    //Thus, I feel the use of 'goto' is justified.
    //See: http://fms.komkon.org/EMUL8/HOWTO.html
    switch(OP)
    {
        case LDA_Z:
            A = RAM[fetchbyte()];
            NZ_FLAGS(A);
            goto end;

        case BNE:
            change = (char)fetchbyte();
            if(!(P & 2)) goto branch;
            goto end;

        case JMP_A:
            PC = fetchword();
            goto end;

        case INX:
            PC++;
            X++;
            NZ_FLAGS(X);
            goto end;

        case BRK:
            pushw(PC + 2);
            pushb(P |= 0x30);
            P |= 4;
            PC = (RAM[0xFFFF] << 8) | RAM[0xFFFE];
            goto end;

        case ORA_IX:
            byte = fetchbyte() + X;
            A |= RAM[(RAM[(byte+1) & 0xFF] << 8) | RAM[byte]];
            NZ_FLAGS(A);
            goto end;

        case ORA_Z:
            A |= RAM[fetchbyte()];
            NZ_FLAGS(A);
            goto end;

        case ASL_Z:
            addr = fetchbyte();
            goto asl;

        case PHP:
            PC++;
            pushb(P | 48);
            goto end;

        case ORA_I:
            A |= fetchbyte();
            NZ_FLAGS(A);
            goto end;

        case ASL_AC:
            PC++;
            P &= 254;
            P |= A >> 7;
            A <<= 1;
            NZ_FLAGS(A);
            goto end;

        case ORA_A:
            A |= RAM[fetchword()];
            NZ_FLAGS(A);
            goto end;

        case ASL_A:
            addr = fetchword();
            goto asl;

        case BPL:
            change = (char)fetchbyte();
            if (!(P & 128)) goto branch;
            goto end;

        case ORA_IY:
            __IY__
            A |= RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case ORA_ZX:
            A |= RAM[(fetchbyte() + X) & 0xFF];
            NZ_FLAGS(A);
            goto end;

        case ASL_ZX:
            addr = (fetchbyte() + X) & 0xFF;
            goto asl;

        case CLC:
            PC++;
            P &= 254;
            goto end;

        case ORA_AY:
            __AY__
            A |= RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case ORA_AX:
            __AX__
            A |= RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case ASL_AX:
            __AX__
            goto asl;

        case JSR:
            pushw(PC+2); //PC+3 for addr read/instruction. -1 for addr storage method
            PC = fetchword();
            goto end;

        case AND_IX:
            byte = fetchbyte() + X;
            A &= RAM[(RAM[(byte+1) & 0xFF] << 8) | RAM[byte]];
            NZ_FLAGS(A);
            goto end;

        case BIT_Z:
            byte = RAM[fetchbyte()];
            result = A & byte;
            P &= 63;
            P |= (byte & 192);
            if (!result) P |= 2;
            else P &= 0xFD;
            goto end;

        case AND_Z:
            A &= RAM[fetchbyte()];
            NZ_FLAGS(A);
            goto end;

        case ROL_Z:
            addr = fetchbyte();
            goto rol;

        case PLP:
            PC++;
            P = popb();
            P &= 0xEF;
            goto end;

        case AND_I:
            A &= fetchbyte();
            NZ_FLAGS(A);
            goto end;

        case ROL_AC:
            PC++;
            byte = A;
            A <<= 1;
            A |= P & 1;
            P &= 254;
            P |= byte >> 7;
            NZ_FLAGS(A);
            goto end;

        case BIT_A:
            addr = fetchword();
            byte = RAM[addr];
            if(addr == 0x2002)
            {
                RAM[0x2002] &= 127;
                extern unsigned char toggle;
                toggle = false;
            }
            P &= 63;
            P |= byte & 192;
            if (!(byte & A)) P |= 2;
            else P &= 0xFD;
            goto end;

        case AND_A:
            A &= RAM[fetchword()];
            NZ_FLAGS(A);
            goto end;

        case ROL_A:
            addr = fetchword();
            goto rol;

        case BMI:
            change = (char)fetchbyte();
            if(P & 128) goto branch;
            goto end;

        case AND_IY:
            __IY__
            A &= RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case AND_ZX:
            A &= RAM[(fetchbyte() + X) & 0xFF];
            NZ_FLAGS(A);
            goto end;

        case ROL_ZX:
            addr = (fetchbyte() + X) & 0xFF;
            goto rol;

        case SEC:
            PC++;
            P |= 1;
            goto end;

        case AND_AY:
            __AY__
            A &= RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case AND_AX:
            __AX__
            A &= RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case ROL_AX:
            __AX__
            goto rol;

        case RTI:
            this->inNMI = 0;
            P = popb();
            PC = popw();
            P &= 0xEF;
            goto end;
        
        case EOR_IX:
            byte = fetchbyte() + X;
            A ^= RAM[(RAM[(byte+1) & 0xFF] << 8) | RAM[byte]];
            NZ_FLAGS(A);
            goto end;

        case EOR_Z:
            A ^= RAM[fetchbyte()];
            NZ_FLAGS(A);
            goto end;

        case LSR_Z:
            addr = fetchbyte();
            goto lsr;

        case PHA:
            PC++;
            pushb(A);
            goto end;

        case EOR_I:
            A ^= fetchbyte();
            NZ_FLAGS(A);
            goto end;

        case LSR_AC:
            PC++;
            P &= 254;
            P |= A & 1;
            A >>= 1;
            NZ_FLAGS(A);
            goto end;

        case EOR_A:
            A ^= RAM[fetchword()];
            NZ_FLAGS(A);
            goto end;

        case LSR_A:
            addr = fetchword();
            goto lsr;

        case BVC:
            change = (char)fetchbyte();
            if(!(P & 64)) goto branch;
            goto end;

        case EOR_IY:
            __IY__
            A ^= RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case EOR_ZX:
            A ^= RAM[(fetchbyte() + X) & 0xFF];
            NZ_FLAGS(A);
            goto end;

        case LSR_ZX:
            addr = (fetchbyte() + X) & 0xFF;
            goto lsr;

        case CLI:
            PC++;
            P &= 251;
            goto end;

        case EOR_AY:
            __AY__
            A ^= RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case EOR_AX:
            __AX__
            A ^= RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case LSR_AX:
            __AX__
            goto lsr;

        case RTS:
            PC = popw();
            PC++;
            goto end;

        case ADC_IX:
            byte = fetchbyte() + X;
            byte = RAM[(RAM[(byte+1) & 0xFF] << 8) | RAM[byte]];
            goto adc;

        case ADC_Z:
            byte = RAM[fetchbyte()];
            goto adc;

        case ROR_Z:
            addr = fetchbyte();
            goto ror;

        case PLA:
            PC++;
            A = popb();
            NZ_FLAGS(A);
            goto end;

        case ADC_I:
            byte = fetchbyte();
            goto adc;

        case ROR_AC:
            PC++;
            byte = A;
            A >>= 1;
            A |= P << 7;
            P &= 254;
            P |= byte & 1;
            NZ_FLAGS(A);
            goto end;

        case JMP_IN:
            addr = fetchword();
            PC = (RAM[((addr+1) & 0xFF) | (addr & 0xFF00)] << 8) | RAM[addr];
            goto end;

        case ADC_A:
            byte = RAM[fetchword()];
            goto adc;

        case ROR_A:
            addr = fetchword();
            goto ror;

        case BVS:
            change = (char)fetchbyte();
            if(P & 64) goto branch;
            goto end;

        case ADC_IY:
            __IY__
            byte = RAM[addr];
            goto adc;

        case ADC_ZX:
            byte = RAM[(fetchbyte() + X) & 0xFF];
            goto adc;

        case ROR_ZX:
            addr = (fetchbyte() + X) & 0xFF;
            goto ror;

        case SEI:
            PC++;
            P |= 4;
            goto end;

        case ADC_AY:
            __AY__
            byte = RAM[addr];
            goto adc;

        case ADC_AX:
            __AX__
            byte = RAM[addr];
            goto adc;

        case ROR_AX:
            __AX__
            goto ror;

        case STA_IX:
            byte = fetchbyte() + X;
            RAM[(RAM[(byte+1) & 0xFF] << 8) | RAM[byte]] = A;
            goto end;

        case STY_Z:
            RAM[fetchbyte()] = Y;
            goto end;

        case STA_Z:
            RAM[fetchbyte()] = A;
            goto end;

        case STX_Z:
            RAM[fetchbyte()] = X;
            goto end;

        case DEY:
            PC++;
            Y--;
            NZ_FLAGS(Y);
            goto end;

        case TXA:
            PC++;
            A = X;
            NZ_FLAGS(A);
            goto end;

        case STY_A:
            addr = fetchword();
            if(addr >= 0x800 && addr < 0x2000)
            {
                char div = addr / 0x800;
                addr -= (0x800 * div);
            }
            RAM[addr] = Y;
            
            goto writeports;

        case STA_A:
            addr = fetchword();
            if(addr >= 0x800 && addr < 0x2000)
            {
                char div = addr / 0x800;
                addr -= (0x800 * div);
            }
            RAM[addr] = A;
            
            goto writeports;

        case STX_A:
            addr = fetchword();
            if(addr >= 0x800 && addr < 0x2000)
            {
                char div = addr / 0x800;
                addr -= (0x800 * div);
            }
            RAM[addr] = X;
            
            goto writeports;

        case BCC:
            change  = (char)fetchbyte();
            if(!(P & 1)) goto branch;
            goto end;

        case STA_IY:
            __IY__
            RAM[addr] = A;
            goto end;

        case STY_ZX:
            RAM[(fetchbyte() + X) & 0xFF] = Y;
            goto end;

        case STA_ZX:
            RAM[(fetchbyte() + X) & 0xFF] = A;
            goto end;

        case STX_ZY:
            RAM[(fetchbyte() + Y) & 0xFF] = X;
            goto end;

        case TYA:
            PC++;
            A = Y;
            NZ_FLAGS(A);
            goto end;

        case STA_AY:
            __AY__
            RAM[addr] = A;
            goto writeports;

        case TXS:
            PC++;
            S = X;
            goto end;

        case STA_AX:
            __AX__
            RAM[addr] = A;
            goto writeports;

        case LDY_I:
            Y = fetchbyte();
            NZ_FLAGS(Y);
            goto end;

        case LDA_IX:
            byte = (fetchbyte() + X) & 0xFF;
            A = RAM[(RAM[(byte+1) & 0xFF] << 8) | RAM[byte]];
            NZ_FLAGS(A);
            goto end;

        case LDX_I:
            X = fetchbyte();
            NZ_FLAGS(X);
            goto end;

        case LDY_Z:
            Y = RAM[fetchbyte()];
            NZ_FLAGS(Y);
            goto end;

        case LDX_Z:
            X = RAM[fetchbyte()];
            NZ_FLAGS(X);
            goto end;

        case TAY:
            PC++;
            Y = A;
            NZ_FLAGS(Y);
            goto end;

        case LDA_I:
            A = fetchbyte();
            NZ_FLAGS(A);
            goto end;

        case TAX:
            PC++;
            X = A;
            NZ_FLAGS(X);
            goto end;

        case LDY_A:
            addr = fetchword();
            if(addr >= 0x800 && addr < 0x2000)
            {
                char div = addr / 0x800;
                addr -= (0x800 * div);
            }
            if(addr == 0x2007) PPUread();
            if(addr == 0x2004) PPUsprread();
            if(addr == 0x4016) ReadInput();
            if(addr == 0x4017) ReadInput2();
            Y = RAM[addr];
            if(addr == 0x2002)
            {
                RAM[0x2002] &= 127;
                extern unsigned char toggle;
                toggle = false;
            }
            NZ_FLAGS(Y);
            goto end;

        case LDA_A:
            addr = fetchword();
            if(addr >= 0x800 && addr < 0x2000)
            {
                char div = addr / 0x800;
                addr -= (0x800 * div);
            }
            if(addr == 0x2007) PPUread();
            if(addr == 0x2004) PPUsprread();
            if(addr == 0x4016) ReadInput();
            if(addr == 0x4017) ReadInput2();
            A = RAM[addr];
            if(addr == 0x2002)
            {
                RAM[0x2002] &= 127;
                extern unsigned char toggle;
                toggle = false;
            }
            NZ_FLAGS(A);
            goto end;

        case LDX_A:
            addr = fetchword();
            if(addr >= 0x800 && addr < 0x2000)
            {
                char div = addr / 0x800;
                addr -= (0x800 * div);
            }
            if(addr == 0x2007) PPUread();
            if(addr == 0x2004) PPUsprread();
            if(addr == 0x4016) ReadInput();
            if(addr == 0x4017) ReadInput2();
            X = RAM[addr];
            if(addr == 0x2002)
            {
                RAM[0x2002] &= 127;
                extern unsigned char toggle;
                toggle = false;
            }
            NZ_FLAGS(X);
            goto end;

        case BCS:
            change = (char)fetchbyte();
            if(P & 1) goto branch;
            goto end;

        case LDA_IY:
            __IY__
            A = RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case LDY_ZX:
            Y = RAM[(fetchbyte() + X) & 0xFF];
            NZ_FLAGS(Y);
            goto end;

        case LDA_ZX:
            A = RAM[(fetchbyte() + X) & 0xFF];
            NZ_FLAGS(A);
            goto end;

        case LDX_ZY:
            X = RAM[(fetchbyte() + Y) & 0xFF];
            NZ_FLAGS(X);
            goto end;

        case CLV:
            PC++;
            P &= 191;
            goto end;

        case LDA_AY:
            __AY__
            A = RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case TSX:
            PC++;
            X = S;
            NZ_FLAGS(X);
            goto end;

        case LDY_AX:
            __AX__
            Y = RAM[addr];
            NZ_FLAGS(Y);
            goto end;

        case LDA_AX:
            __AX__
            if(addr == 0x4016) ReadInput();
            if(addr == 0x4017) ReadInput2();
            A = RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case LDX_AY:
            __AY__
            X = RAM[addr];
            NZ_FLAGS(X);
            goto end;

        case CPY_I:
            byte = fetchbyte();
            result = Y - byte;
            if(Y >= byte) P |= 1;
            else P &= 254;
            NZ_FLAGS(result);
            goto end;

        case CMP_IX:
            byte = fetchbyte() + X;
            byte = RAM[(RAM[(byte+1) & 0xFF] << 8) | RAM[byte]];
            goto cmp;

        case CPY_Z:
            byte = RAM[fetchbyte()];
            result = Y - byte;
            if(Y >= byte) P |= 1;
            else P &= 254;
            NZ_FLAGS(result);
            goto end;

        case CMP_Z:
            byte = RAM[fetchbyte()];
            goto cmp;
        
        case DEC_Z:
            addr = fetchbyte();
            RAM[addr]--;
            NZ_FLAGS(RAM[addr]);
            goto end;

        case INY:
            PC++;
            Y++;
            NZ_FLAGS(Y);
            goto end;

        case CMP_I:
            byte = fetchbyte();
            goto cmp;

        case DEX:
            PC++;
            X--;
            NZ_FLAGS(X);
            goto end;

        case CPY_A:
            byte = RAM[fetchword()];
            result = Y - byte;
            if(Y >= byte) P |= 1;
            else P &= 254;
            NZ_FLAGS(result);
            goto end;

        case CMP_A:
            byte = RAM[fetchword()];
            goto cmp;

        case DEC_A:
            addr = fetchword();
            RAM[addr]--;
            NZ_FLAGS(RAM[addr]);
            goto end;

        case CMP_IY:
            __IY__
            byte = RAM[addr];
            goto cmp;

        case CMP_ZX:
            byte = RAM[(fetchbyte() + X) & 0xFF];
            goto cmp;

        case DEC_ZX:
            addr = (fetchbyte() + X) & 0xFF;
            RAM[addr]--;
            NZ_FLAGS(RAM[addr]);
            goto end;

        case CLD:
            PC++;
            P &= 247;
            goto end;

        case CMP_AY:
            __AY__
            byte = RAM[addr];
            goto cmp;

        case CMP_AX:
            __AX__
            byte = RAM[addr];
            goto cmp;

        case DEC_AX:
            __AX__
            --RAM[addr];
            NZ_FLAGS(RAM[addr]);
            goto end;

        case CPX_I:
            byte = fetchbyte();
            result = X - byte;
            if(X >= byte) P |= 1;
            else P &= 254;
            NZ_FLAGS(result);
            goto end;

        case SBC_IX:
            byte = fetchbyte() + X;
            byte = RAM[(RAM[(byte+1) & 0xFF] << 8) | RAM[byte]];
            goto sbc;

        case CPX_Z:
            byte = RAM[fetchbyte()];
            result = X - byte;
            if(X >= byte) P |= 1;
            else P &= 254;
            NZ_FLAGS(result);
            goto end;

        case SBC_Z:
            byte = RAM[fetchbyte()];
            goto sbc;

        case INC_Z:
            addr = fetchbyte();
            RAM[addr]++;
            NZ_FLAGS(RAM[addr]);
            goto end;

        case SBC_I:
        //UNDOC
        case SBC_EB:
            byte = fetchbyte();
            goto sbc;


        case NOP:
        //UNDOC
        case NOP_1A:
        case NOP_3A:
        case NOP_5A:
        case NOP_7A:
        case NOP_DA:
        case NOP_FA:
            PC++;
            goto end;

        case CPX_A:
            byte = RAM[fetchword()];
            result = X - byte;
            if(X >= byte) P |= 1;
            else P &= 254;
            NZ_FLAGS(result);
            goto end;

        case SBC_A:
            byte = RAM[fetchword()];
            goto sbc;

        case INC_A:
            addr = fetchword();
            RAM[addr]++;
            NZ_FLAGS(RAM[addr]);
            goto end;

        case BEQ:
            change = (char)fetchbyte();
            if(P & 2) goto branch;
            goto end;

        case SBC_IY:
            __IY__
            byte = RAM[addr];
            goto sbc;

        case SBC_ZX:
            byte = RAM[(fetchbyte() + X) & 0xFF];
            goto sbc;

        case INC_ZX:
            addr = (fetchbyte() + X) & 0xFF;
            RAM[addr]++;
            NZ_FLAGS(RAM[addr]);
            goto end;

        case SED:
            PC++;
            P |= 8;
            goto end;

        case SBC_AY:
            __AY__
            byte = RAM[addr];
            goto sbc;

        case SBC_AX:
            __AX__
            byte = RAM[addr];
            goto sbc;

        case INC_AX:
            __AX__
            ++RAM[addr];
            NZ_FLAGS(RAM[addr]);
            goto end;

        /* Undocumented Opcodes */

        case HLT_02:
        case HLT_12:
        case HLT_22:
        case HLT_32:
        case HLT_42:
        case HLT_52:
        case HLT_62:
        case HLT_72:
        case HLT_92:
        case HLT_B2:
        case HLT_D2:
        case HLT_F2:        
            Project8::playing = false;


        case SKB_04:
        case SKB_14:
        case SKB_34:
        case SKB_44:
        case SKB_54:
        case SKB_64:
        case SKB_74:
        case SKB_80:
        case SKB_82:
        case SKB_89:
        case SKB_C2:
        case SKB_D4:
        case SKB_E2:
        case SKB_F4:
            PC += 2;
            goto end;

        case SKW_0C:
        case SKW_1C:
        case SKW_3C:
        case SKW_5C:
        case SKW_7C:
        case SKW_DC:
        case SKW_FC:
            PC += 3;
            goto end;

        case LAX_Z:
            A = RAM[fetchbyte()];
            X = A;
            NZ_FLAGS(A);
            goto end;

        case LAX_ZY:
            A = RAM[(fetchbyte() + Y) & 0xFF];
            X = A;
            NZ_FLAGS(A);
            goto end;

        case LAX_A:
            A = RAM[fetchword()];
            X = A;
            NZ_FLAGS(A);
            goto end;

        case LAX_AY:
            __AY__
            A = RAM[addr];
            X = A;
            NZ_FLAGS(A);
            goto end;

        case LAX_IX:
            byte = (fetchbyte() + X) & 0xFF;
            A = RAM[(RAM[(byte+1) & 0xFF] << 8) | RAM[byte]];
            X = A;
            NZ_FLAGS(A);
            goto end;

        case LAX_IY:
            __IY__
            A = RAM[addr];
            X = A;
            NZ_FLAGS(A);
            goto end;

        case SAX_Z:
            RAM[fetchbyte()] = A & X;
            goto end;

        case SAX_ZY:
            RAM[(fetchbyte() + Y) & 0xFF] = A & X;
            goto end;

        case SAX_IX:
            byte = fetchbyte() + X;
            RAM[(RAM[(byte+1) & 0xFF] << 8) | RAM[byte]] = A & X;
            goto end;

        case SAX_A:
            RAM[fetchword()] = A & X;
            goto end;

        case DCP_Z:
            byte = --RAM[fetchbyte()];
            goto cmp;

        case DCP_ZX:
            byte = --RAM[(fetchbyte() + X) & 0xFF];
            goto cmp;

        case DCP_A:
            byte = --RAM[fetchword()];
            goto cmp;

        case DCP_AX:
            __AX__
            byte = --RAM[addr];
            goto cmp;

        case DCP_AY:
            __AY__
            byte = --RAM[addr];
            goto cmp;

        case DCP_IX:
            byte = fetchbyte() + X;
            byte = --RAM[(RAM[(byte+1) & 0xFF] << 8) | RAM[byte]];
            goto cmp;

        case DCP_IY:
            __IY__
            byte = --RAM[addr];
            goto cmp;

        case ISB_Z:
            byte = ++RAM[fetchbyte()];
            goto sbc;

        case ISB_ZX:
            byte = ++RAM[(fetchbyte() + X) & 0xFF];
            goto sbc;

        case ISB_A:
            byte = ++RAM[fetchword()];
            goto sbc;

        case ISB_AX:
            __AX__
            byte = ++RAM[addr];
            goto sbc;

        case ISB_AY:
            __AY__
            byte = ++RAM[addr];
            goto sbc;

        case ISB_IX:
            byte = fetchbyte() + X;
            byte = ++RAM[(RAM[(byte+1) & 0xFF] << 8) | RAM[byte]];
            goto sbc;

        case ISB_IY:
            __IY__
            byte = ++RAM[addr];
            goto sbc;

        case SLO_Z:
            addr = fetchbyte();
            goto SLO;

        case SLO_ZX:
            addr = (fetchbyte() + X) & 0xFF;
            goto SLO;

        case SLO_A:
            addr = fetchword();
            goto SLO;

        case SLO_AX:
            __AX__
            goto SLO;

        case SLO_AY:
            __AY__
            goto SLO;

        case SLO_IX:
            byte = (fetchbyte() + X) & 0xFF;
            addr = (RAM[(byte+1) & 0xFF] << 8) | RAM[byte];
            goto SLO;

        case SLO_IY:
            __IY__

        SLO:
            P &= 254;
            P |= RAM[addr] >> 7;
            RAM[addr] <<= 1;
            A |= RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case RLA_Z:
            addr = fetchbyte();
            goto RLA;

        case RLA_ZX:
            addr = (fetchbyte() + X) & 0xFF;
            goto RLA;

        case RLA_A:
            addr = fetchword();
            goto RLA;

        case RLA_AX:
            __AX__
            goto RLA;

        case RLA_AY:
            __AY__
            goto RLA;

        case RLA_IX:
            byte = (fetchbyte() + X) & 0xFF;
            addr = (RAM[(byte+1) & 0xFF] << 8) | RAM[byte];
            goto RLA;

        case RLA_IY:
            __IY__

        RLA:
            byte = RAM[addr];
            RAM[addr] <<= 1;
            RAM[addr] |= P & 1;
            P &= 254;
            P |= byte >> 7;
            A &= RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case SRE_Z:
            addr = fetchbyte();
            goto SRE;

        case SRE_ZX:
            addr = (fetchbyte() + X) & 0xFF;
            goto SRE;

        case SRE_A:
            addr = fetchword();
            goto SRE;

        case SRE_AX:
            __AX__
            goto SRE;

        case SRE_AY:
            __AY__
            goto SRE;

        case SRE_IX:
            byte = (fetchbyte() + X) & 0xFF;
            addr = (RAM[(byte+1) & 0xFF] << 8) | RAM[byte];
            goto SRE;

        case SRE_IY:
            __IY__

        SRE:
            P &= 254;
            P |= RAM[addr] & 1;
            RAM[addr] >>= 1;
            A ^= RAM[addr];
            NZ_FLAGS(A);
            goto end;

        case RRA_Z:
            addr = fetchbyte();
            goto RRA;

        case RRA_ZX:
            addr = (fetchbyte() + X) & 0xFF;
            goto RRA;

        case RRA_A:
            addr = fetchword();
            goto RRA;

        case RRA_AX:
            __AX__
            goto RRA;

        case RRA_AY:
            __AY__
            goto RRA;

        case RRA_IX:
            byte = (fetchbyte() + X) & 0xFF;
            addr = (RAM[(byte+1) & 0xFF] << 8) | RAM[byte];
            goto RRA;

        case RRA_IY:
            __IY__

        RRA:
            byte = RAM[addr];
            RAM[addr] >>= 1;
            RAM[addr] |= P << 7;
            P &= 254;
            P |= byte & 1;
            byte =  RAM[addr];
            goto adc;

        case AAC_0B:
        case AAC_2B:
            byte = fetchbyte();
            A &= byte;
            P &= 254;
            P |= A >> 7;
            NZ_FLAGS(A);
            goto end;

        case ARR:
            A &= fetchbyte();
            byte = A;
            A >>= 1;
            A |= P << 7;
            P &= 0xFE;
            P |= byte & 1;
            byte = (A & 0x60) >> 5;

            P &= 0xBE;
            if(byte == 1) P |= 0x40;
            else if(byte == 2) P |= 0x41;
            else if(byte == 3) P |= 0x01;

            NZ_FLAGS(A);
            goto end;

        case ASR:
            A &= fetchbyte();
            P &= 0xFE;
            P |= A & 1;
            A >>= 1;
            NZ_FLAGS(A);
            goto end;

        case ATX:
            A = 0xFF; //Strange data hiccup. Not guaranteed correct. Read docs
            A &= fetchbyte();
            X = A;
            NZ_FLAGS(A);
            goto end;

        case AXA_AY:
            __AY__
            goto AXA;

        case AXA_IY:
            __IY__

        AXA:
            byte = A & X & ((addr >> 8) + 1);
            RAM[addr] = byte;
            goto end;

        case AXS:
            X &= A;
            result = X;
            X -= fetchbyte();

            if (result >= X) P |= 1;
            else P &= 254;

            NZ_FLAGS(X);
            goto end;

        case LAR_AY:
            __AY__
            S = S & RAM[addr];
            A = S;
            X = S;
            NZ_FLAGS(A);
            goto end;
        
        case SXA_AY:
            __AY__
            if((addr & 0xFF00) == ((addr-Y) & 0xFF00))
                RAM[addr] = X & (((addr-Y) >> 8)+1);
            goto end;

        case SYA_AX:
            __AX__
            if((addr & 0xFF00) == ((addr-X) & 0xFF00))
                RAM[addr] = Y & (((addr-X) >> 8)+1);
            goto end;

        case XAA:
            A = X;
            A &= fetchbyte();
            NZ_FLAGS(A);
            goto end;

        case XAS_AY:
            __AY__
            S = A & X;
            RAM[addr] = S & ((addr+1) >> 8);
            goto end;

        default:
            PC++;
            goto end;

    writeports:
            if(addr == 0x2000) PPU2000write();
            else if(addr == 0x2003) PPUspraddr();
            else if(addr == 0x2004) PPUsprwrite();
            else if(addr == 0x2005) PPUscroll();
            else if(addr == 0x2006) PPUaddress();
            else if(addr == 0x2007) PPUwrite();
            else if(addr == 0x4014) PPUsprDMA();
            else if(addr == 0x4016) WriteInput();
            goto end;

        adc:
            result = A;
            A += byte + (P&1);
            if(result > A - (P&1)) P |= 1;
            else P &= 254;
            if(((result&128)&&(byte&128)&&(~A&128))||
                ((~result&128)&&(~byte&128)&&(A&128)))
                P |= 64;
            else P &= 0xBF;
            NZ_FLAGS(A);
            goto end;

        sbc:
            result = A;
            A += ~byte + (P&1);
            if(result > A - (P&1)) P |= 1;
            else P &= 254;
            if(((result&128)&&(~byte&128)&&(~A&128))||
                ((~result&128)&&(byte&128)&&(A&128)))
                P |= 64;
            else P &= 0xBF;
            NZ_FLAGS(A);
            goto end;

        cmp:
            result = A - byte;
            if(A >= byte) P |= 1;
            else P &= 254;
            NZ_FLAGS(result);
            goto end;

        asl:
            P &= 254;
            P |= RAM[addr] >> 7;
            RAM[addr] <<= 1;
            NZ_FLAGS(RAM[addr]);
            goto end;

        lsr:
            P &= 254;
            P |= RAM[addr] & 1;
            RAM[addr] >>= 1;
            NZ_FLAGS(RAM[addr]);
            goto end;

        rol:
            byte = RAM[addr];
            RAM[addr] <<= 1;
            RAM[addr] |= P & 1;
            P &= 254;
            P |= byte >> 7;
            NZ_FLAGS(RAM[addr]);
            goto end;

        ror:
            byte = RAM[addr];
            RAM[addr] >>= 1;
            RAM[addr] |= P << 7;
            P &= 254;
            P |= byte & 1;
            NZ_FLAGS(RAM[addr]);
            goto end;

        branch:
            ++instrCycles;
            --cycles;
            if(((PC + change) & 0xFF00) != (PC & 0xFF00))
            {
                ++instrCycles;
                --cycles;
            }
            PC += change;
            goto end;

           end:
            //this->A = A;
            //this->X = X;
            //this->Y = Y;
            //this->P = P;
            //this->PC = PC;
            UpdatePPU(instrCycles*3);
            if(cycles > 0) goto start;
    }
}
