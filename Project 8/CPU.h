class CPU 
{
public:
    unsigned short NMI;
    bool inNMI;
    //These cause problems with the globals
    //unsigned char A, X, Y, P, S;
    //unsigned short PC;
    
    void StartCPU();
    void RunCPU(int cycles);
};