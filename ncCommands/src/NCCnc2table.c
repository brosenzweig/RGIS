#include <NCtable.h>

void do_help(char *progName,bool extend)
{
	if(extend) {
		fprintf(stderr,"Usage: %s [OPTIONS] \n",progName);
		fprintf(stderr,"  Flags:\n");
		fprintf(stderr,"  -d,--debug             => initiate debug output\n");
		fprintf(stderr,"  -f,--file [filename]   => set input file name\n");
		fprintf(stderr,"  -h,--help              => print this message\n");
		fprintf(stderr,"  -o,--output [filename] => set output file name [default: stdout]\n");
		fprintf(stderr,"  -t,--table [tablename] => table name [default: time]\n");
	} else {
		fprintf(stderr,"Usage: %s [OPTIONS] \n",progName);
		fprintf(stderr,"  Flags:\n");
		fprintf(stderr,"  -d,--debug\n");
		fprintf(stderr,"  -f,--file [filename]\n");
		fprintf(stderr,"  -h,--help\n");
		fprintf(stderr,"  -o,--output [filename]\n");
		fprintf(stderr,"  -t,--table\n");
	}
}

// *** MAIN

#define cleanup(ret) if((fname != (char *) NULL) && (nc_close(ncid) != NC_NOERR)) fprintf(stderr,"Error closing file!\n"); \
	NCtableClose(tbl); if(output != stdout) fclose(output); printMemInfo(); return ret;

int main(int argc, char* argv[])
{
	int argPos = 0, argNum = argc, ncid;
	char *fname = (char *) NULL, *tname = "time";
	FILE *output = stdout;
	NCtable_t *tbl = (NCtable_t *) NULL;

	initMemInfo();
	for(argPos = 1; argPos < argNum;) {
		if (NCcmArgTest(argv[argPos],"-d","--debug")) { SetDebug(); NCcmArgShiftLeft(argPos,argv,argc); argNum--; continue; }
		if (NCcmArgTest(argv[argPos],"-h","--help")) {
			if((argPos + 1 < argNum) && (argv[argPos+1][0] == 'e')) do_help(NCcmProgName(argv[0]),true);
			else do_help(NCcmProgName(argv[0]),false);
			cleanup(NCsucceeded);
		}
		if (NCcmArgTest(argv[argPos],"-t","--table"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			tname = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-f","--file"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			fname = argv[argPos];
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest(argv[argPos],"-o","--output"))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			if(output != stdout) { fprintf(stderr,"Output file defined twice!\n"); cleanup(NCfailed); }
			if((output = fopen(argv[argPos],"w")) == (FILE *) NULL)
				{ fprintf(stderr,"Cannot open for writing: %s\n",argv[argPos]); cleanup(NCfailed); }
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ fprintf(stderr,"Unknown option: %s!\n",argv[argPos]); cleanup(NCfailed); }
		argPos++;
	}
	if(fname == (char *) NULL) { do_help(argv[0],false); fprintf(stderr,"\nNo file specified!\n"); cleanup(NCfailed); }
	if(nc_open(fname,NC_NOWRITE,&ncid) != NC_NOERR) { fprintf (stderr,"Error opening file!\n"); cleanup(NCfailed); }
	if((tbl = NCtableOpen(ncid,tname)) == (NCtable_t *) NULL) { fprintf(stderr,"Error opening table!\n"); cleanup(NCfailed); }
	if(GetDebug()) fprintf(stderr,"Loaded file!\n");
	NCtableExportAscii(tbl,output);
	cleanup(NCsucceeded);
}
