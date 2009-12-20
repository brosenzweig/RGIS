/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

DBObjData.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>

DBDataHeader::DBDataHeader ()

	{
	ByteOrderVAR = 1;
	Type (DBFault);
	MajorVAR = 2;
	MinorVAR = 1;
	Projection	(0);
	Precision	(0);
	SpatialUnit	(0);
	ValueUnit	(0);
	MinScale		(3);
	MaxScale 	(7);
	}

DBDataHeader::DBDataHeader (const DBDataHeader &header)

	{
	ByteOrderVAR = 1;
	Type (header.Type ());
	MajorVAR = 2;
	MinorVAR = 1;
	Projection	(header.Projection ());
	Precision	(header.Precision ());
	SpatialUnit	(header.SpatialUnit ());
	ValueUnit	(header.ValueUnit ());
	MinScale		(header.MinScale ());
	MaxScale 	(header.MaxScale ());
	ExtentVAR = header.Extent();
	}

void DBDataHeader::Swap ()

	{
	DBByteOrderSwapHalfWord (&ByteOrderVAR);
	DBByteOrderSwapHalfWord (&TypeVAR);
	DBByteOrderSwapHalfWord (&MajorVAR);
	DBByteOrderSwapHalfWord (&MinorVAR);

	DBByteOrderSwapHalfWord (&ProjectionVAR);
	DBByteOrderSwapHalfWord (&PrecisionVAR);
	DBByteOrderSwapHalfWord (&SpatialUnitVAR);
	DBByteOrderSwapHalfWord (&ValueUnitVAR);

	DBByteOrderSwapHalfWord (&MaxScaleVAR);
	DBByteOrderSwapHalfWord (&MinScaleVAR);
	DBByteOrderSwapWord (&DummyVAR);
	ExtentVAR.Swap ();
	LastModVAR.Swap ();
	}

void DBObjData::Initialize ()

	{
	TablesPTR = new DBObjectLIST<DBObjTable>  ("Data Tables");
	DocsPTR   = new DBObjectLIST<DBObjRecord> ("Doc Records");
	ArraysPTR   = new DBObjectLIST<DBObjRecord> ("Data Records");
	DispPTR = new DBObjectLIST<DBObject> ("Data Display");
	LinkedDataPTR = (DBObjData *)  NULL;
	strcpy (FileNameSTR,"");
	}

DBObjTable *_DBCreateDataBlockSymbols ();
DBObjTable *_DBCreateDataBlockPoints	 ();
DBObjTable *_DBCreateDataBlockNodes	 ();
DBObjTable *_DBCreateDataBlockLines	 ();
DBObjTable *_DBCreateDataBlockContours();
DBObjTable *_DBCreateDataBlockPolygons();
DBObjTable *_DBCreateDataBlockNetBasins();
DBObjTable *_DBCreateDataBlockNetCells ();
DBObjTable *_DBCreateDataBlockGridLayers ();
DBObjTable *_DBCreateDataBlockGridVariableStats ();
DBObjTable *_DBCreateDataBlockGridCategoryTable ();
DBObjTable *_DBCreateDataBlockRelateTable ();

DBObjData::DBObjData (DBObjData &data) : DBObject (data), DBDataHeader (data._Header ())
	{
	DBObjTable *table;
	DBObjRecord *record, *obj;
	DBObjTableField *field;
	DBObjectLIST<DBObjTableField> *fields;
	strcpy (FileNameSTR,"");
	TablesPTR = new DBObjectLIST<DBObjTable>  (*data.TablesPTR);
	DocsPTR   = new DBObjectLIST<DBObjRecord> (*data.DocsPTR);
	ArraysPTR = new DBObjectLIST<DBObjRecord> (*data.ArraysPTR);
	DispPTR = new DBObjectLIST<DBObject> ("Data Display");
	LinkedDataPTR = (DBObjData *)  NULL;
	for (table = TablesPTR->First ();table != (DBObjTable *) NULL;table = TablesPTR->Next ())
		{
		fields = table->Fields ();
		for (record = table->First ();record != (DBObjRecord *) NULL;record = table->Next ())
			{
			for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
				switch (field->Type ())
					{
					case DBTableFieldTableRec:
					case DBTableFieldDataRec:
						if ((obj = field->Record (record)) != NULL) field->Record (record,(DBObjRecord *) obj->RowID ());
						else field->Record (record,(DBObjRecord *) DBFault);
						break;
					}
			}
		}
	BuildFields ();
	}

void DBObjData::Type (DBInt type)

	{
	DBInt realData = true;
	DBDataHeader::Type (type);
	switch (type)
		{
		case DBTypeVectorPoint:
			{
			DBObjTable *items, *symbols;
			DBObjTableField *symFLD;
			TablesPTR->Add (symbols = _DBCreateDataBlockSymbols());
			TablesPTR->Add (items   = _DBCreateDataBlockPoints	());
			symFLD = items->Field (DBrNSymbol);	symFLD->RecordProp (symbols->RowID ());
			break;
			}
		case DBTypeVectorLine:
			{
			DBObjTable *items, *nodes, *symbols;
			DBObjTableField *symFLD;
			DBObjTableField *fromNodeFLD;
			DBObjTableField *toNodeFLD;

			TablesPTR->Add (symbols	= _DBCreateDataBlockSymbols ());
			TablesPTR->Add (nodes	= _DBCreateDataBlockNodes	());
			TablesPTR->Add (items 	= _DBCreateDataBlockLines	());
			symFLD 		= items->Field (DBrNSymbol);		symFLD->RecordProp		(symbols->RowID ());
			fromNodeFLD	= items->Field (DBrNFromNode);	fromNodeFLD->RecordProp (nodes->RowID ());
			toNodeFLD	= items->Field (DBrNToNode);		toNodeFLD->RecordProp	(nodes->RowID ());
			break;
			}
		case DBTypeVectorPolygon:
			{
			DBObjTable *items, *nodes, *lines, *symbols;
			DBObjTableField *symFLD;
			DBObjTableField *fromNodeFLD;
			DBObjTableField *toNodeFLD;
			DBObjTableField *nextLineFLD;
			DBObjTableField *prevLineFLD;
			DBObjTableField *leftPolyFLD;
			DBObjTableField *rightPolyFLD;
			DBObjTableField *firstLineFLD;

			TablesPTR->Add (symbols = _DBCreateDataBlockSymbols  ());
			TablesPTR->Add (nodes	= _DBCreateDataBlockNodes    ());
			TablesPTR->Add (lines	= _DBCreateDataBlockContours ());
			TablesPTR->Add (items   = _DBCreateDataBlockPolygons ());

			symFLD		= items->Field (DBrNSymbol);		symFLD->RecordProp 		(symbols->RowID ());
			fromNodeFLD	= lines->Field (DBrNFromNode);	fromNodeFLD->RecordProp (nodes->RowID ());
			toNodeFLD	= lines->Field (DBrNToNode);		toNodeFLD->RecordProp	(nodes->RowID ());
			nextLineFLD	= lines->Field (DBrNNextLine);	nextLineFLD->RecordProp	(lines->RowID ());
			prevLineFLD	= lines->Field (DBrNPrevLine);	prevLineFLD->RecordProp	(lines->RowID ());
			leftPolyFLD	= lines->Field (DBrNLeftPoly);	leftPolyFLD->RecordProp	(items->RowID ());
			rightPolyFLD= lines->Field (DBrNRightPoly);	rightPolyFLD->RecordProp(items->RowID ());
			firstLineFLD= items->Field (DBrNFirstLine);	firstLineFLD->RecordProp(lines->RowID ());
			break;
			}
		case DBTypeNetwork:
			{
			DBObjTable *basins, *symbols;
			DBObjTableField *symField;
			TablesPTR->Add (symbols = _DBCreateDataBlockSymbols  ());
			TablesPTR->Add (basins	= _DBCreateDataBlockNetBasins ());
			TablesPTR->Add (_DBCreateDataBlockNetCells  ());
			TablesPTR->Add (_DBCreateDataBlockGridLayers ());
			symField = basins->Field  (DBrNSymbol);		symField->RecordProp (symbols->RowID ());
			break;
			}
		case DBTypeGridContinuous:
			{
			TablesPTR->Add (_DBCreateDataBlockGridLayers ());
			TablesPTR->Add (_DBCreateDataBlockGridVariableStats ());
			break;
			}
		case DBTypeGridDiscrete:
			{
			DBObjTable *items, *symbols;
			DBObjTableField *symFLD;

			TablesPTR->Add (symbols = _DBCreateDataBlockSymbols  ());
			TablesPTR->Add (_DBCreateDataBlockGridLayers ());
			TablesPTR->Add (items = _DBCreateDataBlockGridCategoryTable ());
			symFLD		= items->Field (DBrNSymbol);		symFLD->RecordProp 		(symbols->RowID ());
			break;
			}
		case	DBTypeTable:
			TablesPTR->Add (new DBObjTable (DBrNItems));
			break;
		default:	realData = false;	break;
		}
	if (realData) TablesPTR->Add (_DBCreateDataBlockRelateTable ());
	}

void DBObjData::Document (const char *docName,const char *string)

	{
	DBObjRecord *obj = DocsPTR->Item (docName);

	if (obj == (DBObjRecord *) NULL)
		{
		DocsPTR->Add (obj = new DBObjRecord (docName,sizeof (DBVarString)));
		memset (obj->Data (),0,sizeof (DBVarString));
		((DBVarString *) obj->Data ())->VString (string);
		}
	else ((DBVarString *) obj->Data ())->VString (string);
	}

char *DBObjData::Document (const char *docName)

	{
	DBObjRecord *obj = DocsPTR->Item (docName);
	if (obj == (DBObjRecord *) NULL)
		{
		if ((strcmp (docName,DBDocSubject) == 0) || (strcmp (docName,DBDocGeoDomain) == 0))
			{ Document (docName,(char *) "unspecified"); return ((char *) "unspecified"); }
		else return ((char *) "");
		}
	return (((DBVarString *) obj->Data ())->VString ());
	}

DBRegion DBObjData::Extent (DBObjRecord *record)

	{
	DBRegion extent;
	DBCoordinate coord;
	DBFloat delta;
	DBObjTable *items;
	DBObjTableField *field;

	if (record == (DBObjRecord *) NULL) return (Extent ());

	switch (Type ())
		{
		case DBTypeVectorPoint:
			delta = pow ((double) 10.0,(double) Precision ());
			items = TablesPTR->Item (DBrNItems);
			field  = items->Field (DBrNCoord);
			coord = field->Coordinate (record);
			extent.Expand (coord + delta); extent.Expand (coord - delta);
			return (extent);
		case DBTypeVectorLine:
		case DBTypeVectorPolygon:
			items = TablesPTR->Item (DBrNItems);
			field = items->Field (DBrNRegion);
			return (field->Region (record));
		case DBTypeGridDiscrete:
		case DBTypeGridContinuous:	return (Extent ());
		case DBTypeNetwork:
			{
			DBInt cellID, cellNum;
			DBObjRecord *cellRec;
			DBNetworkIO *netIO = new DBNetworkIO (this);

			cellRec = netIO->MouthCell (record);
			cellNum = netIO->CellBasinCells (cellRec) + cellRec->RowID ();
			for (cellID = cellRec->RowID ();cellID < cellNum;++cellID)
				{
				cellRec = netIO->Cell (cellID);
				extent.Expand (netIO->Center (cellRec) + (netIO->CellSize () / 2.0));
				extent.Expand (netIO->Center (cellRec) - (netIO->CellSize () / 2.0));
				}
			delete netIO;
			} return (extent);
		case DBTypeTable:
		default:	return (extent);
		}
	}

void DBObjData::RecalcExtent ()
	{
	DBRegion extent;

	switch (Type ())
		{
		case DBTypeVectorPoint:
		case DBTypeVectorLine:
		case DBTypeVectorPolygon:
			{
			DBVectorIO *vectorIO = new DBVectorIO (this);
			DBInt recordID;
			for (recordID = 0;recordID < vectorIO->ItemNum ();++recordID)
				extent.Expand (Extent (vectorIO->Item (recordID)));
			delete vectorIO;
			}
			break;
		case DBTypeGridDiscrete:
		case DBTypeGridContinuous:
			{
			DBGridIO *gridIO = new DBGridIO (this);
			extent.LowerLeft = Extent ().LowerLeft;
			extent.UpperRight.X = extent.LowerLeft.X + gridIO->ColNum () * gridIO->CellWidth ();
			extent.UpperRight.Y = extent.LowerLeft.Y + gridIO->RowNum () * gridIO->CellHeight ();
			delete gridIO;
			}
			break;
		case DBTypeNetwork:
			{
			DBNetworkIO *netIO = new DBNetworkIO (this);
			extent.LowerLeft = Extent ().LowerLeft;
			extent.UpperRight.X = extent.LowerLeft.X + netIO->ColNum () * netIO->CellWidth ();
			extent.UpperRight.Y = extent.LowerLeft.Y + netIO->RowNum () * netIO->CellHeight ();
			delete netIO;
			}
			break;
		case DBTypeTable:
		default:	return;
		}
	Extent (extent);
	}

DBInt DBObjData::SelectObject (DBObjRecord *record,DBInt select)

	{
	DBObjTable *items = TablesPTR->Item (DBrNItems);

	if (record == (DBObjRecord *) NULL) return (false);
	if (items->Item (record->RowID ()) != record) return (false);

	record->Flags (DBObjectFlagSelected,select);
	if (Type () == DBTypeNetwork)
		{
		DBInt cellID;
		DBObjRecord *cellRec;
		DBNetworkIO *netIO = new DBNetworkIO (this);

		for (cellID = 0;cellID < netIO->CellNum ();++cellID)
			{
			cellRec = netIO->Cell (cellID);
			if (netIO->CellBasinID (cellRec) == record->RowID () + 1)
				cellRec->Flags (DBObjectFlagSelected,select);
			}
		delete netIO;
		}
	return (DBSuccess);
	}

char *DBDataTypeString (int dataType)

	{
	char *typeString;
	switch (dataType)
		{
		case DBTypeVectorPoint: 	typeString = (char *) "Point";      break;
		case DBTypeVectorLine:		typeString = (char *) "Line";       break;
		case DBTypeVectorPolygon:	typeString = (char *) "Polygon";    break;
		case DBTypeGridContinuous:	typeString = (char *) "Continuous"; break;
		case DBTypeGridDiscrete:	typeString = (char *) "Discrete";   break;
		case DBTypeNetwork:			typeString = (char *) "Network";    break;
		case DBTypeTable:           typeString = (char *) "Tabular";    break;
		default:                    typeString = (char *) "UNKNOWN";    break;
		}
	return (typeString);
	}
