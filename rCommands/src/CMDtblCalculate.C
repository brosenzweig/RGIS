/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDtblCalculate.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>
#include <CMDexp.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	DBInt expr, expNum = 0, tmpVar;
	char *tableName = (char *) NULL;
	char *fieldName = (char *) NULL;
	CMDExpression **expressions = (CMDExpression **) NULL;
	DBInt recID;
	DBObjectLIST<DBObject> *variables = new DBObjectLIST<DBObject> ("Variables");
	DBObjData *data;
	DBObjTable *table;
	DBObjRecord *record;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-a","--table"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing table name!\n");  return (CMfailed); }
			tableName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-f","--field") || CMargTest (argv [argPos],"-t","--tmpfield"))
			{
			tmpVar = CMargTest (argv [argPos],"-t","--tmpfield") ? true : false;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing field name!\n");  return (CMfailed); }
			fieldName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing expression!\n");  return (CMfailed); }
			expressions = expNum < 1 ? (CMDExpression **) calloc (1,sizeof (CMDExpression *)) :
								(CMDExpression **) realloc (expressions,(expNum + 1) * sizeof (CMDExpression *));
			if (expressions == (CMDExpression **) NULL)
				{ perror ("Memory Allocation error!"); return (CMfailed); }
			expressions [expNum] = new CMDExpression (fieldName, argv [argPos],tmpVar);
			if ((expressions [expNum])->Expand (variables) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid Expression!\n"); return (CMfailed); }
			expNum++;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-V","--verbose"))
			{
			verbose = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			CMmsgPrint (CMmsgInfo,"%s [options] <input file> <output file>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -a,--table     [table name]\n");
			CMmsgPrint (CMmsgInfo,"     -f,--field     [fieldname] [expression]\n");
			CMmsgPrint (CMmsgInfo,"     -t,--tmpfield  [fieldname] [expression]\n");
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
		{ delete data; if (argNum > 1) printf ("File error in: %s\n", argv[1]); return(DBFault); }

	if (tableName == (char *) NULL) tableName = DBrNItems;

	if ((table = data->Table (tableName)) == (DBObjTable *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Invalid table!\n"); delete data; return (CMfailed); }

	for (expr = 0;expr < expNum;++expr)
		if (expressions [expr]->Configure (table) == DBFault)
			{ CMmsgPrint (CMmsgUsrError,"Invalid expression\n"); return (CMfailed); }
	
	for (recID = 0;recID < table->ItemNum ();++recID)
		{
		record = table->Item (recID);
		for (expr = 0;expr < expNum;++expr) expressions [expr]->Evaluate (record);
		}
	for (expr = 0;expr < expNum;++expr) delete expressions [expr];

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	free (expressions);
	delete data;
	delete variables;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
