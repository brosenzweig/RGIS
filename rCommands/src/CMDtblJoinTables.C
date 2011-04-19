/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDtblJoinTables.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	DBObjData *data, *joinData;
	char *joinDataName = (char *) NULL;
	char *relateTableName = (char *) NULL, *joinTableName = (char *) NULL;
	char *relateFieldName = (char *) NULL, *joinFieldName = (char *) NULL;
	DBObjTable *relateTable, *joinTable;
	DBObjTableField *relateField, *joinField;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-a","--dataset"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing join dataset!\n"); return (CMfailed); }
			joinDataName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-e","--relate"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing relate table name!\n"); return (CMfailed); }
			relateTableName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-o","--join"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing join table name!\n"); return (CMfailed); }
			joinTableName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-r","--rfield"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing relate field name!\n"); return (CMfailed); }
			relateFieldName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-j","--jfield"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing join field name!\n"); return (CMfailed); }
			joinFieldName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-t","--title"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing title!\n");        return (CMfailed); }
			title = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-u","--subject"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing subject!\n");      return (CMfailed); }
			subject = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-d","--domain"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing domain!\n");       return (CMfailed); }
			domain  = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-v","--version"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing version!\n");      return (CMfailed); }
			version  = argv [argPos];
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
			CMmsgPrint (CMmsgInfo,"     -a,--dataset   [join dataset]\n");
			CMmsgPrint (CMmsgInfo,"     -e,--relate    [relate table]\n");
			CMmsgPrint (CMmsgInfo,"     -o,--join      [join table]\n");
			CMmsgPrint (CMmsgInfo,"     -r,--rfield    [relate field]\n");
			CMmsgPrint (CMmsgInfo,"     -j,--jfield    [join field]\n");
			CMmsgPrint (CMmsgInfo,"     -t,--title     [dataset title]\n");
			CMmsgPrint (CMmsgInfo,"     -u,--subject   [subject]\n");
			CMmsgPrint (CMmsgInfo,"     -d,--domain    [domain]\n");
			CMmsgPrint (CMmsgInfo,"     -v,--version   [version]\n");
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

	data = new DBObjData ();
	if (((argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Read (argv [1]) : data->Read (stdin)) == DBFault)
		{ delete data; return (CMfailed); }

	if (title	!= (char *) NULL) data->Name (title);
	if (subject != (char *) NULL) data->Document (DBDocSubject,subject);
	if (domain	!= (char *) NULL)	data->Document (DBDocGeoDomain,domain);
	if (version != (char *) NULL) data->Document (DBDocVersion,version);	

	if (joinDataName == (char *) NULL) joinData = data;
	else
		{
		joinData = new DBObjData ();
		if (joinData->Read (joinDataName) == DBFault)
			{ CMmsgPrint (CMmsgUsrError,"Invalid join dataset!\n"); delete data; delete joinData; return (CMfailed); }
		}

	if (relateTableName == (char *) NULL) relateTableName = DBrNItems;
	if (joinTableName   == (char *) NULL) joinTableName   = DBrNItems;
	if ((data == joinData) && (strcmp (relateTableName,joinTableName) == 0))
		{
		CMmsgPrint (CMmsgUsrError,"Join tables are identical\n");
		if (joinData != data) delete joinData,
		delete data;
		return (CMfailed);
		}

	if ((relateTable = data->Table (relateTableName)) == (DBObjTable *) NULL)
		{
		CMmsgPrint (CMmsgUsrError,"Invalid relate table: %s\n",relateTableName);
		if (joinData != data) delete joinData;
		delete data;
		return (CMfailed);
		}

	if ((joinTable = joinData->Table (joinTableName)) == (DBObjTable *) NULL)
		{
		CMmsgPrint (CMmsgUsrError,"Invalid join table: %s!\n",joinTableName);
		if (joinData != data) delete joinData;
		delete data;
		return (CMfailed);
		}

	if (relateFieldName != (char *) NULL)
		{
		if ((relateField = relateTable->Field (relateFieldName)) == (DBObjTableField *) NULL)
			{
			CMmsgPrint (CMmsgUsrError,"Invalid relate field: %s!\n",relateFieldName);
			if (joinData != data) delete joinData;
			delete data;
			return (CMfailed);
			}
		}
	else relateField = (DBObjTableField *) NULL;

	if (joinFieldName != (char *) NULL)
		{
		if ((joinField = joinTable->Field (joinFieldName)) == (DBObjTableField *) NULL)
			{
			CMmsgPrint (CMmsgUsrError,"Invalid join field: %s!\n",joinFieldName);
			if (joinData != data) delete joinData;
			delete data;
			return (CMfailed);
			}
		}
	else joinField = (DBObjTableField *) NULL;

	if ((ret = RGLibTableJoin (relateTable, relateField, joinTable, joinField)) == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	delete data;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
