#include <cm.h>
#include <sys/types.h>
#include <regex.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <NCcore.h>

static regex_t _NCfileListRegex;

static int _NCfileFilter (const struct dirent *dirent)
{
	regmatch_t pmatch;
	return (regexec (&_NCfileListRegex,dirent->d_name,1, &pmatch, REG_NOTBOL | REG_NOTEOL) == REG_NOMATCH ? false : true);
}

char **NCfileList (const char *template, size_t *n)
{
	bool regexIsSet = false;
	int i, j;
	size_t strLength, ndots = 0, ndirents = 0;
	struct dirent **dirents = (struct dirent **) NULL;
	char *path, *pattern, *allocPath = (char *) NULL, **fileList = (char **) NULL;

	strLength = strlen (template);
	for (i = 0;i < strLength;++i)  if (template [i] == '.') ndots++;
	if ((allocPath = (char *) malloc (strLength + ndots + 1)) == (char *) NULL)
	{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); goto ABORT; }
	strcpy (allocPath,template);
	for (i = 0;i < strLength + ndots;++i)
		if (allocPath [i] == '.')
		{
			for (j = strLength + ndots - 1;j > i;--j) allocPath [j] = allocPath [j - 1];
			allocPath [i] = '\\';
			i++;
		}
		else if (allocPath [i] == '?') allocPath [i] = '.';
	allocPath [i] = '\0';

	for (i = strLength;i >= 0;i--) if (allocPath [i] == '/') break;
	if (allocPath [i] == '/')
	{ path = allocPath; pattern = allocPath + i + 1; allocPath [i] = '\0'; }
	else { path = "./";      pattern = allocPath; }

	if (regcomp (&_NCfileListRegex,pattern,REG_EXTENDED | REG_NOSUB) != 0)
	{ CMmsgPrint (CMmsgAppError, "Regular expression error in: %s %d",__FILE__,__LINE__); goto ABORT; }
	regexIsSet = true;	

	if ((ndirents = scandir (path,&dirents,_NCfileFilter,alphasort)) < 0)
	{ CMmsgPrint (CMmsgSysError, "Directory scanning error in: %s %d",__FILE__,__LINE__); goto ABORT; }
	if (ndirents < 1)
	{ CMmsgPrint (CMmsgUsrError, "No file is matching pattern [%s]",template); goto ABORT; }
	if ((fileList = (char **) calloc (ndirents,sizeof (char *))) == (char **) NULL)
	{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); goto ABORT; }
	for (i = 0;i < ndirents;++i) fileList [i] = (char *) NULL;
	strLength = strlen (path) + 1;
	for (i = 0;i < ndirents;++i)
	{
		if ((fileList [i] = (char *) malloc (strLength + strlen (dirents [i]->d_name) + 1)) == (char *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: NCfileList ()"); goto ABORT; }
		sprintf (fileList [i],"%s/%s",path,dirents [i]->d_name);
	}

	free (allocPath);
	for (i = 0;i < ndirents;++i) free (dirents [i]);
	if (dirents != (struct dirent **) NULL) free (dirents);
	if (regexIsSet) regfree (&_NCfileListRegex);
	*n = ndirents;
	return (fileList);

ABORT:
	if (allocPath != (char *)  NULL) free (allocPath);
	if (fileList  != (char **) NULL) { for ( ;i >= 0;--i) free (fileList [i]); free (fileList); }
	if (dirents   != (struct dirent **) NULL)
	{ for (i = 0 ;i < ndirents;++i) free (fileList [i]); free (fileList); }
	if (regexIsSet) regfree (&_NCfileListRegex);
	*n = 0;
	return ((char **) NULL);
}

void NCfileListFree (char **fileList, size_t n)
{
	int i;

	if (fileList  == (char **) NULL) return;
	for (i = 0 ;i < n;++i) if (fileList [i] != (char *) NULL) free (fileList [i]);
	free (fileList);
}
