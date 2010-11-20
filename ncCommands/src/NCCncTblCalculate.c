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
			{ fprintf(stderr,"Error closing file: %s!\n",file->fname); abort(); }
		NCtableClose(file->tbl);
		file->tbl = (NCtable_t *) NULL;
		return;
	}
	else if((file->file == (FILE *) NULL) || (file->file == stdout)) return;
	else if(fclose(file->file) != 0) { fprintf(stderr,"Error closing file: %s!\n",file->fname); abort(); }
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

	if(argNum == 1) { do_help(NCcmProgName(argv[0])); return (NCsucceeded); }
	if ((argNum == 2) && (argv[1][0] == '-')) { if (NCcmArgTest(argv[1],"-d","--debug")) SetDebug(); do_help(NCcmProgName(argv[0])); return (NCsucceeded); }
	inFile.fname = (char *) NULL;
	inFile.ncTYPE = false;
	inFile.file = (FILE *) NULL;
	inFile.curRow = (char *) NULL;
	inFile.row = (char **) NULL;
	inFile.numCol = 0;
	initMemInfo();
	for(argPos = 1; argPos < argNum;)
	{
		if (NCcmArgTest(argv[argPos],"-d","--debug")) { SetDebug(); NCcmArgShiftLeft(argPos,argv,argc); argNum--; continue; }
		if (NCcmArgTest(argv[argPos],"-l","--lisp")) { setLisp(); NCcmArgShiftLeft(argPos,argv,argc); argNum--; continue; }
		if (NCcmArgTest(argv[argPos],"-h","--help")) { do_help(NCcmProgName(argv[0])); cleanup(NCsucceeded); }
		if (NCcmArgTest(argv[argPos],"-t","--table"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if((inFile.ncTYPE != false) && (inFile.file != (FILE *) NULL)) { fprintf(stderr,"-t flag must precede -f flag!\n"); cleanup(NCfailed); }
			tablename = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-f","--file"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if((inFile.ncTYPE != false) && (inFile.file != (FILE *) NULL)) { fprintf(stderr,"multiple -f flags!\n"); cleanup(NCfailed); }
			inFile.fname = argv[argPos];
			if(nc_open(inFile.fname,NC_WRITE,&i) == NC_NOERR)
			{
				inFile.ncTYPE = true;
				inFile.ncid = i;
				if((inFile.tbl = NCtableOpen(i,tablename)) == (NCtable_t *) NULL)
					{ fprintf(stderr,"Error encountered!\n"); cleanup(NCfailed); }
			} else if((inFile.file = fopen(inFile.fname,"r")) != (FILE *) NULL)
			{
				inFile.ncTYPE = false;
				inFile.curRow = (char *) NULL;
				getline(&(inFile.curRow),&i,inFile.file);
				if (inFile.curRow == (char *) NULL) { fprintf(stderr,"Empty File '%s'",inFile.fname); cleanup(NCfailed); }
				else { i = strlen(inFile.curRow); (inFile.curRow)[i - 1] = '\0'; }
				inFile.row = (char **) NULL;
				inFile.numCol = NCstringTokenize(inFile.curRow, &(inFile.row),'\t');
			} else { fprintf(stderr,"Cannot open file '%s'!\n",inFile.fname); cleanup(NCfailed); }
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-o","--output"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if(strcmp(argv[argPos],"-") == 0) outFile = stdout;
			else if ((outFile = fopen(argv[argPos],"w")) == (FILE *) NULL)
				{ fprintf(stderr,"Error opening file '%s' for writing!\n",argv[argPos]); cleanup(NCfailed); }
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		argPos++;
	}
	if((inFile.ncTYPE == false) && (outFile == (FILE *) NULL)) outFile = stdout;
	for(argPos = 1; argPos < argNum;) {
		if (NCcmArgTest(argv[argPos],"-v","--variable"))
		{ // constant variable
			if(NCstringMatch(argv[argPos + 1],0,"VAR")) { fprintf(stderr,"Variable names cannot start with 'VAR'!\n"); cleanup(NCfailed); }
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			i = strlen(argv[argPos]);
			for(j = 0; (j < i) && (argv[argPos][j] != '='); j++);
			name = NCstringSubstr(argv[argPos],0,j - 1);
			tmp = NCstringSubstr(argv[argPos],j + 1,i);
			if((k = NCmathAddVar(-1,name,false)) == NCfailed) cleanup(NCfailed);
			if(NCmathIsNumber(tmp)) NCmathSetVarVal(k,atof(tmp));
			else { fprintf(stderr,"%s is not a <double>!\n",tmp); cleanup(NCfailed); }
			free(tmp);
			free(name);
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-e","--expression"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if(input != (char *) NULL) { fprintf(stderr,"Expression defined twice!\n"); cleanup(NCfailed); }
			if((input = malloc((strlen(argv[argPos]) + 1) * sizeof(char))) == NULL)
				{ perror("Memory Allocation error in: NCtblCalculate ()\n"); cleanup(NCfailed); }
			strcpy(input,argv[argPos]);
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-r","--rename"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if(fieldname == (char *) NULL)
			{
				if((fieldname = malloc(sizeof(char) * (strlen(argv[argPos]) + 1))) == NULL)
					{ perror("Memory Allocation error in: NCtblCalculate ()\n"); cleanup(NCfailed); }
				strcpy(fieldname,argv[argPos]);
			}
			else { fprintf(stderr,"Output field name defined twice!\n"); cleanup(NCfailed); }
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ fprintf(stderr,"Unknown option: %s!\n",argv[argPos]); cleanup(NCfailed); }
		// if nothing else it must be an equation
		if(input == (char *) NULL) {
			if((input = malloc((strlen(argv[argPos]) + 1) * sizeof(char))) == NULL)
			 { perror("Memory Allocation error in: NCtblCalculate ()\n"); cleanup(NCfailed); }
			strcpy(input,argv[argPos]);
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		fprintf(stderr,"Unknown option: %s!\n",argv[argPos]);
		cleanup(NCfailed);
//		argPos++;
	}

	if(GetDebug()) fprintf(stderr,"Checking expression...\n");
	if(input == (char *) NULL)
	{
		fprintf(stderr,"Missing <expression>\n\n");
		do_help(NCcmProgName(argv[0]));
		cleanup(NCfailed);
/*		fprintf(stderr,"\n<expression> =? ");
		getline(&input,&inLen,stdin);
		cons++;
		if(GetDebug()) Dprint(stderr,"Main(): malloc(%p)\n",input);
		input[strlen(input) - 1] = '\0';*/
	}
	if(strcmp(input,"") == 0) { printf("Nothing to do\n"); cleanup(NCsucceeded); }

	while(NCstringStripch(&input, ' ') || NCstringStripbr(&input));
	i = 0;
	if(fieldname == (char *) NULL)
	{
		if((fieldname = malloc(sizeof(char) * (strlen("Results") + 1))) == NULL)
			{ perror("Memory Allocation error in: NCtblCalculate ()\n"); cleanup(NCfailed); }
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
							{ fprintf(stderr,"Multiple matches for column '%s'!\n",tmp); free(tmp); free(input); cleanup(NCfailed); }
						colnum = k;
					}
				if(colnum == -1) { 
					if(!NCstringUnStripch(&tmp,'"')) { free(tmp); free(input); cleanup(NCfailed); }
					for(k = 0; k < (inFile.tbl)->NFields; k++)
						if(strcmp(((inFile.tbl)->Fields)[k].Name,tmp) == 0)
						{
							if(colnum != -1)
								{ printf("Multiple matches for column '%s'!\n",tmp); free(tmp); free(input); cleanup(NCfailed); }
							colnum = k;
						}
					if(colnum == -1) { fprintf(stderr,"Missing column '%s'!",tmp); free(tmp); free(input); cleanup(NCfailed); }
				}
			} else if(inFile.file == (FILE *) NULL) { fprintf(stderr,"No input file given!\n"); cleanup(NCfailed);
			} else
			{
				colnum = -1;
				for(k = 0; k < inFile.numCol; k++)
					if(strcmp((inFile.row)[k],tmp) == 0)
					{
						if(colnum != -1)
							{ fprintf(stderr,"Multiple matches for column '%s'!\n",tmp); free(tmp); free(input); cleanup(NCfailed); }
						colnum = k;
					}
				if(colnum == -1)
				{
					if(!NCstringUnStripch(&tmp,'"')) { free(tmp); free(input); cleanup(NCfailed); }
					for(k = 0; k < inFile.numCol; k++)
						if(strcmp((inFile.row)[k],tmp) == 0)
						{
							if(colnum != -1)
								{ fprintf(stderr,"Multiple matches for column '%s'!\n",tmp); free(tmp); free(input); cleanup(NCfailed); }
							colnum = k;
						}
					if(colnum == -1) { fprintf(stderr,"Missing column '%s'!",tmp); free(tmp); free(input); cleanup(NCfailed); }
				}
			}
			free(tmp);
			if((name = malloc(sizeof(char) * 6)) == NULL)
				{ perror("Memory Allocation error in: NCtblCalculate ()\n"); cleanup(NCfailed); }
			strcpy(name,"VAR");
			if((tmp = malloc(sizeof(char) * 3)) == NULL) { perror("Memory Allocation error in: NCtblCalculate ()\n"); cleanup(NCfailed); }
			sprintf(tmp,"%.2d",NCmathGetVarNum() + 1);
			tmp[2] = '\0';
			strcat(name,tmp);
			free(tmp);
			name[5] = '\0';
			NCstringReplace(&input,i,j+1,name);
			if(NCmathAddVar(colnum,name,true) == NCfailed) cleanup(NCfailed);
			free(name);
			i = i + 6;
		}
	}
	if(GetDebug())
	{
		fprintf(stderr,"echo: '%s' strlen=%d\n",input,strlen(input));
		for(i = 0; i < NCmathGetVarNum(); i++)
		{
			if(NCmathGetVarVary(i)) fprintf(stderr,"%s = colnum(%d)\n",NCmathGetVarName(i),NCmathGetVarColNum(i));
			else fprintf(stderr,"%s = %f\n",NCmathGetVarName(i),NCmathGetVarVal(i));
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
		if(GetDebug()) { fprintf(stderr,"\nUsing equation: ("); printInorderI(iHead,stderr); fprintf(stderr,")\n"); }
	} else
	{
		tHead = mkTree(input);
		NCmathEqtnFixTree(&tHead);
		if(GetDebug()) { fprintf(stderr,"\nUsing equation: ("); printInorder(tHead,stderr); fprintf(stderr,")\n"); }
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
			if (getline(&(inFile.curRow),&i,inFile.file) == -1) break;
//			if (inFile.curRow == (char *) NULL) { fprintf(stderr,"Unexpected end of file!"); cleanup(NCfailed); }
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
				if(GetDebug()) { printInorderI(iHead,stderr); fprintf(stderr," => "); }
				if(CalculateI(iHead)) fprintf(outFile,"%s\ttrue\n",inFile.curRow);
				else fprintf(outFile,"%s\tfalse\n",inFile.curRow);
			}
			else
			{
				if(GetDebug()) { printInorder(tHead,stderr); fprintf(stderr," => "); }
				output = Calculate(tHead);
				if(NCmathEqualValues(output,(double) ((int) output))) fprintf(outFile,"%s\t%d\n",inFile.curRow,(int) output);
				else fprintf(outFile,"%s\t%f\n",inFile.curRow,output);
			}
		}
	}
	cleanup(NCsucceeded);
}
