/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

DBMisc.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>

DBInt DBMiscTestWhiteChar (char c)

	{
	if (c == ' ')  return (true);
	if (c == '\n') return (true);
	if (c == '\t') return (true);
	if (c == '\r') return (true);
	return (false);
	}

void DBMiscTrimWhiteSpaces (char *str)

	{
	DBInt i, j, strLen;

	strLen = (int) strlen (str);
	for (i = strLen - 1;i >= 0;--i) if (DBMiscTestWhiteChar (str [i])) str [i] = '\0'; else break;

	strLen = (int) strlen (str);
	for (j = 0;j < strLen;++j) if (DBMiscTestWhiteChar (str [j]) == false) break;
	if (j > 0) { for (i = 0;i < strLen - j; ++i) str [i] = str [i + j]; str [i] = '\0'; }

	}

void DBMiscTrimBrackets (char *str)

	{
	DBInt i, bracketNum = 0, strLen = strlen (str);

	if ((str [0] != '(') || (str [strLen - 1] != ')')) return;

	for (i = 1 ;i < strLen - 1;++i)
		{
		if (str [i] == '(') bracketNum++;
		if ((bracketNum > 0) && str [i] == ')') bracketNum--;
		}
	if (bracketNum > 0) return;		

	for (i = 0;i < strLen - 2; ++i) str [i] = str [i + 1];
	str [i] = '\0';
	}

DBInt DBMiscTrimQuotes (char *str)

	{
	DBInt i, quoteNum = 0, strLen = strlen (str);

	DBMiscTrimWhiteSpaces (str);
	if (((str [0] != '\"') || (str [strLen - 1] != '\"')) &&
		 ((str [0] != '\'') || (str [strLen - 1] != '\''))) return (quoteNum);

	for (i = 0;i < strLen - 2; ++i) str [i] = str [i + 1];
	str [i] = '\0';
	return (DBMiscTrimQuotes (str) + 1);
	}

