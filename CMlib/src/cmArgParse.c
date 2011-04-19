/******************************************************************************

GHAAS Command Line Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

cmArgParse.c

balazs.fekete@unh.edu

*******************************************************************************/
#include <string.h>
#include <cm.h>

int CMargShiftLeft (int argPos, char *argList [], int argNum) {
	int i;
	char *tmp;

	tmp = argList [argPos];
	for (i = argPos + 1;i < argNum;++i) argList [i - 1] = argList [i];
	argList [i - 1] = tmp;
	return (argNum - 1);
}

int CMoptLookup (const char *choices [], const char *option, bool matchLength) {
	int i;

	if ((choices == (const char **) NULL) || (option == (const char *) NULL)) return (CMfailed);

	if (matchLength) {
		for (i = 0;choices [i] != (char *) NULL;++i)
			if (strcmp (choices [i],option) == 0) return (i);
	}
	else {
		for (i = 0;choices [i] != (char *) NULL;++i)
			if (strncmp (choices [i],option,strlen (choices [i])) == 0) return (i);
	}
	return (CMfailed);
}

void CMoptPrintList (CMmsgType msgType, const char *optName, const char *choices []) {
	int i;
	CMmsgPrint (msgType,"Help [%s] options:",optName);
	for (i = 0;choices [i] != (char *) NULL;++i) CMmsgPrint (msgType," %s",choices [i]);
	CMmsgPrint (msgType,"\n");
}

const char *CMprgName (const char *arg0) {
	int i;
	for (i = strlen (arg0) - 1;i > 0;i--) if (arg0 [i] == '/') {i++; break; }
	return (arg0 + i);
}
