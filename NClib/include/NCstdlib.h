#ifndef _NCGSTDLIB_H
#define _NCGSTDLIB_H

#include<stdlib.h>
#include<NCcore.h>

#if defined(__cplusplus)
extern "C" {
#endif

void SetDebug();
bool GetDebug();

	#ifdef NCGDEBUG
	#include<malloc.h>
		extern int _StartSize;
		extern int _MaxSize;
		#define Dprint(arg,arg2,arg3) fprintf(arg,arg2,arg3)
		#define Dprint2(arg,arg2,arg3,arg4) fprintf(arg,arg2,arg3,arg4)
			// Used for printing info ONLY when 'DEBUG' is defined.
		#define initMemInfo() _StartSize = mallinfo().uordblks;
		void printMemInfo();
	#else
		#define Dprint(arg,arg2,arg3)
		#define Dprint2(arg,arg2,arg3,arg4)
		#define initMemInfo()
		#define printMemInfo()
	#endif

#if defined(__cplusplus)
}
#endif

#endif

