/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

CMDtblConv2Point.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	char *tableName = DBrNItems;
	char *nameSTR   = (char *) NULL;
	char *xcoordSTR = (char *) "XCoord";
	char *ycoordSTR = (char *) "YCoord";
	char *title     = (char *) NULL, *subject = (char *) NULL;
	char *domain    = (char *) NULL, *version = (char *) NULL;
	DBObjData  *data, *pntData;
	DBObjTable *table;
	DBCoordinate coord;
	DBObjTableField *nField, *xField, *yField, *tblFLD, *pntFLD;
	DBObjRecord *tblRec, *pntRec, *symRec;
	DBObjTable *pntTable;
	DBObjTable *symbols;
	DBObjTableField *symbolIDFLD;
	DBObjTableField *fgFLD;
	DBObjTableField *bgFLD;
	DBObjTableField *stFLD;
	DBObjTableField *coordFLD;
	DBObjTableField *symbolFLD;
	DBRegion extent;

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
		if (CMargTest (argv [argPos],"-n","--name"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing next field name!\n"); return (CMfailed); }
			nameSTR = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-x","--xcoord"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing source field name!\n"); return (CMfailed); }
			xcoordSTR = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-y","--ycoord"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing target field name!\n"); return (CMfailed); }
			ycoordSTR = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <=argPos) break;
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
			CMmsgPrint (CMmsgInfo,"     -n,--name      [name field]\n");
			CMmsgPrint (CMmsgInfo,"     -x,--xcoord    [xcoord field]\n");
			CMmsgPrint (CMmsgInfo,"     -y,--ycoord    [ycoord field]\n");
			CMmsgPrint (CMmsgInfo,"     -t,--title     [dataset title]\n");
			CMmsgPrint (CMmsgInfo,"     -u,--subject   [subject]\n");
			CMmsgPrint (CMmsgInfo,"     -d,--domain    [domain]\n");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose\n");
			CMmsgPrint (CMmsgInfo,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && ((int) strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (tableName == (char *) NULL) tableName = DBrNItems;

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	data = new DBObjData ();
	if (((argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Read (argv [1]) : data->Read (stdin)) == DBFault)
		{ delete data; if (argNum > 1) printf ("File error in: %s\n", argv[1]); return(DBFault); }

	if ((table = data->Table (tableName)) == (DBObjTable *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Invalid table!\n"); delete data; return (CMfailed); }
	if (nameSTR == (char *) NULL) nField = (DBObjTableField *) NULL;
	else if ((nField = table->Field (nameSTR)) == ((DBObjTableField *) NULL))
		{ CMmsgPrint (CMmsgUsrError,"Name field!\n");   delete data; return (CMfailed); }
	if ((xField = table->Field (xcoordSTR)) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"XCoord field!\n"); delete data; return (CMfailed); }
	if ((yField = table->Field (ycoordSTR)) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"YCoord field!\n"); delete data; return (CMfailed); }

	pntData = new DBObjData ("",DBTypeVectorPoint);
	if (title	== (char *) NULL)  title  = (char *) "Converted Point coverage";
	if (subject == (char *) NULL) subject = (char *) "Points";
	if (domain	== (char *) NULL) domain  = data->Document (DBDocGeoDomain);
	if (version == (char *) NULL) version = (char *) "0.01pre";
	pntData->Name (title);
	pntData->Document (DBDocSubject,subject);
	pntData->Document (DBDocGeoDomain,domain);
	pntData->Document (DBDocVersion,version);

	pntTable    = pntData->Table (DBrNItems);
	symbols     = pntData->Table (DBrNSymbols);
	symbolIDFLD = symbols->Field (DBrNSymbolID);
	fgFLD       = symbols->Field (DBrNForeground);
	bgFLD       = symbols->Field (DBrNBackground);
	stFLD       = symbols->Field (DBrNStyle);
	coordFLD    = pntTable->Field (DBrNCoord);
	symbolFLD   = pntTable->Field (DBrNSymbol);

	symRec = symbols->Add ("Default Symbol");
	symbolIDFLD->Int (symRec,0);
	fgFLD->Int (symRec,1);
	bgFLD->Int (symRec,2);
	stFLD->Int (symRec,0);

	for (tblRec = table->First ();tblRec != (DBObjRecord *) NULL;tblRec = table->Next ())
		{
		if (CMmathEqualValues (coord.X = xField->Float (tblRec),xField->FloatNoData ())) continue;
		if (CMmathEqualValues (coord.Y = yField->Float (tblRec),yField->FloatNoData ())) continue;
		if (nField == (DBObjTableField *) NULL) pntRec = pntTable->Add (tblRec->Name ());
		else pntRec = pntTable->Add (nField->String (tblRec));
		coordFLD->Coordinate (pntRec,coord);
		symbolFLD->Record (pntRec,symRec);
		extent.Expand (coord);
		}
	pntData->Extent (extent);
    pntData->Projection (DBMathGuessProjection (extent));
    pntData->Precision  (DBMathGuessPrecision  (extent));
	for (tblFLD = table->Fields ()->First ();tblFLD != (DBObjTableField *) NULL;tblFLD = table->Fields ()->Next ())
		if ((tblFLD != nField) && (tblFLD != xField) && (tblFLD != yField) && DBTableFieldIsVisible (tblFLD))
			{
			pntTable->AddField (pntFLD = new DBObjTableField (*tblFLD));
			pntFLD->Required (false);
			pntRec = pntTable->First ();
			for (tblRec = table->First ();tblRec != (DBObjRecord *) NULL;tblRec = table->Next ())
				{
				if (CMmathEqualValues (coord.X = xField->Float (tblRec),xField->FloatNoData ())) continue;
				if (CMmathEqualValues (coord.Y = yField->Float (tblRec),yField->FloatNoData ())) continue;
				switch (tblFLD->Type ())
					{
					case DBTableFieldString:	pntFLD->String	(pntRec,tblFLD->String (tblRec));	break;
					case DBTableFieldInt:		pntFLD->Int		(pntRec,tblFLD->Int (tblRec));		break;
					case DBTableFieldFloat:		pntFLD->Float	(pntRec,tblFLD->Float (tblRec));		break;
					case DBTableFieldDate:		pntFLD->Date	(pntRec,tblFLD->Date (tblRec));		break;
					default: fprintf (stderr,"Invalid Field Type in: RGISToolsConvertToPointCBK ()\n");		break;
					}
				pntRec = pntTable->Next ();
				}
			}

	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? pntData->Write (argv [2]) : pntData->Write (stdout);

	delete data;
	delete pntData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
