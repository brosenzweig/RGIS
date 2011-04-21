#include <cm.h>
#include <NCstdlib.h>

static bool _NCdebug = false;

void SetDebug() { _NCdebug = true; }
bool GetDebug() { return (_NCdebug); }

#ifdef NCDEBUG

int _StartSize = -1;
int _MaxSize = 0;

void printMemInfo() {
	if (_StartSize == -1) { CMmsgPrint (CMmsgDebug, "No previous call to initMemInfo()!"); return; }
	CMmsgPrint (CMmsgDebug, "Max Memory used: %d Current memory: ",_MaxSize);
	CMmsgPrint (CMmsgDebug, "uordblks=%d - _StartSize=%d = %d",mallinfo().uordblks,_StartSize,mallinfo().uordblks - _StartSize);
}

#endif
