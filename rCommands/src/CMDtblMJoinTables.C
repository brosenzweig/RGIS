/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDtblMJoinTables.C

andras@ob.sr.unh.edu

*******************************************************************************/

#include <math.h>
#include <cm.h>
#include <DB.H>
#include <DBif.H>

struct Groups
	{
	char *relate;
	DBObjTableField *relateFLD;
	char *join;
	DBObjTableField *joinFLD;
	};

struct Fields
	{
	DBObjTableField *relateFLD;
	DBObjTableField *joinFLD;
	};

void doHelp(bool extended, const char *progName)
	{
	if(extended)
		{
		CMmsgPrint (CMmsgInfo,"%s [options] [field] ... <input file> ...",progName);
		CMmsgPrint (CMmsgInfo,"  -b,--table   [inTableName] [outTableName] => Set the name of the table to use.\t(If either is set to NULL, then default table will be used)");
		CMmsgPrint (CMmsgInfo,"  -d,--domain  [domain]                     => Specify domain of output table.");
		CMmsgPrint (CMmsgInfo,"  -m,--match   [inFieldName] [outFieldName] => Which fields to match");
		CMmsgPrint (CMmsgInfo,"  -o,--output  [filename]                   => Specify output filename, else use STDOUT");
		CMmsgPrint (CMmsgInfo,"  -a,--dataset [filename]                   => Join dataset");
		CMmsgPrint (CMmsgInfo,"  -s,--subject [subject]                    => Specify subject of output table.");
		CMmsgPrint (CMmsgInfo,"  -t,--title   [title]                      => Specify title of output table.");
		CMmsgPrint (CMmsgInfo,"  -v,--version [version]                    => Specify version of output table.");
		CMmsgPrint (CMmsgInfo,"  -V,--verbose                              => Output all debuging statements to STDERR (Must be first flag)");
		CMmsgPrint (CMmsgInfo,"  -c,--ascii                                => Output file in ascii format.");
		CMmsgPrint (CMmsgInfo,"  -h,--help                                 => Print this usage information.");
		}
	else
		{
		CMmsgPrint (CMmsgInfo,"%s [options] [field] ... <input file> ...",progName);
		CMmsgPrint (CMmsgInfo,"  -b,--table   [inTableName] [outTableName]");
		CMmsgPrint (CMmsgInfo,"  -d,--domain  [domain]");
		CMmsgPrint (CMmsgInfo,"  -m,--match   [inFieldName] [outFieldName]");
		CMmsgPrint (CMmsgInfo,"  -o,--output  [filename]");
		CMmsgPrint (CMmsgInfo,"  -a,--dataset [filename]");
		CMmsgPrint (CMmsgInfo,"  -s,--subject [subject]");
		CMmsgPrint (CMmsgInfo,"  -t,--title   [title]");
		CMmsgPrint (CMmsgInfo,"  -v,--version [version]");
		CMmsgPrint (CMmsgInfo,"  -V,--verbose");
		CMmsgPrint (CMmsgInfo,"  -c,--ascii");
		CMmsgPrint (CMmsgInfo,"  -h,--help    => For more help type -h e, or --help extend.");
		}
	}

int main (int argc,char *argv [])
	{
	bool ascii = false, verbose = false;
	int argPos, argNum = argc, numGrps = 0, numFlds = 0;
	char *relateTableName = DBrNItems, *joinTableName = DBrNItems;
	char *title = (char *) NULL, *subject = (char *) NULL, *domain = (char *) NULL, *version = (char *) NULL;
	Groups **groups = (Groups **) NULL;
	Fields **fields = (Fields **) NULL;
	FILE *outFile = (FILE *) NULL;
	DBObjData *relateData = (DBObjData *) NULL, *joinData = (DBObjData *) NULL;
	DBObjTable *relateTable, *joinTable;
	DBObjTableField *field;
	DBObjRecord *relateRecord, *joinRecord;

	if(argc <= 2) { doHelp(false,CMprgName(argv[0])); return(DBSuccess); }

	groups = (Groups **) malloc(sizeof(Groups *));
	fields = (Fields **) malloc(sizeof(Fields *));

	if(CMargTest(argv[1],"-V","--verbose")) { verbose = true; argNum = CMargShiftLeft (1,argv,argNum); }

	for (argPos = 1;argPos < argNum;)
		{
		if (CMargTest(argv[argPos],"-h","--help"))
			{
			argNum = CMargShiftLeft (argPos,argv,argNum);
			if(CMargTest(argv[argPos],"e","extend"))
				{
				doHelp(true,CMprgName(argv[0]));
				argNum = CMargShiftLeft (argPos,argv,argNum);
				}
			else
				doHelp(false,CMprgName(argv[0]));
			return(DBSuccess);
			}
		if (CMargTest(argv[argPos],"-b","--table"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing inTable name!");  return (CMfailed); }
			if(argv[argPos] != (char *) NULL) relateTableName = argv[argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing outTable name!");  return (CMfailed); }
			if(argv[argPos] != (char *) NULL) joinTableName = argv[argPos];
			if(verbose) CMmsgPrint (CMmsgDebug, "relateTableName: '%s'joinTableName: '%s'",relateTableName,joinTableName);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-m","--match"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos + 1)
				{ CMmsgPrint (CMmsgUsrError,"Missing match field(s)!"); return (CMfailed); }
			if((groups = (Groups **) realloc(groups,(numGrps + 1) * sizeof(Groups *))) == (Groups **) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return(DBFault); }
			groups[numGrps] = new Groups();
			groups[numGrps]->join = argv[argPos];
			groups[numGrps]->relate = argv[argPos + 1];
			numGrps++;
			argNum = CMargShiftLeft (argPos,argv,argNum);
			argNum = CMargShiftLeft (argPos,argv,argNum);
			continue;
			}
		if (CMargTest(argv[argPos],"-o","--output"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing output filename!"); return (CMfailed); }
			if((outFile = fopen(argv[argPos],"w")) == (FILE *) NULL)
				{ CMmsgPrint (CMmsgUsrError,"Cannot open file %s",argv[argPos]); return(DBFault); }
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-a","--dataset"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing output filename!"); return (CMfailed); }
			joinData = new DBObjData();
			joinData->Read(argv[argPos]);
			if(verbose) CMmsgPrint (CMmsgDebug, "joinData: '%s'",argv[argPos]);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-t","--title"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing title!");       return (CMfailed); }
			title = argv[argPos];
			if(verbose) CMmsgPrint (CMmsgDebug, "title: '%s'",title);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-s","--subject"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing subject!");     return (CMfailed); }
			subject = argv[argPos];
			if(verbose) CMmsgPrint (CMmsgDebug, "subject: '%s'",subject);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-d","--domain"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing domain!");      return (CMfailed); }
			domain = argv[argPos];
			if(verbose) CMmsgPrint (CMmsgDebug, "domain: '%s'",domain);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-v","--version"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing version!");     return (CMfailed); }
			version = argv[argPos];
			if (verbose) CMmsgPrint (CMmsgDebug, "version: '%s'",version);
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest(argv[argPos],"-c","--ascii"))
			{
			ascii = true;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if ((argv[argPos][0] == '-') && (strlen (argv[argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError, "Unknown option: %s!",argv[argPos]); return (CMfailed); }
		argPos++;
		}

	if(outFile == (FILE *) NULL) outFile = stdout;
	relateData = new DBObjData();
	if (joinData == (DBObjData *) NULL) joinData = relateData;
	if ((argNum > 1) && (strcmp(argv[1],"-") != 0)) relateData->Read(argv[1]);
	else { CMmsgPrint (CMmsgInfo, "WARN: reading from STDIN!"); relateData->Read(stdin); }

	if (title	!= (char *) NULL) relateData->Name(title);
	if (subject != (char *) NULL) relateData->Document(DBDocSubject,subject);
	if (domain	!= (char *) NULL)	relateData->Document(DBDocGeoDomain,domain);
	if (version != (char *) NULL) relateData->Document(DBDocVersion,version);

	if((relateTable = relateData->Table(relateTableName)) == (DBObjTable *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Invalid Relate table: %s!",relateTableName); delete relateData; return (CMfailed); }
	if((joinTable = joinData->Table(joinTableName)) == (DBObjTable *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Invalid Join table: %s!",joinTableName); delete joinData; return (CMfailed); }
	if((relateData == joinData) && (relateTable == joinTable))
		{ CMmsgPrint (CMmsgUsrError, "Tables are identical!"); delete relateData; delete joinData; return(DBFault); }

	for(int i = 0; i < numGrps; i++)
		{
		if((field = relateTable->Field(groups[i]->relate)) == (DBObjTableField *) NULL) { CMmsgPrint (CMmsgUsrError, "Invalid Relate Table group name: %s",groups[i]->relate); return (CMfailed); }
		groups[i]->relateFLD = field;
		if((field = joinTable->Field(groups[i]->join)) == (DBObjTableField *) NULL) { CMmsgPrint (CMmsgUsrError, "Invalid Join Table group name: %s",groups[i]->join); return (CMfailed); }
		groups[i]->joinFLD = field;
		if(verbose) CMmsgPrint (CMmsgUsrError, "Added Group #%d: join: '%s' <-> relate: '%s'",i,groups[i]->join,groups[i]->relate);
		}
	if(verbose) CMmsgPrint (CMmsgDebug, "relateItemNum: %d joinItemNum: %d",relateTable->ItemNum(),joinTable->ItemNum());

	for(int joinFieldID = 0; joinFieldID < joinTable->FieldNum();++joinFieldID)
		{
		int relateFieldID;
		DBObjTableField *joinField = joinTable->Field(joinFieldID);
		if (DBTableFieldIsVisible (joinField) == false) continue; // We don't want to copy hidden fields.

		for(relateFieldID = 0; relateFieldID < relateTable->FieldNum();++relateFieldID)
			{
			DBObjTableField *relateField = relateTable->Field(relateFieldID);
			if(strcmp(joinField->Name(),relateField->Name()) == 0) break;
			}
		if(relateFieldID == relateTable->FieldNum ())
			{
			if((fields = (Fields **) realloc(fields,(numFlds + 1) * sizeof(Fields *))) == (Fields **) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return(DBFault); }
			fields[numFlds] = new Fields();
			fields[numFlds]->joinFLD = joinField;
			fields[numFlds]->relateFLD = new DBObjTableField (*joinField);
			fields[numFlds]->relateFLD->Required (false);
			relateTable->AddField(fields[numFlds]->relateFLD);
			numFlds++;
			}
		}

/* Search for matches and create output table*************/
	for (int joinRecID = 0;joinRecID < joinTable->ItemNum();++joinRecID)
//	for (int joinRecID = joinTable->ItemNum() - 1;joinRecID != -1;++joinRecID) // use this line when multiple matches add LAST found
		{
		joinRecord = joinTable->Item(joinRecID);
		for(int relateRecID = 0; relateRecID < relateTable->ItemNum();++relateRecID)
			{
			relateRecord = relateTable->Item(relateRecID);
			int i = 0;
			while (i < numGrps && DBTableFieldMatch(groups[i]->joinFLD,joinRecord,groups[i]->relateFLD,relateRecord)) i++;
			if (i == numGrps)
				{
				i = 0;
				while(i < numFlds)
					{
					if (fields[i]->relateFLD->Required ()) continue; // We don't want to overwrite protected fields.
					switch(fields[i]->relateFLD->Type())
						{
						case DBTableFieldInt:
							fields[i]->relateFLD->Int(relateRecord,fields[i]->joinFLD->Int(joinRecord));
						break;
						case DBTableFieldString:
							fields[i]->relateFLD->String(relateRecord,fields[i]->joinFLD->String(joinRecord));
						break;
						case DBTableFieldFloat:
							fields[i]->relateFLD->Float(relateRecord,fields[i]->joinFLD->Float(joinRecord));
						break;
						case DBTableFieldDate:
							fields[i]->relateFLD->Date(relateRecord,fields[i]->joinFLD->Date(joinRecord));
						break;
						}
					i++;
					}
				}
			}
		}

	if(ascii) DBExportASCIITable (relateTable,outFile); else relateData->Write(outFile);

/* CLEANUP ***********************************************/
	if(outFile != stdout) fclose(outFile);
	delete relateData;
	delete joinData;
	delete groups;
	return(DBSuccess);
}
