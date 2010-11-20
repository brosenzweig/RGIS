#include<NCstdlib.h>

static bool _NCGdebug = false;

void SetDebug() { _NCGdebug = true; }
bool GetDebug() { return (_NCGdebug); }

#ifdef NCGDEBUG

int _StartSize = -1;
int _MaxSize = 0;

void printMemInfo() {
	if (_StartSize == -1) { fprintf(stderr,"No previous call to initMemInfo()!\n"); return; }
	fprintf(stderr,"Max Memory used: %d Current memory: ",_MaxSize);
	fprintf(stderr,"uordblks=%d - _StartSize=%d = %d\n",mallinfo().uordblks,_StartSize,mallinfo().uordblks - _StartSize);
}

#endif
