void StartPPU();
void UpdatePPU(unsigned int pixels);
void PPUCleanUp();

void PPUaddress();
void PPUscroll();
void PPUwrite();
void PPUread();
void PPUsprDMA();
void PPUspraddr();
void PPUsprread();
void PPUsprwrite();
void PPU2000write();

void DrawPixels(unsigned short howMany, unsigned short Pos);
void ContinueDraw(unsigned int pixels);
