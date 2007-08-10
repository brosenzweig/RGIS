/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

CMDtblDeleteField.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, i, fieldNum = 0, verbose = false;
	DBObjData *data;
	char *tableName = (char *) NULL, **fieldNames;
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
			fieldNames = fieldNum > 0 ? (char **) realloc (fieldNames,(fieldNum + 1) * sizeof (char *)) :
												 (char **) calloc (1,sizeof (char *));
			if (fieldNames == (char **) NULL) { perror ("Memory allocation error"); return (CMfailed); }
			fieldNames [fieldNum++] = argv [argPos];
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
	if (fieldNum < 1) { CMmsgPrint (CMmsgUsrError,"Missing field name!\n"); return (CMfailed); }

	data = new DBObjData ();
	if (((argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Read (argv [1]) : data->Read (stdin)) == DBFault)
		{ delete data; return (CMfailed); }

	if ((table = data->Table (tableName)) == (DBObjTable *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Invalid table: %s!\n",tableName); delete data; return (CMfailed); }

	for (i = 0;i < fieldNum;++i)
		{
		if ((field = table->Field (fieldNames [i])) == (DBObjTableField *) NULL)
			{ CMmsgPrint (CMmsgUsrError,"Invalid field: %s!\n",fieldNames [i]); continue; }
		if (DBTableFieldIsOptional (field) != true)
			{ CMmsgPrint (CMmsgUsrError,"Required field!\n"); continue; }
		table->DeleteField (field); 
		}

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	free (fieldNames);
	delete data;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
