#include <cm.h>
#include <NCtable.h>

void do_help(char *progName,bool extend)
{
	if(extend) {
		CMmsgPrint (CMmsgUsrError, "Usage: %s [OPTIONS] ",progName);
		CMmsgPrint (CMmsgUsrError, "  Flags:");
		CMmsgPrint (CMmsgUsrError, "  -d,--debug             => initiate debug output");
		CMmsgPrint (CMmsgUsrError, "  -f,--file [filename]   => set input file name");
		CMmsgPrint (CMmsgUsrError, "  -h,--help              => print this message");
		CMmsgPrint (CMmsgUsrError, "  -o,--output [filename] => set output file name [default: stdout]");
		CMmsgPrint (CMmsgUsrError, "  -t,--table [tablename] => table name [default: time]");
	} else {
		CMmsgPrint (CMmsgUsrError, "Usage: %s [OPTIONS] ",progName);
		CMmsgPrint (CMmsgUsrError, "  Flags:");
		CMmsgPrint (CMmsgUsrError, "  -d,--debug");
		CMmsgPrint (CMmsgUsrError, "  -f,--file [filename]");
		CMmsgPrint (CMmsgUsrError, "  -h,--help");
		CMmsgPrint (CMmsgUsrError, "  -o,--output [filename]");
		CMmsgPrint (CMmsgUsrError, "  -t,--table");
	}
}

// *** MAIN

#define cleanup(ret) if((fname != (char *) NULL) && (nc_close(ncid) != NC_NOERR)) CMmsgPrint (CMmsgUsrError, "Error closing file!"); \
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
			if(output != stdout) { CMmsgPrint (CMmsgUsrError, "Output file defined twice!"); cleanup(NCfailed); }
			if((output = fopen(argv[argPos],"w")) == (FILE *) NULL)
				{ CMmsgPrint (CMmsgUsrError, "Cannot open for writing: %s",argv[argPos]); cleanup(NCfailed); }
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError, "Unknown option: %s!",argv[argPos]); cleanup(NCfailed); }
		argPos++;
	}
	if(fname == (char *) NULL) { do_help(argv[0],false); CMmsgPrint (CMmsgUsrError, "No file specified!"); cleanup(NCfailed); }
	if(nc_open(fname,NC_NOWRITE,&ncid) != NC_NOERR) { CMmsgPrint (CMmsgUsrError, "Error opening file!"); cleanup(NCfailed); }
	if((tbl = NCtableOpen(ncid,tname)) == (NCtable_t *) NULL) { CMmsgPrint (CMmsgUsrError, "Error opening table!"); cleanup(NCfailed); }
	if(GetDebug()) CMmsgPrint (CMmsgUsrError, "Loaded file!");
	NCtableExportAscii(tbl,output);
	cleanup(NCsucceeded);
}
