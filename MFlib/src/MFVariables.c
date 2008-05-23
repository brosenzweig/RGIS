/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

MFVariables.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cm.h>
#include <MF.h>

static MFVariable_t *_MFVariables = (MFVariable_t *) NULL;
static int _MFVariableNum = 0;

MFVariable_t *MFVarGetByID (int id) {
	return ((id > 0) && (id <= _MFVariableNum) ? _MFVariables + id - 1: (MFVariable_t *) NULL);
}

char *MFVarTypeString (int type) {
	switch (type) {
		case MFInput:  return ("input");
		case MFOutput: return ("output");
		case MFRoute:  return ("route");
		case MFByte:   return ("byte");
		case MFShort:  return ("short");
		case MFInt:    return ("int");
		case MFFloat:  return ("float");
		case MFDouble: return ("double");
		default: CMmsgPrint (CMmsgAppError,"Error: Invalide Type [%d] in: %s:%d\n",__FILE__,__LINE__); break;
	}
	return ("");
}

static MFVariable_t *_MFVarNewEntry (const char *name) {
	MFVariable_t *var;
	_MFVariables = (MFVariable_t *) realloc (_MFVariables,(_MFVariableNum + 1) * sizeof (MFVariable_t));
	if (_MFVariables == (MFVariable_t *) NULL) {
	 	CMmsgPrint (CMmsgSysError,"Error: Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
		return ((MFVariable_t *) NULL);
	}
	var = _MFVariables + _MFVariableNum;
	var->ID = _MFVariableNum + 1;
	strncpy (var->Name,name,sizeof (var->Name) - 1);
	strcpy  (var->Unit,MFNoUnit);
	strcpy  (var->Header.Date,MFDateClimatologyStr);
	var->Header.ItemNum  = 0;
	var->Header.DataType = MFInput;
	var->Data      = (void *) NULL;
	var->Func      = (void (*) (int)) NULL;
	var->InPath    = (char *) NULL;
	var->OutPath   = (char *) NULL;
	var->InStream  = (MFDataStream_t *) NULL;
	var->OutStream = (MFDataStream_t *) NULL;
	var->TStep     = MFTimeStepYear;
	var->Set       = false;
	var->Flux      = false;
	var->Initial   = false;
	var->Route     = false;
	var->NStep     = 1;
	_MFVariableNum++;
	return (var);
}

static MFVariable_t *_MFVarFindEntry (const char *name) {
	int i;

	for (i = 0;i < _MFVariableNum;++i) if (strcmp (_MFVariables [i].Name,name) == 0) break;
	if (i < _MFVariableNum) return (_MFVariables + i);
	return ((MFVariable_t *) NULL);
}

MFVariable_t *MFVarSetPath (const char *name,const char *path, int type) {
	MFVariable_t *var;

	var = _MFVarFindEntry (name);
	switch (type) {
		case MFInput:
			if (var != (MFVariable_t *) NULL) {
				CMmsgPrint (CMmsgWarning,"Warning: Ignoring variable [%s] redefiniion!\n",name);
				return ((MFVariable_t *) NULL);
			}
			if ((var = _MFVarNewEntry (name)) == (MFVariable_t *) NULL)
				return ((MFVariable_t *) NULL);
			if ((var->InPath = (char *) malloc (strlen (path) + 1)) == (char *) NULL) {
				CMmsgPrint (CMmsgSysError,"Error: Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
				return ((MFVariable_t *) NULL);
			}
			strcpy (var->InPath,path);
			if ((var->InStream = MFDataStreamOpen (path,"r")) == (MFDataStream_t *) NULL)
				return ((MFVariable_t *) NULL);
			if (MFDataStreamRead (var) == CMfailed) return ((MFVariable_t *) NULL);	
			switch (strlen (var->Header.Date)) {
				default:
				case  4: var->TStep = MFTimeStepYear;  break;
				case  7: var->TStep = MFTimeStepMonth; break;
				case 10: var->TStep = MFTimeStepDay;   break;
			}
			break;
		case MFOutput:
			if (var == (MFVariable_t *) NULL) {
				CMmsgPrint (CMmsgWarning,"Warning: Ignoring unspecified output variable [%s]!\n",name);
				return ((MFVariable_t *) NULL);
			}
			if ((var->OutPath = (char *) malloc (strlen (path) + 1)) == (char *) NULL) {
				CMmsgPrint (CMmsgSysError,"Error: Memory allocation error in: %s:%d\n",__FILE__,__LINE__);
				return ((MFVariable_t *) NULL);
			}
			strcpy (var->OutPath, path);
			var->OutStream = MFDataStreamOpen (var->OutPath,"w");
			break;
	}
	return (var);
}

int MFVarGetID (char *name,char *unit,int type, bool flux, bool initial) {
	MFVariable_t *var;

	if ((var = _MFVarFindEntry (name)) == (MFVariable_t *) NULL) {
		if ((var = _MFVarNewEntry (name)) == (MFVariable_t *) NULL)
			return (CMfailed);
		if (type == MFRoute) var->Route = true;
		var->Header.DataType = type == MFInput ? MFInput : MFOutput;
		var->Set      = type == MFInput ? initial : true;
	}
	switch (var->Header.DataType) {
		case MFInput:		break;
		case MFRoute:
		case MFOutput:
			switch (type) {
				case MFInput: if (!var->Set) type = MFInput; break;
				default: 
					var->Header.DataType = type;
					switch (var->Header.DataType) {
						case MFByte:	var->Header.Missing.Int   = MFDefaultMissingByte;  break;
						case MFShort:
						case MFInt:		var->Header.Missing.Int   = MFDefaultMissingInt;   break;
						case MFFloat:
						case MFDouble:	var->Header.Missing.Float = MFDefaultMissingFloat; break;
					}
				case MFRoute:
				case MFOutput: break;
			}
			break;
		default:
			switch (type) {
				default:
					if (type != var->Header.DataType)
						CMmsgPrint (CMmsgWarning,"Warning: Ignoring type redefinition (%s,%s)\n",var->Name,MFVarTypeString (type));
				case MFInput: var->Set = var->InStream != (MFDataStream_t *) NULL ? true : false; break;
				case MFRoute:
				case MFOutput:	break;
			}
	}

	if (initial != var->Initial) {
		if (initial) var->Initial = initial;
		else CMmsgPrint (CMmsgWarning,"Warning: Ignoring initial redefinition (%s [%s])!\n",var->Name,var->Unit);
	}

	if (strncmp (unit,var->Unit,strlen (unit)) != 0) {
		if (strcmp (var->Unit,MFNoUnit) == 0) strncpy (var->Unit,unit,sizeof (var->Unit) - 1);
		else CMmsgPrint (CMmsgWarning,"Warning: Ignoring Unit redefinition (%s [%s],%d)!\n",var->Name,var->Unit,type);
	}
	if (flux != var->Flux) {
		if (flux)  var->Flux = flux;
		else CMmsgPrint (CMmsgWarning,"Warning: Ignoring flux redefinition (%s [%s])!\n",var->Name,var->Unit);
	}
	return (var->ID);
}

int MFVarGetTStep (int id) {
	MFVariable_t *var;
	return ((var = MFVarGetByID (id)) != (MFVariable_t *) NULL ? var->NStep : MFTimeStepDay);
} 

static bool _MFVarTestMissingVal (MFVariable_t *var,int itemID)
	{
	switch (var->Header.DataType) {
		case MFByte:	return ((int) (((char *)  var->Data) [itemID]) == var->Header.Missing.Int);
		case MFShort:	return ((int) (((short *) var->Data) [itemID]) == var->Header.Missing.Int);
		case MFInt:		return ((int) (((int *)   var->Data) [itemID]) == var->Header.Missing.Int);
		case MFFloat:
			return (CMmathEqualValues ((((float *)  var->Data) [itemID]),var->Header.Missing.Float));
		case MFDouble:
			return (CMmathEqualValues ((((double *) var->Data) [itemID]),var->Header.Missing.Float));
	}
	CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%s,%d] type [%d] in %s:%d\n",var->Name, itemID, var->Header.DataType,__FILE__,__LINE__);
	return (true);
}

bool MFVarTestMissingVal (int id,int itemID)
	{
	MFVariable_t *var;

	if ((var = MFVarGetByID (id)) == (MFVariable_t *) NULL)  {
		CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%d] in: %s:%d\n",id,__FILE__,__LINE__);
		return (true);
	}
	if ((itemID < 0) || (itemID >= var->Header.ItemNum)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid item [%s,%d] in: %s:%d\n",var->Name,itemID,__FILE__,__LINE__);
		return (true);
	}
	return  (_MFVarTestMissingVal (var,itemID));
}

void MFVarSetMissingVal (int id, int itemID)
	{
	MFVariable_t *var;
//printf("Missing var %s\n ",var->Name);
	if (((var = MFVarGetByID (id)) == (MFVariable_t *) NULL) || (itemID < 0) || (itemID >= var->Header.ItemNum)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%d,%d] in: %s:%d\n",id,itemID,__FILE__,__LINE__);
		return;
	}
	switch (var->Header.DataType) {
		case MFByte:	((char *)   var->Data) [itemID] = (char)   var->Header.Missing.Int;		break;
		case MFShort:	((short *)  var->Data) [itemID] = (short)  var->Header.Missing.Int;		break;
		case MFInt:		((int *)    var->Data) [itemID] = (int)    var->Header.Missing.Int;		break;
		case MFFloat:	((float *)  var->Data) [itemID] = (float)  var->Header.Missing.Float;	break;
		case MFDouble:	((double *) var->Data) [itemID] = (double) var->Header.Missing.Float;	break;
	}
}

void MFVarSetFloat (int id,int itemID,double val) {
	MFVariable_t *var;

	if (((var = MFVarGetByID (id)) == (MFVariable_t *) NULL) || (itemID < 0) || (itemID >= var->Header.ItemNum)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%d,%d] in: MFVarSetFloat ()\n",id,itemID);
		return;
	}

	var->Set = true;
	if (var->Flux) val = val * (double) var->NStep;
	switch (var->Header.DataType) {
		case MFByte:	((char *)   var->Data) [itemID] = (char)  val; break;
		case MFShort:	((short *)  var->Data) [itemID] = (short) val; break;
		case MFInt:		((int *)    var->Data) [itemID] = (int)   val; break;
		case MFFloat:	((float *)  var->Data) [itemID] = (float) val; break;
		case MFDouble:	((double *) var->Data) [itemID] =         val; break;
		default:
			CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%s,%d] type [%d] in %s:%d\n",var->Name, itemID, var->Header.DataType,__FILE__,__LINE__);
			break;
	}
}

double MFVarGetFloat (int id,int itemID,double missingVal) {
	double val;
	MFVariable_t *var;

	if (((var = MFVarGetByID (id)) == (MFVariable_t *) NULL) || (itemID < 0) || (itemID >= var->Header.ItemNum)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%d,%d] in: MFVarGetFloat ()\n",id,itemID);
		return (MFDefaultMissingFloat);
	}
	if ((itemID == 0) && (var->Set != true)) CMmsgPrint (CMmsgWarning,"Warning: Unset variable [%s]!\n",var->Name);
	if (_MFVarTestMissingVal (var,itemID)) return (missingVal);

	switch (var->Header.DataType) {
		case MFByte:	val = (double) (((char *)   var->Data) [itemID]); break;
		case MFShort:	val = (double) (((short *)  var->Data) [itemID]); break;
		case MFInt:		val = (double) (((int *)    var->Data) [itemID]); break;
		case MFFloat:	val = (double) (((float *)  var->Data) [itemID]); break;
		case MFDouble:	val = (double) (((double *) var->Data) [itemID]); break;
		default:
			CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%s,%d] type [%d] in %s:%d\n",var->Name, itemID, var->Header.DataType,__FILE__,__LINE__);
			return (MFDefaultMissingFloat);
	}
 	return (var->Flux ? val = val / (double) var->NStep : val);
}

void MFVarSetInt (int id,int itemID,int val) {
	MFVariable_t *var;

	if (((var = MFVarGetByID (id)) == (MFVariable_t *) NULL) || (itemID < 0) || (itemID >= var->Header.ItemNum)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%d,%d] in: %s:%d\n",id,itemID,__FILE__,__LINE__);
		return;
	}

	var->Set = true;
	if (var->Flux) val = val * var->NStep;
	switch (var->Header.DataType) {
		case MFByte:	((char *)   var->Data) [itemID] = (char)   val;	break;
		case MFShort:	((short *)  var->Data) [itemID] = (short)  val;	break;
		case MFInt:		((int *)    var->Data) [itemID] = (int)    val;	break;
		case MFFloat:	((float *)  var->Data) [itemID] = (float)  val;	break;
		case MFDouble:	((double *) var->Data) [itemID] = (double) val;	break;
		default:
			CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%s,%d] type [%d] in %s:%d\n",var->Name, itemID, var->Header.DataType,__FILE__,__LINE__);
			break;
	}
}

int MFVarGetInt (int id,int itemID, int missingVal) {
	int val;
	MFVariable_t *var;

	if (((var = MFVarGetByID (id)) == (MFVariable_t *) NULL) || (itemID < 0) || (itemID >= var->Header.ItemNum)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%d,%d] in: %s:%d\n",id,itemID,__FILE__,__LINE__);
		return (MFDefaultMissingInt);
	}

	if (var->Set != true) CMmsgPrint (CMmsgWarning,"Warning: Unset variable %s\n",var->Name);
	if (_MFVarTestMissingVal (var,itemID)) return (missingVal);
	
	switch (var->Header.DataType) {
		case MFByte:	val = (int) (((char *)   var->Data) [itemID]); break;
		case MFShort:	val = (int) (((short *)  var->Data) [itemID]); break;
		case MFInt:		val = (int) (((int *)    var->Data) [itemID]); break;
		case MFFloat:	val = (int) (((float *)  var->Data) [itemID]); break;
		case MFDouble:	val = (int) (((double *) var->Data) [itemID]); break;
		default:
			CMmsgPrint (CMmsgAppError,"Error: Invalid variable [%s,%d] type [%d] in %s:%d\n",var->Name, itemID, var->Header.DataType,__FILE__,__LINE__);
			return (MFDefaultMissingInt);
	}
	return (var->Flux ? (val / var->NStep) : val);
}	

int MFVarItemSize (int type) {
	switch (type) {
		case MFByte:	return (sizeof (char));
		case MFShort:	return (sizeof (short));
		case MFInt:		return (sizeof (int));
		case MFFloat:	return (sizeof (float));
		case MFDouble:	return (sizeof (double));
		default:
			CMmsgPrint (CMmsgAppError,"Error: Invalid type [%d] in: %s:%d\n",type,__FILE__,__LINE__);
			break;
	}
	return (CMfailed);
}

bool MFVarReadHeader (MFVarHeader_t *header,FILE *inFile) {

	if ((inFile == (FILE *) NULL) || (fread (header,sizeof (MFVarHeader_t),1,inFile) != 1)) return (false);

	if (header->Swap != 1) {
		MFSwapHalfWord (&(header->DataType));
		MFSwapWord     (&(header->ItemNum));
		switch (header->DataType) {
			case MFByte:
			case MFShort:
			case MFInt:		MFSwapWord     (&(header->Missing.Int));   break;
			case MFFloat:
			case MFDouble:	MFSwapLongWord (&(header->Missing.Float)); break;
		}
	}
	return (true);
}

bool MFVarWriteHeader (MFVarHeader_t *header,FILE *outFile) {

	if (outFile == (FILE *) NULL) return (false);

	header->Swap = 1;
	if (fwrite (header,sizeof (MFVarHeader_t),1,outFile) != 1) {
		CMmsgPrint (CMmsgSysError,"Header writing error in: %s:%d\n",__FILE__,__LINE__);
		return (false);
	}
	return (true);
}
