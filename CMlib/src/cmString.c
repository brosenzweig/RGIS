#include <string.h>
#include <stdlib.h>
#include <cm.h>

char *CMstrAppend    (char *string, const char *append, const char *separator) {
	size_t separatorLen, stringLen;

	if (append == (char *) NULL) return (string);

	separatorLen = separator == (char *) NULL ? 0 : strlen (separator);
	stringLen    = string    == (char *) NULL ? 0 : strlen (string);
	if ((string = (char *) realloc (string, stringLen + separatorLen + strlen (append) + 1)) == (char *) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d\n",__FILE__,__LINE__);
		return ((char *) NULL);
	}
	if (separatorLen > 0) strcpy (string + stringLen, separator);
	strcpy (string + stringLen + separatorLen,append);
	return (string);
}

char *CMstrDuplicate (const char *inString)
{
	char *outString;

	if (inString == (const char *) NULL) return ((char *) NULL);
	if ((outString = (char *) malloc (strlen (inString) + 1)) == (char *) NULL) {
	   CMmsgPrint (CMmsgSysError, "Error: Memory allocation in: %s:%d\n",__FILE__,__LINE__);
		perror ("Perror:");
	   return ((char *) NULL);
	}
	strcpy (outString,inString);
	return (outString);
}
