/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

MFConfig.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>

int MFLoadConfig (char *fileName, int (*func) (const char *,const char *)) {
	FILE *fp;
	char *buffer = (char *) NULL, *name, *content;
	int size = 0, lNum = 0, i, len;

	if ((fp = fopen (fileName,"r")) == (FILE *) NULL) {
		CMmsgPrint (CMmsgUsrError, "Configuration file [%s] opening error!\n", fileName);
		return (CMfailed);
	}
	while ((buffer = CMbufGetLine (buffer,&size,fp)) != (char *) NULL) {
		lNum ++;
		buffer = CMbufTrim (buffer);
		len    = strlen (buffer);
		if (len == 0)          continue;
		if (buffer [0] == '#') continue;
		for (i = 0; i < len; ++i) if (buffer [i] == '=') break;
		if (i == len) { CMmsgPrint (CMmsgWarning,"Skipping line [%s:%d %s]\n",fileName,lNum, buffer); continue; }
		buffer [i] = '\0';
		name = CMbufTrim (buffer);
		content = CMbufStripDQuotes (CMbufStripSQuotes (CMbufTrim (buffer + i + 1)));
		len = strlen (content);
		for (i = 0;i < len;++i) if (content [i] == '#') break;
		if (i < len) { content [i] = '\0'; content = CMbufTrim (buffer); }
		if (func (name, content) != CMsucceeded) return (CMfailed);
	}
	return (CMsucceeded);
}
