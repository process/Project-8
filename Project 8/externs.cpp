unsigned char RAM[0x10000];  //64 kb
unsigned char VRAM[0x4000];  //16 kb
unsigned char SPRRAM[0x100]; //256 bytes

struct ROMINFO
{
    char id[4];
    char prg;
    char vram;
    char info1;
    char info2;
    char ram; //possibly unused?
    char info3;
    char reserved[6];
}ROMINFO;
