#include "StandardBuffer.h"
#include "Windows.h"

class WinBuff : public StandardBuffer
{

private:

	HGLOBAL handle;
  VOID *data;

public:
	WinBuff();
	~WinBuff();

	short CreateBuffer(unsigned long Length);
	short GetDataPointer(void **data);
	short GetSize(unsigned long *size);
	short Lock();
	short Unlock();
	short DestroyBuffer();


};