/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

CMDtblMergeCoord.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	DBInt recID;
	int deg, min, sec;
	char coordStr [DBStringLength];
	char *tableName = (char *) NULL;
	char *fieldName = (char *) NULL;
	char *degFieldName = (char *) NULL;
	char *minFieldName = (char *) NULL;
	char *secFieldName = (char *) NULL;
	DBObjData *data;
	DBObjTable *table;
	DBObjTableField *dstField;
	DBObjTableField *degField = (DBObjTableField *) NULL;
	DBObjTableField *minField = (DBObjTableField *) NULL;
	DBObjTableField *secField = (DBObjTableField *) NULL;
	DBObjRecord *record;

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
		if (CMargTest (argv [argPos],"-d","--degree"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing degree field name!\n"); return (CMfailed); }
			degFieldName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-m","--minute"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing minutefield name!\n"); return (CMfailed); }
			minFieldName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-s","--second"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing second field name!\n"); return (CMfailed); }
			secFieldName = argv [argPos];
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
			CMmsgPrint (CMmsgInfo,"     -d,--degree    [degree field]\n");
			CMmsgPrint (CMmsgInfo,"     -m,--minute    [minute field]\n");
			CMmsgPrint (CMmsgInfo,"     -s,--second    [secon field]\n");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose\n");
			CMmsgPrint (CMmsgInfo,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && ((int) strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	data = new DBObjData ();
	if (((argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Read (argv [1]) : data->Read (stdin)) == DBFault)
		{ delete data; return (CMfailed); }

	if (tableName == (char *) NULL) tableName = DBrNItems;

	if ((table = data->Table (tableName)) == (DBObjTable *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Invalid table!\n"); delete data; return (CMfailed); }

	if (degFieldName != (char *) NULL)
		{
		if ((degField = table->Field (degFieldName)) == (DBObjTableField *) NULL)
			{ CMmsgPrint (CMmsgUsrError,"Invalid degree field [%s]!\n",degFieldName); delete data; return (CMfailed); }
		}
	
	if (minFieldName != (char *) NULL)
		{
		if ((minField = table->Field (minFieldName)) == (DBObjTableField *) NULL)
			{ CMmsgPrint (CMmsgUsrError,"Invalid min field [%s]!\n",minFieldName); delete data; return (CMfailed); }
		}
	
	if (secFieldName != (char *) NULL)
		{
		if (minField == (DBObjTableField *) NULL)
			{ CMmsgPrint (CMmsgUsrError,"Minute field is not set!\n"); delete data; return (CMfailed); }
		if ((secField = table->Field (secFieldName)) == (DBObjTableField *) NULL)
			{ CMmsgPrint (CMmsgUsrError,"Invalid second field [%s]",secFieldName); delete data; return (CMfailed); }
		}

	if (fieldName == (char *) NULL) fieldName = "Coord[ddd:mm\'ss\"]";
	if ((dstField = table->Field (fieldName)) == (DBObjTableField *) NULL)
		{
		dstField = new DBObjTableField (fieldName,DBTableFieldString,"%s",16,false);
		table->AddField (dstField);
		}

	for (recID = 0;recID < table->ItemNum ();++recID)
		{
		record = table->Item (recID);
	
		if ((degField == (DBObjTableField *) NULL) ||
			 ((deg = degField->Int  (record)) == degField->IntNoData ()))
			strcpy (coordStr,"");
		else if ((minField == (DBObjTableField *) NULL) ||
					((min = minField->Int (record)) == minField->IntNoData ()))
			sprintf (coordStr,"%4d",deg);
		else if ((secField == (DBObjTableField *) NULL) ||
					((sec = secField->Int (record)) == secField->IntNoData ()))
			sprintf (coordStr,"%4d:%02d",deg,min);
		else	sprintf (coordStr,"%4d:%02d\'%02d\"",deg,min,sec);
		dstField->String (record,coordStr);
		}

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	delete data;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
