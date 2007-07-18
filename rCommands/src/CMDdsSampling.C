/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

CMDdsSampling.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <MF.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv []) {
	int argPos, argNum = argc, ret = CMfailed, itemSize, i, id, idNum;
	FILE *itemFile = (FILE *) NULL, *inFile = stdin, *outFile = stdout;
	char *fileName = (char *) NULL;
	void *items    = (void *) NULL;
	int  *ids      = (int *)  NULL;
	MFVarHeader_t header;

	if (argNum < 2) goto Help;

	for (argPos = 1;argPos < argNum;) {
		if (CMargTest(argv[argPos],"-i","--itemlist")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			fileName = argv [argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
Help:	if (CMargTest(argv[argPos],"-h","--help")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) < argPos) break;
			CMmsgPrint (CMmsgUsrError,"%s [options] <in datastream> <out datastream>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgUsrError,"  -i, --itemlist [itemlist]\n");
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
	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); goto Stop; }

	if (fileName == (char *) NULL) { CMmsgPrint (CMmsgUsrError, "Missing itemlist!\n"); goto Stop; }
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
	if ((itemFile = fopen (fileName,"r")) == (FILE *) NULL) {
		CMmsgPrint (CMmsgAppError, "Itemlist file opening error\n");
		perror (":");
		goto Stop;
	}
	idNum = 0;
	while (fscanf (itemFile,"%d", &id) == 1) {
		if ((ids = (int *) realloc (ids,(idNum + 1) * sizeof (int))) == (int *) NULL) {
			CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
			perror (":");
			goto Stop;
		}
		ids [idNum] = id;
		idNum++;
	}
	fclose (itemFile);
	itemFile = (FILE *) NULL;

	while (MFVarReadHeader (&header,inFile)) {
		if (items == (void *) NULL) {
			itemSize = MFVarItemSize(header.DataType);
			if ((items  = (void *) calloc (header.ItemNum, itemSize)) == (void *)   NULL) {
				CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
				perror (":");
				goto Stop;
			}
		}
		if ((int) fread (items,itemSize,header.ItemNum,inFile) != header.ItemNum) {
			CMmsgPrint (CMmsgSysError, "Input reading error in: %s:%d\n",__FILE__,__LINE__);
			perror (":");
			goto Stop;
		}
		for(i = 0; i < idNum; i++) {
			if ((ids [i] < 0) || (ids [i] >= header.ItemNum)) {
				CMmsgPrint (CMmsgAppError, "Invalid Item id [%d]\n",ids [i]);
				continue;
			}
			switch (header.DataType) {
				case MFByte:
					if (((char *)   items) [ids [i]] != header.Missing.Int)
						fprintf (outFile,"%d\t%s\t%d\n",ids [i],header.Date, (int)    ((char *)   items) [ids [i]]); 
					else
						fprintf (outFile,"%d\t%s\t\n",  ids [i],header.Date); 
					break;
				case MFShort:
					if (header.Swap != 1) MFSwapHalfWord (((short *)  items) + ids [i]);
					if (((short *)  items) [ids [i]] != header.Missing.Int)
						fprintf (outFile,"%d\t%s\t%d\n",ids [i],header.Date, (int)    ((short *)  items) [ids [i]]); 
					else
						fprintf (outFile,"%d\t%s\t\n",  ids [i],header.Date); 
					break;
				case MFInt:
					if(header.Swap != 1) MFSwapWord     (((int *)    items) + ids [i]);
					if (((int *)    items) [ids [i]] != header.Missing.Int)
						fprintf (outFile,"%d\t%s\t%d\n",ids [i],header.Date, (int)    ((int *)    items) [ids [i]]); 
					else
						fprintf (outFile,"%d\t%s\t\n",  ids [i],header.Date); 
					break;
				case MFFloat:
					if (header.Swap != 1) MFSwapWord     (((float *)  items) + ids [i]);
					if (MFMathEqualValues (((float *)   items) [ids [i]],header.Missing.Float) == false)
						fprintf (outFile,"%d\t%s\t%f\n",ids [i],header.Date, (float)  ((float *)  items) [ids [i]]); 
					else
						fprintf (outFile,"%d\t%s\t\n",  ids [i],header.Date); 
					break;
				case MFDouble:
					if (header.Swap != 1) MFSwapLongWord (((double *) items) + ids [i]);
					if (MFMathEqualValues (((double *)  items) [ids [i]],header.Missing.Float) == false)
						fprintf (outFile,"%d\t%s\t%lf\n",ids [i],header.Date,(double) ((double *) items) [ids [i]]); 
					else
						fprintf (outFile,"%d\t%s\t\n",ids [i],header.Date); 
				break;
			}
		}
	}
	ret = CMsucceeded;
Stop:
	if (ids      != (int *)  NULL) free   (ids);
	if (itemFile != (FILE *) NULL) fclose (itemFile);
	if (inFile   != stdin)         fclose (inFile);
	if (outFile  != stdout)        fclose (outFile);
	return (ret);
}