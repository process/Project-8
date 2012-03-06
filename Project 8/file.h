struct ROMINFO
{
    static char id[4];
    static char prg;
    static char vram;
    static char info1;
    static char info2;
    static char ram; //possibly unused?
    static char info3;
    static char reserved[6];
};

void OpenFile(char * filepath);
void ReadData(HANDLE, void*, int);
char GetRomInfo(HANDLE);