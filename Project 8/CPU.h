class CPU 
{
public: //Change this later!
    void pushw(unsigned short word);
    void pushb(unsigned char byte);
    unsigned short popw();
    unsigned char popb();

//public:
    unsigned short NMI;
    bool inNMI;
    //These cause problems with the globals
    //unsigned char A, X, Y, P, S;
    //unsigned short PC;
    
    void StartCPU();
    void RunCPU(int cycles);
};