class CPU
{

private:

public:

    void joypad();

};

void CPU::joypad()
{
    static bool strobe = false;

    if(JOYREAD)
    {
        currbit++;
        JOYREAD = 0;
    }

    if(JOYWRITE)
    {
        if(strobe)
        {
            if(RAM[0x4016] == 0)
            {
                currbit = 0;
                strobe = false;
            }
        }

        else if(RAM[0x4016] == 1)
            strobe = true;

        else
            strobe = false;

        JOYWRITE = 0;
    }

    if(currbit < 8)
        RAM[0x4016] = 0x40 | ((JOYPAD >> currbit) & 1);
    else 
        RAM[0x4016] = 0x41;
}