/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

CMDdsAggregate.c

andras@ob.sr.unh.edu

*******************************************************************************/

#include <cm.h>
#include <MF.h>
#include <stdlib.h>
#include <string.h>

enum { DAY = 10, MONTH = 7, YEAR = 4 };
enum { AVG = 1, SUM = 2};

int main(int argc,char *argv []) {
	int argPos, argNum = argc, ret = CMfailed, itemSize, i, step = CMfailed, mode = CMfailed;
	FILE *inFile, *outFile;
	char date [MFDateStringLength];
	MFVarHeader_t header, outHeader;
	void   *items  = (void *)   NULL;
	double *array  = (double *) NULL;
	float  *record = (float *)  NULL;
	int    *obsNum = (int *)    NULL, maxObs = 0;

	if (argNum < 2) goto Help;
	date [0] = '\0';

	for (argPos = 1;argPos < argNum;) {
		if (CMargTest(argv[argPos],"-e","--step")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			if (step != CMfailed) CMmsgPrint (CMmsgUsrError,"Skipping aggregate step that is previously set!\n");
			else {
				const char *options [] = { "year", "month", "day", (char *) NULL };
				int codes [] = { YEAR, MONTH, DAY }, code;

				if ((code = CMoptLookup (options,argv [argPos],false)) == CMfailed) {
					CMmsgPrint (CMmsgWarning,"Ignoring illformed step option [%s]!\n",argv [argPos]);
				}
				else step = codes [code];
			}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest(argv[argPos],"-a","--aggregate")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			if (mode != CMfailed) CMmsgPrint (CMmsgUsrError,"Skipping aggregate mode that is previously set!\n");
			else {
				const char *options [] = { "avg", "sum", (char *) NULL };
				int codes [] = { AVG, SUM }, code;

				if ((code = CMoptLookup (options,argv [argPos],false)) == CMfailed) {
					CMmsgPrint (CMmsgWarning,"Ignoring illformed step option [%s]!\n",argv [argPos]);
				}
				else mode = codes [code];
			}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
Help:	if (CMargTest(argv[argPos],"-h","--help")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) < argPos) break;
			CMmsgPrint (CMmsgUsrError,"%s [options] <in datastream> <out datastream>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgUsrError,"  -e, --step [year|month|day]\n");
			CMmsgPrint (CMmsgUsrError,"  -a, --aggregate [avg|sum]\n");
			CMmsgPrint (CMmsgUsrError,"  -h,--help\n");
			ret = CMsucceeded;
			goto Stop;
		}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1)) {
			CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]);
			return (CMfailed);
		}
      argPos++;
	}
	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }

	if (mode == CMfailed) mode = AVG;
	if (step == CMfailed) mode = DAY;

   if ((inFile  = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? fopen (argv [1],"r") : stdin)  == (FILE *) NULL) {
		CMmsgPrint (CMmsgAppError, "Input file opening error\n");
		perror (":");
		goto Stop;
	}
  	if ((outFile = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? fopen (argv [2],"w") : stdout) == (FILE *) NULL) {
		CMmsgPrint (CMmsgAppError, "Output file opening error\n");
		perror (":");
		goto Stop;
	}

	while (MFVarReadHeader (&header,inFile)) {
		if (strncmp (date,header.Date,step) != 0) {
			if (items == (void *) NULL) {
				itemSize = MFVarItemSize(header.DataType);
				if ((items  = (void *)   calloc (header.ItemNum, itemSize))        == (void *)   NULL) {
					CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
					perror (":");
					goto Stop;
				}
				if ((array  = (double *) calloc (header.ItemNum, sizeof (double))) == (double *) NULL) {
					CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
					perror (":");
					goto Stop;
				}
				if ((record = (float *)  calloc (header.ItemNum, sizeof (float)))  == (float *) NULL) {
					CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
					perror (":");
					goto Stop;
				}
				if ((obsNum = (int *)    calloc (header.ItemNum, sizeof (int)))    == (int *)    NULL) {
					CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
					perror (":");
					goto Stop;
				}
				outHeader.Swap = 1;
				outHeader.DataType      = MFFloat;
				outHeader.ItemNum       = header.ItemNum;
				outHeader.Missing.Float = (header.DataType == MFFloat) || (header.DataType == MFDouble) ? header.Missing.Float : MFDefaultMissingFloat;
				for(i = 0; i < header.ItemNum; i++) obsNum [i] = 0;
				for(i = 0; i < header.ItemNum; i++) array  [i] = 0.0;
				maxObs = 0;
			}
			else {
				switch (mode) {
					default:
					case AVG: for(i = 0; i < header.ItemNum; i++) record [i] = obsNum [i] > 0 ? array  [i] / obsNum [i]                              : outHeader.Missing.Float; break;
					case SUM: for(i = 0; i < header.ItemNum; i++) record [i] = obsNum [i] > 0 ? array  [i] * ((double) maxObs / (double) obsNum [i]) : outHeader.Missing.Float; break;
				}
				strncpy (outHeader.Date,date,step);
				outHeader.Date [step] = '\0';
				if (MFVarWriteHeader (&outHeader, outFile)) {
					if ((int) fwrite (record,sizeof (float),outHeader.ItemNum,outFile) != outHeader.ItemNum) {
						CMmsgPrint (CMmsgSysError, "Output writing error in: %s:%d\n",__FILE__,__LINE__);
						perror (":");
						goto Stop;
					}
				}
				for(i = 0; i < header.ItemNum; i++) obsNum [i] = 0;
				for(i = 0; i < header.ItemNum; i++) array  [i] = 0.0;
				maxObs = 0;
			}
			strncpy (date,header.Date,step);
			date [step] = '\0';
		}
		if ((int) fread (items,itemSize,header.ItemNum,inFile) != header.ItemNum) {
			CMmsgPrint (CMmsgSysError, "Input reading error in: %s:%d\n",__FILE__,__LINE__);
			perror (":");
			goto Stop;
		}
		for(i = 0; i < header.ItemNum; i++) {
			switch (header.DataType) {
				case MFByte:
					if (((char *)   items) [i] != header.Missing.Int) {
						array  [i] += ((char *)   items) [i];
						obsNum [i] += 1;
						if (obsNum [i] > maxObs) maxObs = obsNum [i];
					}
					break;
				case MFShort:
					if (header.Swap != 1)    MFSwapHalfWord (((short *)  items) + i);
					if (((short *)  items) [i] != header.Missing.Int) {
						array [i]  += ((short *)  items) [i];
						obsNum [i] += 1;
						if (obsNum [i] > maxObs) maxObs = obsNum [i];
					}
					break;
				case MFInt:
					if(header.Swap != 1) MFSwapWord     (((int *)    items) + i);
					if (((int *)    items) [i] != header.Missing.Int) {
						array [i]  += ((int *)    items) [i];
						obsNum [i] += 1;
						if (obsNum [i] > maxObs) maxObs = obsNum [i];
					}
				break;
				case MFFloat:
					if (header.Swap != 1)   MFSwapWord      (((float *)  items) + i);
					if (CMmathEqualValues (((float *)   items) [i],header.Missing.Float) == false) {
						array [i]  += ((float *)  items) [i];
						obsNum [i] += 1;
						if (obsNum [i] > maxObs) maxObs = obsNum [i];
					}
				break;
				case MFDouble:
					if (header.Swap != 1)   MFSwapLongWord  (((double *) items) + i);
					if (CMmathEqualValues (((double *)  items) [i],header.Missing.Float) == false) {
						array [i]  += ((double *) items) [i];
						obsNum [i] += 1;
						if (obsNum [i] > maxObs) maxObs = obsNum [i];
					}
				break;
			}
		}
	}
	switch (mode) {
		default:
		case AVG: for(i = 0; i < header.ItemNum; i++) record [i] = obsNum [i] > 0 ? array  [i] / obsNum [i]                              : outHeader.Missing.Float; break;
		case SUM: for(i = 0; i < header.ItemNum; i++) record [i] = obsNum [i] > 0 ? array  [i] * ((double) maxObs / (double) obsNum [i]) : outHeader.Missing.Float; break;
	}
	strncpy (outHeader.Date,header.Date,step);
	outHeader.Date [step] = '\0';
	if (MFVarWriteHeader (&outHeader, outFile)) {
		if ((int) fwrite (record,sizeof (float),outHeader.ItemNum,outFile) != outHeader.ItemNum) {
			CMmsgPrint (CMmsgSysError, "Output writing error in: %s:%d\n",__FILE__,__LINE__);
			perror (":");
			goto Stop;
		}
	}
	ret = CMsucceeded;
Stop:
	if (items   != (void *)   NULL) free (items);
	if (array   != (double *) NULL) free (array);
	if (obsNum  != (int *)    NULL) free (obsNum);
	if (inFile  != stdin)  fclose (inFile);
	if (outFile != stdout) fclose (outFile);
	return (ret);
}
