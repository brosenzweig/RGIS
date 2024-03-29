/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDgrdCalculate.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>
#include <CMDexp.H>

class CMDgrdVariable
	{
	private:
		DBGridIF *GridIF;
		DBObjTableField *SourceFLD;
		DBObjTableField *TargetFLD;
		DBObjRecord *LayerRec;
	public:
	CMDgrdVariable (char *varName)
		{
		GridIF = (DBGridIF *) NULL;
		SourceFLD = (DBObjTableField *) NULL;
		TargetFLD = new DBObjTableField (varName,DBVariableFloat,"%10.3f",sizeof (DBFloat),false);
		}
	~CMDgrdVariable ()
		{
		DBObjData *data;

		if (GridIF != (DBGridIF *) NULL)
			{
			data = GridIF->Data ();
			delete GridIF;
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
			{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return (CMfailed); }
		strcpy (dataName, TargetFLD->Name ());

		for (i = 0;i < (DBInt) strlen (dataName);++i)
			if (dataName [i] == ':') { fieldName = dataName + i + 1; dataName [i] = '\0'; i = 0; break;}

		data = new DBObjData ();
		if ((strcmp (dataName,"stdin") == 0 ? data->Read (stdin) : data->Read (dataName)) == DBFault)
			{ CMmsgPrint (CMmsgUsrError,"%s",dataName); return (CMfailed); }

		if (data->Type () == DBTypeGridDiscrete)
			{
			DBObjTable *table = data->Table (DBrNItems);

			if (fieldName == (char *) NULL) fieldName = DBrNGridValue;
			if ((SourceFLD = table->Field (fieldName)) == (DBObjTableField *) NULL)
				{
				CMmsgPrint (CMmsgUsrError,"Invalid field [%s]!",fieldName);
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
				{ CMmsgPrint (CMmsgUsrError,"Continuous grid field is referenced!"); free (dataName); return (CMfailed); }
			}
		table->AddField (TargetFLD);
		GridIF = new DBGridIF (data, flat);
		free (dataName);
		return (DBSuccess);
		}
	DBInt    Projection () const { return (GridIF->Data ()->Projection ()); };
	DBRegion Extent () const { DBObjData *data = GridIF->Data (); return (data->Extent ()); }
	DBFloat CellWidth  () const { return (GridIF->CellWidth ()); }
	DBFloat CellHeight () const { return (GridIF->CellWidth ()); }
	DBInt LayerNum () const { return (GridIF->LayerNum ()); }

	char *CurrentLayer (DBInt layerID)
		{
		if (layerID == DBFault) LayerRec = GridIF->Layer (0);
		else	LayerRec = layerID < GridIF->LayerNum () ? GridIF->Layer (layerID) :
								GridIF->Layer (layerID % GridIF->LayerNum ());
		return (LayerRec->Name ());
		}

	DBInt FindLayer (char *layerName)
		{
		DBInt layerID, i = 0;
		DBObjRecord *layerRec;
		char *name;

		for (layerID = 0;layerID < GridIF->LayerNum ();++layerID)
			{
			layerRec = GridIF->Layer (layerID);

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
		layerRec = GridIF->Layer (layerID);
		date.Set (layerRec->Name ());
		if (date.Year () != DBDefaultMissingIntVal) return (true);
		return (date.Month () != DBDefaultMissingIntVal ? true : false);
		}
	void GetVariable (DBObjRecord *record,DBCoordinate coord)
		{
		switch ((GridIF->Data ())->Type ())
			{
			case DBTypeGridContinuous:
				{
				DBFloat value;
				if (GridIF->Value (LayerRec,coord,&value))
						TargetFLD->Float (record,value);
				else	TargetFLD->Float (record,TargetFLD->FloatNoData ());
				} break;
			case DBTypeGridDiscrete:
				{
				DBObjRecord *grdRec;
				if ((grdRec = GridIF->GridItem (LayerRec,coord)) != (DBObjRecord *) NULL)
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
				CMmsgPrint (CMmsgUsrError,"Invalid grid type in: CMDgrdVariable:GetVariable ()");
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
	DBGridIF *gridIF;
	CMDExpression **expressions = (CMDExpression **) NULL;
	CMDgrdVariable **grdVar = (CMDgrdVariable **) NULL;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-c","--calculate"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing expression!");   return (CMfailed); }
			if (expStr != (char *) NULL)
				{ CMmsgPrint (CMmsgUsrError,"Expression is already specified!"); return (CMfailed); }
			expStr = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-r","--variable"))
			{
			char *varName;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing variable name!"); return (CMfailed); }
			varName = argv [argPos];
			fieldPTR = new DBObjTableField (varName,DBVariableFloat,"%10.3f",sizeof (DBFloat),false);
			table->AddField (fieldPTR);
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing expression!");   return (CMfailed); }
			expressions = expNum < 1 ? (CMDExpression **) calloc (1,sizeof (CMDExpression *)) :
								(CMDExpression **) realloc (expressions,(expNum + 1) * sizeof (CMDExpression *));
			if (expressions == (CMDExpression **) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory Allocation error in: %s %d",__FILE__,__LINE__); return (CMfailed); }
			expressions [expNum] = new CMDExpression (varName, argv [argPos]);
			if ((expressions [expNum])->Expand (variables) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid Expression!"); return (CMfailed); }
			expNum++;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-x","--extent"))
			{
			int codes [] = { true, false };
			const char *names [] = { "minimum","maximum", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing extent mode!");     return (CMfailed); }
			if ((ret = CMoptLookup (names,argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid extent mode!");     return (CMfailed); }
			shrink = codes [ret];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-n","--interpolate"))
			{
			int codes [] = { false, true };
			const char *names [] = { "surface","flat", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing interpolation mode!");     return (CMfailed); }
			if ((ret = CMoptLookup (names,argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid interpolation mode!");     return (CMfailed); }
			flat = codes [ret];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-t","--title"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing title!");        return (CMfailed); }
			title = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-u","--subject"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing subject!");      return (CMfailed); }
			subject = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-d","--domain"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing domain!");            return (CMfailed); }
			domain  = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-v","--version"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing version!");      return (CMfailed); }
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
				{ CMmsgPrint (CMmsgUsrError,"Missing shadeset!");     return (CMfailed); }
			if ((shadeSet = CMoptLookup (shadeSets,argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid shadeset!");     return (CMfailed); }
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
			CMmsgPrint (CMmsgInfo,"%s [options] <output file>",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -c,--calculate   [expression]");
			CMmsgPrint (CMmsgInfo,"     -r,--variable    [variable expression]");
			CMmsgPrint (CMmsgInfo,"     -n,--interpolate [surface|flat]");
			CMmsgPrint (CMmsgInfo,"     -x,--extent      [maximum minimum]");
			CMmsgPrint (CMmsgInfo,"     -t,--title       [dataset title]");
			CMmsgPrint (CMmsgInfo,"     -u,--subject     [subject]");
			CMmsgPrint (CMmsgInfo,"     -d,--domain      [domain]");
			CMmsgPrint (CMmsgInfo,"     -v,--version     [version]");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset    [standard|grey|blue|blue-to-red|elevation]");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose");
			CMmsgPrint (CMmsgInfo,"     -h,--help");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && ((int) strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (expStr == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Missing expression!"); return (CMfailed); }

	if (argNum > 2) { CMmsgPrint (CMmsgUsrError,"Extra arguments!"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	operand = new DBMathOperand (expStr);
	if (operand->Expand (variables) == DBFault) return (CMfailed);

	cellSize.X = cellSize.Y = DBHugeVal;
	for (recID = 0;recID < variables->ItemNum ();++recID)
		{
		obj = variables->Item (recID);
		if ((fieldPTR = table->Field (obj->Name ())) != (DBObjTableField *) NULL) continue;

		grdVar = (CMDgrdVariable **) realloc (grdVar,sizeof (CMDgrdVariable *) * (varNum + 1));
		if (grdVar == (CMDgrdVariable **) NULL)
			{ CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__); return (CMfailed); }
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

//	TODO CMmsgPrint (CMmsgDebug, ,"%f %f %f %f",extent.LowerLeft.X, extent.LowerLeft.Y, extent.UpperRight.X, extent.UpperRight.Y);
	if (shrink) for (i = 0;i < varNum;++i)	{
//	TODO	CMmsgPrint (CMmsgDebug, ,"%f %f %f %f",grdVar [i]->Extent ().LowerLeft.X, grdVar [i]->Extent ().LowerLeft.Y, grdVar [i]->Extent ().UpperRight.X, grdVar [i]->Extent ().UpperRight.Y);
		extent.Shrink (grdVar [i]->Extent ());
	}
// TODO	CMmsgPrint (CMmsgDebug, ,"%f %f %f %f",extent.LowerLeft.X, extent.LowerLeft.Y, extent.UpperRight.X, extent.UpperRight.Y);

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
	data->Projection(grdVar [0]->Projection ()); // Taking projection from first grid variable

	gridIF = new DBGridIF (data);
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
		if (layerID > 0) gridIF->AddLayer ((char *) "New Layer");
		layerRec = gridIF->Layer (layerID);
		gridIF->RenameLayer (layerRec,layerName);
		for (pos.Row = 0;pos.Row < gridIF->RowNum ();++pos.Row)
			for (pos.Col = 0;pos.Col < gridIF->ColNum ();++pos.Col)
				{
			 	gridIF->Pos2Coord (pos,coord);
				for (i = 0;i < varNum;++i) grdVar [i]->GetVariable (record,coord);
				for (i = 0;i < expNum;++i) expressions [i]->Evaluate (record);
				gridIF->Value (layerRec,pos,operand->Float (record));
				}
		gridIF->RecalcStats (layerRec);
		}

	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Write (argv [1]) : data->Write (stdout);

	for (i = 0;i < varNum;++i) delete grdVar [i];
	delete variables;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
