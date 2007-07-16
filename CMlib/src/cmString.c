#include <string.h>
#include <stdlib.h>
#include <cm.h>

char *CMstrDuplicate (const char *inString)
{
	char *outString;

	if ((outString = (char *) malloc (strlen (inString) + 1)) == (char *) NULL) {
	   CMmsgPrint (CMmsgSysError, "Error: Memory allocation in: %s:%d\n",__FILE__,__LINE__);
		perror ("Perror:");
	   return ((char *) NULL);
	}
	strcpy (outString,inString);
	return (outString);
}
