void InitMemory();
void CopyPRG(char *, char);

//Stack functions
void pushw(unsigned short word);
void pushb(unsigned char byte);
unsigned short popw();
unsigned char popb();
