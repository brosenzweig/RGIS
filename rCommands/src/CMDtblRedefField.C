/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDtblRedefField.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	DBInt type = DBFault, length = DBFault, dWidth = DBFault, dDecimals = DBFault;
	DBObjData *data;
	char *tableName = (char *) NULL, *fieldName = (char *) NULL, *newName = (char *) NULL;
	DBObjTable *table;
	DBObjTableField *field, *newField;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-a","--table"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing table name!\n"); return (CMfailed); }
			tableName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-f","--field"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing field name!\n"); return (CMfailed); }
			fieldName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-r","--rename"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing new field name!\n"); return (CMfailed); }
			newName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-y","--type"))
			{
			DBInt typeCodes [] = {DBTableFieldString, DBTableFieldInt, DBTableFieldFloat, DBTableFieldDate };
			const char *types [] = { "string", "int", "float", "date", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing field type!\n"); return (CMfailed); }
			if ((type = CMoptLookup (types,argv [argPos],true)) == DBFault)
					{ CMmsgPrint (CMmsgUsrError,"Invalid field type!\n");				return (CMfailed); }
			type = typeCodes [type];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-l","--length"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing field length!\n"); return (CMfailed); }
			if (sscanf (argv [argPos],"%d",&length) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid field length!\n"); return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-w","--width"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing format width!\n"); return (CMfailed); }
			if (sscanf (argv [argPos],"%d",&dWidth) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid format width!\n"); return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-e","--decimals"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing decimals!\n");   return (CMfailed); }
			if (sscanf (argv [argPos],"%d",&dDecimals) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid number of decimals!\n");	return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-V","--verbose"))
			{
			verbose = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			CMmsgPrint (CMmsgInfo,"%s [options] <input file> <output file>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -a,--table     [table name]\n");
			CMmsgPrint (CMmsgInfo,"     -f,--field     [field name]\n");
			CMmsgPrint (CMmsgInfo,"     -r,--rename    [new field name]\n");
			CMmsgPrint (CMmsgInfo,"     -y,--type      [string|int|float|date]\n");
			CMmsgPrint (CMmsgInfo,"     -l,--length    [field length]\n");
			CMmsgPrint (CMmsgInfo,"     -w,--width     [display width]\n");
			CMmsgPrint (CMmsgInfo,"     -e,--decimals  [display decimals]\n");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose\n");
			CMmsgPrint (CMmsgInfo,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	if (tableName == (char *) NULL) tableName = DBrNItems;
	if (fieldName == (char *) NULL) 
		{ CMmsgPrint (CMmsgUsrError,"Missing field name!\n"); return (CMfailed); }

	data = new DBObjData ();
	if (((argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Read (argv [1]) : data->Read (stdin)) == DBFault)
		{ delete data; return (CMfailed); }

	if ((table = data->Table (tableName)) == (DBObjTable *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Invalid table: %s!\n",tableName); delete data; return (CMfailed); }
	if ((field = table->Field (fieldName)) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Invalid field: %s!\n",fieldName); delete data; return (CMfailed); }
	if (DBTableFieldIsOptional (field) != true)
		{ CMmsgPrint (CMmsgUsrError,"Required field!\n"); delete data; return (CMfailed); }
	if (newName != (char *) NULL) field->Name (newName);
	if ((type != DBFault) && (type != field->Type ()))
		{
		newField = new DBObjTableField (*field);
		newField->Type (type);
		if (length == DBFault)
			switch (type)
				{
				default:
				case DBVariableString:
					for (length = 0;(0x01 << length) <= field->FormatWidth ();length++);
					length = 0x01 << length;
					break;
				case DBVariableInt:
					if ((length == DBFault) || (length != sizeof (DBInt)) || (length != sizeof (DBShort)))
						length = sizeof (DBInt);
					break;
				case DBVariableFloat:
					if ((length == DBFault) || (length != sizeof (DBFloat4)) || (length != sizeof (DBFloat)))
						length = sizeof (DBFloat);
					break;
				case DBVariableDate:
					if ((length == DBFault) || (length != sizeof (DBDate)))
						length = sizeof (DBDate);
				break;
				}
		newField->Length (length);
		table->RedefineField (field,newField);
		delete newField;
		}
	else if (length != DBFault)
		{
		newField = new DBObjTableField (*field);
		newField->Length (length);
		table->RedefineField (field,newField);
		delete newField;
		}
	if (dWidth != DBFault) field->FormatWidth (dWidth);
	if (dDecimals != DBFault)
		{
		if (type != DBTableFieldFloat)
			{
			CMmsgPrint (CMmsgUsrError,"Invalid field type for decimals!\n");
			delete data;
			return (CMfailed);
			}
		else field->FormatDecimals (dDecimals);
		}

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	delete data;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
