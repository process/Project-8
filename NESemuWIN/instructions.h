        case LDA_Z:
            a = RAM[fetchbyte()];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case BNE:
            change = (char)fetchbyte();
            if(!(P & 2)) PC += change;
            goto end;

        case JMP_A:
            PC = fetchword();
            goto end;

        case INX:
            PC++;
            x++;
            N_FLAGS(x);
            Z_FLAGS(x);
            goto end;

        case BRK:
            PC++;
            P |= 16;
            pushw(PC+1);
            pushb(P);
            PC = (RAM[0xFFFF] << 8) | RAM[0xFFFE];
            goto end;

        case ORA_IX:
            byte = fetchbyte() + x;
            a |= RAM[(RAM[byte+1] << 8) | RAM[byte]];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ORA_Z:
            a |= RAM[fetchbyte()];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ASL_Z:
            addr = fetchbyte();
            P &= 254;
            P |= RAM[addr] >> 7;
            RAM[addr] <<= 1;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case PHP:
            PC++;
            P |= 16;    //Set BRK bit. It's a bug.
            pushb(P);
            goto end;

        case ORA_I:
            a |= fetchbyte();
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ASL_AC:
            PC++;
            P &= 254;
            P |= a >> 7;
            a <<= 1;
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ORA_A:
            a |= RAM[fetchword()];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ASL_A:
            addr = fetchword();
            P &= 254;
            P |= RAM[addr] >> 7;
            RAM[addr] <<= 1;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case BPL:
            change = (char)fetchbyte();
            if (!(P & 128)) PC += change;
            goto end;

        case ORA_IY:
            byte = fetchbyte();
            a |= RAM[((RAM[byte+1] << 8) | RAM[byte]) + y];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ORA_ZX:
            a |= RAM[(fetchbyte() + x) & 0xFF];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ASL_ZX:
            addr = (fetchbyte() + x) & 0xFF;
            P &= 254;
            P |= RAM[addr] >> 7;
            RAM[addr] <<= 1;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case CLC:
            PC++;
            P &= 254;
            goto end;

        case ORA_AY:
            a |= RAM[(fetchword() + y) & 0xFFFF];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ORA_AX:
            a |= RAM[(fetchword() + x) & 0xFFFF];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ASL_AX:
            addr = fetchword() + x;
            P &= 254;
            P |= RAM[addr] >> 7;
            RAM[addr] <<= 1;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case JSR:
            pushw(PC+2);    //PC+3 for addr read/instruction. -1 for addr storage method
            PC = fetchword();
            goto end;

        case AND_IX:
            byte = fetchbyte() + x;
            a &= RAM[(RAM[byte+1] << 8) | RAM[byte]];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case BIT_Z:
            byte = RAM[fetchbyte()];
            result = a & byte;
            P &= 63;
            P |= (byte & 192);
            Z_FLAGS(result);
            goto end;

        case AND_Z:
            a &= RAM[fetchbyte()];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ROL_Z:
            addr = fetchbyte();
            byte = RAM[addr];
            RAM[addr] <<= 1;
            RAM[addr] |= P & 1;
            P &= 254;
            P |= byte >> 7;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case PLP:
            PC++;
            P = popb();
            P |= 32; //bit 5 is ALWAYS set
            goto end;

        case AND_I:
            a &= fetchbyte();
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ROL_AC:
            PC++;
            byte = a;
            a <<= 1;
            a |= P & 1;
            P &= 254;
            P |= byte >> 7;
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case BIT_A:
            byte = RAM[fetchword()];
            P &= 63;
            P |= byte & 192;
            Z_FLAGS(byte & a);
            goto end;

        case AND_A:
            a &= RAM[fetchword()];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ROL_A:
            addr = fetchword();
            byte = RAM[addr];
            RAM[addr] <<= 1;
            RAM[addr] |= P & 1;
            P &= 254;
            P |= byte >> 7;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case BMI:
            change = (char)fetchbyte();
            if(P & 128) PC += change;
            goto end;

        case AND_IY:
            byte = fetchbyte();
            a &= RAM[((RAM[byte+1] << 8) | RAM[byte]) + y];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case AND_ZX:
            a &= RAM[(fetchbyte() + x) & 0xFF];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ROL_ZX:
            addr = fetchbyte() + x;
            byte = RAM[addr];
            RAM[addr] <<= 1;
            RAM[addr] |= P & 1;
            P &= 254;
            P |= byte >> 7;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case SEC:
            PC++;
            P |= 1;
            goto end;

        case AND_AY:
            a &= RAM[(fetchword() + y) & 0xFFFF];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case AND_AX:
            a &= RAM[(fetchword() + x) & 0xFFFF];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ROL_AX:
            addr = fetchword() + x;
            byte = RAM[addr];
            RAM[addr] <<= 1;
            RAM[addr] |= P & 1;
            P &= 254;
            P |= (byte >> 7);
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case RTI:
            IN_INT = 0;
            P = popb();
            PC = popw();
            P &= 0xEF; //break flag
            goto end;
        
        case EOR_IX:
            byte = fetchbyte() + x;
            a ^= RAM[(RAM[byte+1] << 8) | RAM[byte]];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case EOR_Z:
            a ^= RAM[fetchbyte()];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case LSR_Z:
            addr = fetchbyte();
            P &= 254;
            P |= RAM[addr] & 1;
            RAM[addr] >>= 1;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case PHA:
            PC++;
            pushb(a);
            goto end;

        case EOR_I:
            a ^= fetchbyte();
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case LSR_AC:
            PC++;
            P &= 254;
            P |= a & 1;
            a >>= 1;
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case EOR_A:
            a ^= RAM[fetchword()];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case LSR_A:
            addr = fetchword();
            P &= 254;
            P |= RAM[addr] & 1;
            RAM[addr] >>= 1;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case BVC:
            change = (char)fetchbyte();
            if(!(P & 64)) PC += change;
            goto end;

        case EOR_IY:
            byte = fetchbyte();
            a ^= RAM[((RAM[byte+1] << 8) | RAM[byte]) + y];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case EOR_ZX:
            a ^= RAM[(fetchbyte() + x) & 0xFF];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case LSR_ZX:
            addr = (fetchbyte() + x) & 0xFF;
            P &= 254;
            P |= RAM[addr] & 1;
            RAM[addr] >>= 1;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case CLI:
            PC++;
            P &= 251;
            goto end;

        case EOR_AY:
            a ^= RAM[(fetchword() + y) & 0xFFFF];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case EOR_AX:
            a ^= RAM[(fetchword() + x) & 0xFFFF];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case LSR_AX:
            addr = fetchword() + x;
            P &= 254;
            P |= RAM[addr] & 1;
            RAM[addr] >>= 1;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case RTS:
            PC = popw();
            PC++;
            goto end;

        case ADC_IX:
            byte = fetchbyte() + x;
            byte = RAM[(RAM[byte+1] << 8) | RAM[byte]];
            goto adc;
            goto end;

        case ADC_Z:
            byte = RAM[fetchbyte()];
            goto adc;
            goto end;

        case ROR_Z:
            addr = fetchbyte();
            byte = RAM[addr];
            RAM[addr] >>= 1;
            RAM[addr] |= P << 7;
            P &= 254;
            P |= byte & 1;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case PLA:
            PC++;
            a = popb();
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case ADC_I:
            byte = fetchbyte();
            goto adc;
            goto end;

        case ROR_AC:
            PC++;
            byte = a;
            a >>= 1;
            a |= P << 7;
            P &= 254;
            P |= byte & 1;
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case JMP_IN:
            addr = fetchword();
            PC = (RAM[((addr+1) & 0xFF) | (addr & 0xFF00)] << 8) | RAM[addr];
            goto end;

        case ADC_A:
            byte = RAM[fetchword()];
            goto adc;
            goto end;

        case ROR_A:
            addr = fetchword();
            byte = RAM[addr];
            RAM[addr] >>= 1;
            RAM[addr] |= P << 7;
            P &= 254;
            P |= (byte & 1);
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case BVS:
            change = (char)fetchbyte();
            if(P & 64) PC += change;
            goto end;

        case ADC_IY:
            byte = fetchbyte();
            byte = ((RAM[byte+1] << 8) | RAM[byte]) + y;
            goto adc;
            goto end;

        case ADC_ZX:
            byte = RAM[(fetchbyte() + x) & 0xFF];
            goto adc;
            goto end;

        case ROR_ZX:
            addr = (fetchbyte() + x) & 0xFF;
            byte = RAM[addr];
            RAM[addr] >>= 1;
            RAM[addr] |= P << 7;
            P &= 254;
            P |= byte & 1;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case SEI:
            PC++;
            P |= 4;
            goto end;

        case ADC_AY:
            byte = RAM[(fetchword() + y) & 0xFFFF];
            goto adc;
            goto end;

        case ADC_AX:
            byte = RAM[(fetchword() + x) & 0xFFFF];
            goto adc;
            goto end;

        case ROR_AX:
            addr = fetchword() + x;
            byte = RAM[addr];
            RAM[addr] >>= 1;
            RAM[addr] |= P << 7;
            P &= 254;
            P |= byte & 1;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case STA_IX:
            byte = fetchbyte() + x;
            RAM[(RAM[byte+1] << 8) | RAM[byte]] = a;
            goto end;

        case STY_Z:
            RAM[fetchbyte()] = y;
            goto end;

        case STA_Z:
            RAM[fetchbyte()] = a;
            goto end;

        case STX_Z:
            RAM[fetchbyte()] = x;
            goto end;

        case DEY:
            PC++;
            y--;
            Z_FLAGS(y);
            N_FLAGS(y);
            goto end;

        case TXA:
            PC++;
            a = x;
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case STY_A:
            addr = fetchword();
            if(addr >= 0x800 && addr < 0x2000)
            {
                char div = addr / 0x800;
                addr -= (0x800 * div);
            }
            RAM[addr] = y;
            CheckPortsW(addr);
            if(addr == 0x4014) ppu.sprdma(y);
            goto end;

        case STA_A:
            addr = fetchword();
            if(addr >= 0x800 && addr < 0x2000)
            {
                char div = addr / 0x800;
                addr -= (0x800 * div);
            }
            RAM[addr] = a;
            CheckPortsW(addr);
            if(addr == 0x4014) ppu.sprdma(a);
            goto end;

        case STX_A:
            addr = fetchword();
            if(addr >= 0x800 && addr < 0x2000)
            {
                char div = addr / 0x800;
                addr -= (0x800 * div);
            }
            RAM[addr] = x;
            CheckPortsW(addr);
            if(addr == 0x4014) ppu.sprdma(x);
            goto end;

        case BCC:
            change  = (char)fetchbyte();
            if(!(P & 1)) PC += change;
            goto end;

        case STA_IY:
            byte = fetchbyte();
            RAM[((RAM[byte+1] << 8) | RAM[byte]) + y] = a;
            goto end;

        case STY_ZX:
            RAM[(fetchbyte() + x) & 0xFF] = y;
            goto end;

        case STA_ZX:
            RAM[(fetchbyte() + x) & 0xFF] = a;
            goto end;

        case STX_ZY:
            RAM[(fetchbyte() + y) & 0xFF] = x;
            goto end;

        case TYA:
            PC++;
            a = y;
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case STA_AY:
            addr = fetchword() + y;
            RAM[addr] = a;
            CheckPortsW(addr);
            if(addr == 0x4014) ppu.sprdma(a);
            goto end;

        case TXS:
            PC++;
            S = x;
            goto end;

        case STA_AX:
            addr = fetchword() + x;
            RAM[addr] = a;
            CheckPortsW(addr);
            if(addr == 0x4014) ppu.sprdma(a);
            goto end;

        case LDY_I:
            y = fetchbyte();
            Z_FLAGS(y);
            N_FLAGS(y);
            goto end;

        case LDA_IX:
            byte = fetchbyte() + x;
            a = RAM[(RAM[byte+1] << 8) | RAM[byte]];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case LDX_I:
            x = fetchbyte();
            Z_FLAGS(x);
            N_FLAGS(x);
            goto end;

        case LDY_Z:
            y = RAM[fetchbyte()];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case LDX_Z:
            x = RAM[fetchbyte()];
            N_FLAGS(x);
            Z_FLAGS(x);
            goto end;

        case TAY:
            PC++;
            y = a;
            N_FLAGS(y);
            Z_FLAGS(y);
            goto end;

        case LDA_I:
            a = fetchbyte();
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case TAX:
            PC++;
            x = a;
            N_FLAGS(x);
            Z_FLAGS(x);
            goto end;

        case LDY_A:
            addr = fetchword();
            if(addr >= 0x800 && addr < 0x2000)
            {
                char div = addr / 0x800;
                addr -= (0x800 * div);
            }
            if(addr == 0x2007 && !BADREAD && !PALETTE)
            {
                ppu.ppuread();
                BADREAD = 1;
                N_FLAGS(y);
                Z_FLAGS(y);
                goto end;
            }
            y = RAM[addr];
            if(addr == 0x2002)
            {
                RAM[0x2002] &= 127;
                toggle = false;
            }
            if(addr == 0x2004) SPRREAD = 1;
            if(addr == 0x2007) ppu.ppuread();
            if(addr == 0x4016) JOYREAD = 1;
            N_FLAGS(y);
            Z_FLAGS(y);
            goto end;

        case LDA_A:
            addr = fetchword();
            if(addr >= 0x800 && addr < 0x2000)
            {
                char div = addr / 0x800;
                addr -= (0x800 * div);
            }
            if(addr == 0x2007 && !BADREAD && !PALETTE)
            {
                ppu.ppuread();
                BADREAD = 1;
                N_FLAGS(a);
                Z_FLAGS(a);
                goto end;
            }
            a = RAM[addr];
            if(addr == 0x2002)
            {
                RAM[addr] &= 127;
                toggle = false;
            }
            else if(addr == 0x2004) SPRREAD = 1;
            else if(addr == 0x2007) ppu.ppuread();
            else if(addr == 0x4016) JOYREAD = 1;
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case LDX_A:
            addr = fetchword();
            if(addr >= 0x800 && addr < 0x2000)
            {
                char div = addr / 0x800;
                addr -= (0x800 * div);
            }
            if(addr == 0x2007 && !BADREAD && !PALETTE)
            {
                ppu.ppuread();
                BADREAD = 1;
                N_FLAGS(x);
                Z_FLAGS(x);
                goto end;
            }
            x = RAM[addr];
            if(addr == 0x2002)
            {
                RAM[0x2002] &= 127;
                toggle = false;
            }
            if(addr == 0x2004) SPRREAD = 1;
            if(addr == 0x2007) ppu.ppuread();
            if(addr == 0x4016) JOYREAD = 1;
            N_FLAGS(x);
            Z_FLAGS(x);
            goto end;

        case BCS:
            change = (char)fetchbyte();
            if(P & 1) PC += change;
            goto end;

        case LDA_IY:
            byte = fetchbyte();
            a = RAM[((RAM[byte+1] << 8) | RAM[byte]) + y];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case LDY_ZX:
            y = RAM[(fetchbyte() + x) & 0xFF];
            N_FLAGS(y);
            Z_FLAGS(y);
            goto end;

        case LDA_ZX:
            a = RAM[(fetchbyte() + x) & 0xFF];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case LDX_ZY:
            x = RAM[(fetchbyte() + y) & 0xFF];
            N_FLAGS(x);
            Z_FLAGS(x);
            goto end;

        case CLV:
            PC++;
            P &= 191;
            goto end;

        case LDA_AY:
            a = RAM[(fetchword() + y) & 0xFFFF];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case TSX:
            PC++;
            x = S;
            N_FLAGS(x);
            Z_FLAGS(x);
            goto end;

        case LDY_AX:
            y = RAM[(fetchword() + x) & 0xFFFF];
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        case LDA_AX:
            addr = fetchword() + x;
            a = RAM[addr];
            if(addr == 0x4016) JOYREAD = 1;
            Z_FLAGS(a);
            N_FLAGS(a);
            goto end;

        case LDX_AY:
            x = RAM[(fetchword() + y) & 0xFFFF];
            N_FLAGS(x);
            Z_FLAGS(x);
            goto end;

        case CPY_I:
            byte = fetchbyte();
            result = y - byte;
            if(y >= byte) P |= 1;
            else P &= 254;
            N_FLAGS(result);
            Z_FLAGS(result);
            goto end;

        case CMP_IX:
            byte = fetchbyte() + x;
            byte = RAM[(RAM[byte+1] << 8) | RAM[byte]];
            goto cmp;
            goto end;

        case CPY_Z:
            byte = RAM[fetchbyte()];
            result = y - byte;
            if(y >= byte) P |= 1;
            else P &= 254;
            N_FLAGS(result);
            Z_FLAGS(result);
            goto end;

        case CMP_Z:
            byte = RAM[fetchbyte()];
            goto cmp;
            goto end;
        
        case DEC_Z:
            addr = fetchbyte();
            RAM[addr]--;
            Z_FLAGS(RAM[addr]);
            N_FLAGS(RAM[addr]);
            goto end;

        case INY:
            PC++;
            y++;
            N_FLAGS(y);
            Z_FLAGS(y);
            goto end;

        case CMP_I:
            byte = fetchbyte();
            goto cmp;
            goto end;

        case DEX:
            PC++;
            x--;
            N_FLAGS(x);
            Z_FLAGS(x);
            goto end;

        case CPY_A:
            byte = RAM[fetchword()];
            result = y - byte;
            if(y >= byte) P |= 1;
            else P &= 254;
            N_FLAGS(result);
            Z_FLAGS(result);
            goto end;

        case CMP_A:
            byte = RAM[fetchword()];
            goto cmp;
            goto end;

        case DEC_A:
            addr = fetchword();
            RAM[addr]--;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case CMP_IY:
            byte = fetchbyte();
            byte = RAM[(RAM[byte+1] << 8) | RAM[byte]];
            goto cmp;
            goto end;

        case CMP_ZX:
            byte = RAM[(fetchbyte() + x) & 0xFF];
            goto cmp;
            goto end;

        case DEC_ZX:
            addr = fetchbyte() + x;
            RAM[addr]--;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case CLD:
            PC++;
            P &= 247;
            goto end;

        case CMP_AY:
            byte = RAM[(fetchword() + y) & 0xFFFF];
            goto cmp;
            goto end;

        case CMP_AX:
            byte = RAM[(fetchword() + x) & 0xFFFF];
            goto cmp;
            goto end;

        case DEC_AX:
            addr = fetchword() + x;
            RAM[addr]--;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case CPX_I:
            byte = fetchbyte();
            result = x - byte;
            if(x >= byte) P |= 1;
            else P &= 254;
            N_FLAGS(result);
            Z_FLAGS(result);
            goto end;

        case SBC_IX:
            byte = fetchbyte() + x;
            byte = RAM[(RAM[byte+1] << 8) | RAM[byte]];
            goto sbc;
            goto end;

        case CPX_Z:
            byte = RAM[fetchbyte()];
            result = x - byte;
            if(x >= byte) P |= 1;
            else P &= 254;
            N_FLAGS(result);
            Z_FLAGS(result);
            goto end;

        case SBC_Z:
            byte = RAM[fetchbyte()];
            goto sbc;
            goto end;

        case INC_Z:
            addr = fetchbyte();
            RAM[addr]++;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case SBC_I:
            byte = fetchbyte();
            goto sbc;
            goto end;

        case NOP:
            PC++;
            goto end;

        case CPX_A:
            byte = RAM[fetchword()];
            result = x - byte;
            if(x >= byte) P |= 1;
            else P &= 254;
            N_FLAGS(result);
            Z_FLAGS(result);
            goto end;

        case SBC_A:
            byte = RAM[fetchword()];
            goto sbc;
            goto end;

        case INC_A:
            addr = fetchword();
            RAM[addr]++;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case BEQ:
            change = (char)fetchbyte();
            if(P & 2) PC += change;
            goto end;

        case SBC_IY:
            byte = fetchbyte();
            byte = RAM[((RAM[byte+1] << 8) | RAM[byte]) + y];
            goto sbc;
            goto end;

        case SBC_ZX:
            byte = RAM[(fetchbyte() + x) & 0xFF];
            goto sbc;
            goto end;

        case INC_ZX:
            addr = fetchbyte() + x;
            RAM[addr]++;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);
            goto end;

        case SED:
            PC++;
            P |= 8;
            goto end;

        case SBC_AY:
            byte = RAM[(fetchword() + y) & 0xFFFF];
            goto sbc;
            goto end;

        case SBC_AX:
            byte = RAM[(fetchword() + x) & 0xFFFF];
            goto sbc;
            goto end;

        case INC_AX:
            addr = fetchword() + x;
            RAM[addr]++;
            N_FLAGS(RAM[addr]);
            Z_FLAGS(RAM[addr]);;
            goto end;

        default:
            PC++;
            goto end;

        adc:
            result = a;    //used as temp var
            a += byte + (P & 1);
            if(result > a) P |= 1;
            else P &= 254;
            if(((result&128)&&(byte&128)&&(~a&128))||
                ((~result&128)&&(~byte&128)&&(a&128)))
                P |= 64;
            else P &= 0xBF;
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;

        cmp:
            result = a - byte;
            if(a >= byte) P |= 1;
            else P &= 254;
            N_FLAGS(result);
            Z_FLAGS(result);
            goto end;

        sbc:
            result = a;
            a = a - byte - (~P&1);
            if(a < result) P |= 1;
            else P &= 254;
            if(((result&128)&&(~byte&128)&&(~a&128))||
                ((~result&128)&&(byte&128)&&(a&128)))
                P |= 64;
            else P &= 0xBF;
            N_FLAGS(a);
            Z_FLAGS(a);
            goto end;
