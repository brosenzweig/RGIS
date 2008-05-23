/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

DBImpARC.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>

int DBImportARCPoint (DBObjData *,const char *);
int DBImportARCLine	(DBObjData *,const char *);
int DBImportARCPoly	(DBObjData *,const char *);

int DBImportARCVector (DBObjData *vecData,const char *arcCov,const char *nameFieldStr,const char *symbolFieldStr)

	{
	char symbolName [DBStringLength + 1];
	DBObjTable *items 	= vecData->Table (DBrNItems);
	DBObjTable *symbols	= vecData->Table (DBrNSymbols);
	DBObjTableField *nameFLD = NULL, *symbolResFLD, *symbolFLD = NULL;
	DBObjTableField *symbolIDFLD = symbols->Field (DBrNSymbolID);
	DBObjTableField *foregroundFLD = symbols->Field (DBrNForeground);
	DBObjTableField *backgroundFLD = symbols->Field (DBrNBackground);
	DBObjTableField *styleFLD = symbols->Field (DBrNStyle);
	DBObjRecord *record;
	DBObjRecord *symbolRec;
		
	if (DBInfoGetFields (items,DBInfoFileName (arcCov,vecData->Type ())) == DBSuccess)
		if (DBInfoGetTable (items,DBInfoFileName (arcCov,vecData->Type ())) == DBFault) return (DBFault);
	switch (vecData->Type ())
		{
		case DBTypeVectorPoint:
			if (DBImportARCPoint (vecData,arcCov) == DBFault)	return (DBFault); else	break;
		case DBTypeVectorLine:
			if (DBImportARCLine	(vecData,arcCov) == DBFault)	return (DBFault);	else	break;
		case DBTypeVectorPolygon:
			if (DBImportARCPoly	(vecData,arcCov) == DBFault)	return (DBFault);	else	break;
		default: 	return (DBFault);
		}

	nameFLD 	= nameFieldStr 	!= NULL ? items->Field (nameFieldStr)		: (DBObjTableField  *) NULL;
	symbolFLD = symbolFieldStr	!= NULL ? items->Field (symbolFieldStr)	: (DBObjTableField  *) NULL;
 	symbolResFLD = items->Field (DBrNSymbol);
 	if (symbolFLD != NULL)
 		{
 		for (record = items->First ();record != (DBObjRecord *) NULL;record = items->Next ())
 			{
			switch (symbolFLD->Type ())
				{
				case DBTableFieldString:
					if ((symbolRec = (DBObjRecord *) symbols->Item (symbolFLD->String (record))) == NULL)
						{
						if ((symbolRec = symbols->Add (symbolFLD->String (record))) == NULL)
							{ fprintf (stderr,"Symbol Object Creation Error in: DBImportARC ()\n"); return (DBFault); }
						symbolIDFLD->Int (symbolRec,symbolRec->RowID ());
						foregroundFLD->Int (symbolRec,1);
						backgroundFLD->Int (symbolRec,0);
						styleFLD->Int (symbolRec,0);
						}
					break;
				case DBTableFieldInt:
					sprintf (symbolName,"Symbol:%5d",symbolFLD->Int (record));
					if ((symbolRec = (DBObjRecord *) symbols->Item (symbolName)) == NULL)
						{
						if ((symbolRec = symbols->Add (symbolName)) == NULL)
							{ fprintf (stderr,"Symbol Object Creation Error in: DBImportARC ()\n"); return (DBFault); }
						symbolIDFLD->Int (symbolRec,symbolFLD->Int (record));
						foregroundFLD->Int (symbolRec,1);
						backgroundFLD->Int (symbolRec,0);
						styleFLD->Int (symbolRec,0);
						}
					break;
				default:
					fprintf (stderr,"Invalid Field Type in: DBImportARC ()\n");
					break;
				}
			symbolResFLD->Record (record,symbolRec);
			}
		}
	else
		{
		symbols->Add ("Default Symbol");
		if ((symbolRec = (DBObjRecord *) symbols->Item ()) == (DBObjRecord *) NULL)
			{ fprintf (stderr,"Symbol Object Creation Error in: DBImportARC ()\n"); return (DBFault); }
		symbolIDFLD->Int (symbolRec,0);
		foregroundFLD->Int (symbolRec,1);
		backgroundFLD->Int (symbolRec,0);
		styleFLD->Int (symbolRec,0);
		for (record = items->First ();record != (DBObjRecord *) NULL;record = items->Next ())
			symbolResFLD->Record (record,symbolRec);
		}
	if (nameFLD != NULL)
		for (record = items->First ();record != (DBObjRecord *) NULL;record = items->Next ())
			record->Name (nameFLD->String (record));
	return (DBSuccess);
	}
