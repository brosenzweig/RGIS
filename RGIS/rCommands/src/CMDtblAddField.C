/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

CMDtblAddField.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	DBInt type = DBFault, length = DBFault, dWidth = DBFault, dDecimals = DBFault;
	DBObjData *data;
	char *tableName = (char *) NULL, *fieldName = (char *) NULL, *newName = (char *) NULL;
	DBObjTable *table;
	DBObjTableField *field;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-a","--table"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing table name!\n");   return (CMfailed); }
			tableName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-f","--field"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing field name!\n");   return (CMfailed); }
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
				{ CMmsgPrint (CMmsgUsrError,"Missing field type!\n");   return (CMfailed); }
			if ((type = CMoptLookup (types,argv [argPos],true)) == DBFault)
					{ CMmsgPrint (CMmsgUsrError,"Invalid field type!\n"); return (CMfailed); }
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
				{ CMmsgPrint (CMmsgUsrError,"Missing decimals!\n");     return (CMfailed); }
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
			CMmsgPrint (CMmsgInfo,"     -y,--type      [field type]\n");
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

	if (type == DBFault) { type = DBTableFieldString; }
	if (length == DBFault)
		switch (type)
			{
			default:
			case DBTableFieldString:	length = 16;	break;
			case DBTableFieldInt:		length =  2;	break;
			case DBTableFieldFloat:		length =  4;	break;
			case DBTableFieldDate:		length = sizeof (DBDate);	break;
			}
	else
		switch (type)
			{
			default:
			case DBTableFieldString:	break;
			case DBTableFieldInt:		
				if ((length != 1) && (length != 2) && (length != 4))
					{ CMmsgPrint (CMmsgUsrError,"Invalid field length\n"); delete data; return (CMfailed); }
				break;
			case DBTableFieldFloat:
				if ((length != 4) && (length != 8))
					{ CMmsgPrint (CMmsgUsrError,"Invalid field length\n"); delete data; return (CMfailed); }
				break;
			case DBTableFieldDate:
				if (length != sizeof (DBDate))
					{ CMmsgPrint (CMmsgUsrError,"Invalid field length\n"); delete data; return (CMfailed); }
				break;
			}
	if (dWidth == DBFault)
		switch (type)
			{
			default:
			case DBTableFieldString:	dWidth = length;	break;
			case DBTableFieldInt:		dWidth =      8;	break;
			case DBTableFieldFloat:		dWidth =     10;	break;
			case DBTableFieldDate:		dWidth =     10;	break;
			}
	if (dDecimals != DBFault)
		{
		if (type != DBTableFieldFloat)
			{ CMmsgPrint (CMmsgUsrError,"Invalid field type for decimals!\n"); delete data; return (CMfailed); }
		dDecimals = 2;
		}
	field = new DBObjTableField (fieldName,length);	
	field->Type (type);
	field->FormatWidth (dWidth);
	if (type == DBTableFieldFloat) field->FormatDecimals (dDecimals);
	table->AddField (field);

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	delete data;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
