#include <cm.h>
#include <NC.h> // or use NCmath.h if math libraries needed

void do_help(char *progName,bool extend)
{
	if(extend) {
		CMmsgPrint (CMmsgUsrError, "Usage: %s [OPTIONS] ",progName);
		CMmsgPrint (CMmsgUsrError, "  Flags:");
		CMmsgPrint (CMmsgUsrError, "  -d,--debug                     => initiate debug output");
		CMmsgPrint (CMmsgUsrError, "  -h,--help                      => print this message");
	} else {
		CMmsgPrint (CMmsgUsrError, "Usage: %s [OPTIONS] ",progName);
		CMmsgPrint (CMmsgUsrError, "  Flags:");
		CMmsgPrint (CMmsgUsrError, "  -d,--debug");
		CMmsgPrint (CMmsgUsrError, "  -h,--help");
	}
}

// *** MAIN

#define cleanup(ret) printMemInfo(); return ret;

int main(int argc, char* argv[])
{
	int argPos = 0, argNum = argc;

	initMemInfo();
	for(argPos = 1; argPos < argNum;) {
		if (NCcmArgTest(argv[argPos],"-d","--debug")) { SetDebug(); NCcmArgShiftLeft(argPos,argv,argc); argNum--; continue; }
		if (NCcmArgTest(argv[argPos],"-h","--help")) {
			if(argv[argPos+1][0] == 'e') do_help(NCcmProgName(argv[0]),true); else do_help(NCcmProgName(argv[0]),false);
			cleanup(NCsucceeded);
		}
		if (NCcmArgTest(argv[argPos],"- ","-- "))
		{
			NCcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError, "Unknown option: %s!",argv[argPos]); cleanup(NCfailed); }
		argPos++;
	}

	cleanup(NCsucceeded);
}
