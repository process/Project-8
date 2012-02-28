typedef int * SOUNDBUFFER;

void InitDSound();
void KillDSound();
SOUNDBUFFER CreateBuffer();
void WriteBuffer(int * DestBuf, short * SrcBuf, int datalen);
void PlayBuffer(int * Buffer);
void StopBuffer(int * Buffer);
void ReleaseBuffer(int * Buffer);
