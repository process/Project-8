class Debug
{
    static HWND debugHWND;
    static bool debugging;
    static unsigned short breakAddr;

public:
    static void makeDebugger();
    static bool checkBreakAddr(unsigned short addr);
    static void updateFPS();
    static void updateIPS();
    static void getBreakAddr();
    static void getPauseInfo();
    static void destroyDebugger();
};
