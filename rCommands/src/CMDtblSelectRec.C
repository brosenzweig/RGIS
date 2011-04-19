/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDtblSelectRec.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

int main (int argc,char *argv [])
	{
	int argPos, argNum = argc, ret, verbose = false;
	DBInt fromSelection = false, selectMode = true, recID;
	char *tableName = (char *) NULL;
	char *expr = (char *) NULL;
	DBMathOperand *operand;
	DBObjectLIST<DBObject> *variables = new DBObjectLIST<DBObject> ("Variables");
	DBObjData *data;
	DBObjTable *table;
	DBObjRecord *record;

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
		if (CMargTest (argv [argPos],"-c","--condition"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing expression!\n"); return (CMfailed); }
			if (expr != (char *) NULL) { CMmsgPrint (CMmsgUsrError,"Expression is already set\n"); return (CMfailed); } 
			expr = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-f","--from"))
			{
			int modeCodes [] = { false, true };
			const char *modes [] = { "all", "selection", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing selection mode!\n"); return (CMfailed); }
			if ((fromSelection = CMoptLookup (modes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid selection mode!\n"); return (CMfailed); }
			fromSelection = modeCodes [fromSelection];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-m","--mode"))
			{
			int modeCodes [] = { true, false };
			const char *modes [] = { "select", "unselect", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing selection mode!\n"); return (CMfailed); }
			if ((selectMode = CMoptLookup (modes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid selection mode!\n"); return (CMfailed); }
			selectMode = modeCodes [selectMode];
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
			CMmsgPrint (CMmsgInfo,"     -a,--table     [ table name ]\n");
			CMmsgPrint (CMmsgInfo,"     -c,--condition [ fieldname expression ]\n");
			CMmsgPrint (CMmsgInfo,"     -f,--from      [ [all] | selection ]\n");
			CMmsgPrint (CMmsgInfo,"     -m,--mode      [ [select] | unselect ]\n");
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

	if (expr != (char *) NULL)
		{
		if (operand->Configure (table->Fields()) == DBFault)
			{ delete data; delete operand; return (CMfailed); }
	
		for (recID = 0;recID < table->ItemNum ();++recID)
			{
			record = table->Item (recID);
			if (fromSelection && ((record->Flags () & DBObjectFlagSelected) != DBObjectFlagSelected))
				continue;
			if ((ret = operand->Int (record)) == true)
			 	record->Flags (DBObjectFlagSelected,selectMode ? DBSet : DBClear);
			else if (selectMode)	record->Flags (DBObjectFlagSelected,DBClear);
			}
		}
	else
		{
		for (recID = 0;recID < table->ItemNum ();++recID)
			{
			record = table->Item (recID);
			if (fromSelection && ((record->Flags () & DBObjectFlagSelected) != DBObjectFlagSelected))
				continue;
			record->Flags (DBObjectFlagSelected,selectMode ? DBSet : DBClear);
			}
		}

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	if (expr != (char *) NULL) delete operand;
	delete data;
	delete variables;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
