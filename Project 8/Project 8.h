class Project8
{
public:
    static bool running;
    static bool playing;
    static bool logging;
    static bool paused;
    static bool soundActive;
};

//------

void UpdateNES();
void StartNES();
void ResetNES();
void PauseNES();

extern unsigned char RAM[0x10000];
extern unsigned char VRAM[0x4000]; 
extern unsigned char SPRRAM[0x100];
