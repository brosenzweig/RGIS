/******************************************************************************

GHAAS RiverGIS Libarary V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

RGlibGrid.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>
#include <RG.H>

DBInt RGlibGridRemovePits (DBObjData *netData,DBObjData *grdData)

	{
	DBInt layerID, cellID, maxProgress;
	DBFloat elev, elev0, elev1, minDrop;
	DBCoordinate coord0, coord1;
	DBGridIO *gridIO = new DBGridIO (grdData);
	DBNetworkIO *netIO = new DBNetworkIO (netData);
	DBObjRecord *layerRec, *cellRec, *fromCell;
	
	maxProgress = netIO->CellNum () * gridIO->LayerNum ();
	for (layerID = 0;layerID < gridIO->LayerNum (); ++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		for (cellID = netIO->CellNum () - 1;cellID >= 0;--cellID)
			{
			if (DBPause (((layerID + 1) * netIO->CellNum () - cellID) * 100 / maxProgress)) goto Stop;
			cellRec = netIO->Cell (cellID);
			if (netIO->CellLength (cellRec) > 0.0)
				{
				coord0 = netIO->Center (cellRec);
				coord1 = coord0 + netIO->Delta (cellRec);
				if (gridIO->Value (layerRec,coord0, &elev0) == false) continue;
				if (gridIO->Value (layerRec,coord1, &elev1) == false) continue;

				minDrop = 0.0;
				if (((fromCell = netIO->FromCell (cellRec)) != (DBObjRecord *) NULL) &&
				    (gridIO->Value (layerRec,netIO->Center (fromCell), &elev) == true))
					minDrop = 0.02 * (elev - elev1);
				if (minDrop < RGlibMinSLOPE * netIO->CellLength (cellRec)) minDrop = RGlibMinSLOPE * netIO->CellLength (cellRec);
				elev0 = elev0 - minDrop;
				if (elev0 < elev1) gridIO->Value (layerRec,coord1,elev0);
				}
			}
		gridIO->RecalcStats (layerRec);
		}
Stop:
	delete gridIO;
	delete netIO;
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
	DBGridIO *inGridIO;
	DBGridIO *outGridIO;
	DBNetworkIO *netIO;
	
	inGridIO = new DBGridIO (inGData);	
	for (layerID = 0;layerID < inGridIO->LayerNum ();++layerID)
		{
		inLayerRec = inGridIO->Layer (layerID);
		if ((inLayerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{
		fprintf (stderr,"No Layer to Process in: RGlibPointSubbasinCente ()\n");
		delete inGridIO;
		return (DBFault);
		}

	outGridIO = new DBGridIO (outGData);
	netIO = netData != (DBObjData *) NULL ? new DBNetworkIO (netData) : (DBNetworkIO *) NULL;

	maxProgress = layerNum * outGridIO->RowNum ();
	outLayerRec = outGridIO->Layer (0);
	for (layerID = 0;layerID < inGridIO->LayerNum ();++layerID)
		{
		inLayerRec = inGridIO->Layer (layerID);
		if ((inLayerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		outGridIO->RenameLayer (outLayerRec,inLayerRec->Name ());
		switch (inGData->Type ())
			{
			case DBTypeGridContinuous:
				for (pos.Row = 0;pos.Row < outGridIO->RowNum ();pos.Row++)
					{
					if (DBPause (progress * 100 / maxProgress)) goto Stop;
					progress++;
					for (pos.Col = 0;pos.Col < outGridIO->ColNum ();pos.Col++)
						if ((netIO != (DBNetworkIO *) NULL) && (netIO->Cell (pos) == (DBObjRecord *) NULL))
							outGridIO->Value (outLayerRec,pos,DBDefaultMissingFloatVal);
						else
							{
							outGridIO->Pos2Coord (pos,coord);
							if (inGridIO->Value (inLayerRec,coord,&value))
								outGridIO->Value (outLayerRec,pos,value);
							else outGridIO->Value (outLayerRec,pos,DBDefaultMissingFloatVal);
							}
					}
				outGridIO->RecalcStats (outLayerRec);	
				break;
			case DBTypeGridDiscrete:
				{
				DBObjRecord *inGrdRec;
				for (pos.Row = 0;pos.Row < outGridIO->RowNum ();pos.Row++)
					{
					if (DBPause (progress * 100 / maxProgress)) goto Stop;
					progress++;
					for (pos.Col = 0;pos.Col < outGridIO->ColNum ();pos.Col++)
						if ((netIO != (DBNetworkIO *) NULL) && (netIO->Cell (pos) == (DBObjRecord *) NULL))
							outGridIO->Value (outLayerRec,pos,noDataRec != (DBObjRecord *) NULL ? noDataRec->RowID () : DBFault);
						else
							{
							outGridIO->Pos2Coord (pos,coord);
							inGrdRec = inGridIO->GridItem (inLayerRec,coord);
							outGridIO->Value (outLayerRec,pos,inGrdRec != (DBObjRecord *) NULL ? inGrdRec->RowID () :
													(noDataRec != (DBObjRecord *) NULL ? noDataRec->RowID () : DBFault));
							}
					}
				}
				break;
			default:
				fprintf (stderr,"Invalid Data Type in: RGlibPointSubbasinCenter ()\n");	goto Stop;
			}
		if (outGridIO->LayerNum () < layerNum) outLayerRec = outGridIO->AddLayer ("Next Layer");
		}
Stop:
	delete inGridIO;
	if (netIO != (DBNetworkIO *) NULL) delete netIO;
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
			outGridIO->DiscreteStats ();
			}
		else
			{
			outGData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
			outGData->Flags (inGData->Flags () & DBDataFlagDispModeContShadeSets,DBSet);
			}
		delete outGridIO;
		return (DBSuccess);
		}
	delete outGridIO;
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
	DBGridIO *gridIO, *runoffIO;

	if (grdTBL->Field (RGlibNextStation) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Missing Next Station Field!\n"); return (DBFault); }
	if (grdTBL->Field (RGlibArea) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Missing Subbasin Area Field!\n"); return (DBFault); }
	if (grdTBL->Field (grdRelate) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Invalid Relate Field!\n"); return (DBFault); }
	if ((tsJoinFLD = tsTBL->Field (tsJoin)) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Invalid Join Field!\n"); return (DBFault); }
	if ((tsTimeFLD = tsTBL->Field (tsTime)) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Invalid Time Field!\n"); return (DBFault); }
	if ((tsVarFLD = tsTBL->Field (tsVar)) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Invalid Variable Field!\n"); return (DBFault); }
	
	fields = new DBObjectLIST<DBObjTableField> ("Field List");
	fields->Add (new DBObjTableField (*tsTimeFLD));
	fields->Add (new DBObjTableField (*tsJoinFLD));
	tsTBL->ListSort (fields);
	delete fields;

	gridIO = new DBGridIO (gridData);
	runoffIO = new DBGridIO (runoffData);
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
				layerRec = runoffIO->Layer ((DBInt) 0);
				runoffIO->RenameLayer (layerRec,date);
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
						else	grdTmpValueFLD->Float (grdRec,runoffIO->MissingValue ());
						}
				layerRec = runoffIO->AddLayer (date);
				for (pos.Row = 0;pos.Row < gridIO->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < gridIO->ColNum ();++pos.Col)
						if ((grdRec = gridIO->GridItem (pos)) != (DBObjRecord *) NULL)
							{
							grdRec = grdTBL->Item (grdRec->RowID ());
							runoffIO->Value (layerRec,pos,grdTmpValueFLD->Float (grdRec));
							}
						else	runoffIO->Value (layerRec,pos,runoffIO->MissingValue ());
				runoffIO->RecalcStats (layerRec);
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
				if (DBMathEqualValues (tsVarFLD->Float (tsRec),tsVarFLD->FloatNoData ()) != true)
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
				else	grdTmpValueFLD->Float (grdRec,runoffIO->MissingValue ());
				}

		for (pos.Row = 0;pos.Row < gridIO->RowNum ();++pos.Row)
			for (pos.Col = 0;pos.Col < gridIO->ColNum ();++pos.Col)
				if ((grdRec = gridIO->GridItem (pos)) != (DBObjRecord *) NULL)
					{
					grdRec = grdTBL->Item (grdRec->RowID ());
					runoffIO->Value (layerRec,pos,grdTmpValueFLD->Float (grdRec));
					}
				else	runoffIO->Value (layerRec,pos,runoffIO->MissingValue ());
		runoffIO->RecalcStats (layerRec);
		ret = DBSuccess;
		}
	else	ret = DBFault;

	delete grdTBL;
	delete gridIO;
	delete runoffIO;
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
	DBGridIO *grdIO, *outIO;

	if ((relateFLD = grdTBL->Field (relateName)) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Invalid relate field [%s] in: RGlibUniformGrid ()",relateName); return (DBFault); }
	if ((joinFLD = datTBL->Field (joinName)) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Invalid join field [%s] in: RGlibUniformGrid ()",joinName); return (DBFault); }
	if ((valueFLD = datTBL->Field (valueName)) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Invalid value field [%s] in: RGlibUniformGrid ()",valueName); return (DBFault); }
	if (dateName != (char *) NULL)
		{
		if ((dateFLD = datTBL->Field (dateName)) == (DBObjTableField *) NULL)
			{ fprintf (stderr,"Invalid date field [%s] in: RGlibUniformGrid ()",dateName); return (DBFault); }
		}
	else	dateFLD = (DBObjTableField *) NULL;

	if (dateFLD != (DBObjTableField *) NULL) datTBL->ListSort (dateFLD);

	grdIO = new DBGridIO (gridData);
	outIO = new DBGridIO (outData);
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
				layerRec = outIO->Layer ((DBInt) 0);
				outIO->RenameLayer (layerRec,date);
				}
			else
				{
				for (pos.Row = 0;pos.Row < grdIO->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < grdIO->ColNum ();++pos.Col)
						if ((grdRec = grdIO->GridItem (pos)) != (DBObjRecord *) NULL)
							{
							grdRec = grdTBL->Item (grdRec->RowID ());
							outIO->Value (layerRec,pos,tmpValueFLD->Float (grdRec));
							}
						else	outIO->Value (layerRec,pos,outIO->MissingValue ());
				outIO->RecalcStats (layerRec);
				layerRec = outIO->AddLayer (date);
				}
			for (grdRec = grdTBL->First ();grdRec != (DBObjRecord *) NULL;grdRec = grdTBL->Next ())
				tmpValueFLD->Float (grdRec,tmpValueFLD->FloatNoData ());
			strcpy (curDate,date);
			}
		for (grdRec = grdTBL->First ();grdRec != (DBObjRecord *) NULL;grdRec = grdTBL->Next ())
			if (DBTableFieldMatch (relateFLD,grdRec,joinFLD,datRec))
				{
				if (DBMathEqualValues (valueFLD->Float (datRec),valueFLD->FloatNoData ()) != true)
					tmpValueFLD->Float (grdRec,valueFLD->Float (datRec));
				break;
				}
		}
Stop:
	if (tsRowNum == datTBL->ItemNum ())
		{
		for (pos.Row = 0;pos.Row < grdIO->RowNum ();++pos.Row)
			for (pos.Col = 0;pos.Col < grdIO->ColNum ();++pos.Col)
				if ((grdRec = grdIO->GridItem (pos)) != (DBObjRecord *) NULL)
					{
					grdRec = grdTBL->Item (grdRec->RowID ());
					outIO->Value (layerRec,pos,tmpValueFLD->Float (grdRec));
					}
				else	outIO->Value (layerRec,pos,outIO->MissingValue ());
		outIO->RecalcStats (layerRec);
		ret = DBSuccess;
		}
	else	ret = DBFault;

	delete grdTBL;
	delete grdIO;
	delete outIO;
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
	DBGridIO *srcIO, *dstIO;
	DBPosition pos;

	if ((srcField = srcItemTable->Field (srcFieldName)) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Invalid source field in: RGlibGridReclassDiscrete ()\n"); return (DBFault); }

	dstField = new DBObjTableField (*srcField);		
	dstItemTable->AddField (dstField);

	srcIO = new DBGridIO (srcData);
	dstIO = new DBGridIO (dstData);

	maxProgress = srcIO->LayerNum () * srcIO->RowNum ();
	for (layerID = 0;layerID < srcIO->LayerNum ();++layerID)
		{
		srcLayerRec = srcIO->Layer (layerID);
		dstLayerRec = dstIO->Layer (layerID);
		dstIO->RenameLayer (dstLayerRec,srcLayerRec->Name ());
		for (pos.Row = 0;pos.Row < srcIO->RowNum ();++pos.Row)
			{
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			for (pos.Col = 0;pos.Col < srcIO->ColNum ();++pos.Col)
					if ((srcGrdRec = srcIO->GridItem (pos)) != (DBObjRecord *) NULL)
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
						dstIO->Value (dstLayerRec,pos,dstGrdRec->RowID ());
						}
					else	dstIO->Value (dstLayerRec,pos,DBFault);
			}
		if (dstIO->LayerNum () < srcIO->LayerNum ()) dstIO->AddLayer ("Next Layer");
		}
Stop:
	delete srcIO;
	if (progress == maxProgress)
		{
		if (dstField->Type () == DBVariableInt)
			{
			dstItemTable->ListSort (dstGValField);
			for (layerID = 0;layerID < dstIO->LayerNum ();++layerID)
				{
				dstLayerRec = dstIO->Layer (layerID);
				for (pos.Row = 0;pos.Row < dstIO->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < dstIO->ColNum ();++pos.Col)
						{
						if ((dstGrdRec = dstIO->GridItem (pos)) == (DBObjRecord *) NULL) continue;
						dstIO->Value (dstLayerRec,pos,dstGrdRec->ListPos ());
						}
				}
			dstItemTable->ItemSort ();
			}
		dstIO->DiscreteStats ();
		ret = DBSuccess;
		}
	else	ret = DBFault;

	delete dstIO;
	return (ret);
	}

DBInt RGlibGridReclassContinuous (DBObjData *srcData,char *srcFieldName,DBObjData *dstData)

	{
	DBInt layerID, intVal, progress = 0, maxProgress;
	DBFloat value;
	DBObjTable *srcItemTable = srcData->Table (DBrNItems);
	DBObjTableField *srcField;
	DBObjRecord *srcLayerRec, *dstLayerRec, *srcGrdRec;
	DBGridIO *srcIO, *dstIO;
	DBPosition pos;

	if ((srcField = srcItemTable->Field (srcFieldName)) == (DBObjTableField *) NULL)
		{ fprintf (stderr,"Invalid Field in: RGlibGridReclassContinuous ()\n"); return (DBFault); }

	srcIO = new DBGridIO (srcData);
	dstIO = new DBGridIO (dstData);
	
	maxProgress = srcIO->LayerNum () * srcIO->RowNum ();
	for (layerID = 0;layerID < srcIO->LayerNum ();++layerID)
		{
		srcLayerRec = srcIO->Layer (layerID);
		dstLayerRec = dstIO->Layer (layerID);
		dstIO->RenameLayer (dstLayerRec,srcLayerRec->Name ());
		for (pos.Row = 0;pos.Row < srcIO->RowNum ();++pos.Row)
			{
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			for (pos.Col = 0;pos.Col < srcIO->ColNum ();++pos.Col)
					if ((srcGrdRec = srcIO->GridItem (pos)) != (DBObjRecord *) NULL)
						{
						switch (srcField->Type ())
							{
							case DBTableFieldInt:
								if ((intVal =  srcField->Int (srcGrdRec)) == srcField->IntNoData ())
									value = dstIO->MissingValue (dstLayerRec);
								else
									value = (DBFloat) intVal;
								break;
							case DBTableFieldFloat:
								value = srcField->Float (srcGrdRec);
								if (DBMathEqualValues (value,srcField->FloatNoData ()))
									value = dstIO->MissingValue (dstLayerRec);
								break;
							default:	break;
							}
						dstIO->Value (dstLayerRec,pos,value);
						}
					else	dstIO->Value (dstLayerRec,pos,dstIO->MissingValue (dstLayerRec));
			}
		dstIO->RecalcStats (dstLayerRec);
		if (dstIO->LayerNum () < srcIO->LayerNum ()) dstIO->AddLayer ("Next Layer");
		}

Stop:
	delete srcIO;
	delete dstIO;
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
	DBGridIO *srcGridIO = new DBGridIO (srcData);
	DBGridIO *dstGridIO = new DBGridIO (dstData);

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

	maxProgress =  srcGridIO->LayerNum () * srcGridIO->RowNum ();
	for (layerID = 0;layerID < srcGridIO->LayerNum ();++layerID)
		{
		srcLayerRec = srcGridIO->Layer (layerID);
		dstLayerRec = dstGridIO->Layer (layerID);
		dstGridIO->RenameLayer (dstLayerRec,srcLayerRec->Name ());
		for (pos.Row = 0;pos.Row < srcGridIO->RowNum ();pos.Row++)
			{
			if (DBPause (progress * 100 / maxProgress)) goto Stop;
			progress++;
			for (pos.Col = 0;pos.Col < srcGridIO->ColNum ();pos.Col++)
				if (srcGridIO->Value (srcLayerRec,pos,&value))
					{
					for (bin = 0;bin < binNum;++bin)	if (value < binValues [bin]) break;
					dstGridIO->Value (dstLayerRec,pos,bin);
					}
				else dstGridIO->Value (dstLayerRec,pos,DBFault);
			}
		if (dstGridIO->LayerNum () < srcGridIO->LayerNum ()) dstGridIO->AddLayer ("Next Layer");
		}
Stop:
	if (progress == maxProgress)
		{
		dstGridIO->DiscreteStats ();
		ret = DBSuccess;
		}
	else	ret = DBFault;
	delete srcGridIO;
	delete dstGridIO;
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
	DBGridIO *zGrdIO = new DBGridIO (zGrdData);
	DBGridIO *cGrdIO = new DBGridIO (cGrdData);
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
		perror ("Memory Allocation Error in: RGlibGridZoneHistogram ()");
		delete zGrdIO;
		delete cGrdIO;
		return (DBFault);
		}
	if ((areaARR = (DBFloat *) calloc (zoneTable->ItemNum () * classTable->ItemNum (),sizeof (DBFloat))) == (DBFloat *) NULL)
		{
		perror ("Memory Allocation Error in: RGlibZoneHistogram ()");
		free (cellNumARR);
		delete zGrdIO;
		delete cGrdIO;
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

	maxProgress = zGrdIO->LayerNum () * cGrdIO->LayerNum () * zGrdIO->RowNum ();
	for (zLayerID = 0;zLayerID < zGrdIO->LayerNum ();++zLayerID)
		{
		zLayerRec = zGrdIO->Layer (zLayerID);
		if ((zLayerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		for (cLayerID = 0;cLayerID < cGrdIO->LayerNum ();++cLayerID)
			{
			cLayerRec = cGrdIO->Layer (cLayerID);
			if ((cLayerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			for (zoneID = 0;zoneID < zoneTable->ItemNum ();++zoneID)
				for (classID = 0;classID < classTable->ItemNum ();++classID)
					{
					cellNumARR [zoneID * classTable->ItemNum () + classID] = 0;
					areaARR [zoneID * classTable->ItemNum () + classID] = 0.0;
					}
			for (pos.Row = 0;pos.Row < zGrdIO->RowNum ();++pos.Row)
				{
				if (DBPause (progress * 100 / maxProgress))	goto Stop;
				progress++;
		
				for (pos.Col = 0;pos.Col < zGrdIO->ColNum ();++pos.Col)
					{
					zGrdIO->Pos2Coord (pos,coord);
					if ((zoneRec = zGrdIO->GridItem (zLayerRec,pos)) == (DBObjRecord *) NULL) continue;
					if ((classRec = cGrdIO->GridItem (cLayerRec,coord)) == (DBObjRecord *) NULL) continue;
					zoneID = zoneRec->RowID ();
					classID = classRec->RowID ();
					cellNumARR [zoneID * classTable->ItemNum () + classID] += 1;
					areaARR [zoneID * classTable->ItemNum () + classID] += zGrdIO->CellArea (pos);
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
	delete zGrdIO;
	delete cGrdIO;
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
	DBGridIO *zGrdIO = new DBGridIO (zGrdData);
	DBGridIO *wGrdIO = new DBGridIO (wGrdData);
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
	outTable->AddField (outZLayerIDFLD	= new DBObjTableField ("ZoneLayerID",		DBTableFieldInt,		"%4d",sizeof (DBShort)));
	outTable->AddField (outZLayerNameFLD= new DBObjTableField ("ZoneLayerName",	DBTableFieldString,	"%s",DBStringLength));
	outTable->AddField (outZoneIDFLD		= new DBObjTableField ("ZoneGridID",		DBTableFieldInt,		"%8d",sizeof (DBInt)));
	outTable->AddField (outZoneNameFLD	= new DBObjTableField ("ZoneGridName",		DBTableFieldString,	"%s",zoneNameLen));
	outTable->AddField (outWLayerIDFLD	= new DBObjTableField ("WeightLayerID",	DBTableFieldInt,		"%4d",sizeof (DBShort)));
	outTable->AddField (outWLayerNameFLD= new DBObjTableField ("WeightLayerName",	DBTableFieldString,	"%s",DBStringLength));
	outTable->AddField (outZoneAreaFLD	= new DBObjTableField (RGlibZoneArea,		DBTableFieldFloat,	"%10.1f",sizeof (DBFloat4)));
	outTable->AddField (outAverageFLD	= new DBObjTableField (RGlibZonalMean,		DBTableFieldFloat,	wGrdIO->ValueFormat (),sizeof (DBFloat4)));
	outTable->AddField (outMinimumFLD	= new DBObjTableField (RGlibZonalMin,		DBTableFieldFloat,	wGrdIO->ValueFormat (),sizeof (DBFloat4)));
	outTable->AddField (outMaximumFLD	= new DBObjTableField (RGlibZonalMax,		DBTableFieldFloat,	wGrdIO->ValueFormat (),sizeof (DBFloat4)));
	outTable->AddField (outStdDevFLD 	= new DBObjTableField (RGlibZonalStdDev,	DBTableFieldFloat,	wGrdIO->ValueFormat (),sizeof (DBFloat4)));
	zoneTable->AddField (tmpSumWeightFLD= new DBObjTableField ("TMPSumWeight",		DBTableFieldFloat,	wGrdIO->ValueFormat (),sizeof (DBFloat4)));
	zoneTable->AddField (tmpPSumValFLD	= new DBObjTableField ("TMPPSumVal",		DBTableFieldFloat,	wGrdIO->ValueFormat (),sizeof (DBFloat4)));
	zoneTable->AddField (tmpWSumValFLD	= new DBObjTableField ("TMPWSumVal",		DBTableFieldFloat,	wGrdIO->ValueFormat (),sizeof (DBFloat4)));
	zoneTable->AddField (tmpSumValSqrFLD= new DBObjTableField ("TMPSumValSqr",		DBTableFieldFloat,	wGrdIO->ValueFormat (),sizeof (DBFloat4)));
	zoneTable->AddField (tmpMinimumFLD	= new DBObjTableField ("TMPMin",				DBTableFieldFloat,	wGrdIO->ValueFormat (),sizeof (DBFloat4)));
	zoneTable->AddField (tmpMaximumFLD	= new DBObjTableField ("TMPMax",				DBTableFieldFloat,	wGrdIO->ValueFormat (),sizeof (DBFloat4)));

	maxProgress = zGrdIO->LayerNum () * wGrdIO->LayerNum () * zGrdIO->RowNum ();
	for (zLayerID = 0;zLayerID < zGrdIO->LayerNum ();++zLayerID)
		{
		zLayerRec = zGrdIO->Layer (zLayerID);
		if ((zLayerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		for (wLayerID = 0;wLayerID < wGrdIO->LayerNum ();++wLayerID)
			{
			wLayerRec = wGrdIO->Layer (wLayerID);
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
			for (pos.Row = 0;pos.Row < zGrdIO->RowNum ();++pos.Row)
				{
				if (DBPause (progress * 100 / maxProgress))	goto Stop;
				progress++;

				for (pos.Col = 0;pos.Col < zGrdIO->ColNum ();++pos.Col)
					{
					zGrdIO->Pos2Coord (pos,coord);
					if ((zoneRec = zGrdIO->GridItem (zLayerRec,pos)) == (DBObjRecord *) NULL) continue;
					if (wGrdIO->Value (wLayerRec,coord,&value) == false) continue;
					zoneRec = zoneTable->Item (zoneRec->RowID ());
					tmpSumWeightFLD->Float (zoneRec,tmpSumWeightFLD->Float (zoneRec) + zGrdIO->CellArea (pos));
					tmpPSumValFLD->Float (zoneRec,tmpPSumValFLD->Float (zoneRec) + value);
					tmpWSumValFLD->Float (zoneRec,tmpWSumValFLD->Float (zoneRec) + value * zGrdIO->CellArea (pos));
					tmpSumValSqrFLD->Float (zoneRec,tmpSumValSqrFLD->Float (zoneRec) + value * value * zGrdIO->CellArea (pos));
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
	delete zGrdIO;
	delete wGrdIO;
	delete zoneTable;
	return (ret);
	}

DBInt RGlibGridNoNegatives (DBObjData *grdData,DBInt setZero)

	{
	DBInt progress = 0, maxProgress;
	DBInt layerID;
	DBFloat value;
	DBGridIO *gridIO = new DBGridIO (grdData);
	DBPosition pos;
	DBObjRecord *layerRec;

	maxProgress = gridIO->LayerNum () * gridIO->RowNum ();
	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		for (pos.Row = 0;pos.Row < gridIO->RowNum ();++pos.Row)
			{
			if (DBPause (progress * 100 / maxProgress))	goto Stop;
			progress++;

			for (pos.Col = 0;pos.Col < gridIO->ColNum ();++pos.Col)
				if (gridIO->Value (layerRec,pos,&value))
					if (value < 0.0)	gridIO->Value (layerRec,pos,setZero ? 0.0 : gridIO->MissingValue ());
			}
		gridIO->RecalcStats (layerRec);
		}
Stop:	
	delete gridIO;
	return (progress == maxProgress ? DBSuccess : DBFault);
	}

DBInt RGlibGridCellStats (DBObjData *srcData,DBObjData *statData)

	{
	DBInt layerID, obsNum, ret = DBSuccess;
	DBFloat sumX, sumX2, sumY, sumY2, sumXY, min, max, value, avg, dev, b0, b1, cor, se2, R2, S, b0Bound, b1Bound, signif;
	DBPosition pos;
	DBGridIO *srcIO  = new DBGridIO (srcData);
	DBGridIO *statIO = new DBGridIO (statData);

	if (srcIO->LayerNum () <= 2) { delete srcIO; delete statIO; return (DBFault); }
	statIO->RenameLayer (statIO->Layer ((DBInt) 0),"Average");
	statIO->AddLayer ("Minimum");
	statIO->AddLayer ("Maximum");
	statIO->AddLayer ("Range");
	statIO->AddLayer ("Relative Range");
	statIO->AddLayer ("StdDev");
	statIO->AddLayer ("ObsNum");
	if (srcIO->LayerNum () > DBMathRegressionMin)
		{
		statIO->AddLayer ("InterceptB0");
		statIO->AddLayer ("SlopeB1");
		statIO->AddLayer ("CorrelationCoefficient");
		statIO->AddLayer ("RegressionRSquared");
		statIO->AddLayer ("B0Boundary");
		statIO->AddLayer ("B1Boundary");
		statIO->AddLayer ("Significance");
		statIO->AddLayer ("SigSlopeB1");
		}

	for (pos.Row = 0;pos.Row < srcIO->RowNum ();++pos.Row)
		{
		if (DBPause (100 * pos.Row / srcIO->RowNum ())) goto Stop;
		for (pos.Col = 0;pos.Col < srcIO->ColNum ();++pos.Col)
			{
			min = DBHugeVal;
			max = -DBHugeVal;
			sumX = sumX2 = sumY = sumY2 = sumXY = 0.0;
			obsNum = 0;
			for (layerID = 0;layerID < srcIO->LayerNum ();++layerID)
				if (srcIO->Value (srcIO->Layer (layerID),pos,&value))
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
				statIO->Value (statIO->Layer (layerID++),pos,avg);
				statIO->Value (statIO->Layer (layerID++),pos,min);
				statIO->Value (statIO->Layer (layerID++),pos,max);
				statIO->Value (statIO->Layer (layerID++),pos,(max - min));
				statIO->Value (statIO->Layer (layerID++),pos,(max - min) / (fabs (max) + fabs (min) >  DBMathEpsilon ? fabs (max) + fabs (min) : 1.0));
				statIO->Value (statIO->Layer (layerID++),pos,sqrt (dev));
				statIO->Value (statIO->Layer (layerID++),pos,obsNum);
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
					statIO->Value (statIO->Layer (layerID++),pos,b0);
					statIO->Value (statIO->Layer (layerID++),pos,b1);
					statIO->Value (statIO->Layer (layerID++),pos,cor);
					statIO->Value (statIO->Layer (layerID++),pos,R2);
					statIO->Value (statIO->Layer (layerID++),pos,b0Bound);
					statIO->Value (statIO->Layer (layerID++),pos,b1Bound);
					statIO->Value (statIO->Layer (layerID++),pos,signif);
					statIO->Value (statIO->Layer (layerID++),pos,b1 * fabs (signif));
					}
				else
					for (;layerID < statIO->LayerNum ();++layerID)
						statIO->Value (statIO->Layer (layerID),pos,statIO->MissingValue ());
				}
			else
				for (layerID = 0;layerID < statIO->LayerNum ();++layerID)
					statIO->Value (statIO->Layer (layerID),pos,statIO->MissingValue ());
			}
		}
Stop:
	if (pos.Row < srcIO->RowNum ())	ret = DBFault;
	else	{ statIO->RecalcStats (); ret = DBSuccess; }
	delete srcIO;
	delete statIO;
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
	DBGridIO *xSrcGridIO = new DBGridIO (xSrcData);
	DBGridIO *ySrcGridIO = new DBGridIO (ySrcData);
	DBGridIO *statGridIO = new DBGridIO (statData);

	if (xSrcGridIO->LayerNum () < DBMathRegressionMin)
		{ fprintf (stderr,"Insufficent number of layers in: RGlibGridBivarCellStats ()\n"); goto Stop; }

	statGridIO->RenameLayer (statGridIO->Layer ((DBInt) 0),"InterceptB0");
	statGridIO->AddLayer ("SlopeB1");
	statGridIO->AddLayer ("CorrelationCoefficient");
	statGridIO->AddLayer ("RegressionRSquared");
	statGridIO->AddLayer ("B0Boundary");
	statGridIO->AddLayer ("B1Boundary");
	statGridIO->AddLayer ("Significance");
	statGridIO->AddLayer ("SigSlopeB1");

	for (pos.Row = 0;pos.Row < xSrcGridIO->RowNum ();++pos.Row)
		{
		if (DBPause (100 * pos.Row / xSrcGridIO->RowNum ())) goto Stop;
		for (pos.Col = 0;pos.Col < xSrcGridIO->ColNum (); ++pos.Col)
			{
			sumX = sumX2 = sumY = sumY2 = sumXY = 0.0;
			obsNum = 0;
			for (layerID = 0;layerID < xSrcGridIO->LayerNum ();++layerID)
				{
				xLayerRec = xSrcGridIO->Layer (layerID);
				if (xSrcGridIO->Value (xLayerRec, pos, &xValue) == false) continue;
				if ((yLayerRec = ySrcGridIO->Layer (xLayerRec->Name ())) == (DBObjRecord *) NULL) continue;
				xSrcGridIO->Pos2Coord (pos,coord);
				if (ySrcGridIO->Value (yLayerRec,coord,&yValue) == false) continue;
				
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
				statGridIO->Value (statGridIO->Layer (layerID++),pos,b0);
				statGridIO->Value (statGridIO->Layer (layerID++),pos,b1);
				statGridIO->Value (statGridIO->Layer (layerID++),pos,cor);
				statGridIO->Value (statGridIO->Layer (layerID++),pos,R2);
				statGridIO->Value (statGridIO->Layer (layerID++),pos,b0Bound);
				statGridIO->Value (statGridIO->Layer (layerID++),pos,b1Bound);
				statGridIO->Value (statGridIO->Layer (layerID++),pos,signif);
				statGridIO->Value (statGridIO->Layer (layerID++),pos,b1 * fabs (signif));
				}
			else
				for (;layerID < statGridIO->LayerNum ();++layerID)
					statGridIO->Value (statGridIO->Layer (layerID),pos,statGridIO->MissingValue ());
			}
		}
Stop:
	if (pos.Row < xSrcGridIO->RowNum ())	ret = DBFault;
	else	{ statGridIO->RecalcStats (); ret = DBSuccess; }
	delete xSrcGridIO;
	delete ySrcGridIO;
	delete statGridIO;
	return (ret);
	}

DBInt RGlibCycleMean (DBObjData *tsData,DBObjData *data,DBInt cycleStepNum,DBInt offset)

	{
	char recordName [DBStringLength];
	DBInt tsLayerID, *obsNum, step, ret = DBFault;
	DBFloat value, *sum;
	DBPosition pos;
	DBGridIO *tsGridIO = new DBGridIO (tsData);
	DBGridIO *gridIO   = new DBGridIO (data);
	
	if ((tsGridIO->RowNum () != gridIO->RowNum ()) || (tsGridIO->ColNum () != gridIO->ColNum ()))
		{ delete tsGridIO; delete gridIO; return (DBFault); }

	sprintf (recordName,"Layer: %2d",0);
	gridIO->RenameLayer (gridIO->Layer ((DBInt) 0),recordName);
	for (step = 1;step < cycleStepNum;++step)
		{
		sprintf (recordName,"Layer: %2d",step);
		gridIO->AddLayer (recordName);
		}

	if ((sum = (DBFloat *) calloc (cycleStepNum,sizeof (DBFloat))) == (DBFloat *) NULL)
		{
		perror ("Memory Allocation Error in: RGlibCycleMean ()");
		delete tsGridIO;
		delete gridIO;
		return (DBFault);
		}
	if ((obsNum = (DBInt *) calloc (cycleStepNum,sizeof (DBInt))) == (DBInt *) NULL)
		{
		perror ("Memory Allocation Error in: RGlibCycleMean ()");
		free (sum);
		delete tsGridIO;
		delete gridIO;
		return (DBFault);
		}

	for (pos.Row = 0;pos.Row < tsGridIO->RowNum ();pos.Row++)
		{
		if (DBPause (100 * pos.Row / tsGridIO->RowNum ())) goto Stop;
		for (pos.Col = 0;pos.Col < tsGridIO->ColNum ();pos.Col++)
			{
			for (step = 0;step < cycleStepNum;++step) { sum [step] = 0.0; obsNum [step] = 0; }
			for (tsLayerID = 0;tsLayerID < tsGridIO->LayerNum ();++tsLayerID)
				if (tsGridIO->Value (tsGridIO->Layer (tsLayerID),pos,&value))
					{
					step = RGlibCycleStep (cycleStepNum,tsLayerID + offset);
					sum [step] += value;
					obsNum [step] += 1;
					}
			for (step = 0;step < cycleStepNum;++step)
				{
				if (obsNum [step] > 0)
						gridIO->Value (gridIO->Layer (step),pos,(sum [step] / (DBFloat) obsNum [step]));
				else	gridIO->Value (gridIO->Layer (step),pos,gridIO->MissingValue ());
				}
			}
		}
	gridIO->RecalcStats ();

	ret = DBSuccess;
Stop:
	free (sum);
	free (obsNum);
	delete tsGridIO;
	delete gridIO;
	return (ret);
	}

DBInt RGlibLayerAggregate (DBObjData *tsData,DBObjData *data,DBInt cycleStepNum,DBInt offset,DBInt doSum)

	{
	char recordName [DBStringLength];
	DBInt tsLayerID, *obsNum, step, ret = DBFault;
	DBFloat value, *sum;
	DBPosition pos;
	DBGridIO *tsGridIO = new DBGridIO (tsData);
	DBGridIO *gridIO   = new DBGridIO (data);
	
	if ((tsGridIO->RowNum () != gridIO->RowNum ()) || (tsGridIO->ColNum () != gridIO->ColNum ()))
		{ delete tsGridIO; delete gridIO; return (DBFault); }

	sprintf (recordName,"Layer: %2d",0);
	gridIO->RenameLayer (gridIO->Layer ((DBInt) 0),recordName);
	for (step = 1;step < (int) (ceil ((float) tsGridIO->LayerNum () / (float) cycleStepNum));++step)
		{
		sprintf (recordName,"Layer: %2d",step);
		gridIO->AddLayer (recordName);
		}

	if ((sum = (DBFloat *) calloc (gridIO->LayerNum (),sizeof (DBFloat))) == (DBFloat *) NULL)
		{
		perror ("Memory Allocation Error in: RGlibCycleMean ()");
		delete tsGridIO;
		delete gridIO;
		return (DBFault);
		}

	if ((obsNum = (DBInt *) calloc (gridIO->LayerNum (),sizeof (DBInt))) == (DBInt *) NULL)
		{
		perror ("Memory Allocation Error in: RGlibCycleMean ()");
		free (sum);
		delete tsGridIO;
		delete gridIO;
		return (DBFault);
		}

	for (pos.Row = 0;pos.Row < tsGridIO->RowNum ();pos.Row++)
		{
		if (DBPause (100 * pos.Row / tsGridIO->RowNum ())) goto Stop;
		for (pos.Col = 0;pos.Col < tsGridIO->ColNum ();pos.Col++)
			{
			for (step = 0;step < gridIO->LayerNum ();++step) { sum [step] = 0.0; obsNum [step] = 0; }
			for (tsLayerID = 0;tsLayerID < tsGridIO->LayerNum ();++tsLayerID)
				if (tsGridIO->Value (tsGridIO->Layer (tsLayerID),pos,&value))
					{
					step = (DBInt) (floor ((tsLayerID + offset) / cycleStepNum));
					while (step >= gridIO->LayerNum ()) step -= gridIO->LayerNum ();
					sum [step] += value;
					obsNum [step] += 1;
					}
			if (doSum)
				for (step = 0;step < gridIO->LayerNum ();++step)
					{
					if (obsNum [step] > 0)
						gridIO->Value (gridIO->Layer (step),pos,(sum [step] / (DBFloat) obsNum [step]) * (DBFloat) gridIO->LayerNum ());
					else gridIO->Value (gridIO->Layer (step),pos,gridIO->MissingValue ());
					}
			else
				for (step = 0;step < gridIO->LayerNum ();++step)
					{
					if (obsNum [step] > 0)
						gridIO->Value (gridIO->Layer (step),pos,(sum [step] / (DBFloat) obsNum [step]));
					else gridIO->Value (gridIO->Layer (step),pos,gridIO->MissingValue ());
					}
			}
		}
	gridIO->RecalcStats ();

	ret = DBSuccess;
Stop:
	free (sum);
	free (obsNum);
	delete tsGridIO;
	delete gridIO;
	return (ret);
	}

DBInt RGlibTSAggregate (DBObjData *tsData,DBObjData *data,DBInt timeStep,DBInt doSum)

	{
	DBDate sDate, eDate, stepDate, date;
	DBInt tsLayerID, *obsNum, stepNum, step, ret = DBFault;
	DBFloat value, *sum;
	DBPosition pos;
	DBObjRecord *layerRec;
	DBGridIO *tsGridIO = new DBGridIO (tsData);
	DBGridIO *gridIO   = new DBGridIO (data);
	
	if ((tsGridIO->RowNum () != gridIO->RowNum ()) || (tsGridIO->ColNum () != gridIO->ColNum ()))
		{ delete tsGridIO; delete gridIO; return (DBFault); }

	layerRec = tsGridIO->Layer (0);
	sDate.Set (layerRec->Name ());
	layerRec = tsGridIO->Layer (tsGridIO->LayerNum () - 1);
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

	if (tsGridIO->LayerNum () < stepNum)	goto End;
	
	layerRec = gridIO->Layer (0);
	gridIO->RenameLayer (layerRec,date.Get ());
	for (step = 1;step < stepNum;++step)
		{ date = date + stepDate; gridIO->AddLayer (date.Get ()); }

	if ((sum = (DBFloat *) calloc (stepNum,sizeof (DBFloat))) == (DBFloat *) NULL)
		{
		perror ("Memory Allocation Error in: RGlibCycleMean ()");
		delete tsGridIO;
		delete gridIO;
		return (DBFault);
		}

	if ((obsNum = (DBInt *) calloc (stepNum,sizeof (DBInt))) == (DBInt *) NULL)
		{
		perror ("Memory Allocation Error in: RGlibCycleMean ()");
		free (sum);
		delete tsGridIO;
		delete gridIO;
		return (DBFault);
		}

	for (pos.Row = 0;pos.Row < tsGridIO->RowNum ();pos.Row++)
		{
		if (DBPause (100 * pos.Row / tsGridIO->RowNum ())) goto Stop;
		for (pos.Col = 0;pos.Col < tsGridIO->ColNum ();pos.Col++)
			{
			for (step = 0;step < stepNum;++step) { sum [step] = 0.0; obsNum [step] = 0; }
			for (tsLayerID = 0;tsLayerID < tsGridIO->LayerNum ();++tsLayerID)
				{
				layerRec = tsGridIO->Layer (tsLayerID);
				date.Set (layerRec->Name ());
				if (tsGridIO->Value (layerRec,pos,&value))
					{
					switch (timeStep)
						{
						case DBTimeStepHour:		step = date.HoursAD ()	- sDate.HoursAD ();	break;
						case DBTimeStepDay:		step = date.DaysAD ()	- sDate.DaysAD ();	break;
						case DBTimeStepMonth:	step = date.MonthsAD ()	- sDate.MonthsAD ();	break;
						case DBTimeStepYear:		step = date.YearsAD ()	- sDate.YearsAD ();	break;
						}
					sum [step] += value;
					obsNum [step] += 1;
					}
				}
			if (doSum)
				for (step = 0;step < gridIO->LayerNum ();++step)
					{
					if (obsNum [step] > 0)
							gridIO->Value (gridIO->Layer (step),pos,sum [step]);
					else	gridIO->Value (gridIO->Layer (step),pos,gridIO->MissingValue ());
					}
			else
				for (step = 0;step < gridIO->LayerNum ();++step)
					{
					if (obsNum [step] > 0)
							gridIO->Value (gridIO->Layer (step),pos,(sum [step] / (DBFloat) obsNum [step]));
					else	gridIO->Value (gridIO->Layer (step),pos,gridIO->MissingValue ());
					}
			}
		}
	gridIO->RecalcStats ();

	ret = DBSuccess;
Stop:
	free (sum);
	free (obsNum);
End:
	delete tsGridIO;
	delete gridIO;
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
	DBGridIO *tsGridIO = new DBGridIO (tsData);
	DBGridIO *gridIO   = new DBGridIO (data);
	
	if ((seasonLen != 2) && (seasonLen != 3) && (seasonLen != 4) && (seasonLen != 6))
		{
		fprintf (stderr,"Invalid season length in: RGlibSeasonAggregate ()\n");
		delete tsGridIO;
		delete gridIO;
		return (DBFault);
		}

	seasonNum = 12 / seasonLen;

	if ((tsGridIO->RowNum () != gridIO->RowNum ()) || (tsGridIO->ColNum () != gridIO->ColNum ()))
		{ delete tsGridIO; delete gridIO; return (DBFault); }

	layerRec = tsGridIO->Layer (0);
	sDate.Set (layerRec->Name ());
	layerRec = tsGridIO->Layer (tsGridIO->LayerNum () - 1);
	eDate.Set (layerRec->Name ());

	stepNum = (eDate.MonthsAD () - sDate.MonthsAD () + 1) / seasonLen;

	if (tsGridIO->LayerNum () < stepNum)	goto End;
	
	layerRec = gridIO->Layer (0);
	startYear = year = sDate.Year ();
	if (year != DBDefaultMissingIntVal) sprintf (layerName,"%4d-Season", year);
	else sprintf (layerName,"XXXX-Season");
	gridIO->RenameLayer (layerRec,layerName);
	for (step = 1;step < stepNum;++step)
		{
		if (startYear != DBDefaultMissingIntVal)
			{ year = step / seasonNum; sprintf (layerName,"%4d-Season", year + startYear); }
		else	sprintf (layerName,"XXXX-Season");
		gridIO->AddLayer (layerName);
		}

	for (month = 0;month < 12;++month)
		monthSeason [((month + offset > 0 ? month  : month + 12) + offset) % 12] = month /seasonLen;

	for (step = 0;step < stepNum;++step)
		{
		season = step % seasonNum;
		layerRec = gridIO->Layer (step);
		strcpy (layerName,layerRec->Name ());
		for (month = offset;month < 12 + offset;month++)
			if (monthSeason [month % 12] == season) sprintf (layerName + strlen (layerName),"_%02d",(month % 12) + 1);
		gridIO->RenameLayer (layerRec,layerName);
		}

	if ((sum = (DBFloat *) calloc (stepNum,sizeof (DBFloat))) == (DBFloat *) NULL)
		{
		perror ("Memory Allocation Error in: RGlibCycleMean ()");
		delete tsGridIO;
		delete gridIO;
		return (DBFault);
		}

	if ((obsNum = (DBInt *) calloc (stepNum,sizeof (DBInt))) == (DBInt *) NULL)
		{
		perror ("Memory Allocation Error in: RGlibCycleMean ()");
		free (sum);
		delete tsGridIO;
		delete gridIO;
		return (DBFault);
		}

	for (pos.Row = 0;pos.Row < tsGridIO->RowNum ();pos.Row++)
		{
		if (DBPause (100 * pos.Row / tsGridIO->RowNum ())) goto Stop;
		for (pos.Col = 0;pos.Col < tsGridIO->ColNum ();pos.Col++)
			{
			for (step = 0;step < stepNum;++step) { sum [step] = 0.0; obsNum [step] = 0; }
			for (tsLayerID = 0;tsLayerID < tsGridIO->LayerNum ();++tsLayerID)
				{
				layerRec = tsGridIO->Layer (tsLayerID);
				date.Set (layerRec->Name ());
				year = date.Year ();
				season = monthSeason [date.Month ()];
				if (tsGridIO->Value (layerRec,pos,&value))
					{
					step = ((year - startYear) * seasonNum + season) % stepNum;
					sum    [step] += value;
					obsNum [step] += 1;
					}
				}
			if (doSum)
				for (step = 0;step < gridIO->LayerNum ();++step)
					{
					if (obsNum [step] > 0)
							gridIO->Value (gridIO->Layer (step),pos,sum [step]);
					else	gridIO->Value (gridIO->Layer (step),pos,gridIO->MissingValue ());
					}
			else
				for (step = 0;step < gridIO->LayerNum ();++step)
					{
					if (obsNum [step] > 0)
							gridIO->Value (gridIO->Layer (step),pos,(sum [step] / (DBFloat) obsNum [step]));
					else	gridIO->Value (gridIO->Layer (step),pos,gridIO->MissingValue ());
					}
			}
		}
	gridIO->RecalcStats ();

	ret = DBSuccess;
Stop:
	free (sum);
	free (obsNum);
End:
	delete tsGridIO;
	delete gridIO;
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
	DBGridIO *tsGridIO = new DBGridIO (tsData);
	DBGridIO *gridIO   = new DBGridIO (data);
	
	if ((seasonLen != 2) && (seasonLen != 3) && (seasonLen != 4) && (seasonLen != 6))
		{
		fprintf (stderr,"Invalid season length in: RGlibSeasonAggregate ()\n");
		delete tsGridIO;
		delete gridIO;
		return (DBFault);
		}

	seasonNum = 12 / seasonLen;

	if ((tsGridIO->RowNum () != gridIO->RowNum ()) || (tsGridIO->ColNum () != gridIO->ColNum ()))
		{ delete tsGridIO; delete gridIO; return (DBFault); }

	layerRec = tsGridIO->Layer (0);
	sDate.Set (layerRec->Name ());
	layerRec = tsGridIO->Layer (tsGridIO->LayerNum () - 1);
	eDate.Set (layerRec->Name ());

	if (tsGridIO->LayerNum () < seasonNum)	goto End;
	
	layerRec = gridIO->Layer (0);
	sprintf (layerName,"XXXX-Season");
	gridIO->RenameLayer (layerRec,layerName);
	for (season = 1;season < seasonNum;++season)
		{ sprintf (layerName,"XXXX-Season"); gridIO->AddLayer (layerName); }
	
	for (month = 0;month < 12;++month)
		{
		season = month / seasonLen;
		monthSeason [((month + offset > 0 ? month  : month + 12) + offset) % 12] = season;
		layerRec = gridIO->Layer (season);
		strcpy (layerName,layerRec->Name ());
		sprintf (layerName + strlen (layerName),"_%02d",(((month + offset > 0 ? month  : month + 12) + offset) % 12) + 1);
		gridIO->RenameLayer (layerRec,layerName);
		}

	if ((sum = (DBFloat *) calloc (seasonNum,sizeof (DBFloat))) == (DBFloat *) NULL)
		{
		perror ("Memory Allocation Error in: RGlibCycleMean ()");
		delete tsGridIO;
		delete gridIO;
		return (DBFault);
		}

	if ((obsNum = (DBInt *) calloc (seasonNum,sizeof (DBInt))) == (DBInt *) NULL)
		{
		perror ("Memory Allocation Error in: RGlibCycleMean ()");
		free (sum);
		delete tsGridIO;
		delete gridIO;
		return (DBFault);
		}

	for (pos.Row = 0;pos.Row < tsGridIO->RowNum ();pos.Row++)
		{
		if (DBPause (100 * pos.Row / tsGridIO->RowNum ())) goto Stop;
		for (pos.Col = 0;pos.Col < tsGridIO->ColNum ();pos.Col++)
			{
			for (season = 0;season < seasonNum;++season) { sum [season] = 0.0; obsNum [season] = 0; }
			for (tsLayerID = 0;tsLayerID < tsGridIO->LayerNum ();++tsLayerID)
				{
				layerRec = tsGridIO->Layer (tsLayerID);
				date.Set (layerRec->Name ());
				season = monthSeason [date.Month ()];
				if (tsGridIO->Value (layerRec,pos,&value))
					{ sum [season] += value; obsNum [season] += 1; }
				}
			if (doSum)
				for (season = 0;season < gridIO->LayerNum ();++season)
					{
					if (obsNum [season] > 0)
							gridIO->Value (gridIO->Layer (season),pos,sum [season]);
					else	gridIO->Value (gridIO->Layer (season),pos,gridIO->MissingValue ());
					}
			else
				for (season = 0;season < gridIO->LayerNum ();++season)
					{
					if (obsNum [season] > 0)
							gridIO->Value (gridIO->Layer (season),pos,(sum [season] / (DBFloat) obsNum [season]));
					else	gridIO->Value (gridIO->Layer (season),pos,gridIO->MissingValue ());
					}
			}
		}
	gridIO->RecalcStats ();

	ret = DBSuccess;
Stop:
	free (sum);
	free (obsNum);
End:
	delete tsGridIO;
	delete gridIO;
	return (ret);
	}

DBInt RGlibMinMax (DBObjData *tsData,DBObjData *data,bool doMin)

	{
	DBInt tsLayerID, layerID;
	DBFloat value, searchVal;
	DBPosition pos;
	DBObjRecord *layerRec;
	DBGridIO *tsGridIO = new DBGridIO (tsData);
	DBGridIO *gridIO   = new DBGridIO (data);
	
	if ((tsGridIO->RowNum () != gridIO->RowNum ()) || (tsGridIO->ColNum () != gridIO->ColNum ()))
		{ delete tsGridIO; delete gridIO; return (DBFault); }

	for (pos.Row = 0;pos.Row < tsGridIO->RowNum ();pos.Row++)
		for (pos.Col = 0;pos.Col < tsGridIO->ColNum ();pos.Col++)
			{
			searchVal = doMin ? HUGE_VAL : - HUGE_VAL;
			layerID = DBFault;
			for (tsLayerID = 0;tsLayerID < tsGridIO->LayerNum ();++tsLayerID)
				{
				layerRec = tsGridIO->Layer (tsLayerID);
				if (tsGridIO->Value (layerRec,pos,&value))
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
					gridIO->Value (pos,layerID);
			else	gridIO->Value (pos,gridIO->MissingValue ());
			}
	gridIO->RenameLayer ("XXXX");
	gridIO->RecalcStats ();

	delete tsGridIO;
	delete gridIO;
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
	DBVPointIO	*pntIO = (DBVPointIO *)	 NULL;
	DBNetworkIO	*netIO = (DBNetworkIO *) NULL;
	DBGridIO *gridIO;
	DBObjRecord *record, *layerRec, *tblRec;

	gridIO = new DBGridIO (grdData);
	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{ fprintf (stderr,"No Layer to Process in RGlibGridSampling ()\n"); delete gridIO; return (DBFault); }


	if (dbData->Type () == DBTypeVectorPoint)
			pntIO = new DBVPointIO (dbData);
	else	netIO = new DBNetworkIO (dbData);

	table->AddField (sampleIDFLD =	new DBObjTableField ("GHAASSampleID",	DBTableFieldInt,	"%8d",sizeof (DBInt)));
	table->AddField (layerIDFLD  =	new DBObjTableField ("LayerID",			DBTableFieldInt,	"%4d",sizeof (DBShort)));
	table->AddField (layerNameFLD =	new DBObjTableField ("LayerName",		DBTableFieldString,"%s",DBStringLength));

	switch (grdData->Type ())
		{
		case DBTypeGridContinuous:
			{
			DBFloat realValue;
		
			newField = new DBObjTableField (grdData->Document(DBDocSubject),DBTableFieldFloat,gridIO->ValueFormat (),sizeof (DBFloat4));
			table->AddField (newField);
			maxProgress = itemTable->ItemNum () * layerNum;
			for (recID = 0;recID < itemTable->ItemNum ();recID++)
				{
				record = itemTable->Item (recID);
				if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
				for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
					{
					layerRec = gridIO->Layer (layerID);
					if (DBPause (progress * 100 / maxProgress)) goto Stop;
					progress++;
					if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
					tblRec = table->Add (record->Name ());
					sampleIDFLD->Int (tblRec,record->RowID () + 1);
					layerIDFLD->Int (tblRec,layerRec->RowID ());
					layerNameFLD->String (tblRec,layerRec->Name ());
					if (pntIO != (DBVPointIO *) NULL) coord = pntIO->Coordinate (record);
					else	coord = netIO->Center (netIO->MouthCell (record));
					if (gridIO->Value (layerRec,coord,&realValue))
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
				for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
					{
					layerRec = gridIO->Layer (layerID);
					if (DBPause (progress * 100 / maxProgress)) goto Stop;
					progress++;
					if ((layerRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
					tblRec = table->Add (record->Name ());
					sampleIDFLD->Int (tblRec,record->RowID () + 1);
					layerIDFLD->Int (tblRec,layerRec->RowID ());
					layerNameFLD->String (tblRec,layerRec->Name ());

					if (pntIO != (DBVPointIO *) NULL) coord = pntIO->Coordinate (record);
					else	coord = netIO->Center (netIO->MouthCell (record));
					if ((grdRec = gridIO->GridItem (layerRec,coord)) != (DBObjRecord *) NULL)
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
	delete gridIO;
	if (pntIO != (DBVPointIO  *) NULL) delete pntIO;
	if (netIO != (DBNetworkIO *) NULL) delete netIO;
	return (progress < maxProgress ? DBFault : DBSuccess);
	}

void RGlibGridSampling (DBObjData *splData,DBObjData *grdData)

	{
	DBInt layerID, layerNum = 0, recordID;
	char *tableName;
	DBCoordinate coord;
	DBObjTable *table;
	DBGridIO *gridIO;
	DBVPointIO	*pntIO = (DBVPointIO *)	 NULL;
	DBNetworkIO *netIO = (DBNetworkIO *) NULL;
	DBObjTableField *newField;
	DBObjRecord *layerRec, *record;

	gridIO = new DBGridIO (grdData);
	for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
		{
		layerRec = gridIO->Layer (layerID);
		if ((layerRec->Flags () & DBObjectFlagIdle) != DBObjectFlagIdle) ++layerNum;
		}
	if (layerNum < 1)
		{ fprintf (stderr,"No Layer to Process in RGlibGridSampling ()\n"); delete gridIO; return; }

	if (splData->Type () == DBTypeVectorPoint)
			{ pntIO = new DBVPointIO  (splData); tableName = DBrNItems; }
	else	{ netIO = new DBNetworkIO (splData); tableName = DBrNCells; }
	table = splData->Table (tableName);

	switch (grdData->Type ())
		{
		case DBTypeGridContinuous:
			{
			DBFloat value;
			DBObjRecord *layerRec;
			
			for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
				{
				layerRec = gridIO->Layer (layerID);
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
					DBPause ((layerRec->RowID () * table->ItemNum () + recordID) * 100 / (gridIO->LayerNum () * table->ItemNum ()));
					if (pntIO != (DBVPointIO *) NULL) coord = pntIO->Coordinate (record);
					else	coord = netIO->Center (record);

					if (gridIO->Value (layerRec,coord,&value))
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
				if (pntIO != (DBVPointIO *) NULL) coord = pntIO->Coordinate (record);
				else	coord = netIO->Center (record);

				if ((grdRec = gridIO->GridItem (coord)) != (DBObjRecord *) NULL)
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
						if (pntIO != (DBVPointIO *) NULL) coord = pntIO->Coordinate (record);
						else	coord = netIO->Center (record);

						if ((grdRec = gridIO->GridItem (coord)) != (DBObjRecord *) NULL)
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
	if (pntIO != (DBVPointIO *) NULL)  delete pntIO;
	if (netIO != (DBNetworkIO *) NULL) delete netIO;
	delete gridIO;
	}
