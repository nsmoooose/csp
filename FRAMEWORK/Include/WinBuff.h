#include "Framework.h"
#include "Windows.h"

class WinBuff : public StandardBuffer
{

private:

  // members
	HGLOBAL    p_hHandle;
  void      *p_vpData;

public:
	WinBuff();
	~WinBuff();

  // functions
	short         Initialize(unsigned long Length);
	void*         GetDataPointer();
	unsigned long GetSize();
	void          Lock();
	void          Unlock();
	void          Uninitialize();


};