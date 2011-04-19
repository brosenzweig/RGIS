/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDtblDeleteRec.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	DBInt fromSelection = false, recID;
	char *tableName = (char *) NULL;
	char *expr = (char *) NULL;
	DBMathOperand *operand;
	DBObjectLIST<DBObject> *variables = new DBObjectLIST<DBObject> ("Variables");
	DBObjData *data;
	DBObjTable *table, *saveTable, *groups = (DBObjTable *) NULL, *saveGroups;
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
		if (CMargTest (argv [argPos],"-c","--condition"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing expression!\n");   return (CMfailed); }
			if (expr != (char *) NULL) { CMmsgPrint (CMmsgUsrError,"Expression is already set\n"); return (CMfailed); } 
			expr = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-s","--selection"))
			{
			fromSelection = true;
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
			CMmsgPrint (CMmsgInfo,"     -c,--condition [expression]\n");
			CMmsgPrint (CMmsgInfo,"     -s,--selection\n");
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

	if (expr != (char *) NULL)
		{
		operand = new DBMathOperand (expr);
		if (operand->Expand (variables) == DBFault) return (CMfailed);
		}

	data = new DBObjData ();
	if (((argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Read (argv [1]) : data->Read (stdin)) == DBFault)
		{ delete data; delete operand; return (CMfailed); }

	if (tableName == (char *) NULL) tableName = DBrNItems;

	if ((table = data->Table (tableName)) == (DBObjTable *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Invalid table!\n"); delete data; delete operand; return (CMfailed); }

	saveTable = new DBObjTable (*table);
	table->DeleteAll ();
	if ((strcmp (tableName,DBrNItems) == 0) && ((groups = data->Table (DBrNGroups)) != (DBObjTable *) NULL))
		{
		saveGroups = new DBObjTable (*groups);
		groups->DeleteAll ();
		}

	if (expr != (char *) NULL)
		{
		if (operand->Configure (saveTable->Fields()) == DBFault)
			{ delete data; delete operand; return (CMfailed); }
	
		for (recID = 0;recID < saveTable->ItemNum ();++recID)
			{
			record = saveTable->Item (recID);
			if ((ret = operand->Int (record)) == true)
				{
				if (!fromSelection) continue;
				if ((record->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected) continue;
				}
			table->Add (new DBObjRecord (*record));
			if (groups != (DBObjTable *) NULL) groups->Add (new DBObjRecord (*(saveGroups->Item (recID))));
			}
		}
	else
		{
		if (fromSelection)
			{
			for (recID = 0;recID < saveTable->ItemNum ();++recID)
				{
				record = saveTable->Item (recID);
				if ((record->Flags () & DBObjectFlagSelected) == DBObjectFlagSelected)
					{
					table->Add (new DBObjRecord (*record));
					if (groups != (DBObjTable *) NULL)
						groups->Add (new DBObjRecord (*(saveGroups->Item (recID))));
					}
				}
			}
		}

	if (expr != (char *) NULL) delete operand;
	delete variables;
	delete saveTable;
	if (groups != (DBObjTable *) NULL) delete saveGroups;

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	delete data;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
