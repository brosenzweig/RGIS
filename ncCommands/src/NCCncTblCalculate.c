#include <NCmathEqtn.h>
#include <NCtable.h>
#include <stdio.h>

void do_help(char *progName)
{
	fprintf(stderr,"Usage: %s [OPTIONS] \"<expression>\"\n",progName);
	fprintf(stderr,"  Flags:\n");
	fprintf(stderr,"  -d,--debug                        => initiate debug output\n");
	fprintf(stderr,"  -l,--lisp                         => use more parentheses when echoing expression\n");
	fprintf(stderr,"  -h,--help                         => print this message\n");
	fprintf(stderr,"  -f,--file <FILENAME>              => specify a tab-delimited or NetCDF file to read data from\n");
	fprintf(stderr,"  -o,--output <FILENAME>            => specify the output file, which will always be ASCII tab-delimited!\n");
	fprintf(stderr,"    If -o is not supplied, then it will write back to the original NetCDF file,\n");
	fprintf(stderr,"    or to stdout as an ASCII tab-delimited table.\n");
	fprintf(stderr,"  -v,--variable <VARNAME>=<double>  => define a static variable to be used in the expression\n");
	fprintf(stderr,"  -r,--rename <COLNAME>             => give a name for results column will be put in (default: \"RESULT\")\n");
	fprintf(stderr,"  -e,--expression \"<expression>\"    => specify the equation to calculate\n");
	fprintf(stderr,"\nThe BNF for an expression is the following:\n");
	fprintf(stderr,"<expression>: <func><WS>(<WS><double><WS>) | <expression><WS><INEQ><WS><expression>\n");
	fprintf(stderr,"              <expression><WS><OP><WS><expression> | <double>\n");
	fprintf(stderr,"<func> = 'abs' | 'sin' | 'cos' | 'tan' | 'ln' | 'log' | 'floor' | 'ceil' |\n");
	fprintf(stderr,"         'asin' | 'acos' | 'atan'\n");
	fprintf(stderr,"<OP> = '+' | '-' | '*' | '/' | '^'\n");
	fprintf(stderr,"<INEQ> = '&&' | '||' | '<' | '<=' | '==' | '!=' | '<>' | '>=' | '>'\n");
	fprintf(stderr,"<WS> = ' '*\n");
	fprintf(stderr,"<double> = <int>*'.'<int>*\n");
	fprintf(stderr,"<int> = 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0\n");
	fprintf(stderr,"\nexample expressions: ");
	fprintf(stderr,"<expression>  = 2 * sin (2 + 3)\n");
	fprintf(stderr,"<expression> != 2 * sin(2 + 3) <=== invalid!\n");
	fprintf(stderr,"<expression>  = 2 * sin ( 2 + 3)\n");
	fprintf(stderr,"<expression>  = 2 * sin (2 + 3 )\n");
	fprintf(stderr,"<expression>  = 2 * sin (    2   +   3   )\n");
	fprintf(stderr,"<expression> != 2*sin(2+3)     <=== invalid!\n");
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
			NCGtable_t *tbl;
			NCGfield_t *outCol;
		};
	};
} Files_t;
	
void NCGcloseFile(Files_t *file)
{
	if(file->ncTYPE)
	{
		if(nc_close(file->ncid) != NC_NOERR)
			{ fprintf(stderr,"Error closing file: %s!\n",file->fname); abort(); }
		NCGtableClose(file->tbl);
		file->tbl = (NCGtable_t *) NULL;
		return;
	}
	else if((file->file == (FILE *) NULL) || (file->file == stdout)) return;
	else if(fclose(file->file) != 0) { fprintf(stderr,"Error closing file: %s!\n",file->fname); abort(); }
	free(file->curRow);
}

// *** MAIN

#define cleanup(ret) { if(tHead) delTree(tHead); if(iHead) delTreeI(iHead); NCGmathFreeVars(); \
	NCGcloseFile(&inFile); if ((outFile != (FILE *) NULL) && (outFile != stdout)) fclose(outFile); if(fieldname != (char *) NULL) free(fieldname); \
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

	if(argNum == 1) { do_help(NCGcmProgName(argv[0])); return (NCGsucceeded); }
	if ((argNum == 2) && (argv[1][0] == '-')) { if (NCGcmArgTest(argv[1],"-d","--debug")) SetDebug(); do_help(NCGcmProgName(argv[0])); return (NCGsucceeded); }
	inFile.fname = (char *) NULL;
	inFile.ncTYPE = false;
	inFile.file = (FILE *) NULL;
	inFile.curRow = (char *) NULL;
	inFile.row = (char **) NULL;
	inFile.numCol = 0;
	initMemInfo();
	for(argPos = 1; argPos < argNum;)
	{
		if (NCGcmArgTest(argv[argPos],"-d","--debug")) { SetDebug(); NCGcmArgShiftLeft(argPos,argv,argc); argNum--; continue; }
		if (NCGcmArgTest(argv[argPos],"-l","--lisp")) { setLisp(); NCGcmArgShiftLeft(argPos,argv,argc); argNum--; continue; }
		if (NCGcmArgTest(argv[argPos],"-h","--help")) { do_help(NCGcmProgName(argv[0])); cleanup(NCGsucceeded); }
		if (NCGcmArgTest(argv[argPos],"-t","--table"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if((inFile.ncTYPE != false) && (inFile.file != (FILE *) NULL)) { fprintf(stderr,"-t flag must precede -f flag!\n"); cleanup(NCGfailed); }
			tablename = argv[argPos];
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-f","--file"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if((inFile.ncTYPE != false) && (inFile.file != (FILE *) NULL)) { fprintf(stderr,"multiple -f flags!\n"); cleanup(NCGfailed); }
			inFile.fname = argv[argPos];
			if(nc_open(inFile.fname,NC_WRITE,&i) == NC_NOERR)
			{
				inFile.ncTYPE = true;
				inFile.ncid = i;
				if((inFile.tbl = NCGtableOpen(i,tablename)) == (NCGtable_t *) NULL)
					{ fprintf(stderr,"Error encountered!\n"); cleanup(NCGfailed); }
			} else if((inFile.file = fopen(inFile.fname,"r")) != (FILE *) NULL)
			{
				inFile.ncTYPE = false;
				inFile.curRow = (char *) NULL;
				getline(&(inFile.curRow),&i,inFile.file);
				if (inFile.curRow == (char *) NULL) { fprintf(stderr,"Empty File '%s'",inFile.fname); cleanup(NCGfailed); }
				else { i = strlen(inFile.curRow); (inFile.curRow)[i - 1] = '\0'; }
				inFile.row = (char **) NULL;
				inFile.numCol = NCGstringTokenize(inFile.curRow, &(inFile.row),'\t');
			} else { fprintf(stderr,"Cannot open file '%s'!\n",inFile.fname); cleanup(NCGfailed); }
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-o","--output"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if(strcmp(argv[argPos],"-") == 0) outFile = stdout;
			else if ((outFile = fopen(argv[argPos],"w")) == (FILE *) NULL)
				{ fprintf(stderr,"Error opening file '%s' for writing!\n",argv[argPos]); cleanup(NCGfailed); }
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		argPos++;
	}
	if((inFile.ncTYPE == false) && (outFile == (FILE *) NULL)) outFile = stdout;
	for(argPos = 1; argPos < argNum;) {
		if (NCGcmArgTest(argv[argPos],"-v","--variable"))
		{ // constant variable
			if(NCGstringMatch(argv[argPos + 1],0,"VAR")) { fprintf(stderr,"Variable names cannot start with 'VAR'!\n"); cleanup(NCGfailed); }
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			i = strlen(argv[argPos]);
			for(j = 0; (j < i) && (argv[argPos][j] != '='); j++);
			name = NCGstringSubstr(argv[argPos],0,j - 1);
			tmp = NCGstringSubstr(argv[argPos],j + 1,i);
			if((k = NCGmathAddVar(-1,name,false)) == NCGfailed) cleanup(NCGfailed);
			if(NCGmathIsNumber(tmp)) NCGmathSetVarVal(k,atof(tmp));
			else { fprintf(stderr,"%s is not a <double>!\n",tmp); cleanup(NCGfailed); }
			free(tmp);
			free(name);
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-e","--expression"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if(input != (char *) NULL) { fprintf(stderr,"Expression defined twice!\n"); cleanup(NCGfailed); }
			if((input = malloc((strlen(argv[argPos]) + 1) * sizeof(char))) == NULL)
				{ perror("Memory Allocation error in: NCGtblCalculate ()\n"); cleanup(NCGfailed); }
			strcpy(input,argv[argPos]);
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest(argv[argPos],"-r","--rename"))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			if(fieldname == (char *) NULL)
			{
				if((fieldname = malloc(sizeof(char) * (strlen(argv[argPos]) + 1))) == NULL)
					{ perror("Memory Allocation error in: NCGtblCalculate ()\n"); cleanup(NCGfailed); }
				strcpy(fieldname,argv[argPos]);
			}
			else { fprintf(stderr,"Output field name defined twice!\n"); cleanup(NCGfailed); }
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ fprintf(stderr,"Unknown option: %s!\n",argv[argPos]); cleanup(NCGfailed); }
		// if nothing else it must be an equation
		if(input == (char *) NULL) {
			if((input = malloc((strlen(argv[argPos]) + 1) * sizeof(char))) == NULL)
			 { perror("Memory Allocation error in: NCGtblCalculate ()\n"); cleanup(NCGfailed); }
			strcpy(input,argv[argPos]);
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		fprintf(stderr,"Unknown option: %s!\n",argv[argPos]);
		cleanup(NCGfailed);
//		argPos++;
	}

	if(GetDebug()) fprintf(stderr,"Checking expression...\n");
	if(input == (char *) NULL)
	{
		fprintf(stderr,"Missing <expression>\n\n");
		do_help(NCGcmProgName(argv[0]));
		cleanup(NCGfailed);
/*		fprintf(stderr,"\n<expression> =? ");
		getline(&input,&inLen,stdin);
		cons++;
		if(GetDebug()) Dprint(stderr,"Main(): malloc(%p)\n",input);
		input[strlen(input) - 1] = '\0';*/
	}
	if(strcmp(input,"") == 0) { printf("Nothing to do\n"); cleanup(NCGsucceeded); }

	while(NCGstringStripch(&input, ' ') || NCGstringStripbr(&input));
	i = 0;
	if(fieldname == (char *) NULL)
	{
		if((fieldname = malloc(sizeof(char) * (strlen("Results") + 1))) == NULL)
			{ perror("Memory Allocation error in: NCGtblCalculate ()\n"); cleanup(NCGfailed); }
		strcpy(fieldname,"Results");
	}
	while(i < strlen(input))
	{
		if(input[i] != '[') i++;
		else
		{
			j = NCGstringEndPar(input,i);
			tmp = NCGstringSubstr(input,i + 1,j - 1);
			if(inFile.ncTYPE) {
				colnum = -1;
				for(k = 0; k < (inFile.tbl)->NFields; k++)
					if(strcmp(((inFile.tbl)->Fields)[k].Name,tmp) == 0)
					{
						if(colnum != -1)
							{ fprintf(stderr,"Multiple matches for column '%s'!\n",tmp); free(tmp); free(input); cleanup(NCGfailed); }
						colnum = k;
					}
				if(colnum == -1) { 
					if(!NCGstringUnStripch(&tmp,'"')) { free(tmp); free(input); cleanup(NCGfailed); }
					for(k = 0; k < (inFile.tbl)->NFields; k++)
						if(strcmp(((inFile.tbl)->Fields)[k].Name,tmp) == 0)
						{
							if(colnum != -1)
								{ printf("Multiple matches for column '%s'!\n",tmp); free(tmp); free(input); cleanup(NCGfailed); }
							colnum = k;
						}
					if(colnum == -1) { fprintf(stderr,"Missing column '%s'!",tmp); free(tmp); free(input); cleanup(NCGfailed); }
				}
			} else if(inFile.file == (FILE *) NULL) { fprintf(stderr,"No input file given!\n"); cleanup(NCGfailed);
			} else
			{
				colnum = -1;
				for(k = 0; k < inFile.numCol; k++)
					if(strcmp((inFile.row)[k],tmp) == 0)
					{
						if(colnum != -1)
							{ fprintf(stderr,"Multiple matches for column '%s'!\n",tmp); free(tmp); free(input); cleanup(NCGfailed); }
						colnum = k;
					}
				if(colnum == -1)
				{
					if(!NCGstringUnStripch(&tmp,'"')) { free(tmp); free(input); cleanup(NCGfailed); }
					for(k = 0; k < inFile.numCol; k++)
						if(strcmp((inFile.row)[k],tmp) == 0)
						{
							if(colnum != -1)
								{ fprintf(stderr,"Multiple matches for column '%s'!\n",tmp); free(tmp); free(input); cleanup(NCGfailed); }
							colnum = k;
						}
					if(colnum == -1) { fprintf(stderr,"Missing column '%s'!",tmp); free(tmp); free(input); cleanup(NCGfailed); }
				}
			}
			free(tmp);
			if((name = malloc(sizeof(char) * 6)) == NULL)
				{ perror("Memory Allocation error in: NCGtblCalculate ()\n"); cleanup(NCGfailed); }
			strcpy(name,"VAR");
			if((tmp = malloc(sizeof(char) * 3)) == NULL) { perror("Memory Allocation error in: NCGtblCalculate ()\n"); cleanup(NCGfailed); }
			sprintf(tmp,"%.2d",NCGmathGetVarNum() + 1);
			tmp[2] = '\0';
			strcat(name,tmp);
			free(tmp);
			name[5] = '\0';
			NCGstringReplace(&input,i,j+1,name);
			if(NCGmathAddVar(colnum,name,true) == NCGfailed) cleanup(NCGfailed);
			free(name);
			i = i + 6;
		}
	}
	if(GetDebug())
	{
		fprintf(stderr,"echo: '%s' strlen=%d\n",input,strlen(input));
		for(i = 0; i < NCGmathGetVarNum(); i++)
		{
			if(NCGmathGetVarVary(i)) fprintf(stderr,"%s = colnum(%d)\n",NCGmathGetVarName(i),NCGmathGetVarColNum(i));
			else fprintf(stderr,"%s = %f\n",NCGmathGetVarName(i),NCGmathGetVarVal(i));
		}
	}

	if(inFile.ncTYPE) {
		if(isIneq(input,&i)) inFile.outCol = NCGtableAddField (inFile.tbl,fieldname,NC_BYTE);
		else inFile.outCol = NCGtableAddField (inFile.tbl,fieldname,NC_DOUBLE);
	} else if(inFile.file == (FILE *) NULL) fprintf(outFile,"\"%s\"\n",fieldname);
	else fprintf(outFile,"%s\t\"%s\"\n",inFile.curRow,fieldname);
	i = 0;
	if(isIneq(input,&i))
	{
		iHead = mkTreeI(input);
		NCGmathEqtnFixTreeI(iHead);
		if(GetDebug()) { fprintf(stderr,"\nUsing equation: ("); printInorderI(iHead,stderr); fprintf(stderr,")\n"); }
	} else
	{
		tHead = mkTree(input);
		NCGmathEqtnFixTree(&tHead);
		if(GetDebug()) { fprintf(stderr,"\nUsing equation: ("); printInorder(tHead,stderr); fprintf(stderr,")\n"); }
	}

	if(inFile.ncTYPE)
	{
		if (inFile.tbl->NFields > 0)
			for(i = 0; i < inFile.tbl->Fields [0].NRecords; i++)
			{ 
				for(j = 0; j < NCGmathGetVarNum(); j++)
					if (NCGfieldGetFloat (inFile.tbl->Fields + NCGmathGetVarColNum(j),i,&output)) break;
					else NCGmathSetVarVal(j,output);
				if(iHead)
				{
						  if(j != NCGmathGetVarNum()) NCGfieldSetFill (inFile.outCol,i);
						  else NCGfieldSetInt (inFile.outCol,i,(int) CalculateI(iHead));
				}
				else
				{
						  if(j != NCGmathGetVarNum()) NCGfieldSetFill (inFile.outCol,i);
						  else NCGfieldSetFloat (inFile.outCol,i,Calculate(tHead));
				}
			}
		if(outFile == (FILE *) NULL) NCGtableCommit(inFile.ncid,inFile.tbl);
		else NCGtableExportAscii(inFile.tbl,outFile);
	}
	else if((inFile.ncTYPE == false) && (inFile.file == (FILE *) NULL))
	{
		if(iHead) { if(CalculateI(iHead)) fprintf(outFile,"true\n"); else fprintf(outFile,"false\n"); }
		else
		{
			output = Calculate(tHead);
			if(NCGmathEqualValues(output,(double) ((int) output))) fprintf(outFile,"%d\n",(int) output);
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
			if (getline(&(inFile.curRow),&i,inFile.file) == -1) break;
//			if (inFile.curRow == (char *) NULL) { fprintf(stderr,"Unexpected end of file!"); cleanup(NCGfailed); }
			else
			{
				i = strlen(inFile.curRow);
				(inFile.curRow)[i - 1] = '\0';
			}
			inFile.numCol = NCGstringTokenize(inFile.curRow, &(inFile.row),'\t');
			for(i = 0; i < NCGmathGetVarNum(); i++)
			{
				if((inFile.row)[NCGmathGetVarColNum(i)] == (char *) NULL) NCGmathSetVarVal(i,FLOAT_NOVALUE);
				else NCGmathSetVarVal(i,atof((inFile.row)[NCGmathGetVarColNum(i)]));
			}
			if(iHead)
			{
				if(GetDebug()) { printInorderI(iHead,stderr); fprintf(stderr," => "); }
				if(CalculateI(iHead)) fprintf(outFile,"%s\ttrue\n",inFile.curRow);
				else fprintf(outFile,"%s\tfalse\n",inFile.curRow);
			}
			else
			{
				if(GetDebug()) { printInorder(tHead,stderr); fprintf(stderr," => "); }
				output = Calculate(tHead);
				if(NCGmathEqualValues(output,(double) ((int) output))) fprintf(outFile,"%s\t%d\n",inFile.curRow,(int) output);
				else fprintf(outFile,"%s\t%f\n",inFile.curRow,output);
			}
		}
	}
	cleanup(NCGsucceeded);
}
