#include <cstring>

#include "Project 8.h"

#include "file.h"
#include "mem.h"

void OpenFile(char * filepath)
{
    HANDLE file = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if(GetRomInfo(file))
        return;

    InitMemory(ROMINFO.prg);

    char * data = (char*)malloc(0x4000);
    
    ReadData(file, data, 0x4000);
    CopyPRG(data, 1);    //copy same data twice

    if(ROMINFO.prg == 2)
    {
        ReadData(file, data, 0x4000); //additional PRG bank
        CopyPRG(data, 2);
    }

    ReadData(file, data, 0x2000);

    memcpy(VRAM, data, 0x2000);

    if(status.play)
        ResetNES();

    else
        status.play = 1;
    
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
    ReadData(file, &ROMINFO, 16);
    
    char NESid[4] = {'N','E','S',0x1A};
    if(memcmp(ROMINFO.id, NESid, 4))
    {
        MessageBox(NULL, "Invalid ROM file.", "Error", MB_OK);
        return 1;
    }

    return 0;
}
