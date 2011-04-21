/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBExpARCPnt.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>

int DBExportARCGenPoint (DBObjData *data,char *fileName)

	{
	FILE *file;
	DBCoordinate coord;
	DBObjTable *items = data->Table (DBrNItems);
	DBObjTableField *coordField = items->Field (DBrNCoord);
	DBObjRecord *pntRec;
	
	if ((file = fopen (fileName,"w")) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgSysError, "ARC Generate File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	for (pntRec = items->First ();pntRec != (DBObjRecord *) NULL;pntRec = items->Next ())
		{
		coord = coordField->Coordinate (pntRec);
		fprintf (file,"%d,%f,%f\n",pntRec->RowID () + 1,coord.X,coord.Y);
		}
	fprintf (file,"END\n");
	fclose (file);
	return (DBSuccess);
	}

int DBExportARCTableDef (DBObjData *data,char *tableName,char *fileName)

	{
	FILE *file;
	DBInt i, len, maxLen = 0;
	char fieldName [DBStringLength + 1];
	DBInt outWidth, decWidth;
	DBObjTable *table = data->Table (tableName);
	DBObjTableField *tableField;
	DBObjectLIST<DBObjTableField> *fields;
	DBObjRecord *itemRec;

	if (table == (DBObjTable *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Invalid Table in: %s %d",__FILE__,__LINE__); return (DBFault); }
	fields = table->Fields ();
	
	if ((file = fopen (fileName,"w")) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Table Definition File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	fprintf (file,"\"ID\" 4 4 B\n");
	for (itemRec = table->First ();itemRec != (DBObjRecord *) NULL;itemRec = table->Next ())
		{
		len = strlen (itemRec->Name ());
		maxLen = maxLen > len ? maxLen : len;
		}
	fprintf (file,"\"RecordName\" %d %d C\n",maxLen,maxLen);
	for (tableField = fields->First ();tableField != (DBObjTableField *) NULL;tableField = fields->Next ())
		if ((strcmp (tableField->Name (),DBrNSymbol) == 0) || (DBTableFieldIsVisible (tableField)))
			{
			strncpy (fieldName,tableField->Name (),sizeof (fieldName) - 1);
			for (i = 0;i < (DBInt) strlen (fieldName); ++i) if (fieldName [i] == ' ') fieldName [i] = '_';
			fprintf (file,"\"%s\" ",fieldName);
			switch (tableField->Type ())
				{
				case DBTableFieldTableRec:
						outWidth = DBStringLength;
						fprintf (file,"%d %d C\n",outWidth,outWidth);
						break;
				case DBTableFieldString:
						outWidth = tableField->Length ();
						fprintf (file,"%d %d C\n",outWidth,outWidth);
						break;
				case DBTableFieldInt:
						sscanf (tableField->Format () + 1,"%d",&outWidth);
						if ((tableField->Format ()) [strlen (tableField->Format ()) - 1] == 'X')
							fprintf (file,"%d %d C\n", outWidth, outWidth);
						else
							fprintf (file,"%d %d B\n",tableField->Length () > 1 ? tableField->Length () : 2,outWidth);
						break;
				case DBTableFieldFloat:
						sscanf (tableField->Format () + 1,"%d.%d",&outWidth,&decWidth);
						fprintf (file,"%d %d F %d\n",tableField->Length (),outWidth,decWidth);
						break;
				case DBTableFieldDate:
						outWidth = DBStringLength;
						fprintf (file,"%d %d C\n",outWidth,outWidth);
				default:
						CMmsgPrint (CMmsgAppError, "Invalid Field Type in: %s %d",__FILE__,__LINE__);
						break; 
				}
			}
			
	fclose (file);
	return (DBSuccess);
	}

int DBExportARCTableData (DBObjData *data,char *tableName,char *fileName)

	{
	FILE *file;
	DBObjTable *table = data->Table (tableName);
	DBObjTableField *tableField;
	DBObjectLIST<DBObjTableField> *fields;
	DBObjRecord *itemRec;
	
	if (table == (DBObjTable *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Invalid Table in: %s %d",__FILE__,__LINE__); return (DBFault); }
	fields = table->Fields ();
	if ((file = fopen (fileName,"w")) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }

	for (itemRec = table->First ();itemRec != (DBObjRecord *) NULL;itemRec = table->Next ())
		{
		fprintf (file,"%d,\"%s\"",itemRec->RowID () + 1,itemRec->Name ());
		for (tableField = fields->First ();tableField != (DBObjTableField *) NULL;tableField = fields->Next ())
			if ((strcmp (tableField->Name (),DBrNSymbol) == 0) || (DBTableFieldIsVisible (tableField)))
				switch (tableField->Type ())
					{
					case DBTableFieldTableRec:	
					case DBTableFieldString:	fprintf (file,",\"%s\"",tableField->String (itemRec));     break;
					case DBTableFieldInt:		fprintf (file,",%d",tableField->Int (itemRec));            break;
					case DBTableFieldFloat:		fprintf (file,",%f",tableField->Float (itemRec));          break;
					case DBTableFieldDate:		fprintf (file,",\"%s\"",tableField->String (itemRec));     break;
					default: CMmsgPrint (CMmsgAppError, "Invalid Field Type in: %s %d",__FILE__,__LINE__); break;
					}
		fprintf (file,"\n");
		}
	fclose (file);
	return (DBSuccess);
	}
