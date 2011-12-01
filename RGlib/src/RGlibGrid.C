/******************************************************************************

GHAAS RiverGIS Libarary V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGlibGrid.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

DBInt RGlibGridRemovePits (DBObjData *netData,DBObjData *grdData)

	{
	DBInt layerID, cellID, maxProgress;
	DBFloat elev, elev0, elev1, minDrop;
	DBCoordinate coord0, coord1;
	DBGridIF *gridIF = new DBGridIF (grdData);
	DBNetworkIF *netIF = new DBNetworkIF (netData);
	DBObjRecord *layerRec, *cellRec, *fromCell;

	maxProgress = netIF->CellNum () * gridIF->LayerNum ();
	for (layerID = 0;layerID < gridIF->LayerNum (); ++layerID)
		{
		layerRec = gridIF->Layer (layerID);
		for (cellID = netIF->CellNum () - 1;cellID >= 0;--cellID)
			{
			if (DBPause (((layerID + 1) * netIF->CellNum () - cellID) * 100 / maxProgress)) goto Stop;
			cellRec = netIF->Cell (cellID);
			if (netIF->CellLength (cellRec) > 0.0)
				{
				coord0 = netIF->Center (cellRec);
				coord1 = coord0 + netIF->Delta (cellRec);
				if (gridIF->Value (layerRec,coord0, &elev0) == false) continue;
				if (gridIF->Value (layerRec,coord1, &elev1) == false) continue;

				minDrop = 0.0;
				if (((fromCell = netIF->FromCell (cellRec)) != (DBObjRecord *) NULL) &&
				    (gridIF->Value (layerRec,netIF->Center (fromCell), &elev) == true))
					minDrop = 0.02 * (elev - elev1);
				if (minDrop < RGlibMinSLOPE * netIF->CellLength (cellRec)) minDrop = RGlibMinSLOPE * netIF->CellLength (cellRec);
				elev0 = elev0 - minDrop;
				if (elev0 < elev1) gridIF->Value (layerRec,coord1,elev0);
				}
			}
		gridIF->RecalcStats (layerRec);
		}
Stop:
	delete gridIF;
	delete netIF;
	return (cellID > 0 ? DBFault : DBSuccess);
	}

DBInt RGlibGridResampling (DBObjData *inGData, DBObjRecord *noDataRec, DBObjData *netData, DBObjData *outGData)

	{
	DBInt layerID, layerNum = 0;
	DBInt progress = 0, maxProgress;
	DBFloat value;
	DBPosition pos;
	DBCoordinate coord;
	DBObjRecord *outLayerRec, *inLayerRec;
	DBGridIF *inGridIF;
	DBGridIF *outGridIF;
	DBNetworkIF *netIF;

	inGridIF = new DBGridIF (inGData);
	for (layerID = 0;layerID < inGridIF->LayerNum ();++layerID)
		{
		inLayerRec = inGridIF->Layer (layerID);
		if ((inLayerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{
		CMmsgPrint (CMmsgAppError, "No Layer to Process in: %s %d",__FILE__,__LINE__);
		delete inGridIF;
		return (DBFault);
		}

	outGridIF = new DBGridIF (outGData);
	netIF = netData != (DBObjData *) NULL ? new DBNetworkIF (netData) : (DBNetworkIF *) NULL;

	maxProgress = layerNum * outGridIF->RowNum ();
	outLayerRec = outGridIF->Layer (0);
	for (layerID = 0;layerID < inGridIF->LayerNum ();++layerID)
		{
		inLayerRec = inGridIF->Layer (layerID);
		if ((inLayerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		outGridIF->RenameLayer (outLayerRec,inLayerRec->Name ());
		switch (inGData->Type ())
			{
			case DBTypeGridContinuous:
				for (pos.Row = 0;pos.Row < outGridIF->RowNum ();pos.Row++)
					{
					if (DBPause (progress * 100 / maxProgress)) goto Stop;
					progress++;
					for (pos.Col = 0;pos.Col < outGridIF->ColNum ();pos.Col++)
						if ((netIF != (DBNetworkIF *) NULL) && (netIF->Cell (pos) == (DBObjRecord *) NULL))
							outGridIF->Value (outLayerRec,pos,DBDefaultMissingFloatVal);
						else
							{
							outGridIF->Pos2Coord (pos,coord);
							if (inGridIF->Value (inLayerRec,coord,&value))
								outGridIF->Value (outLayerRec,pos,value);
							else outGridIF->Value (outLayerRec,pos,DBDefaultMissingFloatVal);
							}
					}
				outGridIF->RecalcStats (outLayerRec);
				break;
			case DBTypeGridDiscrete:
				{
				DBObjRecord *inGrdRec;
				for (pos.Row = 0;pos.Row < outGridIF->RowNum ();pos.Row++)
					{
					if (DBPause (progress * 100 / maxProgress)) goto Stop;
					progress++;
					for (pos.Col = 0;pos.Col < outGridIF->ColNum ();pos.Col++)
						if ((netIF != (DBNetworkIF *) NULL) && (netIF->Cell (pos) == (DBObjRecord *) NULL))
							outGridIF->Value (outLayerRec,pos,noDataRec != (DBObjRecord *) NULL ? noDataRec->RowID () : DBFault);
						else
							{
							outGridIF->Pos2Coord (pos,coord);
							inGrdRec = inGridIF->GridItem (inLayerRec,coord);
							outGridIF->Value (outLayerRec,pos,inGrdRec != (DBObjRecord *) NULL ? inGrdRec->RowID () :
													(noDataRec != (DBObjRecord *) NULL ? noDataRec->RowID () : DBFault));
							}
					}
				}
				break;
			default:
				CMmsgPrint (CMmsgAppError, "Invalid Data Type in: %s %d",__FILE__,__LINE__);	goto Stop;
			}
		if (outGridIF->LayerNum () < layerNum) outLayerRec = outGridIF->AddLayer ((char *) "Next Layer");
		}
Stop:
	delete inGridIF;
	if (netIF != (DBNetworkIF *) NULL) delete netIF;
	if (progress == maxProgress)
		{
		if (outGData->Type () == DBTypeGridDiscrete)
			{
			DBInt dataID, symID;
			DBObjRecord *inDataRec, *outDataRec;
			DBObjRecord *inSymRec, *outSymRec;
			DBObjTable *inTable   = inGData->Table  (DBrNItems);
			DBObjTable *inSymTBL  = inGData->Table  (DBrNSymbols);
			DBObjTable *outTable	 = outGData->Table (DBrNItems);
			DBObjTable *outSymTBL = outGData->Table  (DBrNSymbols);
			DBObjectLIST<DBObjTableField> *inFields  = inTable->Fields ();
			DBObjTableField *inFLD, *outFLD;

			for (dataID = 0;dataID < inTable->ItemNum ();++dataID)
				{
				inDataRec = inTable->Item (dataID);
				outDataRec = outTable->Add ();
				outDataRec->Name (inDataRec->Name ());
				}
			outSymTBL->Delete (0);
			for (symID = 0;symID < inSymTBL->ItemNum ();++symID)
				{
				inSymRec = inSymTBL->Item (symID);
				outSymRec = new DBObjRecord (*inSymRec);
				outSymTBL->Add (outSymRec);
				}
			for (inFLD = inFields->First ();inFLD != (DBObjTableField *) NULL;inFLD = inFields->Next ())
				{
				if ((outFLD = outTable->Field (inFLD->Name ())) == (DBObjTableField *) NULL)
					{ outFLD = new DBObjTableField (*inFLD); outTable->AddField (outFLD); }
				for (dataID = 0;dataID < inTable->ItemNum ();++dataID)
					{
					inDataRec  = inTable->Item (dataID);
					outDataRec = outTable->Item (dataID);
					switch (inFLD->Type ())
						{
						case DBTableFieldString:	outFLD->String (outDataRec,inFLD->String (inDataRec));	break;
						case DBTableFieldInt:		outFLD->Int (outDataRec,inFLD->Int (inDataRec));			break;
						case DBTableFieldFloat:		outFLD->Float (outDataRec,inFLD->Float (inDataRec));		break;
						case DBTableFieldDate:		outFLD->Date (outDataRec,inFLD->Date (inDataRec));			break;
						case DBTableFieldTableRec:
							inSymRec  = inFLD->Record (inDataRec);
							outSymRec = outSymTBL->Item (inSymRec->RowID ());
							outFLD->Record (outDataRec,outSymRec);
							break;
						}
					}
				}
			outGridIF->DiscreteStats ();
			}
		else
			{
			outGData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
			outGData->Flags (inGData->Flags () & DBDataFlagDispModeContShadeSets,DBSet);
			}
		delete outGridIF;
		return (DBSuccess);
		}
	delete outGridIF;
	return (DBFault);
	}

DBInt RGlibGridUniformRunoff (DBObjData *gridData,DBObjData *tsData, char *grdRelate, char *tsJoin, char *tsTime, char *tsVar,DBObjData *runoffData)

	{
	DBInt first = true, tsRowNum = 0, ret;
	DBPosition pos;
	char date [DBStringLength], curDate [DBStringLength];
	DBObjTable *grdTBL = gridData->Table (DBrNItems);
	DBObjTable *tsTBL = tsData->Table (DBrNItems);
	DBObjectLIST<DBObjTableField> *fields;
	DBObjTableField *grdRelateFLD;
	DBObjTableField *grdNextStnFLD;
	DBObjTableField *grdTmpInterFluFLD;
	DBObjTableField *grdTmpValueFLD;
	DBObjTableField *grdAreaFLD;
	DBObjTableField *tsJoinFLD;
	DBObjTableField *tsTimeFLD;
	DBObjTableField *tsVarFLD;
	DBObjRecord *tsRec, *layerRec, *grdRec, *nextGrdRec;
	DBGridIF *gridIF, *runoffIF;

	if (grdTBL->Field (RGlibNextStation) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgUsrError, "Missing Next Station Field!"); return (DBFault); }
	if (grdTBL->Field (RGlibArea) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgUsrError, "Missing Subbasin Area Field!"); return (DBFault); }
	if (grdTBL->Field (grdRelate) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgUsrError, "Invalid Relate Field!"); return (DBFault); }
	if ((tsJoinFLD = tsTBL->Field (tsJoin)) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgUsrError, "Invalid Join Field!"); return (DBFault); }
	if ((tsTimeFLD = tsTBL->Field (tsTime)) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgUsrError, "Invalid Time Field!"); return (DBFault); }
	if ((tsVarFLD = tsTBL->Field (tsVar)) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgUsrError, "Invalid Variable Field!"); return (DBFault); }

	fields = new DBObjectLIST<DBObjTableField> ("Field List");
	fields->Add (new DBObjTableField (*tsTimeFLD));
	fields->Add (new DBObjTableField (*tsJoinFLD));
	tsTBL->ListSort (fields);
	delete fields;

	gridIF = new DBGridIF (gridData);
	runoffIF = new DBGridIF (runoffData);
	grdTBL = new DBObjTable (*grdTBL);
	grdNextStnFLD = grdTBL->Field (RGlibNextStation);
	grdAreaFLD = grdTBL->Field (RGlibArea);
	grdRelateFLD = grdTBL->Field (grdRelate);
	grdTmpInterFluFLD = new DBObjTableField ("TMPInterFlu",DBTableFieldFloat,"%7.1f",sizeof (DBFloat4));
	grdTBL->AddField (grdTmpInterFluFLD);
	grdTmpValueFLD = new DBObjTableField ("TMPRunoff",DBTableFieldFloat,"%7.1f",sizeof (DBFloat4));
	grdTBL->AddField (grdTmpValueFLD);
	grdTBL->ListSort (grdTBL->Field (RGlibArea));
	curDate [0] = '\0';

	for (tsRec = tsTBL->First ();tsRec != (DBObjRecord *) NULL;tsRec = tsTBL->Next ())
		{
		if (DBPause (tsRowNum++ * 100 / tsTBL->ItemNum ())) goto Stop;
		strncpy (date,tsTimeFLD->String (tsRec),sizeof (date) - 1);
		if (strcmp (date,curDate) != 0)
			{
			if (first)
				{
				first = false;
				layerRec = runoffIF->Layer ((DBInt) 0);
				runoffIF->RenameLayer (layerRec,date);
				}
			else
				{
				for (grdRec = grdTBL->Last ();grdRec != (DBObjRecord *) NULL;grdRec = grdTBL->Next (DBBackward))
					{
					if ((grdRec->Flags () & DBObjectFlagLocked) != DBObjectFlagLocked) continue;
					for (nextGrdRec = grdTBL->Item (grdNextStnFLD->Int (grdRec) - 1);
						  nextGrdRec != (DBObjRecord *) NULL;
						  nextGrdRec = grdTBL->Item (grdNextStnFLD->Int (nextGrdRec) - 1))
						if ((nextGrdRec->Flags () & DBObjectFlagLocked) == DBObjectFlagLocked)
							{
							grdTmpInterFluFLD->Float (nextGrdRec,grdTmpInterFluFLD->Float (nextGrdRec) - grdAreaFLD->Float (grdRec));
							grdTmpValueFLD->Float (nextGrdRec,grdTmpValueFLD->Float (nextGrdRec) - grdTmpValueFLD->Float (grdRec));
							break;
							}
					}
				for (grdRec = grdTBL->Last ();grdRec != (DBObjRecord *) NULL;grdRec = grdTBL->Next (DBBackward))
					if ((grdRec->Flags () & DBObjectFlagLocked) == DBObjectFlagLocked)
							grdTmpValueFLD->Float (grdRec,grdTmpValueFLD->Float (grdRec) * 31536.0/ grdTmpInterFluFLD->Float (grdRec));
					else
						{
						if ((nextGrdRec = grdTBL->Item (grdNextStnFLD->Int (grdRec) - 1)) != (DBObjRecord *) NULL)
							grdTmpValueFLD->Float (grdRec,grdTmpValueFLD->Float (nextGrdRec));
						else	grdTmpValueFLD->Float (grdRec,runoffIF->MissingValue ());
						}
				layerRec = runoffIF->AddLayer (date);
				for (pos.Row = 0;pos.Row < gridIF->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < gridIF->ColNum ();++pos.Col)
						if ((grdRec = gridIF->GridItem (pos)) != (DBObjRecord *) NULL)
							{
							grdRec = grdTBL->Item (grdRec->RowID ());
							runoffIF->Value (layerRec,pos,grdTmpValueFLD->Float (grdRec));
							}
						else	runoffIF->Value (layerRec,pos,runoffIF->MissingValue ());
				runoffIF->RecalcStats (layerRec);
				}
			for (grdRec = grdTBL->First ();grdRec != (DBObjRecord *) NULL;grdRec = grdTBL->Next ())
				{
				grdRec->Flags (DBObjectFlagLocked,DBClear);
				grdTmpInterFluFLD->Float (grdRec,grdAreaFLD->Float (grdRec));
				}
			strcpy (curDate,date);
			}
		for (grdRec = grdTBL->First ();grdRec != (DBObjRecord *) NULL;grdRec = grdTBL->Next ())
			if (DBTableFieldMatch (grdRelateFLD,grdRec,tsJoinFLD,tsRec))
				{
				if (CMmathEqualValues (tsVarFLD->Float (tsRec),tsVarFLD->FloatNoData ()) != true)
					{
					grdRec->Flags (DBObjectFlagLocked,DBSet);
					grdTmpValueFLD->Float (grdRec,tsVarFLD->Float (tsRec));
					}
				break;
				}
		}
Stop:
	if (tsRowNum == tsTBL->ItemNum ())
		{
		for (grdRec = grdTBL->Last ();grdRec != (DBObjRecord *) NULL;grdRec = grdTBL->Next (DBBackward))
			{
			if ((grdRec->Flags () & DBObjectFlagLocked) != DBObjectFlagLocked) continue;
			for (nextGrdRec = grdTBL->Item (grdNextStnFLD->Int (grdRec) - 1);
				  nextGrdRec != (DBObjRecord *) NULL;
				  nextGrdRec = grdTBL->Item (grdNextStnFLD->Int (nextGrdRec) - 1))
				if ((nextGrdRec->Flags () & DBObjectFlagLocked) == DBObjectFlagLocked)
					{
					grdTmpInterFluFLD->Float (nextGrdRec,grdTmpInterFluFLD->Float (nextGrdRec) - grdAreaFLD->Float (grdRec));
					grdTmpValueFLD->Float (nextGrdRec,grdTmpValueFLD->Float (nextGrdRec) - grdTmpValueFLD->Float (grdRec));
					break;
					}
			}

		for (grdRec = grdTBL->Last ();grdRec != (DBObjRecord *) NULL;grdRec = grdTBL->Next (DBBackward))
			if ((grdRec->Flags () & DBObjectFlagLocked) == DBObjectFlagLocked)
					grdTmpValueFLD->Float (grdRec,grdTmpValueFLD->Float (grdRec) * 31536.0/ grdTmpInterFluFLD->Float (grdRec));
			else
				{
				if ((nextGrdRec = grdTBL->Item (grdNextStnFLD->Int (grdRec) - 1)) != (DBObjRecord *) NULL)
					grdTmpValueFLD->Float (grdRec,grdTmpValueFLD->Float (nextGrdRec));
				else	grdTmpValueFLD->Float (grdRec,runoffIF->MissingValue ());
				}

		for (pos.Row = 0;pos.Row < gridIF->RowNum ();++pos.Row)
			for (pos.Col = 0;pos.Col < gridIF->ColNum ();++pos.Col)
				if ((grdRec = gridIF->GridItem (pos)) != (DBObjRecord *) NULL)
					{
					grdRec = grdTBL->Item (grdRec->RowID ());
					runoffIF->Value (layerRec,pos,grdTmpValueFLD->Float (grdRec));
					}
				else	runoffIF->Value (layerRec,pos,runoffIF->MissingValue ());
		runoffIF->RecalcStats (layerRec);
		ret = DBSuccess;
		}
	else	ret = DBFault;

	delete grdTBL;
	delete gridIF;
	delete runoffIF;
	runoffData->Flags (DBDataFlagDispModeContBlueScale,DBSet);
	return (ret);
	}

DBInt RGlibGridUniformGrid (DBObjData *gridData,DBObjData *tabData, char *relateName,char *joinName,char *valueName,char *dateName,DBObjData *outData)

	{
	DBInt first = true, tsRowNum = 0, ret;
	DBPosition pos;
	char date [DBStringLength], curDate [DBStringLength];
	DBObjTable *grdTBL = gridData->Table (DBrNItems);
	DBObjTable *datTBL = tabData->Table (DBrNItems);
	DBObjTableField *relateFLD;
	DBObjTableField *joinFLD;
	DBObjTableField *valueFLD;
	DBObjTableField *dateFLD;
	DBObjTableField *tmpValueFLD;
	DBObjRecord *datRec, *layerRec, *grdRec;
	DBGridIF *gridIF, *outIF;

	if ((relateFLD = grdTBL->Field (relateName)) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Invalid relate field [%s] in: %s %d",relateName,__FILE__,__LINE__); return (DBFault); }
	if ((joinFLD = datTBL->Field (joinName)) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Invalid join field [%s] in: %s %d",joinName,__FILE__,__LINE__); return (DBFault); }
	if ((valueFLD = datTBL->Field (valueName)) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Invalid value field [%s] in: %s %d",valueName,__FILE__,__LINE__); return (DBFault); }
	if (dateName != (char *) NULL)
		{
		if ((dateFLD = datTBL->Field (dateName)) == (DBObjTableField *) NULL)
			{ CMmsgPrint (CMmsgAppError, "Invalid date field [%s] in: %d %d",dateName,__FILE__,__LINE__); return (DBFault); }
		}
	else	dateFLD = (DBObjTableField *) NULL;

	if (dateFLD != (DBObjTableField *) NULL) datTBL->ListSort (dateFLD);

	gridIF = new DBGridIF (gridData);
	outIF = new DBGridIF (outData);
	grdTBL = new DBObjTable (*grdTBL);
	relateFLD = grdTBL->Field (relateFLD->Name ());
	tmpValueFLD = new DBObjTableField ("TMPValue",DBTableFieldFloat,"%7.1f",sizeof (DBFloat4));
	grdTBL->AddField (tmpValueFLD);
	curDate [0] = '\0';

	for (datRec = datTBL->First ();datRec != (DBObjRecord *) NULL;datRec = datTBL->Next ())
		{
		if (DBPause (tsRowNum++ * 100 / datTBL->ItemNum ())) goto Stop;
		if (dateFLD != (DBObjTableField *) NULL)
			strncpy (date,dateFLD->String (datRec),sizeof (date) - 1);
		else	strncpy (date,"XXXX",sizeof (date) - 1);

		if (strcmp (date,curDate) != 0)
			{
			if (first)
				{
				first = false;
				layerRec = outIF->Layer ((DBInt) 0);
				outIF->RenameLayer (layerRec,date);
				}
			else
				{
				for (pos.Row = 0;pos.Row < gridIF->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < gridIF->ColNum ();++pos.Col)
						if ((grdRec = gridIF->GridItem (pos)) != (DBObjRecord *) NULL)
							{
							grdRec = grdTBL->Item (grdRec->RowID ());
							outIF->Value (layerRec,pos,tmpValueFLD->Float (grdRec));
							}
						else	outIF->Value (layerRec,pos,outIF->MissingValue ());
				outIF->RecalcStats (layerRec);
				layerRec = outIF->AddLayer (date);
				}
			for (grdRec = grdTBL->First ();grdRec != (DBObjRecord *) NULL;grdRec = grdTBL->Next ())
				tmpValueFLD->Float (grdRec,tmpValueFLD->FloatNoData ());
			strcpy (curDate,date);
			}
		for (grdRec = grdTBL->First ();grdRec != (DBObjRecord *) NULL;grdRec = grdTBL->Next ())
			if (DBTableFieldMatch (relateFLD,grdRec,joinFLD,datRec))
				{
				if (CMmathEqualValues (valueFLD->Float (datRec),valueFLD->FloatNoData ()) != true)
					tmpValueFLD->Float (grdRec,valueFLD->Float (datRec));
				break;
				}
		}
Stop:
	if (tsRowNum == datTBL->ItemNum ())
		{
		for (pos.Row = 0;pos.Row < gridIF->RowNum ();++pos.Row)
			for (pos.Col = 0;pos.Col < gridIF->ColNum ();++pos.Col)
				if ((grdRec = gridIF->GridItem (pos)) != (DBObjRecord *) NULL)
					{
					grdRec = grdTBL->Item (grdRec->RowID ());
					outIF->Value (layerRec,pos,tmpValueFLD->Float (grdRec));
					}
				else	outIF->Value (layerRec,pos,outIF->MissingValue ());
		outIF->RecalcStats (layerRec);
		ret = DBSuccess;
		}
	else	ret = DBFault;

	delete grdTBL;
	delete gridIF;
	delete outIF;
	outData->Flags (DBDataFlagDispModeContGreyScale,DBSet);
	return (ret);
	}

DBInt RGlibGridReclassDiscrete (DBObjData *srcData,char *srcFieldName,DBObjData *dstData)

	{
	DBInt layerID, dstGrdID, progress = 0, maxProgress, doSearch, ret;
	DBObjTable *srcItemTable = srcData->Table (DBrNItems);
	DBObjTable *dstItemTable = dstData->Table (DBrNItems);
	DBObjTableField *srcField;
	DBObjTableField *dstGValField = dstItemTable->Field (DBrNGridValue);
	DBObjTableField *dstField;
	DBObjRecord *srcLayerRec, *dstLayerRec, *srcGrdRec, *dstGrdRec;
	DBGridIF *srcIF, *dstIF;
	DBPosition pos;

	if ((srcField = srcItemTable->Field (srcFieldName)) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgUsrError, "Invalid source field [%s]!",srcFieldName); return (DBFault); }

	dstField = new DBObjTableField (*srcField);
	dstItemTable->AddField (dstField);

	srcIF = new DBGridIF (srcData);
	dstIF = new DBGridIF (dstData);

	maxProgress = srcIF->LayerNum () * srcIF->RowNum ();
	for (layerID = 0;layerID < srcIF->LayerNum ();++layerID)
		{
		srcLayerRec = srcIF->Layer (layerID);
		dstLayerRec = dstIF->Layer (layerID);
		dstIF->RenameLayer (dstLayerRec,srcLayerRec->Name ());
		for (pos.Row = 0;pos.Row < srcIF->RowNum ();++pos.Row)
			{
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			for (pos.Col = 0;pos.Col < srcIF->ColNum ();++pos.Col)
					if ((srcGrdRec = srcIF->GridItem (pos)) != (DBObjRecord *) NULL)
						{
						doSearch = true;
						for (dstGrdID = 0;doSearch && (dstGrdID < dstItemTable->ItemNum ());++dstGrdID)
							{
							dstGrdRec = dstItemTable->Item (dstGrdID);
							switch (srcField->Type ())
								{
								case DBTableFieldString:
									if (strcmp (srcField->String (srcGrdRec),dstField->String (dstGrdRec)) == 0)
										doSearch = false;
									break;
								case DBTableFieldInt:
									if (srcField->Int (srcGrdRec) == dstField->Int (dstGrdRec))
										doSearch = false;
									break;
								default:	break;
								}
							}

						if (doSearch == true)
							{
							dstGrdRec = dstItemTable->Add ();
							switch (dstField->Type ())
								{
								case DBTableFieldString:
									dstField->String (dstGrdRec,srcField->String (srcGrdRec));
									dstGValField->Int (dstGrdRec,dstGrdRec->RowID () + 1);
									break;
								case DBTableFieldInt:
									dstField->Int (dstGrdRec,srcField->Int (srcGrdRec));
									dstGValField->Int (dstGrdRec,srcField->Int (srcGrdRec));
									break;
								default:	break;
								}
							}
						dstIF->Value (dstLayerRec,pos,dstGrdRec->RowID ());
						}
					else	dstIF->Value (dstLayerRec,pos,DBFault);
			}
		if (dstIF->LayerNum () < srcIF->LayerNum ()) dstIF->AddLayer ((char *) "Next Layer");
		}
Stop:
	delete srcIF;
	if (progress == maxProgress)
		{
		if (dstField->Type () == DBVariableInt)
			{
			dstItemTable->ListSort (dstGValField);
			for (layerID = 0;layerID < dstIF->LayerNum ();++layerID)
				{
				dstLayerRec = dstIF->Layer (layerID);
				for (pos.Row = 0;pos.Row < dstIF->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < dstIF->ColNum ();++pos.Col)
						{
						if ((dstGrdRec = dstIF->GridItem (pos)) == (DBObjRecord *) NULL) continue;
						dstIF->Value (dstLayerRec,pos,dstGrdRec->ListPos ());
						}
				}
			dstItemTable->ItemSort ();
			}
		dstIF->DiscreteStats ();
		ret = DBSuccess;
		}
	else	ret = DBFault;

	delete dstIF;
	return (ret);
	}

DBInt RGlibGridReclassContinuous (DBObjData *srcData,char *srcFieldName,DBObjData *dstData)

	{
	DBInt layerID, intVal, progress = 0, maxProgress;
	DBFloat value;
	DBObjTable *srcItemTable = srcData->Table (DBrNItems);
	DBObjTableField *srcField;
	DBObjRecord *srcLayerRec, *dstLayerRec, *srcGrdRec;
	DBGridIF *srcIF, *dstIF;
	DBPosition pos;

	if ((srcField = srcItemTable->Field (srcFieldName)) == (DBObjTableField *) NULL)
		{ CMmsgPrint (CMmsgUsrError, "Invalid Field [%s]!",srcFieldName); return (DBFault); }

	srcIF = new DBGridIF (srcData);
	dstIF = new DBGridIF (dstData);

	maxProgress = srcIF->LayerNum () * srcIF->RowNum ();
	for (layerID = 0;layerID < srcIF->LayerNum ();++layerID)
		{
		srcLayerRec = srcIF->Layer (layerID);
		dstLayerRec = dstIF->Layer (layerID);
		dstIF->RenameLayer (dstLayerRec,srcLayerRec->Name ());
		for (pos.Row = 0;pos.Row < srcIF->RowNum ();++pos.Row)
			{
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			for (pos.Col = 0;pos.Col < srcIF->ColNum ();++pos.Col)
					if ((srcGrdRec = srcIF->GridItem (pos)) != (DBObjRecord *) NULL)
						{
						switch (srcField->Type ())
							{
							case DBTableFieldInt:
								if ((intVal =  srcField->Int (srcGrdRec)) == srcField->IntNoData ())
									value = dstIF->MissingValue (dstLayerRec);
								else
									value = (DBFloat) intVal;
								break;
							case DBTableFieldFloat:
								value = srcField->Float (srcGrdRec);
								if (CMmathEqualValues (value,srcField->FloatNoData ()))
									value = dstIF->MissingValue (dstLayerRec);
								break;
							default:	break;
							}
						dstIF->Value (dstLayerRec,pos,value);
						}
					else	dstIF->Value (dstLayerRec,pos,dstIF->MissingValue (dstLayerRec));
			}
		dstIF->RecalcStats (dstLayerRec);
		if (dstIF->LayerNum () < srcIF->LayerNum ()) dstIF->AddLayer ((char *) "Next Layer");
		}

Stop:
	delete srcIF;
	delete dstIF;
	return (progress == maxProgress ? DBSuccess : DBFault);
	}

#define RGlibBinValue "BinValue"

DBInt RGlibGridMakeDiscrete (DBObjData *srcData, DBObjData *dstData, float binValues [], int binNum)

	{
	char *binFormat, binText [DBStringLength], textFormat [DBStringLength];
	DBInt layerID,  bin, progress = 0, maxProgress, ret;
	DBFloat value;
	DBPosition pos;
	DBObjRecord *srcLayerRec, *dstLayerRec, *record;
	DBObjTable *dstItemTable = dstData->Table (DBrNItems);
	DBObjTableField *dstGValField = dstItemTable->Field (DBrNGridValue);
	DBObjTableField *dstBinField;
	DBGridIF *srcGridIF = new DBGridIF (srcData);
	DBGridIF *dstGridIF = new DBGridIF (dstData);

	binFormat = DBMathFloatAutoFormat (binValues [binNum - 1]);
	dstBinField  = new DBObjTableField (RGlibBinValue,DBTableFieldFloat,binFormat,sizeof (DBFloat));
	dstItemTable->AddField (dstBinField);
	for (bin = 0;bin < binNum;++bin)
		{
		if (bin == 0)
			{
			memset (textFormat,(int) ' ',dstBinField->FormatWidth ());
			sprintf (textFormat + dstBinField->FormatWidth ()," < %s",binFormat);
			sprintf (binText,textFormat,binValues [bin]);
			}
		else
			{
			sprintf (textFormat,"%s - %s",binFormat,binFormat);
			sprintf (binText,textFormat,binValues [bin - 1], binValues [bin]);
			}
		record = dstItemTable->Add (binText);
		dstGValField->Int (record ,bin);
		dstBinField->Float (record,binValues [bin]);
		}

	maxProgress =  srcGridIF->LayerNum () * srcGridIF->RowNum ();
	for (layerID = 0;layerID < srcGridIF->LayerNum ();++layerID)
		{
		srcLayerRec = srcGridIF->Layer (layerID);
		dstLayerRec = dstGridIF->Layer (layerID);
		dstGridIF->RenameLayer (dstLayerRec,srcLayerRec->Name ());
		for (pos.Row = 0;pos.Row < srcGridIF->RowNum ();pos.Row++)
			{
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			for (pos.Col = 0;pos.Col < srcGridIF->ColNum ();pos.Col++)
				if (srcGridIF->Value (srcLayerRec,pos,&value))
					{
					for (bin = 0;bin < binNum;++bin)	if (value < binValues [bin]) break;
					dstGridIF->Value (dstLayerRec,pos,bin);
					}
				else dstGridIF->Value (dstLayerRec,pos,DBFault);
			}
		if (dstGridIF->LayerNum () < srcGridIF->LayerNum ()) dstGridIF->AddLayer ((char *) "Next Layer");
		}
Stop:
	if (progress == maxProgress)
		{
		dstGridIF->DiscreteStats ();
		ret = DBSuccess;
		}
	else	ret = DBFault;
	delete srcGridIF;
	delete dstGridIF;
	return (ret);
	}

DBInt RGlibGridZoneHistogram (DBObjData *zGrdData,DBObjData *cGrdData, DBObjData *tblData)

	{
	DBInt progress = 0, maxProgress, ret;
	DBInt zLayerID, cLayerID, zoneID, classID, *cellNumARR, zoneNameLen, nameLen;
	DBFloat totArea, *areaARR;
	DBPosition pos;
	DBCoordinate coord;
	DBObjTableField *outZoneIDFLD;
	DBObjTableField *outZoneNameFLD;
	DBObjTableField *outZLayerIDFLD;
	DBObjTableField *outZLayerNameFLD;
	DBObjTableField *outCLayerIDFLD;
	DBObjTableField *outCLayerNameFLD;
	DBObjTableField *outClassIDFLD;
	DBObjTableField *outClassNameFLD;
	DBObjTableField *outPercentFLD;
	DBObjTableField *outAreaFLD;
	DBObjTableField *outCellNumFLD;
	DBGridIF *zGrdIF = new DBGridIF (zGrdData);
	DBGridIF *cGrdIF = new DBGridIF (cGrdData);
	DBObjTable *zoneTable = zGrdData->Table (DBrNItems);
	DBObjTable *classTable = cGrdData->Table (DBrNItems);
	DBObjTable *outTable = tblData->Table (DBrNItems);
	DBObjectLIST<DBObjTableField> *fields;
	DBObjRecord *zLayerRec, *zoneRec, *cLayerRec, *classRec, *outRec;

	for (zoneID = 0;zoneID < zoneTable->ItemNum ();++zoneID)
		{
		zoneRec = zoneTable->Item (zoneID);
		nameLen = strlen (zoneRec->Name ());
		zoneNameLen = zoneNameLen > nameLen + 1 ? zoneNameLen : nameLen + 1;
		}
	if ((cellNumARR = (DBInt *) calloc (zoneTable->ItemNum () * classTable->ItemNum (),sizeof (DBInt))) == (DBInt *) NULL)
		{
		CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
		delete zGrdIF;
		delete cGrdIF;
		return (DBFault);
		}
	if ((areaARR = (DBFloat *) calloc (zoneTable->ItemNum () * classTable->ItemNum (),sizeof (DBFloat))) == (DBFloat *) NULL)
		{
		CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
		free (cellNumARR);
		delete zGrdIF;
		delete cGrdIF;
		return (DBFault);
		}

	outTable->AddField (outZLayerIDFLD	= new DBObjTableField ("ZoneLayerID",		DBTableFieldInt,		"%4d",sizeof (DBShort)));
	outTable->AddField (outZLayerNameFLD= new DBObjTableField ("ZoneLayerName",	DBTableFieldString,	"%s",	DBStringLength));
	outTable->AddField (outZoneIDFLD		= new DBObjTableField ("ZoneGridID",		DBTableFieldInt,		"%8d",sizeof (DBInt)));
	outTable->AddField (outZoneNameFLD	= new DBObjTableField ("ZoneGridName",		DBTableFieldString,	"%s",	zoneNameLen));
	outTable->AddField (outCLayerIDFLD	= new DBObjTableField ("CaregoryLayerID",	DBTableFieldInt,		"%4d",sizeof (DBShort)));
	outTable->AddField (outCLayerNameFLD= new DBObjTableField ("CategoryLayerName",DBTableFieldString,	"%s",	DBStringLength));
	outTable->AddField (outClassIDFLD	= new DBObjTableField ("CaregoryID",		DBTableFieldInt,		"%4d",sizeof (DBShort)));
	outTable->AddField (outClassNameFLD	= new DBObjTableField ("CategoryName",		DBTableFieldString,	"%s",	DBStringLength));
	outTable->AddField (outCellNumFLD	= new DBObjTableField ("CellNum",			DBTableFieldInt,		"%8d",sizeof (DBInt)));
	outTable->AddField (outAreaFLD 		= new DBObjTableField (DBrNArea,				DBTableFieldFloat,	"%10.1f",sizeof (DBFloat4)));
	outTable->AddField (outPercentFLD	= new DBObjTableField (DBrNPercent,			DBTableFieldFloat,	"%6.2f",sizeof (DBFloat4)));

	maxProgress = zGrdIF->LayerNum () * cGrdIF->LayerNum () * zGrdIF->RowNum ();
	for (zLayerID = 0;zLayerID < zGrdIF->LayerNum ();++zLayerID)
		{
		zLayerRec = zGrdIF->Layer (zLayerID);
		if ((zLayerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		for (cLayerID = 0;cLayerID < cGrdIF->LayerNum ();++cLayerID)
			{
			cLayerRec = cGrdIF->Layer (cLayerID);
			if ((cLayerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			for (zoneID = 0;zoneID < zoneTable->ItemNum ();++zoneID)
				for (classID = 0;classID < classTable->ItemNum ();++classID)
					{
					cellNumARR [zoneID * classTable->ItemNum () + classID] = 0;
					areaARR [zoneID * classTable->ItemNum () + classID] = 0.0;
					}
			for (pos.Row = 0;pos.Row < zGrdIF->RowNum ();++pos.Row)
				{
				if (DBPause (progress * 100 / maxProgress))	goto Stop;
				progress++;

				for (pos.Col = 0;pos.Col < zGrdIF->ColNum ();++pos.Col)
					{
					zGrdIF->Pos2Coord (pos,coord);
					if ((zoneRec = zGrdIF->GridItem (zLayerRec,pos)) == (DBObjRecord *) NULL) continue;
					if ((classRec = cGrdIF->GridItem (cLayerRec,coord)) == (DBObjRecord *) NULL) continue;
					zoneID = zoneRec->RowID ();
					classID = classRec->RowID ();
					cellNumARR [zoneID * classTable->ItemNum () + classID] += 1;
					areaARR [zoneID * classTable->ItemNum () + classID] += zGrdIF->CellArea (pos);
					}
				}
			for (zoneID = 0;zoneID < zoneTable->ItemNum ();++zoneID)
				{
				zoneRec = zoneTable->Item (zoneID);
				totArea = 0.0;
				for (classID = 0;classID < classTable->ItemNum ();++classID)
					totArea += areaARR [zoneID * classTable->ItemNum () + classID];
				for (classID = 0;classID < classTable->ItemNum ();++classID)
					if (cellNumARR [zoneID * classTable->ItemNum () + classID] > 0)
						{
						classRec = classTable->Item (classID);
						outRec = outTable->Add (zoneRec->Name ());
						outZLayerIDFLD->Int (outRec,zLayerRec->RowID () + 1);
						outZLayerNameFLD->String (outRec,zLayerRec->Name ());
						outZoneIDFLD->Int (outRec,zoneRec->RowID () + 1);
						outZoneNameFLD->String (outRec,zoneRec->Name ());
						outCLayerIDFLD->Int (outRec,cLayerRec->RowID () + 1);
						outCLayerNameFLD->String (outRec,cLayerRec->Name ());
						outClassIDFLD->Int (outRec,classRec->RowID () + 1);
						outClassNameFLD->String (outRec,classRec->Name ());
						outPercentFLD->Float (outRec,areaARR [zoneID * classTable->ItemNum () + classID] * 100.0 / totArea);
						outAreaFLD->Float (outRec,areaARR [zoneID * classTable->ItemNum () + classID]);
						outCellNumFLD->Int (outRec,cellNumARR [zoneID * classTable->ItemNum () + classID]);
						}
				}
			}
		}
Stop:
	if (progress == maxProgress)
		{
		fields = new DBObjectLIST<DBObjTableField> ("Field List");
		fields->Add (new DBObjTableField (*outZLayerIDFLD));
		fields->Add (new DBObjTableField (*outZoneIDFLD));
		fields->Add (new DBObjTableField (*outCLayerIDFLD));
		fields->Add (outAreaFLD = new DBObjTableField (*outAreaFLD));
		outAreaFLD->Flags (DBObjectFlagSortReversed,DBSet);
		outTable->ListSort (fields);
		outTable->ItemSort ();
		delete fields;
		ret = DBSuccess;
		}
	else	ret = DBFault;

	free (cellNumARR);
	free (areaARR);
	delete zGrdIF;
	delete cGrdIF;
	return (ret);
	}

#define RGlibZoneArea		"ZoneArea"
#define RGlibZonalMean		"ZonalMean"
#define RGlibZonalMin 		"ZonalMin"
#define RGlibZonalMax 		"ZonalMax"
#define RGlibZonalStdDev	"ZonalStdDev"

DBInt RGlibGridZoneStatistics (DBObjData *zGrdData, DBObjData *wGrdData, DBObjData *tblData)

	{
	DBInt progress = 0, maxProgress, ret;
	DBInt zLayerID, wLayerID, zoneID, zoneNameLen = 0, nameLen;
	DBFloat value;
	DBPosition pos;
	DBCoordinate coord;
	DBObjTableField *outZoneIDFLD;
	DBObjTableField *outZoneNameFLD;
	DBObjTableField *outZLayerIDFLD;
	DBObjTableField *outZLayerNameFLD;
	DBObjTableField *outWLayerIDFLD;
	DBObjTableField *outWLayerNameFLD;
	DBObjTableField *outZoneAreaFLD;
	DBObjTableField *outAverageFLD;
	DBObjTableField *outMinimumFLD;
	DBObjTableField *outMaximumFLD;
	DBObjTableField *outStdDevFLD;
	DBObjTableField *tmpSumWeightFLD;
	DBObjTableField *tmpPSumValFLD;
	DBObjTableField *tmpWSumValFLD;
	DBObjTableField *tmpSumValSqrFLD;
	DBObjTableField *tmpMinimumFLD;
	DBObjTableField *tmpMaximumFLD;
	DBGridIF *zGrdIF = new DBGridIF (zGrdData);
	DBGridIF *wGrdIF = new DBGridIF (wGrdData);
	DBObjTable *outTable = tblData->Table (DBrNItems);
	DBObjTable *zoneTable = new DBObjTable (*zGrdData->Table (DBrNItems));
	DBObjRecord *zLayerRec, *wLayerRec, *zoneRec, *outRec;
	DBObjectLIST<DBObjTableField> *fields;

	for (zoneID = 0;zoneID < zoneTable->ItemNum ();++zoneID)
		{
		zoneRec = zoneTable->Item (zoneID);
		nameLen = strlen (zoneRec->Name ());
		zoneNameLen = zoneNameLen > nameLen + 1 ? zoneNameLen : nameLen + 1;
		}
	outTable->AddField (outZLayerIDFLD   = new DBObjTableField ("ZoneLayerID",		DBTableFieldInt,	"%4d",sizeof (DBShort)));
	outTable->AddField (outZLayerNameFLD = new DBObjTableField ("ZoneLayerName",	DBTableFieldString,	"%s",DBStringLength));
	outTable->AddField (outZoneIDFLD     = new DBObjTableField ("ZoneGridID",		DBTableFieldInt,	"%8d",sizeof (DBInt)));
	outTable->AddField (outZoneNameFLD	 = new DBObjTableField ("ZoneGridName",		DBTableFieldString,	"%s",zoneNameLen));
	outTable->AddField (outWLayerIDFLD   = new DBObjTableField ("WeightLayerID",	DBTableFieldInt,	"%4d",sizeof (DBShort)));
	outTable->AddField (outWLayerNameFLD = new DBObjTableField ("WeightLayerName",	DBTableFieldString,	"%s",DBStringLength));
	outTable->AddField (outZoneAreaFLD   = new DBObjTableField (RGlibZoneArea,		DBTableFieldFloat,	"%10.1f",sizeof (DBFloat4)));
	outTable->AddField (outAverageFLD    = new DBObjTableField (RGlibZonalMean,		DBTableFieldFloat,	wGrdIF->ValueFormat (),sizeof (DBFloat4)));
	outTable->AddField (outMinimumFLD    = new DBObjTableField (RGlibZonalMin,		DBTableFieldFloat,	wGrdIF->ValueFormat (),sizeof (DBFloat4)));
	outTable->AddField (outMaximumFLD    = new DBObjTableField (RGlibZonalMax,		DBTableFieldFloat,	wGrdIF->ValueFormat (),sizeof (DBFloat4)));
	outTable->AddField (outStdDevFLD     = new DBObjTableField (RGlibZonalStdDev,	DBTableFieldFloat,	wGrdIF->ValueFormat (),sizeof (DBFloat4)));
	zoneTable->AddField (tmpSumWeightFLD = new DBObjTableField ("TMPSumWeight",		DBTableFieldFloat,	wGrdIF->ValueFormat (),sizeof (DBFloat4)));
	zoneTable->AddField (tmpPSumValFLD   = new DBObjTableField ("TMPPSumVal",		DBTableFieldFloat,	wGrdIF->ValueFormat (),sizeof (DBFloat4)));
	zoneTable->AddField (tmpWSumValFLD   = new DBObjTableField ("TMPWSumVal",		DBTableFieldFloat,	wGrdIF->ValueFormat (),sizeof (DBFloat4)));
	zoneTable->AddField (tmpSumValSqrFLD = new DBObjTableField ("TMPSumValSqr",		DBTableFieldFloat,	wGrdIF->ValueFormat (),sizeof (DBFloat4)));
	zoneTable->AddField (tmpMinimumFLD   = new DBObjTableField ("TMPMin",			DBTableFieldFloat,	wGrdIF->ValueFormat (),sizeof (DBFloat4)));
	zoneTable->AddField (tmpMaximumFLD   = new DBObjTableField ("TMPMax",			DBTableFieldFloat,	wGrdIF->ValueFormat (),sizeof (DBFloat4)));

	maxProgress = zGrdIF->LayerNum () * wGrdIF->LayerNum () * zGrdIF->RowNum ();
	for (zLayerID = 0;zLayerID < zGrdIF->LayerNum ();++zLayerID)
		{
		zLayerRec = zGrdIF->Layer (zLayerID);
		if ((zLayerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		for (wLayerID = 0;wLayerID < wGrdIF->LayerNum ();++wLayerID)
			{
			wLayerRec = wGrdIF->Layer (wLayerID);
			if ((wLayerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			for (zoneID = 0;zoneID < zoneTable->ItemNum ();++zoneID)
				{
				zoneRec = zoneTable->Item (zoneID);
				tmpSumWeightFLD->Float (zoneRec,(DBFloat) 0.0);
				tmpPSumValFLD->Float (zoneRec,(DBFloat) 0.0);
				tmpWSumValFLD->Float (zoneRec,(DBFloat) 0.0);
				tmpSumValSqrFLD->Float (zoneRec,(DBFloat) 0.0);
				tmpMinimumFLD->Float (zoneRec,(DBFloat) DBHugeVal);
				tmpMaximumFLD->Float (zoneRec,(DBFloat) -DBHugeVal);
				}
			for (pos.Row = 0;pos.Row < zGrdIF->RowNum ();++pos.Row)
				{
				if (DBPause (progress * 100 / maxProgress))	goto Stop;
				progress++;

				for (pos.Col = 0;pos.Col < zGrdIF->ColNum ();++pos.Col)
					{
					zGrdIF->Pos2Coord (pos,coord);
					if ((zoneRec = zGrdIF->GridItem (zLayerRec,pos)) == (DBObjRecord *) NULL) continue;
					if (wGrdIF->Value (wLayerRec,coord,&value) == false) continue;
					zoneRec = zoneTable->Item (zoneRec->RowID ());
					tmpSumWeightFLD->Float (zoneRec,tmpSumWeightFLD->Float (zoneRec) + zGrdIF->CellArea (pos));
					tmpPSumValFLD->Float (zoneRec,tmpPSumValFLD->Float (zoneRec) + value);
					tmpWSumValFLD->Float (zoneRec,tmpWSumValFLD->Float (zoneRec) + value * zGrdIF->CellArea (pos));
					tmpSumValSqrFLD->Float (zoneRec,tmpSumValSqrFLD->Float (zoneRec) + value * value * zGrdIF->CellArea (pos));
					if (value < tmpMinimumFLD->Float (zoneRec)) tmpMinimumFLD->Float (zoneRec,value);
					if (value > tmpMaximumFLD->Float (zoneRec)) tmpMaximumFLD->Float (zoneRec,value);
					}
				}
			for (zoneID = 0;zoneID < zoneTable->ItemNum ();++zoneID)
				{
				zoneRec = zoneTable->Item (zoneID);
				if (tmpSumWeightFLD->Float (zoneRec) > 0)
					{
					tmpWSumValFLD->Float (zoneRec,tmpWSumValFLD->Float (zoneRec) / tmpSumWeightFLD->Float (zoneRec));
					tmpSumValSqrFLD->Float (zoneRec,tmpSumValSqrFLD->Float (zoneRec)  / tmpSumWeightFLD->Float (zoneRec));
					outRec = outTable->Add (zoneRec->Name ());
					outZLayerIDFLD->Int (outRec,zLayerRec->RowID () + 1);
					outZLayerNameFLD->String (outRec,zLayerRec->Name ());
					outZoneAreaFLD->Float (outRec,tmpSumWeightFLD->Float (zoneRec));
					outZoneIDFLD->Int (outRec,zoneRec->RowID () + 1);
					outZoneNameFLD->String (outRec,zoneRec->Name ());
					outWLayerIDFLD->Int (outRec,wLayerRec->RowID () + 1);
					outWLayerNameFLD->String (outRec,wLayerRec->Name ());
					outAverageFLD->Float (outRec,tmpWSumValFLD->Float (zoneRec));
					outMinimumFLD->Float (outRec,tmpMinimumFLD->Float (zoneRec));
					outMaximumFLD->Float (outRec,tmpMaximumFLD->Float (zoneRec));
					outStdDevFLD->Float (outRec,sqrt (tmpSumValSqrFLD->Float (zoneRec) - outAverageFLD->Float (outRec) * outAverageFLD->Float (outRec)));
					}
				}
			}
		}
Stop:
	if (progress == maxProgress)
		{
		fields = new DBObjectLIST<DBObjTableField> ("Field List");
		fields->Add (new DBObjTableField (*outZLayerIDFLD));
		fields->Add (new DBObjTableField (*outZoneIDFLD));
		fields->Add (new DBObjTableField (*outWLayerIDFLD));
		outTable->ListSort (fields);
		outTable->ItemSort ();
		delete fields;
		ret = DBSuccess;
		}
	else	ret = DBFault;
	delete zGrdIF;
	delete wGrdIF;
	delete zoneTable;
	return (ret);
	}

DBInt RGlibGridNoNegatives (DBObjData *grdData,DBInt setZero)

	{
	DBInt progress = 0, maxProgress;
	DBInt layerID;
	DBFloat value;
	DBGridIF *gridIF = new DBGridIF (grdData);
	DBPosition pos;
	DBObjRecord *layerRec;

	maxProgress = gridIF->LayerNum () * gridIF->RowNum ();
	for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
		{
		layerRec = gridIF->Layer (layerID);
		for (pos.Row = 0;pos.Row < gridIF->RowNum ();++pos.Row)
			{
			if (DBPause (progress * 100 / maxProgress))	goto Stop;
			progress++;

			for (pos.Col = 0;pos.Col < gridIF->ColNum ();++pos.Col)
				if (gridIF->Value (layerRec,pos,&value))
					if (value < 0.0)	gridIF->Value (layerRec,pos,setZero ? 0.0 : gridIF->MissingValue ());
			}
		gridIF->RecalcStats (layerRec);
		}
Stop:
	delete gridIF;
	return (progress == maxProgress ? DBSuccess : DBFault);
	}

DBInt RGlibGridCellStats (DBObjData *srcData,DBObjData *statData)

	{
	DBInt layerID, obsNum, ret = DBSuccess;
	DBFloat sumX, sumX2, sumY, sumY2, sumXY, min, max, value, avg, dev, b0, b1, cor, se2, R2, S, b0Bound, b1Bound, signif;
	DBPosition pos;
	DBGridIF *srcIF  = new DBGridIF (srcData);
	DBGridIF *statIF = new DBGridIF (statData);

	if (srcIF->LayerNum () <= 2) { delete srcIF; delete statIF; return (DBFault); }
	statIF->RenameLayer (statIF->Layer ((DBInt) 0),(char *) "Average");
	statIF->AddLayer ((char *) "Minimum");
	statIF->AddLayer ((char *) "Maximum");
	statIF->AddLayer ((char *) "Range");
	statIF->AddLayer ((char *) "Relative Range");
	statIF->AddLayer ((char *) "StdDev");
	statIF->AddLayer ((char *) "ObsNum");
	if (srcIF->LayerNum () > DBMathRegressionMin)
		{
		statIF->AddLayer ((char *) "InterceptB0");
		statIF->AddLayer ((char *) "SlopeB1");
		statIF->AddLayer ((char *) "CorrelationCoefficient");
		statIF->AddLayer ((char *) "RegressionRSquared");
		statIF->AddLayer ((char *) "B0Boundary");
		statIF->AddLayer ((char *) "B1Boundary");
		statIF->AddLayer ((char *) "Significance");
		statIF->AddLayer ((char *) "SigSlopeB1");
		}

	for (pos.Row = 0;pos.Row < srcIF->RowNum ();++pos.Row)
		{
		if (DBPause (100 * pos.Row / srcIF->RowNum ())) goto Stop;
		for (pos.Col = 0;pos.Col < srcIF->ColNum ();++pos.Col)
			{
			min = DBHugeVal;
			max = -DBHugeVal;
			sumX = sumX2 = sumY = sumY2 = sumXY = 0.0;
			obsNum = 0;
			for (layerID = 0;layerID < srcIF->LayerNum ();++layerID)
				if (srcIF->Value (srcIF->Layer (layerID),pos,&value))
					{
					obsNum += 1;
					sumX   += (DBFloat) layerID;
					sumX2	 += (DBFloat) layerID * (DBFloat) layerID;
					sumY	 += value;
					sumY2  += (value * value);
					sumXY += value * (DBFloat) layerID;
					min = min < value ? min : value;
					max = max > value ? max : value;
					}
			if (obsNum > 0)
				{
				avg = sumY  / ((DBFloat) obsNum);
				dev = (sumY2 / ((DBFloat) obsNum) - avg * avg);
				layerID = 0;
				statIF->Value (statIF->Layer (layerID++),pos,avg);
				statIF->Value (statIF->Layer (layerID++),pos,min);
				statIF->Value (statIF->Layer (layerID++),pos,max);
				statIF->Value (statIF->Layer (layerID++),pos,(max - min));
				statIF->Value (statIF->Layer (layerID++),pos,(max - min) / (fabs (max) + fabs (min) >  CMmathEpsilon ? fabs (max) + fabs (min) : 1.0));
				statIF->Value (statIF->Layer (layerID++),pos,sqrt (dev));
				statIF->Value (statIF->Layer (layerID++),pos,obsNum);
				if (obsNum > DBMathRegressionMin)
					{
      			b1 = (sumXY - sumX * sumY / obsNum) / (sumX2 - sumX * sumX / obsNum);
			      b0 = sumY / obsNum - b1 * sumX / obsNum;
			      cor = (sumXY - sumX * sumY / obsNum) / sqrt (( sumX2 - sumX * sumX / obsNum)  * (sumY2 - sumY * sumY / obsNum));
					se2 = (sumY2 - 2.0 * b1 * sumXY - 2.0 * b0 * sumY + 2.0 * b1 * b0 * sumX + b1 * b1 * sumX2 + obsNum * b0 * b0);
					R2  = 1.0 - se2 / (sumY2 - sumY * sumY / obsNum);
					S   = sqrt (se2 / (obsNum - 2.0));
					b0Bound = DBMathStudentsT (obsNum - 2.0) * sqrt (sumX2 / (obsNum * (sumX2 - sumX * sumX / obsNum))) * S;
					b1Bound = DBMathStudentsT (obsNum - 2.0) * sqrt (se2 / (obsNum - 2.0) / (sumX2 - sumX * sumX / obsNum));
					if ((b1 + b1Bound) < 0.0) signif = -1.0;
					else if ((b1 - b1Bound) > 0.0) signif = 1.0;
					else signif = 0.0;
					statIF->Value (statIF->Layer (layerID++),pos,b0);
					statIF->Value (statIF->Layer (layerID++),pos,b1);
					statIF->Value (statIF->Layer (layerID++),pos,cor);
					statIF->Value (statIF->Layer (layerID++),pos,R2);
					statIF->Value (statIF->Layer (layerID++),pos,b0Bound);
					statIF->Value (statIF->Layer (layerID++),pos,b1Bound);
					statIF->Value (statIF->Layer (layerID++),pos,signif);
					statIF->Value (statIF->Layer (layerID++),pos,b1 * fabs (signif));
					}
				else
					for (;layerID < statIF->LayerNum ();++layerID)
						statIF->Value (statIF->Layer (layerID),pos,statIF->MissingValue ());
				}
			else
				for (layerID = 0;layerID < statIF->LayerNum ();++layerID)
					statIF->Value (statIF->Layer (layerID),pos,statIF->MissingValue ());
			}
		}
Stop:
	if (pos.Row < srcIF->RowNum ())	ret = DBFault;
	else	{ statIF->RecalcStats (); ret = DBSuccess; }
	delete srcIF;
	delete statIF;
	return (ret);
	}

DBInt RGlibGridBivarCellStats (DBObjData *xSrcData,DBObjData *ySrcData, DBObjData *statData)

	{
	DBInt ret = DBSuccess, obsNum, layerID;
	DBPosition pos;
	DBCoordinate coord;
	DBFloat xValue, yValue;
	DBFloat sumX, sumX2, sumY, sumY2, sumXY, b0, b1, cor, se2, R2, S, b0Bound, b1Bound, signif;
	DBObjRecord *xLayerRec, *yLayerRec;
	DBGridIF *xSrcGridIF = new DBGridIF (xSrcData);
	DBGridIF *ySrcGridIF = new DBGridIF (ySrcData);
	DBGridIF *statGridIF = new DBGridIF (statData);

	if (xSrcGridIF->LayerNum () < DBMathRegressionMin)
		{ CMmsgPrint (CMmsgUsrError, "Insufficent number [%d] of layers!",xSrcGridIF->LayerNum ()); goto Stop; }

	statGridIF->RenameLayer (statGridIF->Layer ((DBInt) 0),(char *) "InterceptB0");
	statGridIF->AddLayer ((char *) "SlopeB1");
	statGridIF->AddLayer ((char *) "CorrelationCoefficient");
	statGridIF->AddLayer ((char *) "RegressionRSquared");
	statGridIF->AddLayer ((char *) "B0Boundary");
	statGridIF->AddLayer ((char *) "B1Boundary");
	statGridIF->AddLayer ((char *) "Significance");
	statGridIF->AddLayer ((char *) "SigSlopeB1");

	for (pos.Row = 0;pos.Row < xSrcGridIF->RowNum ();++pos.Row)
		{
		if (DBPause (100 * pos.Row / xSrcGridIF->RowNum ())) goto Stop;
		for (pos.Col = 0;pos.Col < xSrcGridIF->ColNum (); ++pos.Col)
			{
			sumX = sumX2 = sumY = sumY2 = sumXY = 0.0;
			obsNum = 0;
			for (layerID = 0;layerID < xSrcGridIF->LayerNum ();++layerID)
				{
				xLayerRec = xSrcGridIF->Layer (layerID);
				if (xSrcGridIF->Value (xLayerRec, pos, &xValue) == false) continue;
				if ((yLayerRec = ySrcGridIF->Layer (xLayerRec->Name ())) == (DBObjRecord *) NULL) continue;
				xSrcGridIF->Pos2Coord (pos,coord);
				if (ySrcGridIF->Value (yLayerRec,coord,&yValue) == false) continue;

				obsNum += 1;
				sumX   += xValue;
				sumX2	 += xValue * xValue;
				sumY	 += yValue;
				sumY2  += (yValue * yValue);
				sumXY += xValue * yValue;
				}
			layerID = 0;
			if (obsNum > DBMathRegressionMin)
				{
      		b1 = (sumXY - sumX * sumY / obsNum) / (sumX2 - sumX * sumX / obsNum);
			   b0 = sumY / obsNum - b1 * sumX / obsNum;
				cor = (sumXY - sumX * sumY / obsNum) / sqrt (( sumX2 - sumX * sumX / obsNum)  * (sumY2 - sumY * sumY / obsNum));
				se2 = (sumY2 - 2.0 * b1 * sumXY - 2.0 * b0 * sumY + 2.0 * b1 * b0 * sumX + b1 * b1 * sumX2 + obsNum * b0 * b0);
				R2  = 1.0 - se2 / (sumY2 - sumY * sumY / obsNum);
				S   = sqrt (se2 / (obsNum - 2.0));
				b0Bound = DBMathStudentsT (obsNum - 2.0) * sqrt (sumX2 / (obsNum * (sumX2 - sumX * sumX / obsNum))) * S;
				b1Bound = DBMathStudentsT (obsNum - 2.0) * sqrt (se2 / (obsNum - 2.0) / (sumX2 - sumX * sumX / obsNum));
				if ((b1 + b1Bound) < 0.0) signif = -1.0;
				else if ((b1 - b1Bound) > 0.0) signif = 1.0;
				else signif = 0.0;
				statGridIF->Value (statGridIF->Layer (layerID++),pos,b0);
				statGridIF->Value (statGridIF->Layer (layerID++),pos,b1);
				statGridIF->Value (statGridIF->Layer (layerID++),pos,cor);
				statGridIF->Value (statGridIF->Layer (layerID++),pos,R2);
				statGridIF->Value (statGridIF->Layer (layerID++),pos,b0Bound);
				statGridIF->Value (statGridIF->Layer (layerID++),pos,b1Bound);
				statGridIF->Value (statGridIF->Layer (layerID++),pos,signif);
				statGridIF->Value (statGridIF->Layer (layerID++),pos,b1 * fabs (signif));
				}
			else
				for (;layerID < statGridIF->LayerNum ();++layerID)
					statGridIF->Value (statGridIF->Layer (layerID),pos,statGridIF->MissingValue ());
			}
		}
Stop:
	if (pos.Row < xSrcGridIF->RowNum ())	ret = DBFault;
	else	{ statGridIF->RecalcStats (); ret = DBSuccess; }
	delete xSrcGridIF;
	delete ySrcGridIF;
	delete statGridIF;
	return (ret);
	}

DBInt RGlibCycleMean (DBObjData *tsData,DBObjData *data,DBInt cycleStepNum,DBInt offset)

	{
	char recordName [DBStringLength];
	DBInt tsLayerID, *obsNum, step, ret = DBFault;
	DBFloat value, *sum;
	DBPosition pos;
	DBGridIF *tsGridIF = new DBGridIF (tsData);
	DBGridIF *gridIF   = new DBGridIF (data);

	if ((tsGridIF->RowNum () != gridIF->RowNum ()) || (tsGridIF->ColNum () != gridIF->ColNum ()))
		{ delete tsGridIF; delete gridIF; return (DBFault); }

	sprintf (recordName,"Layer: %2d",0);
	gridIF->RenameLayer (gridIF->Layer ((DBInt) 0),recordName);
	for (step = 1;step < cycleStepNum;++step)
		{
		sprintf (recordName,"Layer: %2d",step);
		gridIF->AddLayer (recordName);
		}

	if ((sum = (DBFloat *) calloc (cycleStepNum,sizeof (DBFloat))) == (DBFloat *) NULL)
		{
		CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
		delete tsGridIF;
		delete gridIF;
		return (DBFault);
		}
	if ((obsNum = (DBInt *) calloc (cycleStepNum,sizeof (DBInt))) == (DBInt *) NULL)
		{
		CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
		free (sum);
		delete tsGridIF;
		delete gridIF;
		return (DBFault);
		}

	for (pos.Row = 0;pos.Row < tsGridIF->RowNum ();pos.Row++)
		{
		if (DBPause (100 * pos.Row / tsGridIF->RowNum ())) goto Stop;
		for (pos.Col = 0;pos.Col < tsGridIF->ColNum ();pos.Col++)
			{
			for (step = 0;step < cycleStepNum;++step) { sum [step] = 0.0; obsNum [step] = 0; }
			for (tsLayerID = 0;tsLayerID < tsGridIF->LayerNum ();++tsLayerID)
				if (tsGridIF->Value (tsGridIF->Layer (tsLayerID),pos,&value))
					{
					step = RGlibCycleStep (cycleStepNum,tsLayerID + offset);
					sum [step] += value;
					obsNum [step] += 1;
					}
			for (step = 0;step < cycleStepNum;++step)
				{
				if (obsNum [step] > 0)
						gridIF->Value (gridIF->Layer (step),pos,(sum [step] / (DBFloat) obsNum [step]));
				else	gridIF->Value (gridIF->Layer (step),pos,gridIF->MissingValue ());
				}
			}
		}
	gridIF->RecalcStats ();

	ret = DBSuccess;
Stop:
	free (sum);
	free (obsNum);
	delete tsGridIF;
	delete gridIF;
	return (ret);
	}

DBInt RGlibTSAggregate (DBObjData *tsData,DBObjData *data,DBInt timeStep,DBInt aggrType)

	{
	DBDate sDate, eDate, stepDate, date;
	DBInt tsLayerID, *obsNum, stepNum, step, ret = DBFault;
	DBFloat value, *sum;
	DBPosition pos;
	DBObjRecord *layerRec;
	DBGridIF *tsGridIF = new DBGridIF (tsData);
	DBGridIF *gridIF   = new DBGridIF (data);

	if ((tsGridIF->RowNum () != gridIF->RowNum ()) || (tsGridIF->ColNum () != gridIF->ColNum ()))
		{ delete tsGridIF; delete gridIF; return (DBFault); }

	layerRec = tsGridIF->Layer (0);
	sDate.Set (layerRec->Name ());
	layerRec = tsGridIF->Layer (tsGridIF->LayerNum () - 1);
	eDate.Set (layerRec->Name ());

	switch (timeStep)
		{
		case DBTimeStepHour:
			stepNum = eDate.HoursAD ()		- sDate.HoursAD ()	+ 1;
			stepDate.Set (0,0,0,1);
			date.Set (sDate.Year (),sDate.Month (),sDate.Day (),sDate.Hour ());
			break;
		case DBTimeStepDay:
			stepNum = eDate.DaysAD ()		- sDate.DaysAD ()		+ 1;
			stepDate.Set (0,0,1);
			date.Set (sDate.Year (),sDate.Month (),sDate.Day ());
			break;
		case DBTimeStepMonth:
			stepNum = eDate.MonthsAD ()	- sDate.MonthsAD ()	+ 1;
			stepDate.Set (0,1);
			date.Set (sDate.Year (),sDate.Month ());
			break;
		case DBTimeStepYear:
			stepNum = eDate.YearsAD ()		- sDate.YearsAD ()	+ 1;
			stepDate.Set (1);
			date.Set (sDate.Year ());
			break;
		default:	goto End;
		}

	if (tsGridIF->LayerNum () < stepNum)	goto End;

	layerRec = gridIF->Layer (0);
	gridIF->RenameLayer (layerRec,date.Get ());
	for (step = 1;step < stepNum;++step)
		{ date = date + stepDate; gridIF->AddLayer (date.Get ()); }

	if ((sum = (DBFloat *) calloc (stepNum,sizeof (DBFloat))) == (DBFloat *) NULL)
		{
		CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
		delete tsGridIF;
		delete gridIF;
		return (DBFault);
		}

	if ((obsNum = (DBInt *) calloc (stepNum,sizeof (DBInt))) == (DBInt *) NULL)
		{
		CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
		free (sum);
		delete tsGridIF;
		delete gridIF;
		return (DBFault);
		}

	for (pos.Row = 0;pos.Row < tsGridIF->RowNum ();pos.Row++)
		{
		if (DBPause (100 * pos.Row / tsGridIF->RowNum ())) goto Stop;
		for (pos.Col = 0;pos.Col < tsGridIF->ColNum ();pos.Col++)
			{
			for (step = 0;step < stepNum;++step)
				{
					switch (aggrType)
						{
						default:               sum [step] = 0.0;       obsNum [step] = 0; break;
						case RGlibAggrMinimum: sum [step] =  HUGE_VAL; obsNum [step] = 0; break;
						case RGlibAggrMaximum: sum [step] = -HUGE_VAL; obsNum [step] = 0; break;
						}
				}
			for (tsLayerID = 0;tsLayerID < tsGridIF->LayerNum ();++tsLayerID)
				{
				layerRec = tsGridIF->Layer (tsLayerID);
				date.Set (layerRec->Name ());
				if (tsGridIF->Value (layerRec,pos,&value))
					{
					switch (timeStep)
						{
						case DBTimeStepHour:	step = date.HoursAD ()	- sDate.HoursAD ();	 break;
						case DBTimeStepDay:		step = date.DaysAD ()	- sDate.DaysAD ();	 break;
						case DBTimeStepMonth:	step = date.MonthsAD ()	- sDate.MonthsAD (); break;
						case DBTimeStepYear:	step = date.YearsAD ()	- sDate.YearsAD ();	 break;
						}
					switch (aggrType)
						{
						default:               sum [step] += value;break;
						case RGlibAggrMinimum: sum [step] = sum [step] < value ? sum [step] : value; break;
						case RGlibAggrMaximum: sum [step] = sum [step] > value ? sum [step] : value; break;
						}
					obsNum [step] += 1;
					}
				}
			if (aggrType == RGlibAggrAverage)
				for (step = 0;step < gridIF->LayerNum ();++step)
					{
					if (obsNum [step] > 0)
							gridIF->Value (gridIF->Layer (step),pos,(sum [step] / (DBFloat) obsNum [step]));
					else	gridIF->Value (gridIF->Layer (step),pos,gridIF->MissingValue ());
					}
			else
				for (step = 0;step < gridIF->LayerNum ();++step)
					{
					if (obsNum [step] > 0)
							gridIF->Value (gridIF->Layer (step),pos,sum [step]);
					else	gridIF->Value (gridIF->Layer (step),pos,gridIF->MissingValue ());
					}
			}
		}
	gridIF->RecalcStats ();

	ret = DBSuccess;
Stop:
	free (sum);
	free (obsNum);
End:
	delete tsGridIF;
	delete gridIF;
	return (ret);
	}

DBInt RGlibSeasonAggregate (DBObjData *tsData,DBObjData *data,DBInt seasonLen, DBInt offset,DBInt doSum)

	{
	char layerName [DBStringLength];
	DBInt tsLayerID, *obsNum, step, stepNum, month, ret = DBFault;
	DBInt monthSeason [12], seasonNum, year, startYear, season;
	DBDate sDate, eDate, date;
	DBFloat value, *sum;
	DBPosition pos;
	DBObjRecord *layerRec;
	DBGridIF *tsGridIF = new DBGridIF (tsData);
	DBGridIF *gridIF   = new DBGridIF (data);

	if ((seasonLen != 2) && (seasonLen != 3) && (seasonLen != 4) && (seasonLen != 6))
		{
		CMmsgPrint (CMmsgAppError, "Invalid season length in: %s %d",__FILE__,__LINE__);
		delete tsGridIF;
		delete gridIF;
		return (DBFault);
		}

	seasonNum = 12 / seasonLen;

	if ((tsGridIF->RowNum () != gridIF->RowNum ()) || (tsGridIF->ColNum () != gridIF->ColNum ()))
		{ delete tsGridIF; delete gridIF; return (DBFault); }

	layerRec = tsGridIF->Layer (0);
	sDate.Set (layerRec->Name ());
	layerRec = tsGridIF->Layer (tsGridIF->LayerNum () - 1);
	eDate.Set (layerRec->Name ());

	stepNum = (eDate.MonthsAD () - sDate.MonthsAD () + 1) / seasonLen;

	if (tsGridIF->LayerNum () < stepNum)	goto End;

	layerRec = gridIF->Layer (0);
	startYear = year = sDate.Year ();
	if (year != DBDefaultMissingIntVal) sprintf (layerName,"%4d-Season", year);
	else sprintf (layerName,"XXXX-Season");
	gridIF->RenameLayer (layerRec,layerName);
	for (step = 1;step < stepNum;++step)
		{
		if (startYear != DBDefaultMissingIntVal)
			{ year = step / seasonNum; sprintf (layerName,"%4d-Season", year + startYear); }
		else	sprintf (layerName,"XXXX-Season");
		gridIF->AddLayer (layerName);
		}

	for (month = 0;month < 12;++month)
		monthSeason [((month + offset > 0 ? month  : month + 12) + offset) % 12] = month /seasonLen;

	for (step = 0;step < stepNum;++step)
		{
		season = step % seasonNum;
		layerRec = gridIF->Layer (step);
		strcpy (layerName,layerRec->Name ());
		for (month = offset;month < 12 + offset;month++)
			if (monthSeason [month % 12] == season) sprintf (layerName + strlen (layerName),"_%02d",(month % 12) + 1);
		gridIF->RenameLayer (layerRec,layerName);
		}

	if ((sum = (DBFloat *) calloc (stepNum,sizeof (DBFloat))) == (DBFloat *) NULL)
		{
		CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
		delete tsGridIF;
		delete gridIF;
		return (DBFault);
		}

	if ((obsNum = (DBInt *) calloc (stepNum,sizeof (DBInt))) == (DBInt *) NULL)
		{
		CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
		free (sum);
		delete tsGridIF;
		delete gridIF;
		return (DBFault);
		}

	for (pos.Row = 0;pos.Row < tsGridIF->RowNum ();pos.Row++)
		{
		if (DBPause (100 * pos.Row / tsGridIF->RowNum ())) goto Stop;
		for (pos.Col = 0;pos.Col < tsGridIF->ColNum ();pos.Col++)
			{
			for (step = 0;step < stepNum;++step) { sum [step] = 0.0; obsNum [step] = 0; }
			for (tsLayerID = 0;tsLayerID < tsGridIF->LayerNum ();++tsLayerID)
				{
				layerRec = tsGridIF->Layer (tsLayerID);
				date.Set (layerRec->Name ());
				year = date.Year ();
				season = monthSeason [date.Month ()];
				if (tsGridIF->Value (layerRec,pos,&value))
					{
					step = ((year - startYear) * seasonNum + season) % stepNum;
					sum    [step] += value;
					obsNum [step] += 1;
					}
				}
			if (doSum)
				for (step = 0;step < gridIF->LayerNum ();++step)
					{
					if (obsNum [step] > 0)
							gridIF->Value (gridIF->Layer (step),pos,sum [step]);
					else	gridIF->Value (gridIF->Layer (step),pos,gridIF->MissingValue ());
					}
			else
				for (step = 0;step < gridIF->LayerNum ();++step)
					{
					if (obsNum [step] > 0)
							gridIF->Value (gridIF->Layer (step),pos,(sum [step] / (DBFloat) obsNum [step]));
					else	gridIF->Value (gridIF->Layer (step),pos,gridIF->MissingValue ());
					}
			}
		}
	gridIF->RecalcStats ();

	ret = DBSuccess;
Stop:
	free (sum);
	free (obsNum);
End:
	delete tsGridIF;
	delete gridIF;
	return (ret);
	}

DBInt RGlibSeasonMean (DBObjData *tsData,DBObjData *data,DBInt seasonLen, DBInt offset,DBInt doSum)

	{
	char layerName [DBStringLength];
	DBInt tsLayerID, *obsNum, ret = DBFault;
	DBInt monthSeason [12], seasonNum, season, month;
	DBDate sDate, eDate, date;
	DBFloat value, *sum;
	DBPosition pos;
	DBObjRecord *layerRec;
	DBGridIF *tsGridIF = new DBGridIF (tsData);
	DBGridIF *gridIF   = new DBGridIF (data);

	if ((seasonLen != 2) && (seasonLen != 3) && (seasonLen != 4) && (seasonLen != 6))
		{
		CMmsgPrint (CMmsgAppError, "Invalid season length in: %s %d",__FILE__,__LINE__);
		delete tsGridIF;
		delete gridIF;
		return (DBFault);
		}

	seasonNum = 12 / seasonLen;

	if ((tsGridIF->RowNum () != gridIF->RowNum ()) || (tsGridIF->ColNum () != gridIF->ColNum ()))
		{ delete tsGridIF; delete gridIF; return (DBFault); }

	layerRec = tsGridIF->Layer (0);
	sDate.Set (layerRec->Name ());
	layerRec = tsGridIF->Layer (tsGridIF->LayerNum () - 1);
	eDate.Set (layerRec->Name ());

	if (tsGridIF->LayerNum () < seasonNum)	goto End;

	layerRec = gridIF->Layer (0);
	sprintf (layerName,"XXXX-Season");
	gridIF->RenameLayer (layerRec,layerName);
	for (season = 1;season < seasonNum;++season)
		{ sprintf (layerName,"XXXX-Season"); gridIF->AddLayer (layerName); }

	for (month = 0;month < 12;++month)
		{
		season = month / seasonLen;
		monthSeason [((month + offset > 0 ? month  : month + 12) + offset) % 12] = season;
		layerRec = gridIF->Layer (season);
		strcpy (layerName,layerRec->Name ());
		sprintf (layerName + strlen (layerName),"_%02d",(((month + offset > 0 ? month  : month + 12) + offset) % 12) + 1);
		gridIF->RenameLayer (layerRec,layerName);
		}

	if ((sum = (DBFloat *) calloc (seasonNum,sizeof (DBFloat))) == (DBFloat *) NULL)
		{
		CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
		delete tsGridIF;
		delete gridIF;
		return (DBFault);
		}

	if ((obsNum = (DBInt *) calloc (seasonNum,sizeof (DBInt))) == (DBInt *) NULL)
		{
		CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__);
		free (sum);
		delete tsGridIF;
		delete gridIF;
		return (DBFault);
		}

	for (pos.Row = 0;pos.Row < tsGridIF->RowNum ();pos.Row++)
		{
		if (DBPause (100 * pos.Row / tsGridIF->RowNum ())) goto Stop;
		for (pos.Col = 0;pos.Col < tsGridIF->ColNum ();pos.Col++)
			{
			for (season = 0;season < seasonNum;++season) { sum [season] = 0.0; obsNum [season] = 0; }
			for (tsLayerID = 0;tsLayerID < tsGridIF->LayerNum ();++tsLayerID)
				{
				layerRec = tsGridIF->Layer (tsLayerID);
				date.Set (layerRec->Name ());
				season = monthSeason [date.Month ()];
				if (tsGridIF->Value (layerRec,pos,&value))
					{ sum [season] += value; obsNum [season] += 1; }
				}
			if (doSum)
				for (season = 0;season < gridIF->LayerNum ();++season)
					{
					if (obsNum [season] > 0)
							gridIF->Value (gridIF->Layer (season),pos,sum [season]);
					else	gridIF->Value (gridIF->Layer (season),pos,gridIF->MissingValue ());
					}
			else
				for (season = 0;season < gridIF->LayerNum ();++season)
					{
					if (obsNum [season] > 0)
							gridIF->Value (gridIF->Layer (season),pos,(sum [season] / (DBFloat) obsNum [season]));
					else	gridIF->Value (gridIF->Layer (season),pos,gridIF->MissingValue ());
					}
			}
		}
	gridIF->RecalcStats ();

	ret = DBSuccess;
Stop:
	free (sum);
	free (obsNum);
End:
	delete tsGridIF;
	delete gridIF;
	return (ret);
	}

DBInt RGlibMinMax (DBObjData *tsData,DBObjData *data,bool doMin)

	{
	DBInt tsLayerID, layerID;
	DBFloat value, searchVal;
	DBPosition pos;
	DBObjRecord *layerRec;
	DBGridIF *tsGridIF = new DBGridIF (tsData);
	DBGridIF *gridIF   = new DBGridIF (data);

	if ((tsGridIF->RowNum () != gridIF->RowNum ()) || (tsGridIF->ColNum () != gridIF->ColNum ()))
		{ delete tsGridIF; delete gridIF; return (DBFault); }

	for (pos.Row = 0;pos.Row < tsGridIF->RowNum ();pos.Row++)
		for (pos.Col = 0;pos.Col < tsGridIF->ColNum ();pos.Col++)
			{
			searchVal = doMin ? HUGE_VAL : - HUGE_VAL;
			layerID = DBFault;
			for (tsLayerID = 0;tsLayerID < tsGridIF->LayerNum ();++tsLayerID)
				{
				layerRec = tsGridIF->Layer (tsLayerID);
				if (tsGridIF->Value (layerRec,pos,&value))
					{
					if (doMin)
						{
						if (value < searchVal)
							{
							searchVal = value;
							layerID   = tsLayerID;
							}
						}
					else
						{
						if (value > searchVal)
							{
							searchVal = value;
							layerID   = tsLayerID;
							}
						}
					}
				}
			if (layerID != DBFault)
					gridIF->Value (pos,layerID);
			else	gridIF->Value (pos,gridIF->MissingValue ());
			}
	gridIF->RenameLayer ((char *) "XXXX");
	gridIF->RecalcStats ();

	delete tsGridIF;
	delete gridIF;
	return (DBSuccess);
	}
DBInt RGlibGridSampling (DBObjData *dbData, DBObjData *grdData, DBObjData *tblData)

	{
	DBInt layerID, layerNum = 0, recID, progress = 0, maxProgress;
	DBCoordinate coord;
	DBObjTable *itemTable = dbData->Table (DBrNItems);
	DBObjTable *table = tblData->Table (DBrNItems);
	DBObjTableField *sampleIDFLD;
	DBObjTableField *layerIDFLD;
	DBObjTableField *layerNameFLD;
	DBObjTableField *newField = (DBObjTableField *) NULL;
	DBVPointIF	*pntIF = (DBVPointIF *)	 NULL;
	DBNetworkIF	*netIF = (DBNetworkIF *) NULL;
	DBGridIF *gridIF;
	DBObjRecord *record, *layerRec, *tblRec;

	gridIF = new DBGridIF (grdData);
	for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
		{
		layerRec = gridIF->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{ CMmsgPrint (CMmsgUsrError, "No Layer to Process!"); delete gridIF; return (DBFault); }


	if (dbData->Type () == DBTypeVectorPoint)
			pntIF = new DBVPointIF (dbData);
	else	netIF = new DBNetworkIF (dbData);

	table->AddField (sampleIDFLD =	new DBObjTableField ("GHAASSampleID",	DBTableFieldInt,	"%8d",sizeof (DBInt)));
	table->AddField (layerIDFLD  =	new DBObjTableField ("LayerID",			DBTableFieldInt,	"%4d",sizeof (DBShort)));
	table->AddField (layerNameFLD =	new DBObjTableField ("LayerName",		DBTableFieldString,"%s",DBStringLength));

	switch (grdData->Type ())
		{
		case DBTypeGridContinuous:
			{
			DBFloat realValue;

			newField = new DBObjTableField (grdData->Document(DBDocSubject),DBTableFieldFloat,gridIF->ValueFormat (),sizeof (DBFloat4));
			table->AddField (newField);
			maxProgress = itemTable->ItemNum () * layerNum;
			for (recID = 0;recID < itemTable->ItemNum ();recID++)
				{
				record = itemTable->Item (recID);
				if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
				for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
					{
					layerRec = gridIF->Layer (layerID);
					if (DBPause (progress * 100 / maxProgress)) goto Stop;
					progress++;
					if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
					tblRec = table->Add (record->Name ());
					sampleIDFLD->Int (tblRec,record->RowID () + 1);
					layerIDFLD->Int (tblRec,layerRec->RowID ());
					layerNameFLD->String (tblRec,layerRec->Name ());
					if (pntIF != (DBVPointIF *) NULL) coord = pntIF->Coordinate (record);
					else	coord = netIF->Center (netIF->MouthCell (record));
					if (gridIF->Value (layerRec,coord,&realValue))
							newField->Float (tblRec,realValue);
					else	newField->Float (tblRec,newField->FloatNoData ());
					}
				}
			} break;
		case DBTypeGridDiscrete:
			{
			DBInt fieldID, firstFieldID;
			DBObjTableField *field;
			DBObjTable *grdTable = grdData->Table (DBrNItems);
			DBObjectLIST<DBObjTableField> *fields = grdTable->Fields ();
			DBObjRecord *grdRec;

			maxProgress = itemTable->ItemNum () * layerNum;

			newField = new DBObjTableField (grdData->Document(DBDocSubject),DBTableFieldString,"%s",DBStringLength);
			table->AddField (newField);
			firstFieldID = newField->RowID ();
			for (fieldID = 0;fieldID < fields->ItemNum ();fieldID++)
				if (DBTableFieldIsVisible (field = fields->Item (fieldID)))
					table->AddField (newField = new DBObjTableField (*field));

			for (recID = 0;recID < itemTable->ItemNum ();recID++)
				{
				record = itemTable->Item (recID);
				if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
				for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
					{
					layerRec = gridIF->Layer (layerID);
					if (DBPause (progress * 100 / maxProgress)) goto Stop;
					progress++;
					if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
					tblRec = table->Add (record->Name ());
					sampleIDFLD->Int (tblRec,record->RowID () + 1);
					layerIDFLD->Int (tblRec,layerRec->RowID ());
					layerNameFLD->String (tblRec,layerRec->Name ());

					if (pntIF != (DBVPointIF *) NULL) coord = pntIF->Coordinate (record);
					else	coord = netIF->Center (netIF->MouthCell (record));
					if ((grdRec = gridIF->GridItem (layerRec,coord)) != (DBObjRecord *) NULL)
						{
						newField = table->Field (firstFieldID);
						newField->String (tblRec,grdRec->Name ());
						for (fieldID = firstFieldID + 1;fieldID < table->FieldNum ();fieldID++)
							{
							newField = table->Field (fieldID);
							field = fields->Item (newField->Name ());
							switch (field->Type ())
								{
								case DBTableFieldString:	newField->String (tblRec,field->String (grdRec));	break;
								case DBTableFieldInt:		newField->Int (tblRec,   field->Int (grdRec));		break;
								case DBTableFieldFloat:		newField->Float (tblRec, field->Float (grdRec));	break;
								case DBTableFieldDate:		newField->Date (tblRec,  field->Date (grdRec));		break;
								}
							}
						}
					else
						{
						newField = table->Field (firstFieldID);
						newField->String (tblRec,"");
						for (fieldID = firstFieldID + 1;fieldID < table->FieldNum ();fieldID++)
							{
							newField = table->Field (fieldID);
							field = fields->Item (newField->Name ());
							switch (field->Type ())
								{
								case DBTableFieldString:	newField->String (tblRec,"");								break;
								case DBTableFieldInt:		newField->Int (tblRec,newField->IntNoData ());		break;
								case DBTableFieldFloat:		newField->Float (tblRec,newField->FloatNoData ());	break;
								case DBTableFieldDate:		newField->Date (tblRec,newField->DateNoData ());	break;
								}
							}
						}
					}
				}
			} break;
		}
Stop:
	delete gridIF;
	if (pntIF != (DBVPointIF  *) NULL) delete pntIF;
	if (netIF != (DBNetworkIF *) NULL) delete netIF;
	return (progress < maxProgress ? DBFault : DBSuccess);
	}

void RGlibGridSampling (DBObjData *splData,DBObjData *grdData, bool netMode)

	{
	DBInt layerID, layerNum = 0, recordID;
	char *tableName;
	DBCoordinate coord;
	DBObjTable *table;
	DBGridIF *gridIF;
	DBVPointIF	*pntIF = (DBVPointIF *)	 NULL;
	DBNetworkIF *netIF = (DBNetworkIF *) NULL;
	DBObjTableField *newField;
	DBObjRecord *layerRec, *record;

	gridIF = new DBGridIF (grdData);
	for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
		{
		layerRec = gridIF->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{ CMmsgPrint (CMmsgAppError, "No Layer to Process!"); delete gridIF; return; }

	if (splData->Type () == DBTypeVectorPoint)
			{ pntIF = new DBVPointIF  (splData); tableName = DBrNItems; }
	else	{ netIF = new DBNetworkIF (splData); tableName = DBrNCells; }
	table = splData->Table (tableName);

	switch (grdData->Type ())
		{
		case DBTypeGridContinuous:
			{
			DBFloat value;
			DBObjRecord *layerRec;

			for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
				{
				layerRec = gridIF->Layer (layerID);
				if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
				if ((newField = table->Field (layerRec->Name ())) == (DBObjTableField *) NULL)
					{
					table->AddField (newField = new DBObjTableField (layerRec->Name (),DBTableFieldFloat,"%10.3f",sizeof (DBFloat4)));
					newField->Required (false);
					}
				if (newField->Required ()) continue;
				for (recordID = 0;recordID < table->ItemNum ();recordID++)
					{
					record = table->Item (recordID);
					DBPause ((layerRec->RowID () * table->ItemNum () + recordID) * 100 / (gridIF->LayerNum () * table->ItemNum ()));
					if (pntIF != (DBVPointIF *) NULL) coord = pntIF->Coordinate (record);
               else	coord = netMode ? netIF->Center (record) : netIF->Center (record) + netIF->Delta (record);

					if (gridIF->Value (layerRec,coord,&value))
							newField->Float (record,value);
					else	newField->Float (record,newField->FloatNoData ());
					}
				}
			} break;
		case DBTypeGridDiscrete:
			{
			DBObjTable *grdTable = grdData->Table (DBrNItems);
			DBObjectLIST<DBObjTableField> *fields = grdTable->Fields ();
			DBObjTableField *field;
			DBObjRecord *grdRec;

			if ((newField = table->Field ("CategoryName")) == (DBObjTableField *) NULL)
				table->AddField (newField = new DBObjTableField ("CategoryName",DBTableFieldString,"%s",DBStringLength));
			for (recordID = 0;recordID < table->ItemNum ();recordID++)
				{
				record = table->Item (recordID);
				if (pntIF != (DBVPointIF *) NULL) coord = pntIF->Coordinate (record);
				else	coord = netMode ? netIF->Center (record) : netIF->Center (record) + netIF->Delta (record);

				if ((grdRec = gridIF->GridItem (coord)) != (DBObjRecord *) NULL)
					newField->String (record,grdRec->Name ());
				}

			for (field = fields->First ();field != (DBObjTableField *) NULL;field = fields->Next ())
				if (DBTableFieldIsVisible (field))
					{
					if ((newField = table->Field (field->Name ())) == (DBObjTableField *) NULL)
						{
						table->AddField (newField = new DBObjTableField (*field));
						newField->Required (false);
						}
					else if (newField->Required ()) continue;
					for (recordID = 0;recordID < table->ItemNum ();recordID++)
						{
						record = table->Item (recordID);
						DBPause ((field->RowID () * table->ItemNum () + recordID) * 100 / (fields->ItemNum () * table->ItemNum ()));
						if (pntIF != (DBVPointIF *) NULL) coord = pntIF->Coordinate (record);
						else	coord = netMode ? netIF->Center (record) : netIF->Center (record) + netIF->Delta (record);

						if ((grdRec = gridIF->GridItem (coord)) != (DBObjRecord *) NULL)
							switch (field->Type ())
								{
								default:
								case DBTableFieldString:	newField->String	(record,field->String	(grdRec));	break;
								case DBTableFieldInt:		newField->Int		(record,field->Int		(grdRec));	break;
								case DBTableFieldFloat:		newField->Float	(record,field->Float		(grdRec));	break;
								case DBTableFieldDate:		newField->Date		(record,field->Date		(grdRec));	break;
								}
						}
					}
			} break;
		}
	if (pntIF != (DBVPointIF *) NULL)  delete pntIF;
	if (netIF != (DBNetworkIF *) NULL) delete netIF;
	delete gridIF;
	}
