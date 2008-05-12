/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

CMDdsDuration.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <MF.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc,char *argv []) {
	int argPos, argNum = argc, ret = CMfailed, itemSize, i, bin, binNum = 1000, percent;
	bool valueMode = false;
	FILE  *inFile   = (FILE *)   NULL, *outFile = stdout;
	char  *fileName = (char *)   NULL;
	void  *items    = (void *)   NULL;
	double *max     = (double *) NULL;
	double *min     = (double *) NULL;
	float  *output  = (float *)  NULL;
	int    *bins    = (int *)    NULL;
	double value, binSize, binMax, binMin, percentMax, percentMin;
	MFVarHeader_t header, outHeader;

	if (argNum < 2) goto Help;

	for (argPos = 1;argPos < argNum;) {
		if (CMargTest(argv[argPos],"-i","--input")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			fileName = argv [argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest(argv[argPos],"-b","--bins")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			if (sscanf (argv [argPos],"%d", &binNum) != 1) {
				CMmsgPrint (CMmsgUsrError,"Ilformed bin number!\n");
				goto Stop;
			}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-m","--mode"))
			{
			int mode;
			const char *modes [] = { "percent", "value", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing savesteps!\n");    return (CMfailed); }
			if ((mode = CMoptLookup (modes,argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid savesteps mode!\n"); return (CMfailed); }
			valueMode = mode == 1 ? true : false;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-h","--help")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) < argPos) break;
Help:		CMmsgPrint (CMmsgUsrError,"%s [options] <out datastream>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgUsrError,"  -i, --input [input datastream]\n");
			CMmsgPrint (CMmsgUsrError,"  -b, --bins  [# of bins]\n");
			CMmsgPrint (CMmsgUsrError,"  -m, --mode  [percent|value]\n");
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
	if (argNum > 2) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); goto Stop; }

	if (fileName == (char *) NULL) { CMmsgPrint (CMmsgUsrError, "Missing input file!\n"); goto Stop; }
  	if ((outFile = (argNum > 2) && (strcmp (argv [1],"-") != 0) ? fopen (argv [1],"w") : stdout) == (FILE *) NULL) {
		CMmsgPrint (CMmsgAppError, "Output file opening error\n");
		perror (":");
		goto Stop;
	}
	if ((inFile = fopen (fileName,"r")) == (FILE *) NULL) {
		CMmsgPrint (CMmsgAppError, "Input file opening error\n");
		perror (":");
		goto Stop;
	}

	while (MFVarReadHeader (&header,inFile)) {
		if (items == (void *) NULL) {
			itemSize = MFVarItemSize(header.DataType);
			if ((items  = (void *)   calloc (header.ItemNum,         itemSize))         == (void *)   NULL) {
				CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
				perror (":");
				goto Stop;
			}
			if ((max    = (double *) calloc (header.ItemNum,          sizeof (double))) == (double *) NULL) {
				CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
				perror (":");
				goto Stop;
			}
			if ((min    = (double *) calloc (header.ItemNum,          sizeof (double))) == (double *) NULL) {
				CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
				perror (":");
				goto Stop;
			}
			if ((output = (float *) calloc (header.ItemNum,          sizeof (float)))   == (float *)  NULL) {
				CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
				perror (":");
				goto Stop;
			}
			if ((bins   = (int *)    calloc (header.ItemNum * binNum, sizeof (int)))    == (int *)   NULL) {
				CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
				perror (":");
				goto Stop;
			}
			for(i = 0; i < header.ItemNum; i++) {
				max [i] = - HUGE_VAL;
				min [i] = HUGE_VAL;
				for (bin = 0; bin < binNum; ++bin) bins [bin * header.ItemNum + i] = 0;
			}
			outHeader.Swap          = 1;
			outHeader.DataType      = MFFloat;
			outHeader.ItemNum       = header.ItemNum;
			outHeader.Missing.Float = MFDefaultMissingFloat;
		}
		if ((int) fread (items,itemSize,header.ItemNum,inFile) != header.ItemNum) {
			CMmsgPrint (CMmsgSysError, "Input reading error in: %s:%d\n",__FILE__,__LINE__);
			perror (":");
			goto Stop;
		}
		switch (header.DataType) {
			case MFByte:
				for(i = 0; i < header.ItemNum; i++) {
					if (((char *)   items) [i] == header.Missing.Int) continue;
					value = (double) (((char *)   items) [i]);
					if (max [i] < value) max [i] = value;
					if (min [i] > value) min [i] = value;
				}
				break;
			case MFShort:
				for(i = 0; i < header.ItemNum; i++) {
					if (header.Swap != 1) MFSwapHalfWord (((short *)  items) + i);
					if (((short *)  items) [i] == header.Missing.Int) continue;
					value = (double) (((short *)  items) [i]);
					if (max [i] < value) max [i] = value;
					if (min [i] > value) min [i] = value;
				}
				break;
			case MFInt:
				for(i = 0; i < header.ItemNum; i++) {
					if(header.Swap != 1) MFSwapWord      (((int *)    items) + i);
					if (((int *)    items) [i] == header.Missing.Int) continue;
					value = (double) (((int *)    items) [i]);
					if (max [i] < value) max [i] = value;
					if (min [i] > value) min [i] = value;
				}
				break;
			case MFFloat:
				for(i = 0; i < header.ItemNum; i++) {
					if (header.Swap != 1) MFSwapWord     (((float *)  items) + i);
					if (CMmathEqualValues (((float *)   items) [i],header.Missing.Float) == true)  continue;
					value = (double) (((float *)  items) [i]);
					if (max [i] < value) max [i] = value;
					if (min [i] > value) min [i] = value;
				}
				break;
			case MFDouble:
				for(i = 0; i < header.ItemNum; i++) {
					if (header.Swap != 1) MFSwapLongWord  (((double *) items) + i);
					if (CMmathEqualValues (((double *)  items) [i],header.Missing.Float) == true) continue;
					value = (double) (((double *) items) [i]);
					if (max [i] < value) max [i] = value;
					if (min [i] > value) min [i] = value;
				}
				break;
		}
	}
	rewind (inFile);
	while (MFVarReadHeader (&header,inFile)) {
		if ((int) fread (items,itemSize,header.ItemNum,inFile) != header.ItemNum) {
			CMmsgPrint (CMmsgSysError, "Input reading error in: %s:%d\n",__FILE__,__LINE__);
			perror (":");
			goto Stop;
		}
		switch (header.DataType) {
			case MFByte:
				for(i = 0; i < header.ItemNum; i++) {
					if (((char *)   items) [i] == header.Missing.Int) continue;
					if (!CMmathEqualValues (max [i],min [i])) {
						value = (double) (((char *)   items) [i]);
						bin = (int) floor ((double) (binNum - 1) * (value - min [i]) / (max [i] - min [i]));
						for (  ;bin >= 0; bin--) bins [bin * header.ItemNum + i] += 1;
					}
				}
				break;
			case MFShort:
				for(i = 0; i < header.ItemNum; i++) {
					if (header.Swap != 1) MFSwapHalfWord (((short *)  items) + i);
					if (((short *)  items) [i] == header.Missing.Int) continue;
					if (!CMmathEqualValues (max [i],min [i])) {
						value = (double) (((short *)  items) [i]);
						bin = (int) floor ((double) (binNum - 1) * (value - min [i]) / (max [i] - min [i]));
						for (  ;bin >= 0; bin--) bins [bin * header.ItemNum + i] += 1;
					}
				}
				break;
			case MFInt:
				for(i = 0; i < header.ItemNum; i++) {
					if(header.Swap != 1) MFSwapWord      (((int *)    items) + i);
					if (((int *)    items) [i] == header.Missing.Int) continue;
					if (!CMmathEqualValues (max [i],min [i])) {
						value = (double) (((int *)    items) [i]);
						bin = (int) floor ((double) (binNum - 1) * (value - min [i]) / (max [i] - min [i]));
						for (  ;bin >= 0; bin--) bins [bin * header.ItemNum + i] += 1;
					}
				}
				break;
			case MFFloat:
				for(i = 0; i < header.ItemNum; i++) {
					if (header.Swap != 1) MFSwapWord     (((float *)  items) + i);
					if (CMmathEqualValues (((float *)   items) [i],header.Missing.Float) == true)  continue;
					if (!CMmathEqualValues (max [i],min [i])) {
						value = (double) (((float *)  items) [i]);
						bin = (int) floor ((double) (binNum - 1) * (value - min [i]) / (max [i] - min [i]));
						for (  ;bin >= 0; bin--) bins [bin * header.ItemNum + i] += 1;
					}
				}
				break;
			case MFDouble:
				for(i = 0; i < header.ItemNum; i++) {
					if (header.Swap != 1) MFSwapLongWord  (((double *) items) + i);
					if (CMmathEqualValues (((double *)  items) [i],header.Missing.Float) == true) continue;
					if (!CMmathEqualValues (max [i],min [i])) {
						value = (double) (((double *) items) [i]);
						bin = (int) floor ((double) (binNum - 1) * (value - min [i]) / (max [i] - min [i]));
						for (  ;bin >= 0; bin--) bins [bin * header.ItemNum + i] += 1;
					}
				}
				break;
		}
	}
	if (valueMode) {
		for (percent = 0;percent < 100; ++percent) {
			for(i = 0; i < header.ItemNum; i++) {
				binSize = (max [i] - min [i]) / (float) binNum;
				if (CMmathEqualValues (binSize, 0.0)) output [i] = min [i];
				else {
					for (bin = 0;bin < binNum; ++bin) {
						percentMin = bins [bin * header.ItemNum + i] / bins [i] * 100.0;
						if ((float) percent > percentMin) break;
					}
					binMax = bin < binNum ? (float)  bin      * binSize + min [i] : max [i];
					binMin = bin > 0      ? (float) (bin - 1) * binSize + min [i] : min [i];
					percentMax = bin < binNum ? bins [bin * header.ItemNum + i] / bins [i] * 100.0 : 100.0;
					output [i] = (binMax + binMin) / 2.0;
				}
			}
			sprintf (outHeader.Date,"%3d", percent + 1);
			if (MFVarWriteHeader (&outHeader,outFile) == false) {
				CMmsgPrint (CMmsgSysError, "Output writing error in: %s:%d\n",__FILE__,__LINE__);
				perror (":");
				goto Stop;
			}
			if ((int) fwrite (output,sizeof (float),outHeader.ItemNum,outFile) != outHeader.ItemNum) {
				CMmsgPrint (CMmsgSysError, "Output reading error in: %s:%d\n",__FILE__,__LINE__);
				perror (":");
				goto Stop;
			}
		}
		ret = CMsucceeded;
	}
	else {
		rewind (inFile);
		while (MFVarReadHeader (&header,inFile)) {
			if ((int) fread (items,itemSize,header.ItemNum,inFile) != header.ItemNum) {
				CMmsgPrint (CMmsgSysError, "Input reading error in: %s:%d\n",__FILE__,__LINE__);
				perror (":");
				goto Stop;
			}
			switch (header.DataType) {
				case MFByte:
					for(i = 0; i < header.ItemNum; i++) {
						if (((char *)   items) [i] == header.Missing.Int) output [i] = outHeader.Missing.Float;
						else if (bins [i] > 0) {
							value = (double) (((char *)   items) [i]);
							bin = (int) floor ((double) (binNum - 1) * (value - min [i]) / (max [i] - min [i]));
							output [i] = 100.0 * (double) bins [bin * header.ItemNum + i] / (double) bins [i];
						}
						else output [i] = 100.0;
					}
					break;
				case MFShort:
					for(i = 0; i < header.ItemNum; i++) {
						if (header.Swap != 1) MFSwapHalfWord (((short *)  items) + i);
						if (((short *)  items) [i] == header.Missing.Int) output [i] = outHeader.Missing.Float;
						else if (bins [i] > 0) {
							value = (double) (((short *)  items) [i]);
							bin = (int) floor ((double) (binNum - 1) * (value - min [i]) / (max [i] - min [i]));
							output [i] = 100.0 * (double) bins [bin * header.ItemNum + i] / (double) bins [i];
						}
						else output [i] = 100.0;
					}
					break;
				case MFInt:
					for(i = 0; i < header.ItemNum; i++) {
						if(header.Swap != 1) MFSwapWord      (((int *)    items) + i);
						if (((int *)    items) [i] == header.Missing.Int) output [i] = outHeader.Missing.Float;
						else if (bins [i] > 0) {
							value = (double) (((int *)    items) [i]);
							bin = (int) floor ((double) (binNum - 1) * (value - min [i]) / (max [i] - min [i]));
							output [i] = 100.0 * (double) bins [bin * header.ItemNum + i] / (double) bins [i];
						}
						else output [i] = 100.0;
					}
					break;
				case MFFloat:
					for(i = 0; i < header.ItemNum; i++) {
						if (header.Swap != 1) MFSwapWord     (((float *)  items) + i);
						if (CMmathEqualValues (((float *)   items) [i],header.Missing.Float) == true) output [i] = outHeader.Missing.Float;
						else if (bins [i] > 0) {
							value = (double) (((float *)  items) [i]);
							bin = (int) floor ((double) (binNum - 1) * (value - min [i]) / (max [i] - min [i]));
							output [i] = 100.0 * (double) bins [bin * header.ItemNum + i] / (double) bins [i];
						}
						else output [i] = 100.0;
					}
					break;
				case MFDouble:
					for(i = 0; i < header.ItemNum; i++) {
						if (header.Swap != 1) MFSwapLongWord  (((double *) items) + i);
						if (CMmathEqualValues (((double *)  items) [i],header.Missing.Float) == true) output [i] = outHeader.Missing.Float;
						else if (bins [i] > 0) {
							value = (double) (((double *) items) [i]);
							bin = (int) floor ((double) (binNum - 1) * (value - min [i]) / (max [i] - min [i]));
							output [i] = 100.0 * (double) bins [bin * header.ItemNum + i] / (double) bins [i];
						}
						else output [i] = 100.0;
					}
					break;
			}
			strcpy (outHeader.Date,header.Date);
			if (MFVarWriteHeader (&outHeader,outFile) == false) {
				CMmsgPrint (CMmsgSysError, "Output writing error in: %s:%d\n",__FILE__,__LINE__);
				perror (":");
				goto Stop;
			}
			if ((int) fwrite (output,sizeof (float),outHeader.ItemNum,outFile) != outHeader.ItemNum) {
				CMmsgPrint (CMmsgSysError, "Output reading error in: %s:%d\n",__FILE__,__LINE__);
				perror (":");
				goto Stop;
			}
		}
		ret = CMsucceeded;
	}
Stop:
	if (items    != (void *)   NULL) free (items);
	if (max      != (double *) NULL) free (max);
	if (min      != (double *) NULL) free (min);
	if (inFile   != (FILE *)   NULL) fclose (inFile);
	if (outFile  != stdout)          fclose (outFile);
	return (ret);
}
