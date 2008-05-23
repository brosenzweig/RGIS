/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

MFDataStream.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <cm.h>
#include <MF.h>

#define MFconstStr "const:"
#define MFfileStr  "file:"
#define MFpipeStr  "pipe:"
#define MFhttpStr  "http:"

MFDataStream_t *MFDataStreamOpen (const char *path, const char *mode) {
	MFDataStream_t *dStream;

	if (path == (char *) NULL) return ((MFDataStream_t *) NULL);
	if ((dStream = (MFDataStream_t *) malloc (sizeof (MFDataStream_t))) == (MFDataStream_t *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
		return (MFDataStream_t *) NULL;
	}
	if (strncmp (path,MFconstStr,strlen (MFconstStr)) == 0) {
		if (strcmp (mode,"r") == 0) { dStream->Type = MFConst; return (dStream); }
		CMmsgPrint (CMmsgAppError,"Error: Invalid output data stream [%s] in: %s:%d\n",path + strlen (MFconstStr),__FILE__,__LINE__);
		free (dStream);
		dStream = (MFDataStream_t *) NULL;
	}
	if (strncmp (path,MFpipeStr,strlen (MFpipeStr)) == 0) {
		dStream->Type = MFPipe;
		if ((dStream->Handle.File = popen (path + strlen (MFpipeStr),mode)) == (FILE *) NULL) {
			CMmsgPrint (CMmsgSysError,"Error: Opening datastream pipe [%s] in: %s:%d\n",path + strlen (MFpipeStr),__FILE__,__LINE__);
			free (dStream);
			dStream = (MFDataStream_t *) NULL;
		}
	}
	else if (strncmp (path,MFfileStr, strlen (MFfileStr)) == 0) {
		dStream->Type = MFFile;
		if ((dStream->Handle.File = fopen (path + strlen (MFfileStr),mode)) == (FILE *) NULL) {
			CMmsgPrint (CMmsgSysError,"Error: Opening datastream file [%s] in: %s:%d\n",path + strlen (MFfileStr),__FILE__,__LINE__);
			free (dStream);
			dStream = (MFDataStream_t *) NULL;
		}
	}
	else {
		CMmsgPrint (CMmsgAppError,"Error: Unknown datastream type [%s]!\n",path);
		free (dStream);
		dStream = (MFDataStream_t *) NULL;
	}
	return (dStream);
}

int MFDataStreamClose (MFDataStream_t *dStream)
	{
	if ((dStream == (MFDataStream_t *) NULL) || (dStream->Handle.File == (FILE *) NULL)) return (CMsucceeded);
	switch (dStream->Type) {
		case MFFile: return (fclose (dStream->Handle.File));
		case MFPipe:
		case MFhttp: return (pclose (dStream->Handle.File));
	}
	return (CMsucceeded);
}

int MFDataStreamRead (MFVariable_t *var) {
	int i, sLen;
	MFVarHeader_t header;

	if (var->InStream->Type == MFConst) {
		if (var->Header.ItemNum < 1) {
			sLen = strlen (var->InPath);
			for (i = strlen (MFconstStr);i < sLen;++i) if (var->InPath [i] == '.') break;
			if (i == sLen) {
				if (sscanf (var->InPath + strlen (MFconstStr),"%d",&(var->InStream->Handle.Int)) != 1) {
					CMmsgPrint (CMmsgAppError,"Error: Reading constant [%s]!\n",var->Name);
					return (CMfailed);
				}
				if (var->InStream->Handle.Int < 127) {
					var->Header.DataType    = MFByte;
					var->Header.Missing.Int = MFDefaultMissingByte;
				}
				else if (var->InStream->Handle.Int < 32767) {
					var->Header.DataType    = MFShort;
					var->Header.Missing.Int = MFDefaultMissingInt;
				}
				else {
					var->Header.DataType    = MFInt;
					var->Header.Missing.Int = MFDefaultMissingInt;
				}
				if (var->Header.Missing.Int == var->InStream->Handle.Int) var->Header.Missing.Int = (int) 0;
			}
			else {
				var->Header.DataType = MFFloat;
				if (sscanf (var->InPath + strlen (MFconstStr),"%lf",&var->InStream->Handle.Float) != 1) {
					CMmsgPrint (CMmsgAppError,"Error: Reading constant [%s]!\n",var->Name);
					return (CMfailed);
				}
				var->Header.Missing.Float = CMmathEqualValues (var->InStream->Handle.Float,MFDefaultMissingFloat) ?
					(float) 0.0 : MFDefaultMissingFloat;
			}
		}
		strcpy (var->Header.Date,MFDateClimatologyStr);
	}
	else {
		if (var->InStream->Handle.File == (FILE *) NULL) return (CMfailed);

		if (!MFVarReadHeader (&header,var->InStream->Handle.File)) return (MFStop);

		if (var->Data == (void *) NULL) {
			if ((var->Data = (void *) realloc (var->Data,header.ItemNum * MFVarItemSize (header.DataType))) == (void *) NULL) { 
				CMmsgPrint (CMmsgSysError,"Variable [%s] allocation error in: %s:%d\n",var->Name,__FILE__,__LINE__);
				return (CMfailed); 
			}
			var->Header.ItemNum  = header.ItemNum;
			var->Header.DataType = header.DataType;
			switch (var->Header.DataType) {
				case MFByte:
				case MFShort:
				case MFInt:		var->Header.Missing.Int 	= header.Missing.Int;	break;
				case MFFloat:
				case MFDouble:	var->Header.Missing.Float	= header.Missing.Float; break;
			}
		}
		else	if (header.DataType != var->Header.DataType) {
			CMmsgPrint (CMmsgAppError,"Record Type Missmatch [%d,%d] in: %s:%d varName %s \n", header.DataType, var->Header.DataType,__FILE__,__LINE__, var->Name);
			return (CMfailed);
		}
		else  if (header.ItemNum  != var->Header.ItemNum) {
			CMmsgPrint (CMmsgAppError,"Record Size Missmatch [%d,%d] in: %s:%d\n", header.ItemNum,  var->Header.ItemNum, __FILE__,__LINE__);
			return (CMfailed);
		}
		if ((int) fread (var->Data,MFVarItemSize (header.DataType),header.ItemNum,var->InStream->Handle.File) != header.ItemNum) {
			CMmsgPrint (CMmsgSysError,"Data Reading error in: %s:%d\n",__FILE__,__LINE__);
			
			return (CMfailed);
		}
 
		strncpy (var->Header.Date,   header.Date, sizeof (header.Date));
		if (header.Swap != 1)
			switch (header.DataType) {
				case MFShort:  for (i = 0;i < var->Header.ItemNum;++i) MFSwapHalfWord ((short *)  (var->Data) + i); break;
				case MFInt:    for (i = 0;i < var->Header.ItemNum;++i) MFSwapWord     ((int *)    (var->Data) + i); break;
				case MFFloat:  for (i = 0;i < var->Header.ItemNum;++i) MFSwapWord     ((float *)  (var->Data) + i); break;
				case MFDouble: for (i = 0;i < var->Header.ItemNum;++i) MFSwapLongWord ((double *) (var->Data) + i); break;
				default:	break;
			}
	}
	return (MFContinue);
}

int MFDataStreamWrite (MFVariable_t *var, const char *date) {
	int itemSize;
	MFVarHeader_t header;

	memcpy (&header, &(var->Header), sizeof (MFVarHeader_t));
	strncpy (header.Date, date, sizeof (header.Date) - 1);
	itemSize = MFVarItemSize (header.DataType);
	header.Swap = 1;
	if (!MFVarWriteHeader (&(header),var->OutStream->Handle.File) != CMsucceeded) return (CMfailed);
	if ((int) fwrite (var->Data,itemSize,header.ItemNum,var->OutStream->Handle.File) != header.ItemNum) {
		CMmsgPrint (CMmsgSysError,"Data writing error in: %s:%d\n"__FILE__,__LINE__);
		return (CMfailed);
	}
	return (CMsucceeded);
}
