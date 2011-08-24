#include <string.h>
#include <stdio.h>
#include <cm.h>
#include <NCmathEqtn.h>
#include <NCtable.h>
#include <NCstring.h>

static void doHelp(const char *progName)
{
	CMmsgPrint (CMmsgUsrError, "Usage: %s [OPTIONS] \"<expression>\"",progName);
	CMmsgPrint (CMmsgUsrError, "  Flags:");
	CMmsgPrint (CMmsgUsrError, "  -d,--debug                        => initiate debug output");
	CMmsgPrint (CMmsgUsrError, "  -l,--lisp                         => use more parentheses when echoing expression");
	CMmsgPrint (CMmsgUsrError, "  -h,--help                         => print this message");
	CMmsgPrint (CMmsgUsrError, "  -f,--file <FILENAME>              => specify a tab-delimited or NetCDF file to read data from");
	CMmsgPrint (CMmsgUsrError, "  -o,--output <FILENAME>            => specify the output file, which will always be ASCII tab-delimited!");
	CMmsgPrint (CMmsgUsrError, "    If -o is not supplied, then it will write back to the original NetCDF file,");
	CMmsgPrint (CMmsgUsrError, "    or to stdout as an ASCII tab-delimited table.");
	CMmsgPrint (CMmsgUsrError, "  -v,--variable <VARNAME>=<double>  => define a static variable to be used in the expression");
	CMmsgPrint (CMmsgUsrError, "  -r,--rename <COLNAME>             => give a name for results column will be put in (default: \"RESULT\")");
	CMmsgPrint (CMmsgUsrError, "  -e,--expression \"<expression>\"    => specify the equation to calculate");
	CMmsgPrint (CMmsgUsrError, "The BNF for an expression is the following:");
	CMmsgPrint (CMmsgUsrError, "<expression>: <func><WS>(<WS><double><WS>) | <expression><WS><INEQ><WS><expression>");
	CMmsgPrint (CMmsgUsrError, "              <expression><WS><OP><WS><expression> | <double>");
	CMmsgPrint (CMmsgUsrError, "<func> = 'abs' | 'sin' | 'cos' | 'tan' | 'ln' | 'log' | 'floor' | 'ceil' |");
	CMmsgPrint (CMmsgUsrError, "         'asin' | 'acos' | 'atan'");
	CMmsgPrint (CMmsgUsrError, "<OP> = '+' | '-' | '*' | '/' | '^'");
	CMmsgPrint (CMmsgUsrError, "<INEQ> = '&&' | '||' | '<' | '<=' | '==' | '!=' | '<>' | '>=' | '>'");
	CMmsgPrint (CMmsgUsrError, "<WS> = ' '*");
	CMmsgPrint (CMmsgUsrError, "<double> = <int>*'.'<int>*");
	CMmsgPrint (CMmsgUsrError, "<int> = 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0");
	CMmsgPrint (CMmsgUsrError, "example expressions: ");
	CMmsgPrint (CMmsgUsrError, "<expression>  = 2 * sin (2 + 3)");
	CMmsgPrint (CMmsgUsrError, "<expression> != 2 * sin(2 + 3) <=== invalid!");
	CMmsgPrint (CMmsgUsrError, "<expression>  = 2 * sin ( 2 + 3)");
	CMmsgPrint (CMmsgUsrError, "<expression>  = 2 * sin (2 + 3 )");
	CMmsgPrint (CMmsgUsrError, "<expression>  = 2 * sin (    2   +   3   )");
	CMmsgPrint (CMmsgUsrError, "<expression> != 2*sin(2+3)     <=== invalid!");
}

typedef struct Files
{
	char *fname;
	bool ncTYPE;
	union
	{
		struct // if (!ncTYPE)
		{
			FILE *file;
			char *curRow;
			char **row;
			int numCol;
		};
		struct // if (ncTYPE)
		{
			int ncid;
			NCtable_t *tbl;
			NCfield_t *outCol;
		};
	};
} Files_t;
	
void NCcloseFile(Files_t *file)
{
	if(file->ncTYPE)
	{
		if(nc_close(file->ncid) != NC_NOERR)
			{ CMmsgPrint (CMmsgUsrError, "Error closing file: %s!",file->fname); abort(); }
		NCtableClose(file->tbl);
		file->tbl = (NCtable_t *) NULL;
		return;
	}
	else if((file->file == (FILE *) NULL) || (file->file == stdout)) return;
	else if(fclose(file->file) != 0) { CMmsgPrint (CMmsgUsrError, "Error closing file: %s!",file->fname); abort(); }
	free(file->curRow);
}

// *** MAIN

#define cleanup(ret) { if(tHead) delTree(tHead); if(iHead) delTreeI(iHead); NCmathFreeVars(); \
	NCcloseFile(&inFile); if ((outFile != (FILE *) NULL) && (outFile != stdout)) fclose(outFile); if(fieldname != (char *) NULL) free(fieldname); \
	printMemInfo(); return ret; }

int main(int argc, char* argv[])
{
	TreeNode_t *tHead = NULL;
	IneqNode_t *iHead = NULL;
	Files_t inFile;
	FILE *outFile = (FILE *) NULL;
	char *input = (char *) NULL, *fieldname = (char *) NULL, *tmp = (char *) NULL, *tablename = "time", *name = (char *) NULL;
	int i = 1, j, k, colnum;
	int argPos = 0, argNum = argc;
	double output = 0.0;

	if(argNum == 1) { doHelp(CMprgName(argv[0])); return (NCsucceeded); }
	if ((argNum == 2) && (argv[1][0] == '-')) { if (CMargTest(argv[1],"-d","--debug")) SetDebug(); doHelp(CMprgName(argv[0])); return (NCsucceeded); }
	inFile.fname = (char *) NULL;
	inFile.ncTYPE = false;
	inFile.file = (FILE *) NULL;
	inFile.curRow = (char *) NULL;
	inFile.row = (char **) NULL;
	inFile.numCol = 0;
	initMemInfo();
	for(argPos = 1; argPos < argNum;)
	{
		if (CMargTest(argv[argPos],"-d","--debug")) { SetDebug(); if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos) break; else continue; }
		if (CMargTest(argv[argPos],"-l","--lisp"))  { setLisp();  if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos) break; else continue; }
		if (CMargTest(argv[argPos],"-h","--help"))  { doHelp(CMprgName(argv[0])); cleanup (CMsucceeded); }
		if (CMargTest(argv[argPos],"-t","--table"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing table!"); cleanup(CMfailed); }
			if((inFile.ncTYPE != false) && (inFile.file != (FILE *) NULL))
				{ CMmsgPrint (CMmsgUsrError, "-t flag must precede -f flag!"); cleanup(CMfailed); }
			tablename = argv[argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-f","--file"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing file!"); cleanup(CMfailed); }
			if((inFile.ncTYPE != false) && (inFile.file != (FILE *) NULL)) { CMmsgPrint (CMmsgUsrError, "multiple -f flags!"); cleanup(CMfailed); }
			inFile.fname = argv[argPos];
			if(nc_open(inFile.fname,NC_WRITE,&i) == NC_NOERR)
				{
				inFile.ncTYPE = true;
				inFile.ncid = i;
				if((inFile.tbl = NCtableOpen(i,tablename)) == (NCtable_t *) NULL)
					{ CMmsgPrint (CMmsgUsrError, "Error encountered!"); cleanup(CMfailed); }
				}
			else if((inFile.file = fopen(inFile.fname,"r")) != (FILE *) NULL)
				{
				inFile.ncTYPE = false;
				inFile.curRow = (char *) NULL;
				inFile.curRow = CMbufGetLine(inFile.curRow,&i,inFile.file);
				if (inFile.curRow == (char *) NULL) { CMmsgPrint (CMmsgUsrError, "Empty File '%s'",inFile.fname); cleanup(CMfailed); }
				else { i = strlen(inFile.curRow); (inFile.curRow)[i - 1] = '\0'; }
				inFile.row = (char **) NULL;
				inFile.numCol = NCstringTokenize(inFile.curRow, &(inFile.row),'\t');
				}
			else { CMmsgPrint (CMmsgUsrError, "Cannot open file '%s'!",inFile.fname); cleanup(CMfailed); }
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos) break;
			continue;
		}
		if (CMargTest(argv[argPos],"-o","--output"))
		{
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing output!"); cleanup(CMfailed); }
			if(strcmp(argv[argPos],"-") == 0) outFile = stdout;
			else if ((outFile = fopen(argv[argPos],"w")) == (FILE *) NULL)
				{ CMmsgPrint (CMmsgUsrError, "Error opening file '%s' for writing!",argv[argPos]); cleanup(CMfailed); }
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos) break;
			continue;
		}
		argPos++;
	}
	if((inFile.ncTYPE == false) && (outFile == (FILE *) NULL)) outFile = stdout;
	for(argPos = 1; argPos < argNum;) {
		if (CMargTest(argv[argPos],"-v","--variable"))
		{ // constant variable
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing variable!"); cleanup(CMfailed); }
			if(NCstringMatch(argv[argPos],0,"VAR"))
				{ CMmsgPrint (CMmsgUsrError, "Variable names cannot start with 'VAR'!"); cleanup(CMfailed); }
			i = strlen(argv[argPos]);
			for(j = 0; (j < i) && (argv[argPos][j] != '='); j++);
			name = NCstringSubstr(argv[argPos],0,j - 1);
			tmp = NCstringSubstr(argv[argPos],j + 1,i);
			if((k = NCmathAddVar(-1,name,false)) == CMfailed) cleanup(CMfailed);
			if(NCmathIsNumber(tmp)) NCmathSetVarVal(k,atof(tmp));
			else { CMmsgPrint (CMmsgUsrError, "%s is not a <double>!",tmp); cleanup(CMfailed); }
			free(tmp);
			free(name);
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos) break;
			continue;
		}
		if (CMargTest(argv[argPos],"-e","--expression"))
		{
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing expression!"); cleanup(CMfailed); }
			if(input != (char *) NULL) { CMmsgPrint (CMmsgUsrError, "Expression defined twice!"); cleanup(CMfailed); }
			if((input = malloc((strlen(argv[argPos]) + 1) * sizeof(char))) == NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory Allocation error in: %s %d",__FILE__,__LINE__); cleanup(CMfailed); }
			strcpy(input,argv[argPos]);
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos) break;
			continue;
		}
		if (CMargTest(argv[argPos],"-r","--rename"))
		{
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing name!"); cleanup(CMfailed); }
			if(fieldname == (char *) NULL)
			{
				if((fieldname = malloc(sizeof(char) * (strlen(argv[argPos]) + 1))) == NULL)
					{ CMmsgPrint (CMmsgSysError, "Memory Allocation error in: %s %d",__FILE__,__LINE__); cleanup(CMfailed); }
				strcpy(fieldname,argv[argPos]);
			}
			else { CMmsgPrint (CMmsgUsrError, "Output field name defined twice!"); cleanup(CMfailed); }
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos) break;
			continue;
		}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError, "Unknown option: %s!",argv[argPos]); cleanup(CMfailed); }
		// if nothing else it must be an equation
		if(input == (char *) NULL) {
			if((input = malloc((strlen(argv[argPos]) + 1) * sizeof(char))) == NULL)
			 { CMmsgPrint (CMmsgSysError, "Memory Allocation error in: %s %d",__FILE__,__LINE__); cleanup(CMfailed); }
			strcpy(input,argv[argPos]);
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos) break;
			continue;
		}
		CMmsgPrint (CMmsgUsrError, "Unknown option: %s!",argv[argPos]);
		cleanup(CMfailed);
//		argPos++;
	}

	if(GetDebug()) CMmsgPrint (CMmsgUsrError, "Checking expression...");
	if(input == (char *) NULL)
	{
		CMmsgPrint (CMmsgUsrError, "Missing <expression>");
		doHelp(CMprgName(argv[0]));
		cleanup(CMfailed);
/*		CMmsgPrint (CMmsgUsrError, "<expression> =? ");
		CMbufGetLine(&input,&inLen,stdin);
		cons++;
		if(GetDebug()) Dprint(stderr,"Main(): malloc(%p)",input);
		input[strlen(input) - 1] = '\0';*/
	}
	if(strcmp(input,"") == 0) { printf("Nothing to do"); cleanup(NCsucceeded); }

	while(NCstringStripch(&input, ' ') || NCstringStripbr(&input));
	i = 0;
	if(fieldname == (char *) NULL)
	{
		if((fieldname = malloc(sizeof(char) * (strlen("Results") + 1))) == NULL)
			{ CMmsgPrint (CMmsgSysError, "Memory Allocation error in: %s %d",__FILE__,__LINE__); cleanup(CMfailed); }
		strcpy(fieldname,"Results");
	}
	while(i < strlen(input))
	{
		if(input[i] != '[') i++;
		else
		{
			j = NCstringEndPar(input,i);
			tmp = NCstringSubstr(input,i + 1,j - 1);
			if(inFile.ncTYPE) {
				colnum = -1;
				for(k = 0; k < (inFile.tbl)->NFields; k++)
					if(strcmp(((inFile.tbl)->Fields)[k].Name,tmp) == 0)
					{
						if(colnum != -1)
							{ CMmsgPrint (CMmsgUsrError, "Multiple matches for column '%s'!",tmp); free(tmp); free(input); cleanup(CMfailed); }
						colnum = k;
					}
				if(colnum == -1) { 
					if(!NCstringUnStripch(&tmp,'"')) { free(tmp); free(input); cleanup(CMfailed); }
					for(k = 0; k < (inFile.tbl)->NFields; k++)
						if(strcmp(((inFile.tbl)->Fields)[k].Name,tmp) == 0)
						{
							if(colnum != -1)
								{ printf("Multiple matches for column '%s'!",tmp); free(tmp); free(input); cleanup(CMfailed); }
							colnum = k;
						}
					if(colnum == -1) { CMmsgPrint (CMmsgUsrError, "Missing column '%s'!",tmp); free(tmp); free(input); cleanup(CMfailed); }
				}
			} else if(inFile.file == (FILE *) NULL) { CMmsgPrint (CMmsgUsrError, "No input file given!"); cleanup(CMfailed);
			} else
			{
				colnum = -1;
				for(k = 0; k < inFile.numCol; k++)
					if(strcmp((inFile.row)[k],tmp) == 0)
					{
						if(colnum != -1)
							{ CMmsgPrint (CMmsgUsrError, "Multiple matches for column '%s'!",tmp); free(tmp); free(input); cleanup(CMfailed); }
						colnum = k;
					}
				if(colnum == -1)
				{
					if(!NCstringUnStripch(&tmp,'"')) { free(tmp); free(input); cleanup(CMfailed); }
					for(k = 0; k < inFile.numCol; k++)
						if(strcmp((inFile.row)[k],tmp) == 0)
						{
							if(colnum != -1)
								{ CMmsgPrint (CMmsgUsrError, "Multiple matches for column '%s'!",tmp); free(tmp); free(input); cleanup(CMfailed); }
							colnum = k;
						}
					if(colnum == -1) { CMmsgPrint (CMmsgUsrError, "Missing column '%s'!",tmp); free(tmp); free(input); cleanup(CMfailed); }
				}
			}
			free(tmp);
			if((name = malloc(sizeof(char) * 6)) == NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory Allocation error in: %s %d",__FILE__,__LINE__); cleanup(CMfailed); }
			strcpy(name,"VAR");
			if((tmp = malloc(sizeof(char) * 3)) == NULL) { CMmsgPrint (CMmsgSysError, "Memory Allocation error in: %s %d",__FILE__,__LINE__); cleanup(CMfailed); }
			sprintf(tmp,"%.2d",NCmathGetVarNum() + 1);
			tmp[2] = '\0';
			strcat(name,tmp);
			free(tmp);
			name[5] = '\0';
			NCstringReplace(&input,i,j+1,name);
			if(NCmathAddVar(colnum,name,true) == CMfailed) cleanup(CMfailed);
			free(name);
			i = i + 6;
		}
	}
	if(GetDebug())
	{
		CMmsgPrint (CMmsgUsrError, "echo: '%s' strlen=%d",input,strlen(input));
		for(i = 0; i < NCmathGetVarNum(); i++)
		{
			if(NCmathGetVarVary(i)) CMmsgPrint (CMmsgUsrError, "%s = colnum(%d)",NCmathGetVarName(i),NCmathGetVarColNum(i));
			else CMmsgPrint (CMmsgUsrError, "%s = %f",NCmathGetVarName(i),NCmathGetVarVal(i));
		}
	}

	if(inFile.ncTYPE) {
		if(isIneq(input,&i)) inFile.outCol = NCtableAddField (inFile.tbl,fieldname,NC_BYTE);
		else inFile.outCol = NCtableAddField (inFile.tbl,fieldname,NC_DOUBLE);
	} else if(inFile.file == (FILE *) NULL) fprintf(outFile,"\"%s\"\n",fieldname);
	else fprintf(outFile,"%s\t\"%s\"\n",inFile.curRow,fieldname);
	i = 0;
	if(isIneq(input,&i))
	{
		iHead = mkTreeI(input);
		NCmathEqtnFixTreeI(iHead);
		if(GetDebug()) { CMmsgPrint (CMmsgUsrError, "Using equation: ("); printInorderI(iHead,stderr); CMmsgPrint (CMmsgUsrError, ")"); }
	} else
	{
		tHead = mkTree(input);
		NCmathEqtnFixTree(&tHead);
		if(GetDebug()) { CMmsgPrint (CMmsgUsrError, "Using equation: ("); printInorder(tHead,stderr); CMmsgPrint (CMmsgUsrError, ")"); }
	}

	if(inFile.ncTYPE)
	{
		if (inFile.tbl->NFields > 0)
			for(i = 0; i < inFile.tbl->Fields [0].NRecords; i++)
			{ 
				for(j = 0; j < NCmathGetVarNum(); j++)
					if (NCfieldGetFloat (inFile.tbl->Fields + NCmathGetVarColNum(j),i,&output)) break;
					else NCmathSetVarVal(j,output);
				if(iHead)
				{
						  if(j != NCmathGetVarNum()) NCfieldSetFill (inFile.outCol,i);
						  else NCfieldSetInt (inFile.outCol,i,(int) CalculateI(iHead));
				}
				else
				{
						  if(j != NCmathGetVarNum()) NCfieldSetFill (inFile.outCol,i);
						  else NCfieldSetFloat (inFile.outCol,i,Calculate(tHead));
				}
			}
		if(outFile == (FILE *) NULL) NCtableCommit(inFile.ncid,inFile.tbl);
		else NCtableExportAscii(inFile.tbl,outFile);
	}
	else if((inFile.ncTYPE == false) && (inFile.file == (FILE *) NULL))
	{
		if(iHead) { if(CalculateI(iHead)) fprintf(outFile,"true\n"); else fprintf(outFile,"false\n"); }
		else
		{
			output = Calculate(tHead);
			if(NCmathEqualValues(output,(double) ((int) output))) fprintf(outFile,"%d\n",(int) output);
			else fprintf(outFile,"%f\n",output);
		}
	}
	else
	{
		while(true) {
			for(i = 0; i < inFile.numCol; i++) free((inFile.row)[i]);
			free(inFile.row);
			inFile.row = (char **) NULL;
			free(inFile.curRow);
			inFile.curRow = (char *) NULL;
			if ((inFile.curRow = CMbufGetLine(inFile.curRow,&i,inFile.file)) == (char *) NULL) break;
//			if (inFile.curRow == (char *) NULL) { CMmsgPrint (CMmsgUsrError, "Unexpected end of file!"); cleanup(CMfailed); }
			else
			{
				i = strlen(inFile.curRow);
				(inFile.curRow)[i - 1] = '\0';
			}
			inFile.numCol = NCstringTokenize(inFile.curRow, &(inFile.row),'\t');
			for(i = 0; i < NCmathGetVarNum(); i++)
			{
				if((inFile.row)[NCmathGetVarColNum(i)] == (char *) NULL) NCmathSetVarVal(i,FLOAT_NOVALUE);
				else NCmathSetVarVal(i,atof((inFile.row)[NCmathGetVarColNum(i)]));
			}
			if(iHead)
			{
				if(GetDebug()) { printInorderI(iHead,stderr); CMmsgPrint (CMmsgUsrError, " => "); }
				if(CalculateI(iHead)) fprintf(outFile,"%s\ttrue\n",inFile.curRow);
				else fprintf(outFile,"%s\tfalse\n",inFile.curRow);
			}
			else
			{
				if(GetDebug()) { printInorder(tHead,stderr); CMmsgPrint (CMmsgUsrError, " => "); }
				output = Calculate(tHead);
				if(NCmathEqualValues(output,(double) ((int) output))) fprintf(outFile,"%s\t%d\n",inFile.curRow,(int) output);
				else fprintf(outFile,"%s\t%f\n",inFile.curRow,output);
			}
		}
	}
	cleanup(NCsucceeded);
}
