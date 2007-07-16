/******************************************************************************

GHAAS RiverGIS Libarary V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

RGlibPoint.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>
#include <RG.H>


DBInt RGlibPointSTNCoordinates (DBObjData *dbData,DBObjTableField *field)

	{
	DBInt pointID, ret = DBFault;
	DBCoordinate coord;
	DBPosition pos;
	DBObjData *linkedData = dbData->LinkedData ();
	DBVPointIO *pointIO;
	DBNetworkIO *netIO;
	DBObjRecord *pntRec, *cellRec;
	
	if (linkedData == (DBObjData *) NULL) return (DBFault);
	pointIO = new DBVPointIO (dbData);
	netIO = new DBNetworkIO (linkedData);
	for (pointID = 0;pointID < pointIO->ItemNum (); ++pointID)
		{
		pntRec = pointIO->Item (pointID);
		if (DBPause (pointID * 100 / pointIO->ItemNum ())) goto Stop;
		coord = pointIO->Coordinate (pntRec);
		if (netIO->Coord2Pos (coord,pos) == DBFault) continue;
		netIO->Pos2Coord (pos,coord);
		if ((field != (DBObjTableField *) NULL) &&
			 (!DBMathEqualValues (field->Float (pntRec),field->FloatNoData ())) &&
			 ((cellRec = netIO->Cell (coord,field->Float (pntRec))) != (DBObjRecord *) NULL))
			coord = netIO->Center (cellRec);
		pointIO->Coordinate (pntRec,coord);
		}
	ret = DBSuccess;
Stop:
	delete netIO;
	delete pointIO;
	return (ret);
	}

#define RGlibTEMPPointID "TEMPPointID"
static DBObjTableField *_RGlibTEMPPointIDFLD;

static DBInt _RGlibSetPointID (DBNetworkIO *netIO,DBObjRecord *cellRec,DBInt pointID)

	{ netIO = netIO; _RGlibTEMPPointIDFLD->Int (cellRec,pointID); return (true); }

DBInt RGlibPointSTNCharacteristics (DBObjData *dbData)

	{
	DBInt i, pointID, dPointID, cellID, mouthID, basinID, color, ret = DBFault, dir;
	DBVPointIO *pointIO;
	DBObjTable *pointTable, *cellTable;
	DBObjTableField *cellIDFLD;
	DBObjTableField *basinFLD;
	DBObjTableField *basinNameFLD;
	DBObjTableField *orderFLD;
	DBObjTableField *colorFLD;
	DBObjTableField *basinCellsFLD;
	DBObjTableField *basinLengthFLD;
	DBObjTableField *basinAreaFLD;
	DBObjTableField *interAreaFLD;
	DBObjTableField *nextStationFLD;
	DBObjData *netData;
	DBNetworkIO *netIO;
	DBObjRecord *pointRec, *dPointRec, *cellRec, *fromCell, *basinRec;

	if ((netData = dbData->LinkedData ()) == (DBObjData *) NULL) return (DBFault);
	pointTable = dbData->Table (DBrNItems);
	pointIO = new DBVPointIO (dbData);
	netIO = new DBNetworkIO (netData);
	cellTable = netData->Table (DBrNCells);
	if ((cellIDFLD = pointTable->Field (RGlibCellID)) == NULL)
		{
		cellIDFLD = new DBObjTableField (RGlibCellID,DBTableFieldInt,"%8d",sizeof (DBInt));
		pointTable->AddField (cellIDFLD);
		DBPause (1);
		}
	if ((basinFLD = pointTable->Field (DBrNBasin)) == NULL)
		{
		basinFLD = new DBObjTableField (DBrNBasin,DBTableFieldInt,"%8d",sizeof (DBInt));
		pointTable->AddField (basinFLD);
		DBPause (2);
		}
	if ((basinNameFLD = pointTable->Field (RGlibBasinName)) == NULL)
		{
		basinNameFLD = new DBObjTableField (RGlibBasinName,DBTableFieldString,"%32s",DBStringLength);
		pointTable->AddField (basinNameFLD);
		DBPause (3);
		}
	if ((orderFLD = pointTable->Field (DBrNOrder)) == NULL)
		{
		orderFLD = new DBObjTableField (DBrNOrder,DBTableFieldInt,"%3d",sizeof (DBByte));
		pointTable->AddField (orderFLD);
		DBPause (4);
		}
	if ((colorFLD = pointTable->Field (RGlibColor)) == NULL)
		{
		colorFLD = new DBObjTableField (RGlibColor,DBTableFieldInt,"%2d",sizeof (DBShort));
		pointTable->AddField (colorFLD);
		DBPause (5);
		}
	if ((basinCellsFLD = pointTable->Field (RGlibCellNum)) == NULL)
		{
		basinCellsFLD = new DBObjTableField (RGlibCellNum,DBTableFieldInt,"%8d",sizeof (DBInt));
		pointTable->AddField (basinCellsFLD);
		DBPause (6);
		}
	if ((basinLengthFLD = pointTable->Field (RGlibLength)) == NULL)
		{
		basinLengthFLD = new DBObjTableField (RGlibLength,DBTableFieldFloat,"%10.1f",sizeof (DBFloat4));
		pointTable->AddField (basinLengthFLD);
		DBPause (7);
		}
	if ((basinAreaFLD = pointTable->Field (RGlibArea)) == NULL)
		{
		basinAreaFLD = new DBObjTableField (RGlibArea,DBTableFieldFloat,"%10.1f",sizeof (DBFloat4));
		pointTable->AddField (basinAreaFLD);
		DBPause (8);
		}
	if ((interAreaFLD = pointTable->Field (RGlibInterStation)) == NULL)
		{
		interAreaFLD = new DBObjTableField (RGlibInterStation,DBTableFieldFloat,"%10.1f",sizeof (DBFloat4));
		pointTable->AddField (interAreaFLD);
		DBPause (9);
		}
	if ((nextStationFLD = pointTable->Field (RGlibNextStation)) == NULL)
		{
		nextStationFLD = new DBObjTableField (RGlibNextStation,DBTableFieldInt,"%8d",sizeof (DBInt));
		pointTable->AddField (nextStationFLD);
		DBPause (10);
		}
	if ((_RGlibTEMPPointIDFLD = cellTable->Field (RGlibTEMPPointID)) == NULL)
		{
		_RGlibTEMPPointIDFLD = new DBObjTableField (RGlibTEMPPointID,DBTableFieldInt,"%8d",sizeof (DBInt));
		cellTable->AddField (_RGlibTEMPPointIDFLD);
		}
	for (pointID = 0; pointID < pointTable->ItemNum (); pointID++)
		{
		pointRec = pointTable->Item (pointID);
		if (DBPause (10 + pointID * 10 / pointTable->ItemNum ())) goto Stop;
		if ((pointRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle)
			{
			cellIDFLD->Int (pointRec,cellIDFLD->IntNoData ());
			basinFLD->Int (pointRec,basinFLD->IntNoData ());
			basinNameFLD->String (pointRec,"");
			orderFLD->Int (pointRec,orderFLD->IntNoData ());
			colorFLD->Int (pointRec,colorFLD->IntNoData ());
			basinCellsFLD->Int (pointRec,basinCellsFLD->IntNoData ());
			basinAreaFLD->Float (pointRec,basinAreaFLD->FloatNoData ());
			interAreaFLD->Float (pointRec,interAreaFLD->FloatNoData ());
			continue;
			}
		if ((cellRec = netIO->Cell (pointIO->Coordinate  (pointRec))) == (DBObjRecord *) NULL)
			{
			cellIDFLD->Int (pointRec,0);
			basinFLD->Int (pointRec,0);
			basinNameFLD->String (pointRec,"Water");
			orderFLD->Int (pointRec,colorFLD->IntNoData ());
			colorFLD->Int (pointRec,colorFLD->IntNoData ());
			basinCellsFLD->Int (pointRec,0);
			basinAreaFLD->Float (pointRec,0.0);
			interAreaFLD->Float (pointRec,0.0);
			}
		else
			{
			cellIDFLD->Int (pointRec,cellRec->RowID () + 1);
			basinRec = netIO->Basin (cellRec);
			basinFLD->Int (pointRec,basinRec->RowID () + 1);
			basinNameFLD->String (pointRec,basinRec->Name ());
			orderFLD->Int (pointRec,netIO->CellOrder (cellRec));
			colorFLD->Int (pointRec,0);
			basinCellsFLD->Int (pointRec,netIO->CellBasinCells (cellRec));
			basinLengthFLD->Float (pointRec,netIO->CellBasinLength (cellRec));
			basinAreaFLD->Float (pointRec,netIO->CellBasinArea (cellRec));
			interAreaFLD->Float (pointRec,netIO->CellBasinArea (cellRec));
			}
		nextStationFLD->Int (pointRec,0);
		}
	for (cellID = 0;cellID < cellTable->ItemNum ();++cellID)
		{
		if (DBPause (20 + cellID * 20 / cellTable->ItemNum ())) goto Stop;
		cellRec = cellTable->Item (cellID);
		_RGlibTEMPPointIDFLD->Int (cellRec,DBFault);
		}
	pointTable->ListSort (basinAreaFLD);
	for (pointRec = pointTable->Last ();pointRec != (DBObjRecord *) NULL;pointRec = pointTable->Next (DBBackward))
		{
		if ((pointRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		if (DBPause (40 + pointID * 20 / pointTable->ItemNum ())) goto Stop;
		cellRec = netIO->Cell (pointIO->Coordinate  (pointRec));
		netIO->UpStreamSearch (cellRec,(DBNetworkACTION) _RGlibSetPointID,(void *) pointRec->RowID ());
		}
	for (pointID = 0; pointID < pointTable->ItemNum (); pointID++)
		{
		pointRec = pointTable->Item (pointID);
		if ((pointRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		if (DBPause (60 + pointID * 20 / pointTable->ItemNum ())) goto Stop;
		if ((cellRec = netIO->Cell (pointIO->Coordinate  (pointRec))) != (DBObjRecord *) NULL)
			{
			if ((cellRec = netIO->ToCell (cellRec)) == (DBObjRecord *) NULL) continue;
			if ((dPointID = _RGlibTEMPPointIDFLD->Int (cellRec)) != DBFault)
				{
				dPointRec = pointTable->Item (dPointID);
				nextStationFLD->Int (pointRec,dPointRec->RowID () + 1);
				interAreaFLD->Float (dPointRec,interAreaFLD->Float (dPointRec) - basinAreaFLD->Float (pointRec));
				}
			}
		}
	pointTable->ListSort (interAreaFLD);

	i = 0;
	for (pointRec = pointTable->Last ();pointRec != (DBObjRecord *) NULL;pointRec = pointTable->Next (DBBackward))
		{
		if (DBPause (80 + (i++) * 20 / pointTable->ItemNum ())) goto Stop;
		pointID = pointRec->RowID ();
		if ((pointRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;

		if ((basinID = basinFLD->Int (pointRec)) == 0) continue;
		cellRec = netIO->Cell (pointIO->Coordinate  (pointRec));
		mouthID = cellRec->RowID ();
		color = 1;
Start:
		for (cellID = mouthID;cellID < cellTable->ItemNum ();++cellID)
			{			
			cellRec = cellTable->Item (cellID);
			if (netIO->CellBasinID (cellRec) != basinID) break;
			if (_RGlibTEMPPointIDFLD->Int (cellRec) != pointID) continue;

			for (dir = 0;dir < 8;++dir)
				{
				if ((fromCell = netIO->FromCell (cellRec,0x01 << dir,false)) == (DBObjRecord *) NULL) continue;
				if ((dPointID = _RGlibTEMPPointIDFLD->Int (fromCell)) == pointID) continue;
				if	(dPointID == DBFault) continue;

				dPointRec = pointTable->Item (dPointID);
				if (colorFLD->Int (dPointRec) == color) { color++; goto Start; }
				}
			}
		colorFLD->Int (pointRec,color);
		}
	ret = DBSuccess;	
Stop:
	pointTable->ListSort ();
	cellTable->DeleteField (_RGlibTEMPPointIDFLD);
	delete pointIO;
	delete netIO;
	return (ret);
	}

static DBInt _RGlibSubbasinCenterAction (DBNetworkIO *netIO,DBObjRecord *cellRec,DBCoordinate *massCoord)

	{
	DBCoordinate coord = netIO->Center (cellRec);
	massCoord->X += coord.X;
	massCoord->Y += coord.Y;
	return (true); 
	}

DBInt RGlibPointSubbasinCenter (DBObjData *pntData, DBObjData *netData)

	{
	DBCoordinate massCoord;
	DBVPointIO *pointIO = new DBVPointIO (pntData);
	DBObjTable *pointTable = pntData->Table (DBrNItems);
	DBObjTableField *massCoordXFLD = pointTable->Field (RGlibMassCoordX);
	DBObjTableField *massCoordYFLD = pointTable->Field (RGlibMassCoordY);
	DBNetworkIO *netIO = new DBNetworkIO (netData);
	DBObjRecord *pointRec, *cellRec;

	if (massCoordXFLD == NULL)
		{
		massCoordXFLD = new DBObjTableField (RGlibMassCoordX,DBTableFieldFloat,"%10.3f",sizeof (DBFloat4));
		pointTable->AddField (massCoordXFLD);
		}
	if (massCoordYFLD == NULL)
		{
		massCoordYFLD = new DBObjTableField (RGlibMassCoordY,DBTableFieldFloat,"%10.3f",sizeof (DBFloat4));
		pointTable->AddField (massCoordYFLD);
		}

	for (pointRec = pointIO->FirstItem (); pointRec != (DBObjRecord *) NULL; pointRec = pointIO->NextItem ())
		{
		if ((pointRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle)
			{
			massCoordXFLD->Float (pointRec,massCoordXFLD->FloatNoData ());
			massCoordYFLD->Float (pointRec,massCoordYFLD->FloatNoData ());
			continue;
			}
		if (DBPause (pointRec->RowID () * 100 / pointIO->ItemNum ())) goto Stop;
		if ((cellRec = netIO->Cell (pointIO->Coordinate (pointRec))) == (DBObjRecord *) NULL)
			massCoord = pointIO->Coordinate  (pointRec);
		else
			{
			if (netIO->CellBasinCells (cellRec) > 1)
				{
				massCoord.X = 0.0; massCoord.Y = 0.0;
				netIO->UpStreamSearch (cellRec,(DBNetworkACTION) _RGlibSubbasinCenterAction,&massCoord);
				massCoord.X = massCoord.X / (DBFloat) netIO->CellBasinCells (cellRec);
				massCoord.Y = massCoord.Y / (DBFloat) netIO->CellBasinCells (cellRec);
				}
			else massCoord = netIO->Center (cellRec);
			}
		massCoordXFLD->Float (pointRec,massCoord.X);
		massCoordYFLD->Float (pointRec,massCoord.Y);
		}
Stop:
	if (pointRec != (DBObjRecord *) NULL)
		{
		pointTable->DeleteField (massCoordXFLD);
		pointTable->DeleteField (massCoordYFLD);
		return (DBFault);
		}
	return (DBSuccess);
	}

DBInt RGlibPointInterStationTS (DBObjData *pntData,DBObjData *tsData, char *relateFldName,char *joinFldName)

	{
	DBInt first = true, tsIndex, tsRowNum = 0;
	DBObjTable *pntTBL = pntData->Table (DBrNItems), *tsTBL;
	DBObjectLIST<DBObjTableField> *fields;
	DBObjTableField *pntNextFLD = pntTBL->Field (RGlibNextStation);
	DBObjTableField *pntAreaFLD = pntTBL->Field (RGlibArea);
	DBObjTableField *pntInterStnFLD = pntTBL->Field (RGlibInterStation);
	DBObjTableField *pntRelateFLD;
	DBObjTableField *pntNewNextFLD;
	DBObjTableField *pntNewInterStnFLD;
	DBObjTableField *tsTimeFLD;
	DBObjTableField *tsJoinFLD;
	DBObjTableField *tsNextStnFLD, *tsInterStnFLD;
	DBObjRecord *pntRec, *nextPntRec, *tsRec, *tsIndexRec;
	DBDate curDate, date;

	if (pntNextFLD == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Missing Next Station Field in: RGlibPointInterStationTS ()\n");		return (DBFault); }
	if (pntAreaFLD == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Missing STN Area Field in: RGlibPointInterStationTS ()\n");			return (DBFault); }
	if (pntInterStnFLD == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Missing Interfluvial Area Field in: RGlibPointInterStationTS ()\n");	return (DBFault); }

	tsTBL = tsData->Table (DBrNItems);
	tsNextStnFLD = new DBObjTableField (RGlibNextStation,DBTableFieldInt,"%8d",sizeof (DBInt));
	tsTBL->AddField (tsNextStnFLD);
	tsInterStnFLD = new DBObjTableField (RGlibInterStation,DBTableFieldFloat,"%9.1f",sizeof (DBFloat4));
	tsTBL->AddField (tsInterStnFLD);
	fields = tsTBL->Fields ();
	for (tsTimeFLD = fields->First ();tsTimeFLD != (DBObjTableField *) NULL;tsTimeFLD = fields->Next ())
			if (tsTimeFLD->Type () == DBTableFieldDate) break;
	if (tsTimeFLD == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Missing Date Field in: RGlibPointInterStationTS ()\n");		return (DBFault); }
	if ((tsJoinFLD = tsTBL->Field (joinFldName)) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Missing Join Field in: RGlibPointInterStationTS ()\n"); 	return (DBFault); }
	fields = new DBObjectLIST<DBObjTableField> ("Field List");
	fields->Add (new DBObjTableField (*tsTimeFLD));
	fields->Add (new DBObjTableField (*tsJoinFLD));
	tsTBL->ListSort (fields);
	delete fields;

	pntTBL = new DBObjTable (*pntTBL);
	pntNextFLD = pntTBL->Field (RGlibNextStation);
	pntAreaFLD = pntTBL->Field (RGlibArea);
	pntInterStnFLD = pntTBL->Field (RGlibInterStation);
	pntNewNextFLD = new DBObjTableField ("NextStnTS",pntNextFLD->Type (),pntNextFLD->Format (),pntNextFLD->Length ());
	pntNewInterStnFLD = new DBObjTableField ("InterFluTS",pntInterStnFLD->Type (),pntInterStnFLD->Format (),pntInterStnFLD->Length ());
	pntRelateFLD = pntTBL->Field (relateFldName);
	pntTBL->AddField (pntNewNextFLD);
	pntTBL->AddField (pntNewInterStnFLD);
	pntTBL->ListSort (pntRelateFLD);

	tsIndexRec = tsTBL->First (&tsIndex);
	for (tsRec = tsTBL->First ();tsRec != (DBObjRecord *) NULL;tsRec = tsTBL->Next ())
		{
		DBPause (tsRowNum++ * 100 / tsTBL->ItemNum ());
		date = tsTimeFLD->Date (tsRec);
		if (date != curDate)
			{
			if (first)	first = false;
			else
				{
				for (pntRec = pntTBL->First ();pntRec != (DBObjRecord *) NULL;pntRec = pntTBL->Next ())
					{
					if ((pntRec->Flags () & DBObjectFlagLocked) != DBObjectFlagLocked) continue;
					for (nextPntRec = pntTBL->Item (pntNextFLD->Int (pntRec) - 1);
						  (nextPntRec != (DBObjRecord *) NULL) &&
						  ((nextPntRec->Flags () & DBObjectFlagLocked) != DBObjectFlagLocked);
						  nextPntRec = pntTBL->Item (pntNextFLD->Int (nextPntRec) - 1));
					if (nextPntRec != (DBObjRecord *) NULL)
						{
						pntNewNextFLD->Int (pntRec,nextPntRec->RowID () + 1);
						pntNewInterStnFLD->Float (nextPntRec,pntNewInterStnFLD->Float (nextPntRec) - pntAreaFLD->Float (pntRec));
						}
					}
				pntRec = pntTBL->First ();
				for (;tsIndexRec != (DBObjRecord *) NULL;tsIndexRec = tsTBL->Next (&tsIndex))
					{
					if (tsRec == tsIndexRec) break;
					for (;pntRec != (DBObjRecord *) NULL;pntRec = pntTBL->Next ())
						if (pntRelateFLD->Int (pntRec) == tsJoinFLD->Int (tsIndexRec))
							{
							tsNextStnFLD->Int (tsIndexRec,pntNewNextFLD->Int (pntRec));
							tsInterStnFLD->Float (tsIndexRec,pntNewInterStnFLD->Float (pntRec));
							break;
							}
					if (pntRec == (DBObjRecord *) NULL) pntRec = pntTBL->First ();
					}
				}
			for (pntRec = pntTBL->First ();pntRec != (DBObjRecord *) NULL;pntRec = pntTBL->Next ())
				{
				pntNewNextFLD->Int (pntRec,0);
				pntNewInterStnFLD->Float (pntRec,pntAreaFLD->Float (pntRec));
				pntRec->Flags (DBObjectFlagLocked,DBClear);
				}
			curDate = date;
			pntRec = pntTBL->First ();
			}
		for (;pntRec != (DBObjRecord *) NULL;pntRec = pntTBL->Next ())
			if (pntRelateFLD->Int (pntRec) == tsJoinFLD->Int (tsRec))
				{ pntRec->Flags (DBObjectFlagLocked,DBSet); break; }
		if (pntRec == (DBObjRecord *) NULL) pntRec = pntTBL->First ();
		}
	if (!first)
		{
		for (pntRec = pntTBL->First ();pntRec != (DBObjRecord *) NULL;pntRec = pntTBL->Next ())
			{
			if ((pntRec->Flags () & DBObjectFlagLocked) != DBObjectFlagLocked) continue;
			for (nextPntRec = pntTBL->Item (pntNextFLD->Int (pntRec) - 1);
				  (nextPntRec != (DBObjRecord *) NULL) &&
				  ((nextPntRec->Flags () & DBObjectFlagLocked) != DBObjectFlagLocked);
			nextPntRec = pntTBL->Item (pntNextFLD->Int (nextPntRec) - 1));
			if (nextPntRec != (DBObjRecord *) NULL)
				{
				pntNewNextFLD->Int (pntRec,nextPntRec->RowID () + 1);
				pntNewInterStnFLD->Float (nextPntRec,pntNewInterStnFLD->Float (nextPntRec) - pntAreaFLD->Float (pntRec));
				}
			}
		pntRec = pntTBL->First ();
		for (;tsIndexRec != (DBObjRecord *) NULL;tsIndexRec = tsTBL->Next (&tsIndex))
			{
			if (tsRec == tsIndexRec) break;
			for (;pntRec != (DBObjRecord *) NULL;pntRec = pntTBL->Next ())
			if (pntRelateFLD->Int (pntRec) == tsJoinFLD->Int (tsIndexRec))
				{
				tsNextStnFLD->Int (tsIndexRec,pntNewNextFLD->Int (pntRec));
				tsInterStnFLD->Float (tsIndexRec,pntNewInterStnFLD->Float (pntRec));
				break;
				}
			if (pntRec == (DBObjRecord *) NULL) pntRec = pntTBL->First ();
			}
		}
	delete pntTBL;
	return (DBSuccess);
	}

#define RGlibPointMin		"SubbasinMin"
#define RGlibPointMax		"SubbasinMax"
#define RGlibPointMean		"SubbasinMean"
#define RGlibPointStdDev	"SubbasinStdDev"
#define RGlibPointArea		"SubbasinArea"

static DBFloat _RGlibSubbasinArea;
static DBFloat _RGlibSubbasinMean;
static DBFloat _RGlibSubbasinMin;
static DBFloat _RGlibSubbasinMax;
static DBFloat _RGlibSubbasinStdDev;
static DBGridIO *_RGlibPointGrdIO;
static DBObjRecord *_RGlibPointGrdLayerRec;

static DBInt _RGlibSubbasinStatistics (void *io,DBObjRecord *cellRec)

	{
	DBFloat value;
	DBNetworkIO *netIO = (DBNetworkIO *) io;
	if (cellRec == (DBObjRecord *) NULL) return (false);
	if (_RGlibPointGrdIO->Value (_RGlibPointGrdLayerRec,netIO->Center (cellRec),&value) == false) return (true);
	_RGlibSubbasinArea = _RGlibSubbasinArea + netIO->CellArea (cellRec);
	_RGlibSubbasinMean = _RGlibSubbasinMean + value * netIO->CellArea (cellRec);
	_RGlibSubbasinMin  = _RGlibSubbasinMin < value ? _RGlibSubbasinMin : value;
	_RGlibSubbasinMax  = _RGlibSubbasinMax > value ? _RGlibSubbasinMax : value;
	_RGlibSubbasinStdDev = _RGlibSubbasinStdDev + value * value * netIO->CellArea (cellRec);
	return (true);
	}

DBInt RGlibPointSubbasinStats (DBObjData *pntData, DBObjData *netData, DBObjData *grdData, DBObjData *tblData)

	{
	DBInt layerID, layerNum = 0, progress = 0, maxProgress;
	DBObjTable *table;
	DBObjTableField *pointIDFLD;
	DBObjTableField *layerIDFLD;
	DBObjTableField *layerNameFLD;
	DBObjTableField *minimumFLD;
	DBObjTableField *maximumFLD;
	DBObjTableField *averageFLD;
	DBObjTableField *stdDevFLD;
	DBObjTableField *areaFLD;
	DBVPointIO *pointIO;
	DBNetworkIO *netIO;
	DBObjRecord *pntRec, *tblRec;
	DBObjectLIST<DBObjTableField> *fields;

	_RGlibPointGrdIO = new DBGridIO (grdData);
	for (layerID = 0;layerID < _RGlibPointGrdIO->LayerNum ();++layerID)
		{
		_RGlibPointGrdLayerRec = _RGlibPointGrdIO->Layer (layerID);
		if ((_RGlibPointGrdLayerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{
		fprintf (stderr,"No Layer to Process in RGlibPointSubbasinStats ()\n");
		delete _RGlibPointGrdIO;
		return (DBFault);
		}

	table = tblData->Table (DBrNItems);
	pointIO =new DBVPointIO (pntData);
	netIO = new DBNetworkIO (netData);

	table->AddField (pointIDFLD =		new DBObjTableField ("GHAASPointID",	DBTableFieldInt,"%8d",sizeof (DBInt)));
	table->AddField (layerIDFLD = 	new DBObjTableField ("LayerID",			DBTableFieldInt,"%4d",sizeof (DBShort)));
	table->AddField (layerNameFLD = 	new DBObjTableField ("LayerName",		DBTableFieldString,"%s",DBStringLength));
	table->AddField (averageFLD = 	new DBObjTableField (RGlibPointMean,	DBTableFieldFloat,_RGlibPointGrdIO->ValueFormat (),sizeof (DBFloat4)));
	table->AddField (minimumFLD = 	new DBObjTableField (RGlibPointMin,		DBTableFieldFloat,_RGlibPointGrdIO->ValueFormat (),sizeof (DBFloat4)));
	table->AddField (maximumFLD = 	new DBObjTableField (RGlibPointMax,		DBTableFieldFloat,_RGlibPointGrdIO->ValueFormat (),sizeof (DBFloat4)));
	table->AddField (stdDevFLD  = 	new DBObjTableField (RGlibPointStdDev,	DBTableFieldFloat,_RGlibPointGrdIO->ValueFormat (),sizeof (DBFloat4)));
	table->AddField (areaFLD 	 = 	new DBObjTableField (RGlibPointArea,	DBTableFieldFloat,_RGlibPointGrdIO->ValueFormat (),sizeof (DBFloat4)));

	grdData->Flags (DBObjectFlagProcessed,DBSet);
	maxProgress = pointIO->ItemNum () * _RGlibPointGrdIO->LayerNum ();
	for (layerID = 0;layerID < _RGlibPointGrdIO->LayerNum ();++layerID)
		{
		_RGlibPointGrdLayerRec = _RGlibPointGrdIO->Layer (layerID);
		if ((_RGlibPointGrdLayerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		for (pntRec = pointIO->FirstItem ();pntRec != (DBObjRecord *) NULL;pntRec = pointIO->NextItem ())
			{
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			if ((pntRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			tblRec = table->Add (pntRec->Name ());
			pointIDFLD->Int (tblRec,pntRec->RowID () + 1);
			layerIDFLD->Int (tblRec,_RGlibPointGrdLayerRec->RowID ());
			layerNameFLD->String (tblRec,_RGlibPointGrdLayerRec->Name ());
			_RGlibSubbasinArea = 0.0;
			_RGlibSubbasinMin = DBHugeVal;
			_RGlibSubbasinMax = -DBHugeVal;
			_RGlibSubbasinMean = 0.0;
			_RGlibSubbasinStdDev = 0.0;
			netIO->UpStreamSearch (netIO->Cell (pointIO->Coordinate (pntRec)),(DBNetworkACTION) _RGlibSubbasinStatistics);
			_RGlibSubbasinMean = _RGlibSubbasinMean / _RGlibSubbasinArea;
			_RGlibSubbasinStdDev = _RGlibSubbasinStdDev / _RGlibSubbasinArea;	
			_RGlibSubbasinStdDev = _RGlibSubbasinStdDev - _RGlibSubbasinMean * _RGlibSubbasinMean;
			_RGlibSubbasinStdDev = sqrt (_RGlibSubbasinStdDev);
			minimumFLD->Float (tblRec,_RGlibSubbasinMin);
			maximumFLD->Float (tblRec,_RGlibSubbasinMax);
			averageFLD->Float (tblRec,_RGlibSubbasinMean);
			stdDevFLD->Float  (tblRec,_RGlibSubbasinStdDev);
			areaFLD->Float		(tblRec,_RGlibSubbasinArea);
			}
		}
Stop:
	delete _RGlibPointGrdIO;
	delete netIO;
	delete pointIO;

	if (progress == maxProgress)
		{
		fields = new DBObjectLIST<DBObjTableField> ("Field List");
		fields->Add (new DBObjTableField (*pointIDFLD));
		fields->Add (new DBObjTableField (*layerIDFLD));
		table->ListSort (fields);
		delete fields;
		return (DBSuccess);
		}
	return (DBFault);
	}

class Histogram {
		public:
			DBInt cellNum;
			DBFloat area;
			void Initialize () { cellNum = 0; area = 0.0; }
		};

static Histogram *_RGlibHistogram;

static DBInt _RGlibSubbasinCategories (void *io,DBObjRecord *cellRec)

	{
	DBObjRecord *grdRec;
	DBNetworkIO *netIO = (DBNetworkIO *) io;
	if (cellRec == (DBObjRecord *) NULL) return (false);
	grdRec = _RGlibPointGrdIO->GridItem (_RGlibPointGrdLayerRec,netIO->Center (cellRec));
	if (grdRec == (DBObjRecord *) NULL) return (true);
	_RGlibHistogram [grdRec->RowID ()].cellNum++;
	_RGlibHistogram [grdRec->RowID ()].area += netIO->CellArea (cellRec);
	return (true);
	}

DBInt RGlibPointSubbasinHist (DBObjData *pntData, DBObjData *netData, DBObjData *grdData, DBObjData *tblData)

	{
	DBInt layerID, layerNum = 0, progress = 0, maxProgress;
	DBObjTable *itemTable = grdData->Table (DBrNItems);
	DBObjTable *table = tblData->Table (DBrNItems);
	DBObjTableField *pointIDFLD;
	DBObjTableField *layerIDFLD;
	DBObjTableField *layerNameFLD;
	DBObjTableField *categoryIDFLD;
	DBObjTableField *categoryFLD;
	DBObjTableField *percentFLD;
	DBObjTableField *areaFLD;
	DBObjTableField *cellNumFLD;
	DBVPointIO *pointIO;
	DBNetworkIO *netIO;
	DBObjRecord *pntRec, *itemRec, *tblRec;
	DBObjectLIST<DBObjTableField> *fields;

	_RGlibPointGrdIO = new DBGridIO (grdData);
	for (layerID = 0;layerID < _RGlibPointGrdIO->LayerNum ();++layerID)
		{
		_RGlibPointGrdLayerRec = _RGlibPointGrdIO->Layer (layerID);
		if ((_RGlibPointGrdLayerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{
		fprintf (stderr,"No Layer to Process in RGlibPointSubbasinHist ()\n");
		delete _RGlibPointGrdIO;
		return (DBFault);
		}
	pointIO = new DBVPointIO (pntData);
	netIO = new DBNetworkIO (netData);

	table->AddField (pointIDFLD	= new DBObjTableField ("GHAASPointID",	DBTableFieldInt,		"%8d",sizeof (DBInt)));
	table->AddField (layerIDFLD	= new DBObjTableField ("LayerID",		DBTableFieldInt,		"%4d",sizeof (DBShort)));
	table->AddField (layerNameFLD	= new DBObjTableField ("LayerName",		DBTableFieldString,	"%s",DBStringLength));
	table->AddField (categoryIDFLD= new DBObjTableField (DBrNCategoryID,	DBTableFieldInt,		"%2d",sizeof (DBShort)));
	table->AddField (categoryFLD	= new DBObjTableField (DBrNCategory,	DBTableFieldString,	_RGlibPointGrdIO->ValueFormat (),DBStringLength));
	table->AddField (cellNumFLD	= new DBObjTableField ("CellNum",		DBTableFieldInt,		"%8d",sizeof (DBInt)));
	table->AddField (areaFLD 		= new DBObjTableField (DBrNArea,			DBTableFieldFloat,	"%10.1f",sizeof (DBFloat4)));
	table->AddField (percentFLD	= new DBObjTableField (DBrNPercent,		DBTableFieldFloat,	"%6.2f",sizeof (DBFloat4)));

	_RGlibHistogram = (Histogram *) malloc (itemTable->ItemNum () * sizeof (Histogram));
	if (_RGlibHistogram == (Histogram *) NULL)
		{ perror ("Memory Allocation Error in: RGlibPointSubbasinHist ()"); return (DBFault); }
	maxProgress = pointIO->ItemNum () * _RGlibPointGrdIO->LayerNum ();
	for (layerID = 0;layerID < _RGlibPointGrdIO->LayerNum ();++layerID)
		{
		_RGlibPointGrdLayerRec = _RGlibPointGrdIO->Layer (layerID);
		if ((_RGlibPointGrdLayerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		for (pntRec = pointIO->FirstItem ();pntRec != (DBObjRecord *) NULL;pntRec = pointIO->NextItem ())
			{
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			if ((pntRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			for (itemRec = itemTable->First ();itemRec != (DBObjRecord *) NULL;itemRec = itemTable->Next ())
				_RGlibHistogram [itemRec->RowID ()].Initialize ();
			netIO->UpStreamSearch (netIO->Cell (pointIO->Coordinate (pntRec)),(DBNetworkACTION) _RGlibSubbasinCategories);
			for (itemRec = itemTable->First ();itemRec != (DBObjRecord *) NULL;itemRec = itemTable->Next ())
				if (_RGlibHistogram [itemRec->RowID ()].cellNum > 0)
					{
					tblRec = table->Add (pntRec->Name ());
					pointIDFLD->Int (tblRec,pntRec->RowID () + 1);
					layerIDFLD->Int (tblRec,_RGlibPointGrdLayerRec->RowID ());
					layerNameFLD->String (tblRec,_RGlibPointGrdLayerRec->Name ());
					categoryIDFLD->Int (tblRec,itemRec->RowID () + 1);
					categoryFLD->String (tblRec,itemRec->Name ());
					areaFLD->Float (tblRec,_RGlibHistogram [itemRec->RowID ()].area);
					percentFLD->Float (tblRec,_RGlibHistogram [itemRec->RowID ()].area / netIO->CellBasinArea (netIO->Cell (pointIO->Coordinate (pntRec))) * 100.0);
					cellNumFLD->Int (tblRec,_RGlibHistogram [itemRec->RowID ()].cellNum);
					}
			}
		}
Stop:
	delete _RGlibPointGrdIO;
	delete netIO;
	delete pointIO;
	free (_RGlibHistogram);

	if (progress == maxProgress)
		{
		fields = new DBObjectLIST<DBObjTableField> ("Field List");
		fields->Add (new DBObjTableField (*pointIDFLD));
		fields->Add (new DBObjTableField (*layerIDFLD));
		fields->Add (areaFLD = new DBObjTableField (*areaFLD));
		areaFLD->Flags (DBObjectFlagSortReversed,DBSet);
		table->ListSort (fields);
		delete fields;
		return (DBSuccess);
		}
	return (DBFault);
	}
