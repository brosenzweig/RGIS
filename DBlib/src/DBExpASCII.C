/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBExpASCII.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBif.H>

DBInt DBExportASCIITable (DBObjTable *table, const char *fileName)

	{
	DBInt ret;
	FILE *fp;
	if ((fp = fopen (fileName,"w")) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	ret = DBExportASCIITable (table,fp);
	fclose (fp);
	return (ret);
	}

DBInt DBExportASCIITable (DBObjTable *table, FILE *fp)

	{
	DBObjectLIST<DBObjTableField> *fields = table->Fields (); 
	DBObjRecord *record;
	DBObjTableField *field;
	
	fprintf (fp,"\"ID\"%c\"Name\"",DBASCIISeparator);
	for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
		if ((strcmp (field->Name (),DBrNSymbol) == 0) || DBTableFieldIsVisible (field))
			fprintf (fp,"%c\"%s\"",DBASCIISeparator,field->Name ());
	fprintf (fp,"\n");
	for (record = table->First ();record != (DBObjRecord *) NULL;record = table->Next ())
		{
		if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		fprintf (fp,"%d%c\"%s\"",record->RowID () + 1,DBASCIISeparator,record->Name ());
		for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
			if ((strcmp (field->Name (),DBrNSymbol) == 0) || DBTableFieldIsVisible (field))
				switch	(field->Type ())
					{
					default:
					case DBTableFieldString:
						fprintf (fp,"%c\"%s\"",DBASCIISeparator,field->String (record));
						break;
					case DBTableFieldInt:
						if (field->Int (record) == field->IntNoData ())
								fprintf (fp,"%c",DBASCIISeparator);
						else	fprintf (fp,"%c%d",DBASCIISeparator,field->Int (record));
						break;
					case DBTableFieldFloat:
						if (CMmathEqualValues (field->Float (record),field->FloatNoData ()))
								fprintf (fp,"%c",DBASCIISeparator);
						else	fprintf (fp,"%c%f",DBASCIISeparator,field->Float (record));
						break;
					case DBTableFieldDate:
						fprintf (fp,"%c\"%s\"",DBASCIISeparator,field->String(record));
						break;
					}
		fprintf (fp,"\n");
		}
	return (DBSuccess);
	}
