#include <NC.h> // or use NCGmath.h if math libraries needed

void do_help(char *progName,bool extend)
{
	if(extend) {
		fprintf(stderr,"Usage: %s [OPTIONS] \n",progName);
		fprintf(stderr,"  Flags:\n");
		fprintf(stderr,"  -d,--debug                     => initiate debug output\n");
		fprintf(stderr,"  -h,--help                      => print this message\n");
	} else {
		fprintf(stderr,"Usage: %s [OPTIONS] \n",progName);
		fprintf(stderr,"  Flags:\n");
		fprintf(stderr,"  -d,--debug\n");
		fprintf(stderr,"  -h,--help\n");
	}
}

// *** MAIN

#define cleanup(ret) printMemInfo(); return ret;

int main(int argc, char* argv[])
{
	int argPos = 0, argNum = argc;

	initMemInfo();
	for(argPos = 1; argPos < argNum;) {
		if (NCGcmArgTest(argv[argPos],"-d","--debug")) { SetDebug(); NCGcmArgShiftLeft(argPos,argv,argc); argNum--; continue; }
		if (NCGcmArgTest(argv[argPos],"-h","--help")) {
			if(argv[argPos+1][0] == 'e') do_help(NCGcmProgName(argv[0]),true); else do_help(NCGcmProgName(argv[0]),false);
			cleanup(NCGsucceeded);
		}
		if (NCGcmArgTest(argv[argPos],"- ","-- "))
		{
			NCGcmArgShiftLeft(argPos,argv,argc); argNum--;
			continue;
		}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ fprintf(stderr,"Unknown option: %s!\n",argv[argPos]); cleanup(NCGfailed); }
		argPos++;
	}

	cleanup(NCGsucceeded);
}
