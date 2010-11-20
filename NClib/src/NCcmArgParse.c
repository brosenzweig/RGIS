#include <NCcm.h>

void NCcmArgShiftLeft (int argPos, char *argList [], int argNum)

{
	int i;
	char *tmp;

	tmp = argList [argPos];
	for (i = argPos + 1;i < argNum;++i) argList [i - 1] = argList [i];
	argList [i - 1] = tmp;
}

int NCcmStringLookup (char *choices [], char *option, bool matchLength)

{
	int i, optLen, len;

	if ((choices == (char **) NULL) || (option == (char *) NULL)) return (NCfailed);

	if (matchLength)
	{
		for (i = 0;choices [i] != (char *) NULL;++i)
			if (strcmp (choices [i],option) == 0) return (i);
	}
	else
	{
		optLen = strlen (option);
		for (i = 0;choices [i] != (char *) NULL;++i)
		{
			len = strlen (choices [i]);
			if (strncmp (choices [i],option,optLen < len ? optLen : len) == 0) return (i);
		}
	}
	return (NCfailed);
}

char *NCcmProgName (char *arg0)

{
	int i;
	for (i = strlen (arg0) - 1;i > 0;i--) if (arg0 [i] == '/') {i++; break; }
	return (arg0 + i);
}
