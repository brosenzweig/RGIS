/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDtblTopoCalc.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	DBInt oper = DBMathOperatorAdd;
	char *tableName = (char *) NULL;
	char *srcFieldName = (char *) NULL;
	char *dstFieldName = (char *) NULL;
	char *nextFieldName = (char *) NULL;
	DBObjData *data;
	DBObjTable *table;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-a","--table"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing table name!\n");  return (CMfailed); }
			tableName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-o","--operation"))
			{
			int operCodes [] = {	DBMathOperatorAdd, DBMathOperatorSub };
			const char *operations [] = {	"add", "subtract", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing operation!\n");   return (CMfailed); }
			if ((oper = CMoptLookup (operations,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid operation!\n");		return (CMfailed); }
			oper = operCodes [oper];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-n","--next"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing next field name!\n"); return (CMfailed); }
			nextFieldName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-s","--source"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing source field name!\n"); return (CMfailed); }
			srcFieldName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-t","--target"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing target field name!\n"); return (CMfailed); }
			dstFieldName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-V","--verbose"))
			{
			verbose = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			CMmsgPrint (CMmsgInfo,"%s [options] <input file> <output file>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -a,--table     [table name]\n");
			CMmsgPrint (CMmsgInfo,"     -o,--operation [add|subtract]\n");
			CMmsgPrint (CMmsgInfo,"     -n,--next      [next fieldname]\n");
			CMmsgPrint (CMmsgInfo,"     -s,--source    [source fieldname]\n");
			CMmsgPrint (CMmsgInfo,"     -t,--target    [target fieldname]\n");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose\n");
			CMmsgPrint (CMmsgInfo,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && ((int) strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (srcFieldName == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Missing source field!\n"); return (CMfailed); }
	if (dstFieldName == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Missing target field!\n"); return (CMfailed); }
	if (nextFieldName == (char *) NULL) nextFieldName = RGlibNextStation;
	if (tableName == (char *) NULL) tableName = DBrNItems;

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	data = new DBObjData ();
	if (((argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Read (argv [1]) : data->Read (stdin)) == DBFault)
		{ delete data; if (argNum > 1) printf ("File error in: %s\n", argv[1]); return(DBFault); }

	if ((table = data->Table (tableName)) == (DBObjTable *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Invalid table!\n"); delete data; return (CMfailed); }

	switch (oper)
		{
		default:
		case DBMathOperatorAdd:
			ret = RGlibGenFuncTopoAccum (table,nextFieldName,srcFieldName,dstFieldName);
			break;
		case DBMathOperatorSub:
			ret = RGlibGenFuncTopoSubtract (table,nextFieldName,srcFieldName,dstFieldName);
			break;
		}
	if (ret == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	delete data;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
