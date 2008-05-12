/******************************************************************************

GHAAS RiverGIS Libarary V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

RGlibGenFuncs.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>
#include <RG.H>

DBInt RGlibGenFuncFieldCompare (DBObjTable *table,char *f0Text,char *f1Text,char *rText,DBInt diffMethod)

	{
	DBInt intVal, ret, i, rowID;
	DBFloat val [2];
	DBObjTableField *field [2];
	DBObjTableField *result;
	DBObjRecord *record;
		
	field [0] = table->Field (f0Text);
	field [1] = table->Field (f1Text);
	result = table->Field (rText);
	if ((field [0] == (DBObjTableField *) NULL) || (field [1] == (DBObjTableField *) NULL))
		{ fprintf (stderr,"Invalid Compare Fields in: RGISFuncFieldCompare ()\n"); return (DBFault); }
	if (result == (DBObjTableField *) NULL)
		table->AddField (result = new DBObjTableField (rText,DBTableFieldFloat,"%10.3f",sizeof (DBFloat4)));
	if (diffMethod > 0) result->Format ("%6.2f");
	for (rowID = 0;rowID < table->ItemNum ();++rowID)
		{
		record = table->Item (rowID);
		if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle)
			result->Float (record,result->FloatNoData ());
		else
			{
			for (i = 0;i < 2;++i)
				if (field [i]->Type () == DBTableFieldInt) 
					{
					intVal = field [i]->Int (record);
					ret = intVal == field [i]->IntNoData ();
					val [i] = (DBFloat) intVal;
					}
				else
					{
					val [i] = field [i]->Float (record);
					ret = CMmathEqualValues (val [i],field [i]->FloatNoData ());
					}
			if (ret) result->Float (record,result->FloatNoData ());
			else switch (diffMethod)
				{
				default:	result->Float (record, val [0] - val [1]);	break;
				case 1: 	result->Float (record,(val [0] - val [1]) * 100.0 /  val [0]);	break;
				case 2: 	result->Float (record,(val [0] - val [1]) * 100.0 / (val [0] > val [1] ? val [0] : val [1]));	break;
				}
			}
		}
	return (DBSuccess);
	}

DBInt RGlibGenFuncFieldCalculate (DBObjTable *table,char *f0Text,char *f1Text,char *rText,DBInt oper)

	{
	DBInt intVal, ret = false, i, rowID;
	DBFloat val [2];
	DBObjTableField *field [2];
	DBObjTableField *result;
	DBObjRecord *record;
		
	field [0] = table->Field (f0Text);
	field [1] = table->Field (f1Text);
	result = table->Field (rText);

	if ((field [0] == (DBObjTableField *) NULL) && (sscanf (f0Text,"%lf",val) != 1)) 		return (DBFault);
	if ((field [1] == (DBObjTableField *) NULL) && (sscanf (f1Text,"%lf",val + 1) != 1))	return (DBFault);
	 
	if (result == (DBObjTableField *) NULL)
		table->AddField (result = new DBObjTableField (rText,DBTableFieldFloat,"%10.3f",sizeof (DBFloat4)));
	if (field [0] != (DBObjTableField *) NULL)
		result->Format (field [0]->Format ());
	else if (field [1] != (DBObjTableField *) NULL)
		result->Format (field [1]->Format ());

	for (rowID = 0;rowID < table->ItemNum ();++rowID)
		{
		record = table->Item (rowID);
		if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle)
			result->Float (record,result->FloatNoData ());
		else
			{
			for (i = 0;i < 2;++i)
				if (field [i] != (DBObjTableField *) NULL)
					{
					ret = false;
					if (field [i]->Type () == DBTableFieldInt) 
						{
						intVal = field [i]->Int (record);
						val [i] = (DBFloat) intVal;
						ret = intVal == field [i]->IntNoData ();
						}
					else
						{
						val [i] = field [i]->Float (record);
						ret = CMmathEqualValues (val [i],field [i]->FloatNoData ());
						}
					if (ret)
						{
						result->Float (record,result->FloatNoData ());
						goto Continue;
						}
					}
			if ((oper == DBMathOperatorDiv) && (val [1] == 0))
				result->Float (record,result->FloatNoData ());
			else
				switch (oper)
					{
					case DBMathOperatorAdd:	result->Float (record, val [0] + val [1]);	break;
					case DBMathOperatorSub:	result->Float (record, val [0] - val [1]);	break;
					case DBMathOperatorMul:	result->Float (record, val [0] * val [1]);	break;
					case DBMathOperatorDiv:	result->Float (record, val [0] / val [1]);	break;
					default:	fprintf (stderr,"Invalid Operand in RGISFuncFieldCalculate ()\n");	break;
					}
Continue:
			continue;
			}
		}
	return (DBSuccess);
	}

DBInt RGlibGenFuncTopoAccum (DBObjTable *table,char *nextStnFldName,char *srcFldName,char *dstFldName)

	{
	DBInt rowID;
	DBObjTableField *srcField, *dstField, *nextField;
	DBObjRecord *record, *nextRec;

	if (((srcField = table->Field (srcFldName)) == (DBObjTableField *) NULL) ||
		 ((nextField =table->Field (nextStnFldName)) == (DBObjTableField *) NULL))
		return (DBFault);
	if ((dstField = table->Field (dstFldName)) == (DBObjTableField *) NULL)
		{
		dstField = new DBObjTableField (*srcField);
		dstField->Name (dstFldName);
		table->AddField (dstField);
		}
	for (rowID = 0;rowID < table->ItemNum ();++rowID)
		{
		record = table->Item (rowID);
		dstField->Float (record,srcField->Float (record));
		record->Flags (DBObjectFlagSelected,DBClear);
		}
	for (rowID = 0;rowID < table->ItemNum ();++rowID)
		{
		record = table->Item (rowID);
		DBPause (rowID * 100 / table->ItemNum ());
		if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle)
			{
			dstField->Float (record,dstField->FloatNoData ());
			continue;
			}
		for (nextRec = table->Item (nextField->Int (record) - 1);
			  nextRec != (DBObjRecord *) NULL;
			  nextRec = table->Item (nextField->Int (nextRec) - 1))
			{
			if (nextRec == record) { record->Flags (DBObjectFlagSelected,DBSet); break; }
			if ((nextRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle)
				{
				if (CMmathEqualValues (dstField->Float (nextRec),dstField->FloatNoData ()) ||
					 CMmathEqualValues (srcField->Float (record),srcField->FloatNoData ()))
			 		dstField->Float (nextRec,dstField->FloatNoData ());
				else
			 		dstField->Float (nextRec,dstField->Float (nextRec) + srcField->Float (record));
				}
			}
		}
	return (DBSuccess);
	}

DBInt RGlibGenFuncTopoSubtract (DBObjTable *table,char *nextStnFldName,char *srcFldName,char *dstFldName)

	{
	DBInt rowID;
	DBObjTableField *srcField, *dstField, *nextField;
	DBObjRecord *record, *nextRec;

	if (((srcField = table->Field (srcFldName)) == (DBObjTableField *) NULL) ||
		 ((nextField =table->Field (nextStnFldName)) == (DBObjTableField *) NULL))
		return (DBFault);
	if ((dstField = table->Field (dstFldName)) == (DBObjTableField *) NULL)
		{
		dstField = new DBObjTableField (*srcField);
		dstField->Name (dstFldName);
		table->AddField (dstField);
		}
	for (rowID = 0;rowID < table->ItemNum ();++rowID)
		{
		record = table->Item (rowID);
		dstField->Float (record,srcField->Float (record));
		}
	for (rowID = 0;rowID < table->ItemNum ();++rowID)
		{
		record = table->Item (rowID);
		DBPause (rowID * 100 / table->ItemNum ());
		if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle)
			{
			dstField->Float (record,dstField->FloatNoData ());
			continue;
			}
		for (nextRec = table->Item (nextField->Int (record) - 1);
			  nextRec != (DBObjRecord *) NULL;
			  nextRec = table->Item (nextField->Int (nextRec) - 1))
			if ((nextRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle)
				{
				if (CMmathEqualValues (dstField->Float (nextRec),dstField->FloatNoData ()) ||
					 CMmathEqualValues (srcField->Float (record),srcField->FloatNoData ()))
			 		dstField->Float (nextRec,dstField->FloatNoData ());
				else
					dstField->Float (nextRec,dstField->Float (nextRec) - srcField->Float (record));
				break;
				}
		}
	return (DBSuccess);
	}

int RGlibGenFuncSymbolField (DBObjData *data, const char *fieldName)
	{
	DBInt recID;
	char symbolName [DBStringLength + 1];
	DBObjTable *table = data->Table (DBrNItems);
	DBObjTable *symbols = data->Table (DBrNSymbols);
	DBObjRecord *record, *symbolRec;
	DBObjTableField *field;
	DBObjTableField *symbolFLD;
	DBObjTableField *symbolIDFLD;
	DBObjTableField *foregroundFLD;
	DBObjTableField *backgroundFLD;
	DBObjTableField *styleFLD;

	if (table == (DBObjTable *) NULL) return (DBFault);
	if ((field = table->Field (fieldName)) == (DBObjTableField *) NULL)
	{ fprintf (stderr, "Invalid field name in: RGlibGenFuncSymbolField ()\n");       return (DBFault); }
	if ((symbolFLD = table->Field (DBrNSymbol)) == (DBObjTableField *) NULL)
	{ fprintf (stderr, "Missing symbol field in: RGlibGenFuncSymbolField ()\n");     return (DBFault); }
	if (symbols == (DBObjTable *) NULL)
	{ fprintf (stderr, "Missing symbol table in: RGlibGenFuncSymbolField ()\n");     return (DBFault); }
	if ((symbolIDFLD   = symbols->Field (DBrNSymbolID))   == (DBObjTableField *) NULL)
	{ fprintf (stderr, "Missing symbolID field in: RGlibGenFuncSymbolField ()\n");   return (DBFault); }
	if ((foregroundFLD = symbols->Field (DBrNForeground)) == (DBObjTableField *) NULL)
	{ fprintf (stderr, "Missing foreground field in: RGlibGenFuncSymbolField ()\n"); return (DBFault); }
	if ((backgroundFLD = symbols->Field (DBrNBackground)) == (DBObjTableField *) NULL)
	{ fprintf (stderr, "Missing background field in: RGlibGenFuncSymbolField ()\n"); return (DBFault); }
	if ((styleFLD      = symbols->Field (DBrNStyle))      == (DBObjTableField *) NULL)
	{ fprintf (stderr, "Missing style field in: RGlibGenFuncSymbolField ()\n");      return (DBFault); }

	symbols->DeleteAll ();
	for (recID = 0;recID < table->ItemNum (); ++recID)
		{
		record = table->Item (recID);
		DBPause (record->RowID () * 100 / table->ItemNum ());
		
		if (field->Type () == DBTableFieldString) sprintf (symbolName,"%s",field->String (record));
		else	sprintf (symbolName,"Symbol:%03d",field->Int (record));
		if ((symbolRec = (DBObjRecord *) symbols->Item (symbolName)) == (DBObjRecord *) NULL)
			{
			if ((symbolRec = symbols->Add (symbolName)) == NULL)
				{ fprintf (stderr,"Symbol Object Creation Error in: RGlibGenFuncSymbolField ()\n"); return (DBFault); }
			symbolIDFLD->Int (symbolRec,field->Type () == DBTableFieldString ? symbolRec->RowID () : field->Int (record));
			foregroundFLD->Int (symbolRec,1);
			backgroundFLD->Int (symbolRec,0);
			styleFLD->Int (symbolRec,0);
			}
		symbolFLD->Record (record,symbolRec);
		}
	return (DBSuccess);
	}
