/******************************************************************************

GHAAS RiverGIS Libarary V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGlibDataStream.C

balazs.fekete@unh.edu

*******************************************************************************/

#include<cm.h>
#include<DB.H>
#include<DBif.H>
#include<MF.h>

DBInt RGlibRGIS2DataStream (DBObjData *grdData,DBObjData *tmplData,char *fieldName,FILE *outFile)
	{
	DBInt layerID, ret, itemSize, itemID;
	DBInt intValue;
	DBFloat floatValue;
	void *data;
	MFVarHeader_t varHeader;
	DBObjRecord *layerRec, *gridRec;
	DBObjTableField *fieldPTR = (DBObjTableField *) NULL;
	DBGridIF *gridIF;
	DBVPointIF  *tmplPntIF = (DBVPointIF *)  NULL;
	DBGridIF    *tmplGrdIF = (DBGridIF *)    NULL;
	DBNetworkIF *tmplNetIF = (DBNetworkIF *) NULL;

	gridIF = new DBGridIF (grdData);

	varHeader.Swap = 1;
	if (grdData->Type () == DBTypeGridDiscrete)
		{
		DBObjTable *itemTable = grdData->Table (DBrNItems);

		if (fieldName == (char *) NULL) fieldName = DBrNGridValue;
		if ((fieldPTR = itemTable->Field (fieldName)) == (DBObjTableField *) NULL)
			{ CMmsgPrint (CMmsgAppError, "Error: Invalid field [%s] in: %s %d",fieldName,__FILE__,__LINE__); return (DBFault); }
		itemSize = fieldPTR->Length ();
		switch (fieldPTR->Type ())
			{
			case DBTableFieldInt:
				switch (itemSize)
					{
					default:
					case sizeof (DBByte):  varHeader.DataType = MFByte;      break;
					case sizeof (DBShort): varHeader.DataType = MFShort;     break;
					case sizeof (DBInt):   varHeader.DataType = MFInt;       break;
					}
				varHeader.Missing.Int   = fieldPTR->IntNoData ();
				break;
			case DBTableFieldFloat:
				switch (itemSize)
					{
					default:
					case sizeof (DBFloat4):	varHeader.DataType = MFFloat;		break;
					case sizeof (DBFloat):	varHeader.DataType = MFDouble;	break;
					}
				varHeader.Missing.Float = fieldPTR->FloatNoData ();
				break;
			}
		}
	else
		{
		if (fieldName != (char *) NULL) CMmsgPrint (CMmsgUsrError, "Warning: Fieldname ignored for continuous grid!");
		itemSize = gridIF->ValueSize ();
		switch (gridIF->ValueType ())
			{
			case DBVariableInt:
				switch (itemSize)
					{
					case 1:	varHeader.DataType = MFByte; 		break;
					case 2:	varHeader.DataType = MFShort; 	break;
					case 4:	varHeader.DataType = MFInt;	 	break;
					}
				varHeader.Missing.Int = (int) gridIF->MissingValue ();
				break;
			case DBVariableFloat:
				switch (itemSize)
					{
					case 4:	varHeader.DataType = MFFloat;	 	break;
					case 8:	varHeader.DataType = MFDouble; 	break;
					}
				varHeader.Missing.Float = gridIF->MissingValue ();
				break;
			}
		}

	if (tmplData == (DBObjData *) NULL)
		{
		tmplGrdIF = gridIF;
		varHeader.ItemNum = gridIF->RowNum () * gridIF->ColNum ();
		}
	else
		{
		switch (tmplData->Type ())
			{
			case DBTypeVectorPoint:
				tmplPntIF = new DBVPointIF (tmplData);
				varHeader.ItemNum = tmplPntIF->ItemNum ();
				break;
			case DBTypeGridContinuous:
			case DBTypeGridDiscrete:
				tmplGrdIF = new DBGridIF (tmplData);
				varHeader.ItemNum = gridIF->RowNum () * gridIF->ColNum ();
				break;
			case DBTypeNetwork:
				tmplNetIF = new DBNetworkIF (tmplData);
				varHeader.ItemNum = tmplNetIF->CellNum ();
				break;
			default:	delete gridIF; return (DBFault);
			}
		}
	if ((data = (void *) calloc (varHeader.ItemNum,itemSize)) == (void *) NULL)
		{
		CMmsgPrint (CMmsgSysError, "Error! Allocating %d items of %d size in: %s %d",varHeader.ItemNum,itemSize,__FILE__,__LINE__);
		return (DBFault);
		}

/**************************************************************
*                                                             *
* Point template                                              *
*                                                             *
**************************************************************/

	if (tmplPntIF != (DBVPointIF *) NULL)
		{
		DBObjRecord *pntRec;

		if (fieldPTR == (DBObjTableField *) NULL)
			{
			for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
				{
				layerRec = gridIF->Layer (layerID);
				strncpy (varHeader.Date,layerRec->Name (),MFDateStringLength - 1);
				for (itemID = 0;itemID < varHeader.ItemNum;++itemID)
					{
					pntRec = tmplPntIF->Item (itemID);
					if ((varHeader.DataType == MFByte) || (varHeader.DataType == MFShort) || (varHeader.DataType == MFInt))
						{
						if (gridIF->Value (layerRec,tmplPntIF->Coordinate (pntRec),&intValue) == false) intValue = varHeader.Missing.Int;
						switch (varHeader.DataType)
							{
							case MFByte:  ((char *)  data) [itemID] = (char)  intValue;	break;
							case MFShort: ((short *) data) [itemID] = (short) intValue;	break;
							case MFInt:   ((int *)   data) [itemID] = (short) intValue;	break;
						 	}
						}
					else
						{
						if (gridIF->Value (layerRec,tmplPntIF->Coordinate (pntRec),&floatValue) == false) floatValue = varHeader.Missing.Float;
						switch (varHeader.DataType)
							{
							case MFFloat:  ((float *)  data) [itemID] = (float)  floatValue;	break;
							case MFDouble: ((double *) data) [itemID] = (double) floatValue;	break;
							}
						}
					}
				if ((DBInt) fwrite (&varHeader,sizeof (MFVarHeader_t),1,outFile) != 1)
					{ CMmsgPrint (CMmsgSysError, "Error: Writing record header in: %s %d",__FILE__,__LINE__);	ret = DBFault; break; }
				if ((DBInt) fwrite (data,itemSize,varHeader.ItemNum,outFile) != varHeader.ItemNum)
					{ CMmsgPrint (CMmsgSysError, "Error: Writing data in: %s %d",__FILE__,__LINE__);				ret = DBFault; break; }
				}
			}
		else
			{
			for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
				{
				layerRec = gridIF->Layer (layerID);
				strncpy (varHeader.Date,layerRec->Name (),MFDateStringLength - 1);
				for (itemID = 0;itemID < varHeader.ItemNum;++itemID)
					{
					pntRec = tmplPntIF->Item (itemID);
					gridRec = gridIF->GridItem (layerRec,tmplPntIF->Coordinate (pntRec));
					switch (varHeader.DataType)
						{
						case MFByte:
							((char *)   data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int (gridRec) : fieldPTR->IntNoData ();
							break;
						case MFShort:
							((short *)  data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int (gridRec) : fieldPTR->IntNoData ();
							break;
						case MFInt:
							((int *)    data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int (gridRec) : fieldPTR->IntNoData ();
							break;
						case MFFloat:
							((float *)  data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int (gridRec) : fieldPTR->FloatNoData ();
							break;
						case MFDouble:
							((double *) data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int (gridRec) : fieldPTR->FloatNoData ();
							break;
						}
					}
				if ((DBInt) fwrite (&varHeader,sizeof (MFVarHeader_t),1,outFile) != 1)
					{ CMmsgPrint (CMmsgSysError, "Error: Writing record header in: %s %d",__FILE__,__LINE__);	ret = DBFault; break; }
				if ((DBInt) fwrite (data,itemSize,varHeader.ItemNum,outFile) != varHeader.ItemNum)
					{ CMmsgPrint (CMmsgSysError, "Error: Writing data in: %s %d",__FILE__,__LINE__);				ret = DBFault; break; }
				}
			}
		delete tmplPntIF;
		}

/**************************************************************
*                                                             *
* Grid Template (default when no template coverage is given.  *
*                                                             *
**************************************************************/
	else if (tmplGrdIF != (DBGridIF *) NULL)
		{
		DBPosition pos;
		DBCoordinate coord;

		if (fieldPTR == (DBObjTableField *) NULL)
			{
			for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
				{
				layerRec = gridIF->Layer (layerID);
				strncpy (varHeader.Date,layerRec->Name (),MFDateStringLength - 1);
				for (pos.Row = 0;pos.Row < tmplGrdIF->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < tmplGrdIF->ColNum ();++pos.Col)
						{
						itemID = pos.Row * tmplGrdIF->ColNum () + pos.Col;
						if ((varHeader.DataType == MFByte) || (varHeader.DataType == MFShort) || (varHeader.DataType == MFInt))
							{
							if (tmplGrdIF != gridIF)
								{
								tmplGrdIF->Pos2Coord (pos,coord);
								if (gridIF->Value (layerRec,coord,&intValue) == false) intValue = varHeader.Missing.Int;
								}
							else
								{
								if (gridIF->Value (layerRec,pos,&intValue) == false) intValue = varHeader.Missing.Int;
								}
							switch (varHeader.DataType)
								{
								case MFByte:  ((char *)  data) [itemID] = (char)  intValue;	break;
								case MFShort: ((short *) data) [itemID] = (short) intValue;	break;
								case MFInt:   ((int *)   data) [itemID] = (short) intValue;	break;
								}
							}
						else
							{
							if (tmplGrdIF != gridIF)
								{
								tmplGrdIF->Pos2Coord (pos,coord);
								if (gridIF->Value (layerRec,coord,&floatValue) == false) floatValue = varHeader.Missing.Float;
								}
							else
								{
								if (gridIF->Value (layerRec,pos,&floatValue) == false) floatValue = varHeader.Missing.Float;
								}
							switch (varHeader.DataType)
								{
								case MFFloat:  ((float *)  data) [itemID] = (float)  floatValue;	break;
								case MFDouble: ((double *) data) [itemID] = (double) floatValue;	break;
								}
							}
						}
				if ((DBInt) fwrite (&varHeader,sizeof (MFVarHeader_t),1,outFile) != 1)
					{ CMmsgPrint (CMmsgSysError, "Error: Writing record header in: %s %d",__FILE__,__LINE__);	ret = DBFault; break; }
				if ((DBInt) fwrite (data,itemSize,varHeader.ItemNum,outFile) != varHeader.ItemNum)
					{ CMmsgPrint (CMmsgSysError, "Error: Writing data in: %s %d",__FILE__,__LINE__);				ret = DBFault; break; }
				}
			}
		else
			{
			for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
				{
				layerRec = gridIF->Layer (layerID);
				strncpy (varHeader.Date,layerRec->Name (),MFDateStringLength - 1);
				for (pos.Row = 0;pos.Row < tmplGrdIF->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < tmplGrdIF->ColNum ();++pos.Col)
						{
						itemID = pos.Row * tmplGrdIF->ColNum () + pos.Col;
						if (tmplGrdIF != gridIF)
							{
							tmplGrdIF->Pos2Coord (pos,coord);
							gridRec = gridIF->GridItem (layerRec,coord);
							}
						else gridRec = gridIF->GridItem (layerRec,pos);
						switch (varHeader.DataType)
							{
							case MFByte:
								((char *)   data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int (gridRec) : fieldPTR->IntNoData ();
								break;
							case MFShort:
								((short *)  data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int (gridRec) : fieldPTR->IntNoData ();
								break;
							case MFInt:
								((int *)    data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int (gridRec) : fieldPTR->IntNoData ();
								break;
							case MFFloat:
								((float *)  data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int (gridRec) : fieldPTR->FloatNoData ();
								break;
							case MFDouble:
								((double *) data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int (gridRec) : fieldPTR->FloatNoData ();
								break;
							}
						}
				if ((DBInt) fwrite (&varHeader,sizeof (MFVarHeader_t),1,outFile) != 1)
					{ CMmsgPrint (CMmsgSysError, "Error: Writing record header in: %s %d",__FILE__,__LINE__);	ret = DBFault; break; }
				if ((DBInt) fwrite (data,itemSize,varHeader.ItemNum,outFile) != varHeader.ItemNum)
					{ CMmsgPrint (CMmsgSysError, "Error: Writing data in: %s %d",__FILE__,__LINE__);				ret = DBFault; break; }
				}
			}
		if (tmplGrdIF != gridIF) delete tmplGrdIF;
		}

/**************************************************************
*                                                             *
* Network Template                                            *
*                                                             *
**************************************************************/
	else if (tmplNetIF != (DBNetworkIF *) NULL)
		{
		DBObjRecord *cellRec;

		if (fieldPTR == (DBObjTableField *) NULL)
			{
			for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
				{
				layerRec = gridIF->Layer (layerID);
				strncpy (varHeader.Date,layerRec->Name (),MFDateStringLength - 1);
				for (itemID = 0;itemID < varHeader.ItemNum;++itemID)
					{
					cellRec = tmplNetIF->Cell (itemID);
					if ((varHeader.DataType == MFByte) || (varHeader.DataType == MFShort) || (varHeader.DataType == MFInt))
						{
						if (gridIF->Value (layerRec,tmplNetIF->Center (cellRec),&intValue) == false) intValue = varHeader.Missing.Int;
						switch (varHeader.DataType)
							{
							case MFByte:  ((char *)  data) [itemID] = (char)  intValue;	break;
							case MFShort: ((short *) data) [itemID] = (short) intValue;	break;
							case MFInt:   ((int *)   data) [itemID] = (short) intValue;	break;
						 	}
						}
					else
						{
						if (gridIF->Value (layerRec,tmplNetIF->Center (cellRec),&floatValue) == false) floatValue = varHeader.Missing.Float;
						switch (varHeader.DataType)
							{
							case MFFloat:  ((float *)  data) [itemID] = (float)  floatValue;	break;
							case MFDouble: ((double *) data) [itemID] = (double) floatValue;	break;
							}
						}
					}
				if ((DBInt) fwrite (&varHeader,sizeof (MFVarHeader_t),1,outFile) != 1)
					{ CMmsgPrint (CMmsgSysError, "Error: Writing record header in: %s %d",__FILE__,__LINE__);	ret = DBFault; break; }
				if ((DBInt) fwrite (data,itemSize,varHeader.ItemNum,outFile) != varHeader.ItemNum)
					{ CMmsgPrint (CMmsgSysError, "Error: Writing data in: %s %d",__FILE__,__LINE__);				ret = DBFault; break; }
				}
			}
		else
			{
			for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
				{
				layerRec = gridIF->Layer (layerID);
				strncpy (varHeader.Date,layerRec->Name (),MFDateStringLength - 1);
				for (itemID = 0;itemID < varHeader.ItemNum;++itemID)
					{
					cellRec = tmplNetIF->Cell (itemID);
					gridRec = gridIF->GridItem (layerRec,tmplNetIF->Center (cellRec));
					switch (varHeader.DataType)
						{
						case MFByte:
							((char *)   data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int (gridRec)   : fieldPTR->IntNoData ();
							break;
						case MFShort:
							((short *)  data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int (gridRec)   : fieldPTR->IntNoData ();
							break;
						case MFInt:
							((int *)    data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Int (gridRec)   : fieldPTR->IntNoData ();
							break;
						case MFFloat:
							((float *)  data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Float (gridRec) : fieldPTR->FloatNoData ();
							break;
						case MFDouble:
							((double *) data) [itemID] = gridRec != (DBObjRecord *) NULL ? fieldPTR->Float (gridRec) : fieldPTR->FloatNoData ();
							break;
						}
					}
				if ((DBInt) fwrite (&varHeader,sizeof (MFVarHeader_t),1,outFile) != 1)
					{ CMmsgPrint (CMmsgSysError, "Error: Writing record header in: %s %d",__FILE__,__LINE__);	ret = DBFault; break; }
				if ((DBInt) fwrite (data,itemSize,varHeader.ItemNum,outFile) != varHeader.ItemNum)
					{ CMmsgPrint (CMmsgSysError, "Error: Writing data in: %s %d",__FILE__,__LINE__);				ret = DBFault; break; }
				}
			}
		delete tmplNetIF;
		}

	free (data);
	delete gridIF;
	return (ret);
	}

DBInt RGlibDataStream2RGIS (DBObjData *outData,DBObjData *tmplData, FILE *inFile)
	{
	DBInt layerID = 0, itemSize;
	DBPosition pos;
	DBFloat val;
	void *data = (void *) NULL;
	MFVarHeader_t header;
	DBObjRecord *record;

	switch (tmplData->Type ())
		{
		case DBTypeVectorPoint:
			{
			DBInt itemID;
			DBDate date;
			DBObjTable *itemTable = outData->Table (DBrNItems);
			DBObjTableField *idField      = new DBObjTableField ("ItemID",      DBTableFieldInt,  "%6d",  sizeof (DBInt),  false);
			DBObjTableField *dateField    = new DBObjTableField ("Date",        DBTableFieldDate, "%s",   sizeof (DBDate), false);
			DBObjTableField *valField;
			DBVPointIF *pntIF = new DBVPointIF (tmplData);

			itemTable->AddField (idField);
			itemTable->AddField (dateField);

			while (MFVarReadHeader (&header,inFile))
				{
				if (header.ItemNum != pntIF->ItemNum ())
					{ CMmsgPrint (CMmsgUsrError, "Error: Datastream inconsistency %d %d!",header.ItemNum,pntIF->ItemNum ()); return (DBFault); }
				if (data == (void *) NULL)
					{
					itemSize = MFVarItemSize (header.DataType);
					if ((data = (void *) realloc (data,header.ItemNum * itemSize)) == (void *) NULL)
						{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return (DBFault); }
					switch (header.DataType)
						{
						case MFByte:	valField = new DBObjTableField ("Value", DBTableFieldInt,     "%2d", sizeof (char),     false);
						case MFShort:	valField = new DBObjTableField ("Value", DBTableFieldInt,     "%4d", sizeof (DBShort),  false);
						case MFInt:		valField = new DBObjTableField ("Value", DBTableFieldInt,     "%8d", sizeof (DBInt),    false);
						case MFFloat:	valField = new DBObjTableField ("Value", DBTableFieldFloat, "%8.2f", sizeof (DBFloat4), false);
						case MFDouble:	valField = new DBObjTableField ("Value", DBTableFieldFloat, "%8.2f", sizeof (DBFloat),  false);
						}
					itemTable->AddField (valField);
					}
				if ((int) fread (data,itemSize,header.ItemNum,inFile) != header.ItemNum)
					{ CMmsgPrint (CMmsgSysError, "Error: Data stream read in: %s %d",__FILE__,__LINE__); return (DBFault); }
				for (itemID = 0;itemID < header.ItemNum;++itemID)
					{
					record = itemTable->Add (header.Date);
					date.Set (header.Date);
					idField->Int (record,itemID);
					dateField->Date (record,date);
/*					decDateField->Float (record,date);
*/					switch (header.DataType)
						{
						case MFByte:	valField->Int   (record,((char *)   data) [itemID]); break;
						case MFShort:	valField->Int   (record,((short *)  data) [itemID]); break;
						case MFInt:		valField->Int   (record,((int *)    data) [itemID]); break;
						case MFFloat:	valField->Float (record,((float *)  data) [itemID]); break;
						case MFDouble:	valField->Float (record,((double *) data) [itemID]); break;
						}
					}
				}
			delete pntIF;
			} break;
		case DBTypeGridContinuous:
		case DBTypeGridDiscrete:
			{
			DBGridIF *gridIF = new DBGridIF (outData);

			while (MFVarReadHeader (&header,inFile))
				{
				if (header.ItemNum != gridIF->RowNum () * gridIF->ColNum ())
					{ CMmsgPrint (CMmsgUsrError, "Error: Datastream inconsistency!"); return (DBFault); }
				if (layerID == 0)
					{
					itemSize = MFVarItemSize (header.DataType);
					if ((data = (void *) realloc (data,header.ItemNum * itemSize)) == (void *) NULL)
						{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return (DBFault); }
					record = gridIF->Layer (layerID);
					gridIF->RenameLayer (header.Date);
					}
				else record = gridIF->AddLayer (header.Date);

				switch (header.DataType)
					{
					case MFByte:
					case MFShort:
					case MFInt:		gridIF->MissingValue (record,header.Missing.Int); break;
					case MFFloat:
					case MFDouble:	gridIF->MissingValue (record,header.Missing.Float); break;
					}
				if ((int) fread (data,itemSize,header.ItemNum,inFile) != header.ItemNum)
					{ CMmsgPrint (CMmsgSysError, "Error: Data stream read in: %s %d",__FILE__,__LINE__); return (DBFault); }

				for (pos.Row = 0;pos.Row < gridIF->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < gridIF->ColNum ();++pos.Col)
						{
						switch (header.DataType)
							{
							case MFByte:	val = (DBFloat) (((char *)   data) [pos.Row * gridIF->ColNum () + pos.Col]); break;
							case MFShort:	val = (DBFloat) (((short *)  data) [pos.Row * gridIF->ColNum () + pos.Col]); break;
							case MFInt:		val = (DBFloat) (((int *)    data) [pos.Row * gridIF->ColNum () + pos.Col]); break;
							case MFFloat:	val = (DBFloat) (((float *)  data) [pos.Row * gridIF->ColNum () + pos.Col]); break;
							case MFDouble:	val = (DBFloat) (((double *) data) [pos.Row * gridIF->ColNum () + pos.Col]); break;
							}
						gridIF->Value (record,pos,val);
						}
				layerID++;
				}
			gridIF->RecalcStats ();
			} break;
		case DBTypeNetwork:
			{
			DBInt cellID;
			DBGridIF *gridIF   = new DBGridIF (outData);
			DBNetworkIF *netIF = new DBNetworkIF (tmplData);

			while (MFVarReadHeader (&header,inFile))
				{
				if (header.ItemNum != netIF->CellNum ())
					{ CMmsgPrint (CMmsgUsrError, "Error: Datastream inconsistency!"); return (DBFault); }
				if (layerID == 0)
					{
					itemSize = MFVarItemSize (header.DataType);
					if ((data = (void *) realloc (data,header.ItemNum * itemSize)) == (void *) NULL)
						{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return (DBFault); }
					record = gridIF->Layer (layerID);
					gridIF->RenameLayer (header.Date);
					}
				else record = gridIF->AddLayer (header.Date);
				if ((int) fread (data,itemSize,header.ItemNum,inFile) != header.ItemNum)
					{ CMmsgPrint (CMmsgSysError, "Error: Data stream read in: %s %d",__FILE__,__LINE__); delete netIF; return (DBFault); }

				for (pos.Row = 0;pos.Row < gridIF->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < gridIF->ColNum ();++pos.Col)
						gridIF->Value (record,pos,gridIF->MissingValue ());

				for (cellID = 0;cellID < netIF->CellNum ();++cellID)
					{
					pos = netIF->CellPosition (netIF->Cell (cellID));

					switch (header.DataType)
						{
						case MFByte:	val = (DBFloat) (((char *)   data) [cellID]); break;
						case MFShort:	val = (DBFloat) (((short *)  data) [cellID]); break;
						case MFInt:		val = (DBFloat) (((int *)    data) [cellID]); break;
						case MFFloat:	val = (DBFloat) (((float *)  data) [cellID]); break;
						case MFDouble:	val = (DBFloat) (((double *) data) [cellID]); break;
						}
					gridIF->Value (record,pos,val);
					}
				layerID++;
				}
			gridIF->RecalcStats ();
			} break;
		}
	return (DBSuccess);
	}
