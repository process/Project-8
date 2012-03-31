#include <cstring>
#include <Windows.h>

#include "Project 8.h"

#include "file.h"
#include "mem.h"

//Initialize globals
char ROMINFO::id[4];
char ROMINFO::prg;
char ROMINFO::vram;
char ROMINFO::info1;
char ROMINFO::info2;
char ROMINFO::ram; //possibly unused?
char ROMINFO::info3;
char ROMINFO::reserved[6];

void OpenFile(char * filepath)
{
    HANDLE file = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if(GetRomInfo(file))
        return;

    InitMemory();

    char * data = (char*)malloc(0x4000);
    
    ReadData(file, data, 0x4000);
    CopyPRG(data, 1);    //copy same data twice

    if(ROMINFO::prg == 2)
    {
        ReadData(file, data, 0x4000); //additional PRG bank
        CopyPRG(data, 2);
    }

    ReadData(file, data, 0x2000);

    memcpy(VRAM, data, 0x2000);

    if(Project8::playing)
        ResetNES();

    else
        Project8::playing = false;
    
    StartNES();

    free(data);
    CloseHandle(file);
}

void ReadData(HANDLE file, void * data, int bytes)
{
    data = (char*)data;
    DWORD br; //bytes read
    ReadFile(file, data, bytes, &br, NULL);
}

char GetRomInfo(HANDLE file)
{
    ReadData(file, ROMINFO::id, 4);
    ReadData(file, &ROMINFO::prg, 1);
    ReadData(file, &ROMINFO::vram, 1);
    ReadData(file, &ROMINFO::info1, 1);
    ReadData(file, &ROMINFO::info2, 1);
    ReadData(file, &ROMINFO::ram, 1);
    ReadData(file, &ROMINFO::info3, 1);
    ReadData(file, ROMINFO::reserved, 6);
    
    char NESid[4] = {'N','E','S',0x1A};
    if(memcmp(ROMINFO::id, NESid, 4))
    {
        MessageBox(NULL, "Invalid ROM file.", "Error", MB_OK);
        return 1;
    }

    return 0;
}
