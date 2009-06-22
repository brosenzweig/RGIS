/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

CMDgrdCalculate.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>
#include <CMDexp.H>

class CMDgrdVariable
	{
	private:
		DBGridIO *GridIO;
		DBObjTableField *SourceFLD;
		DBObjTableField *TargetFLD;
		DBObjRecord *LayerRec;
	public:
	CMDgrdVariable (char *varName)
		{
		GridIO = (DBGridIO *) NULL;
		SourceFLD = (DBObjTableField *) NULL;
		TargetFLD = new DBObjTableField (varName,DBVariableFloat,"%10.3f",sizeof (DBFloat),false);
		}
	~CMDgrdVariable ()
		{
		DBObjData *data;

		if (GridIO != (DBGridIO *) NULL)
			{
			data = GridIO->Data ();
			delete GridIO;
			delete data;
			}
		}
	DBInt Configure (DBObjTable *table, bool flat)
		{
		DBInt i;
		char *dataName;
		char *fieldName = (char *) NULL;
		DBObjData *data;

		dataName = TargetFLD->Name ();

		if ((dataName = (char *) malloc (strlen (dataName) + 1)) == (char *) NULL)
			{ perror ("Memory allocation error in: CMDgrdVariable::Configure ()"); return (CMfailed); }
		strcpy (dataName, TargetFLD->Name ());

		for (i = 0;i < (DBInt) strlen (dataName);++i)
			if (dataName [i] == ':') { fieldName = dataName + i + 1; dataName [i] = '\0'; i = 0; break;}

		data = new DBObjData ();
		if ((strcmp (dataName,"stdin") == 0 ? data->Read (stdin) : data->Read (dataName)) == DBFault)
			{ CMmsgPrint (CMmsgUsrError,"%s\n",dataName); return (CMfailed); }

		if (data->Type () == DBTypeGridDiscrete)
			{
			DBObjTable *table = data->Table (DBrNItems);

			if (fieldName == (char *) NULL) fieldName = DBrNGridValue;
			if ((SourceFLD = table->Field (fieldName)) == (DBObjTableField *) NULL)
				{
				CMmsgPrint (CMmsgUsrError,"Invalid field [%s]!\n",fieldName);
				return (CMfailed);
				}
			strcpy (dataName, TargetFLD->Name ());
			delete TargetFLD;
			TargetFLD = new DBObjTableField (*SourceFLD);
			TargetFLD->Name (dataName);
			}
		else
			{
			if (fieldName != (char *) NULL)
				{ CMmsgPrint (CMmsgUsrError,"Continuous grid field is referenced!\n"); free (dataName); return (CMfailed); }
			}
		table->AddField (TargetFLD);
		GridIO = new DBGridIO (data, flat);
		free (dataName);
		return (DBSuccess);
		}
	DBRegion Extent () const { DBObjData *data = GridIO->Data (); return (data->Extent ()); }
	DBFloat CellWidth  () const { return (GridIO->CellWidth ()); }
	DBFloat CellHeight () const { return (GridIO->CellWidth ()); }
	DBInt LayerNum () const { return (GridIO->LayerNum ()); }

	char *CurrentLayer (DBInt layerID)
		{
		if (layerID == DBFault) LayerRec = GridIO->Layer (0);
		else	LayerRec = layerID < GridIO->LayerNum () ? GridIO->Layer (layerID) :
								GridIO->Layer (layerID % GridIO->LayerNum ());
		return (LayerRec->Name ());
		}

	DBInt FindLayer (char *layerName)
		{
		DBInt layerID, i = 0;
		DBObjRecord *layerRec;
		char *name;

		for (layerID = 0;layerID < GridIO->LayerNum ();++layerID)
			{
			layerRec = GridIO->Layer (layerID);

			name = layerRec->Name ();
			if (strncmp (name,"XXXX-",5) == 0)
				{ i = 5; if ((strlen (name) - 5) == 0) return (layerID); }
			if (strncmp (name + i,layerName + i,strlen (name) - i) == 0) return (layerID);
			}
		return (CMfailed);
		}
	DBInt LayerIsDated (DBInt layerID)
		{
		DBObjRecord *layerRec;
		DBDate date;
		layerRec = GridIO->Layer (layerID);
		date.Set (layerRec->Name ());
		if (date.Year () != DBDefaultMissingIntVal) return (true);
		return (date.Month () != DBDefaultMissingIntVal ? true : false);
		}
	void GetVariable (DBObjRecord *record,DBCoordinate coord)
		{
		switch ((GridIO->Data ())->Type ())
			{
			case DBTypeGridContinuous:
				{
				DBFloat value;
				if (GridIO->Value (LayerRec,coord,&value))
						TargetFLD->Float (record,value);
				else	TargetFLD->Float (record,TargetFLD->FloatNoData ());
				} break;
			case DBTypeGridDiscrete:
				{
				DBObjRecord *grdRec;
				if ((grdRec = GridIO->GridItem (LayerRec,coord)) != (DBObjRecord *) NULL)
					switch (SourceFLD->Type ())
						{
						case DBVariableString:
							TargetFLD->String (record,SourceFLD->String (grdRec));	break;
						case DBVariableInt:
							TargetFLD->Int (record,SourceFLD->Int (grdRec));			break;
						case DBVariableFloat:
							TargetFLD->Float (record,SourceFLD->Float (grdRec));		break;
						default:	break;
						}
				else
					switch (SourceFLD->Type ())
						{
						case DBVariableString:
							TargetFLD->String (record,"");									break;
						case DBVariableInt:
							TargetFLD->Int (record,SourceFLD->IntNoData ());			break;
						case DBVariableFloat:
							TargetFLD->Float (record,SourceFLD->FloatNoData ());		break;
						default:	break;
						}
				} break;
			default:
				CMmsgPrint (CMmsgUsrError,"Invalid grid type in: CMDgrdVariable:GetVariable ()\n");
				break;
			}
		}
	};

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	DBInt i, expNum = 0, varNum = 0, masterVar = 0, recID, layerID, dataLayerID, layerNum = 0;
	char *expStr = (char *) NULL;
	char *title  = (char *) NULL, *subject = (char *) NULL;
	char *domain = (char *) NULL, *version = (char *) NULL;
	char *layerName;
	int shadeSet = DBDataFlagDispModeContGreyScale;
	bool shrink = true, flat = false;
	DBFloat var;
	DBRegion extent;
	DBPosition pos;
	DBCoordinate coord, cellSize;
	DBMathOperand *operand;
	DBObjectLIST<DBObject>	*variables = new DBObjectLIST<DBObject>  ("Variables");
	DBObjTable *table = new DBObjTable ("TEMPTable");
	DBObjTableField *fieldPTR;
	DBObject *obj;
	DBObjData *data;
	DBObjRecord *record, *layerRec;
	DBGridIO *gridIO;
	CMDExpression **expressions = (CMDExpression **) NULL;
	CMDgrdVariable **grdVar = (CMDgrdVariable **) NULL;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-c","--calculate"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing expression!\n");   return (CMfailed); }
			if (expStr != (char *) NULL)
				{ CMmsgPrint (CMmsgUsrError,"Expression is already specified!\n"); return (CMfailed); }
			expStr = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-r","--variable"))
			{
			char *varName;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing variable name!\n"); return (CMfailed); }
			varName = argv [argPos];
			fieldPTR = new DBObjTableField (varName,DBVariableFloat,"%10.3f",sizeof (DBFloat),false);
			table->AddField (fieldPTR);
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing expression!\n");   return (CMfailed); }
			expressions = expNum < 1 ? (CMDExpression **) calloc (1,sizeof (CMDExpression *)) :
								(CMDExpression **) realloc (expressions,(expNum + 1) * sizeof (CMDExpression *));
			if (expressions == (CMDExpression **) NULL)
				{ perror ("MNemory Allocation error!"); return (CMfailed); }
			expressions [expNum] = new CMDExpression (varName, argv [argPos]);
			if ((expressions [expNum])->Expand (variables) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid Expression!\n"); return (CMfailed); }
			expNum++;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-x","--extent"))
			{
			int codes [] = { true, false };
			const char *names [] = { "minimum","maximum", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing extent mode!\n");     return (CMfailed); }
			if ((ret = CMoptLookup (names,argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid extent mode!\n");     return (CMfailed); }
			shrink = codes [ret];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-n","--interpolate"))
			{
			int codes [] = { false, true };
			const char *names [] = { "surface","flat", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing interpolation mode!\n");     return (CMfailed); }
			if ((ret = CMoptLookup (names,argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid interpolation mode!\n");     return (CMfailed); }
			flat = codes [ret];
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
				{ CMmsgPrint (CMmsgUsrError,"Missing domain!\n");            return (CMfailed); }
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
		if (CMargTest (argv [argPos],"-s","--shadeset"))
			{
			int shadeCodes [] = {	DBDataFlagDispModeContStandard,
			                        DBDataFlagDispModeContGreyScale,
			                        DBDataFlagDispModeContBlueScale,
			                        DBDataFlagDispModeContBlueRed,
			                        DBDataFlagDispModeContElevation };
			const char *shadeSets [] = { "standard","grey","blue","blue-to-red","elevation", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing shadeset!\n");     return (CMfailed); }
			if ((shadeSet = CMoptLookup (shadeSets,argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid shadeset!\n");     return (CMfailed); }
			shadeSet = shadeCodes [shadeSet];
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
			CMmsgPrint (CMmsgInfo,"%s [options] <output file>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -c,--calculate   [expression]\n");
			CMmsgPrint (CMmsgInfo,"     -r,--variable    [variable expression]\n");
			CMmsgPrint (CMmsgInfo,"     -n,--interpolate [surface|flat]\n");
			CMmsgPrint (CMmsgInfo,"     -x,--extent      [maximum minimum]\n");
			CMmsgPrint (CMmsgInfo,"     -t,--title       [dataset title]\n");
			CMmsgPrint (CMmsgInfo,"     -u,--subject     [subject]\n");
			CMmsgPrint (CMmsgInfo,"     -d,--domain      [domain]\n");
			CMmsgPrint (CMmsgInfo,"     -v,--version     [version]\n");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset    [standard|grey|blue|blue-to-red|elevation]\n");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose\n");
			CMmsgPrint (CMmsgInfo,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && ((int) strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (expStr == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Missing expression!\n"); return (CMfailed); }

	if (argNum > 2) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	operand = new DBMathOperand (expStr);
	if (operand->Expand (variables) == DBFault) return (CMfailed);

	cellSize.X = cellSize.Y = DBHugeVal;
	for (recID = 0;recID < variables->ItemNum ();++recID)
		{
		obj = variables->Item (recID);
		if ((fieldPTR = table->Field (obj->Name ())) != (DBObjTableField *) NULL) continue;

		grdVar = (CMDgrdVariable **) realloc (grdVar,sizeof (CMDgrdVariable *) * (varNum + 1));
		if (grdVar == (CMDgrdVariable **) NULL) { perror ("Memory Allocation Error!\n"); return (CMfailed); }
		grdVar [varNum] = new CMDgrdVariable (obj->Name ());
		if (grdVar [varNum]->Configure (table,flat) == DBFault)
			{
			for (i = 0;i <= varNum;++i) delete grdVar [i];
			free (grdVar);
			return (CMfailed);
			}
		extent.Expand (grdVar [varNum]->Extent ());
		var = grdVar [varNum]->CellWidth ();
		if (cellSize.X > var) cellSize.X = var;
		var = grdVar [varNum]->CellHeight ();
		if (cellSize.Y > var) cellSize.Y = var;
		layerID = (grdVar [varNum])->LayerNum ();
		if (layerNum < layerID) { layerNum = layerID; masterVar = varNum; }
		varNum++;
		}

	for (i = 0;i < expNum;++i) if (expressions [i]->Configure (table) == DBFault) return (CMfailed);
	operand->Configure (table->Fields ());
	if ((record = table->Add ("TEMPRecord")) == (DBObjRecord *) NULL) return (CMfailed);
//	for (i = 0;i < expNum;++i) expressions [i]->Evaluate (record);

	if (shrink) for (i = 0;i < varNum;++i) extent.Shrink (grdVar [i]->Extent ());

	if (title	== (char *) NULL)	title = (char *) "Grid Calculate Result";
	if (subject == (char *) NULL) subject = (char *) "GridCalc";
	if (domain  == (char *) NULL) domain  = (char *) "Non-specified";
	if (version == (char *) NULL) version = (char *) "0.01pre";
	if (shadeSet == DBFault)     shadeSet = DBDataFlagDispModeContGreyScale;

	if ((data = DBGridCreate (title,extent,cellSize)) == (DBObjData *) NULL) return (CMfailed);
	data->Document (DBDocSubject,subject);
	data->Document (DBDocGeoDomain,domain);
	data->Document (DBDocVersion,version);
	data->Flags (DBDataFlagDispModeContShadeSets,DBClear);
	data->Flags (shadeSet, DBSet);

	gridIO = new DBGridIO (data);
	for (layerID = 0;layerID < layerNum;++layerID)
		{
		layerName = grdVar [masterVar]->CurrentLayer (layerID);
		for (i = 0;i < varNum;++i)
			{
			if ((dataLayerID = grdVar [i]->FindLayer (layerName)) != DBFault)
				grdVar [i]->CurrentLayer (dataLayerID);
			else
				{
				if (grdVar [i]->LayerIsDated (0)) continue;
				grdVar [i]->CurrentLayer (dataLayerID);
				}
			}
		if (layerID > 0) gridIO->AddLayer ((char *) "New Layer");
		layerRec = gridIO->Layer (layerID);
		gridIO->RenameLayer (layerRec,layerName);
		for (pos.Row = 0;pos.Row < gridIO->RowNum ();++pos.Row)
			for (pos.Col = 0;pos.Col < gridIO->ColNum ();++pos.Col)
				{
			 	gridIO->Pos2Coord (pos,coord);
				for (i = 0;i < varNum;++i) grdVar [i]->GetVariable (record,coord);
				for (i = 0;i < expNum;++i) expressions [i]->Evaluate (record);
				gridIO->Value (layerRec,pos,operand->Float (record));
				}
		gridIO->RecalcStats (layerRec);
		}

	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Write (argv [1]) : data->Write (stdout);

	for (i = 0;i < varNum;++i) delete grdVar [i];
	delete variables;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
