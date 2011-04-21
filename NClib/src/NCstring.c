#include <cm.h>
#include <NCstring.h>

int NCstringEndPar(char *expr, int i)
{ // returns the number of the char in array of the other side of the parentheses
	int b1num = 0, b2num = 0, b3num = 0, strLen = strlen(expr);
	bool changed = true;
	switch(expr[i])
	{
		case '(': b1num++; break;
		case '[': b2num++; break;
		case '{': b3num++; break;
		case ')': b1num--; break;
		case ']': b2num--; break;
		case '}': b3num--; break;
		default: changed = false; break;
	}
	if(!changed) return -2; // if ith char is not a parenthese
	if(b1num >= 0 && b2num >= 0 && b3num >= 0) {
		for(i++;i < strLen && (b1num != 0 || b2num != 0 || b3num != 0); i++)
		{
			switch(expr[i])
			{
				case '(': b1num++; break;
				case '[': b2num++; break;
				case '{': b3num++; break;
				case ')': b1num--; break;
				case ']': b2num--; break;
				case '}': b3num--; break;
				default: break;
			}
		}
		if(b1num == 0 && b2num == 0 && b3num == 0) return i - 1;
	} else {
		for(i--;i >= 0 && (b1num != 0 || b2num != 0 || b3num != 0); i--)
		{
			switch(expr[i])
			{
				case '(': b1num++; break;
				case '[': b2num++; break;
				case '{': b3num++; break;
				case ')': b1num--; break;
				case ']': b2num--; break;
				case '}': b3num--; break;
				default: break;
			}
		}
		if(b1num == 0 && b2num == 0 && b3num == 0) return i + 1;
	}
	if(b1num > 0) CMmsgPrint (CMmsgUsrError, "NCstringEndPar(): Unmatched '(' in: '%s'",expr);
	if(b2num > 0) CMmsgPrint (CMmsgUsrError, "NCstringEndPar(): Unmatched '[' in: '%s'",expr);
	if(b3num > 0) CMmsgPrint (CMmsgUsrError, "NCstringEndPar(): Unmatched '{' in: '%s'",expr);
	if(b1num < 0) CMmsgPrint (CMmsgUsrError, "NCstringEndPar(): Unmatched ')' in: '%s'",expr);
	if(b2num < 0) CMmsgPrint (CMmsgUsrError, "NCstringEndPar(): Unmatched ']' in: '%s'",expr);
	if(b3num < 0) CMmsgPrint (CMmsgUsrError, "NCstringEndPar(): Unmatched '}' in: '%s'",expr);
	return -1;
}

char* NCstringSubstr(char *str, int s, int e)
{
	char *ret = (char *) NULL;
	int i = 0,strLen = strlen(str);
	if((s < 0) || (strLen < e) || (s > e))
	{
		CMmsgPrint (CMmsgUsrError, "NCstringSubstr(): WARN: substr('%s' strlen=%d ,%d,%d) called!",str,strLen,s,e);
		return (char *) NULL;
	}
//	CMmsgPrint (CMmsgUsrError, "malloc(%d - %d + 2 = %d)\n",e,s,e - s + 2);
	if((ret = malloc((e - s + 2) * sizeof(char))) == (char *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return (char *) NULL; }
	for(i = 0;s <= e;s++,i++) ret[i] = str[s];
	ret[i] = '\0';
	e = i;
	return ret;
}

bool NCstringUnStripch(char **expr, char ch)
{
	register int k = strlen(*expr) + 2;
	if((*expr = realloc(*expr,sizeof(char) * (k + 1))) == (char *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory Allocation error in: %s %d",__FILE__,__LINE__); return false; }
	(*expr)[k--] = '\0';
	(*expr)[k--] = ch;
	for(;k != 0; k--) (*expr)[k] = (*expr)[k-1];
	(*expr)[0] = ch;
	return true;
}

bool NCstringStripch(char **expr, char ch)
{
	register int i = 0,j = 0;
	int strLen = strlen(*expr);
	char *new;
	if(((*expr)[0] != ch) && ((*expr)[strLen - 1] != ch)) return false;
	while((i < strLen) && ((*expr)[i] == ch)) i++;
	j = strLen - 1;
	while((j >= i) && ((*expr)[j] == ch)) j--;
	new = NCstringSubstr(*expr,i,j);
	free(*expr);
	*expr = new;
	return true;
}

bool NCstringStripbr(char **expr)
{
	int strLen = strlen(*expr) - 1, end = NCstringEndPar(*expr,0); // makes (*expr)[end] == ')'
	char *new;
	if(strLen != end) return false;
	switch ((*expr)[0])
	{
		case '(': if((*expr)[end] != ')') return false; break;
		case '[': if((*expr)[end] != ']') return false; break;
		case '{': if((*expr)[end] != '}') return false; break;
		default: break;
	}
	new = NCstringSubstr(*expr,1,end - 1);
	free(*expr);
	*expr = new;
	return true;
}

bool NCstringMatch(char *a,int o, char *b)
{
	register int i;
	int blen = strlen(b);
	if((strlen(a) == 0) || (blen == 0))
	{
		CMmsgPrint (CMmsgUsrError, "NCstringMatch(): BAD string passed!");
		return false;
	}
	for(i = 0; i < blen; i++) if(a[i+o] != b[i]) return false;
	return true;
}

int NCstringTokenize(char *text,char ***tokens,char tok)
{
	register int pos = 0;
	int lpos = 0,strLen = strlen(text), numtokens = 0;

	(*tokens) = (char **) NULL;
	while (pos < strLen)
	{
		(*tokens) = realloc((*tokens),sizeof(char *) * (++numtokens));
		while ((pos < strLen) && (text[pos] != tok)) pos++;
		if(lpos == pos) (*tokens)[numtokens - 1] = (char *) NULL;
		else (*tokens)[numtokens - 1] = NCstringSubstr(text,lpos,pos - 1);
//		CMmsgPrint (CMmsgUsrError, "substr(%s,%d,%d)\n",text,lpos,pos - 1);
		lpos = ++pos;
	}
	return numtokens;
}

NCstate NCstringReplace(char **input, int s, int e, char *newstr) {
	int i,j;
	char *ret;
	if(e < s) { CMmsgPrint (CMmsgUsrError, "NCstringReplace(): end is less than start!\n"); abort(); }
	if((ret = malloc(sizeof(char) * ((strlen(*input) - (e - s)) + strlen(newstr) + 1))) == (char *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return NCfailed; }
	for (i = 0,j = 0; j < s; i++,j++) ret[i] = (*input)[j];
	s = strlen(newstr);
	for (j = 0; j < s; i++,j++) ret[i] = newstr[j];
	s = strlen(*input);
	for (j = e; j < s; i++,j++) ret[i] = (*input)[j];
	ret[i] = '\0';
	free(*input);
	*input = ret;
	return NCsucceeded;
}
