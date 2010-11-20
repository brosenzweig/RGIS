#ifndef _NCGSTRING_H
#define _NCGSTRING_H

#include<NCstdlib.h>
#include<string.h>
#include<stdio.h>

int NCGstringEndPar(char *expr, int i);
// returns the position of the closing bracket in the string
// returns -1 if there is no closing bracket
// returns -2 if the string doesn't start with (,[, or {
char* NCGstringSubstr(char *str, int start, int end);
// returns a pointer to a new string of the elements from 'start' to 'end'
bool NCGstringUnStripch(char **expr,char ch);
// Adds 'ch' to the beginning and end of 'expr' returns true on success
bool NCGstringStripch(char **expr,char ch);
// Truncates a single leading and/or trailing 'ch' character.
// most often used to remove leading and trailing spaces, tabs or \n
// returns true if 'expr' is changed
bool NCGstringStripbr(char **expr);
// Truncates leading and trailing brackets
// returns true if 'expr' is changed
bool NCGstringMatch(char *a,int start, char *b);
// compares two strings starting from position 'start'
int NCGstringTokenize(char *text,char ***tokens,char tok);
// assumes tokens is a NULL pointer. Then takes 'text' and separates it
// into segments delimited by 'tok'. Returns the number of tokens.
NCGstate NCGstringReplace(char **input, int start, int end, char *newstr);
// Takes the 'input' string and removes the characters from 'start' to 'end'
// and replaces them with every character in 'newstr'. strlen(newstr) does
// NOT have to be 'end' - 'start'
#endif
