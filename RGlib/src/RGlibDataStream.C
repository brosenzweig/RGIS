/******************************************************************************

GHAAS RiverGIS Libarary V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGlibDataStream.C

balazs.fekete@unh.edu

*******************************************************************************/

#include<DB.H>
#include<DBio.H>
#include<cm.h>
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
	DBGridIO *gridIO;
	DBVPointIO  *tmplPntIO = (DBVPointIO *)  NULL;
	DBGridIO    *tmplGrdIO = (DBGridIO *)    NULL;
	DBNetworkIO *tmplNetIO = (DBNetworkIO *) NULL;

	gridIO = new DBGridIO (grdData);

	varHeader.Swap = 1;
	if (grdData->Type () == DBTypeGridDiscrete)
		{
		DBObjTable *itemTable = grdData->Table (DBrNItems);

		if (fieldName == (char *) NULL) fieldName = DBrNGridValue;
		if ((fieldPTR = itemTable->Field (fieldName)) == (DBObjTableField *) NULL)
			{ fprintf (stderr,"Error: Invalid field [%s] in: RGlibRGIS2DataStream ()!\n",fieldName); return (DBFault); }
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
		if (fieldName != (char *) NULL) fprintf (stderr,"Warning: Fieldname ignored for continuous grid!\n");
		itemSize = gridIO->ValueSize ();
		switch (gridIO->ValueType ())
			{
			case DBVariableInt:
				switch (itemSize)
					{
					case 1:	varHeader.DataType = MFByte; 		break;
					case 2:	varHeader.DataType = MFShort; 	break;
					case 4:	varHeader.DataType = MFInt;	 	break;
					}
				varHeader.Missing.Int = (int) gridIO->MissingValue ();
				break;
			case DBVariableFloat:
				switch (itemSize)
					{
					case 4:	varHeader.DataType = MFFloat;	 	break;
					case 8:	varHeader.DataType = MFDouble; 	break;
					}
				varHeader.Missing.Float = gridIO->MissingValue ();
				break;
			}
		}

	if (tmplData == (DBObjData *) NULL)
		{
		tmplGrdIO = gridIO;
		varHeader.ItemNum = gridIO->RowNum () * gridIO->ColNum ();
		}
	else
		{
		switch (tmplData->Type ())
			{
			case DBTypeVectorPoint:
				tmplPntIO = new DBVPointIO (tmplData);
				varHeader.ItemNum = tmplPntIO->ItemNum ();
				break;
			case DBTypeGridContinuous:
			case DBTypeGridDiscrete:
				tmplGrdIO = new DBGridIO (tmplData);
				varHeader.ItemNum = gridIO->RowNum () * gridIO->ColNum ();
				break;
			case DBTypeNetwork:
				tmplNetIO = new DBNetworkIO (tmplData);
				varHeader.ItemNum = tmplNetIO->CellNum ();
				break;
			default:	delete gridIO; return (DBFault);
			}
		}
	if ((data = (void *) calloc (varHeader.ItemNum,itemSize)) == (void *) NULL)
		{
		fprintf (stderr,"Error! Allocating %d items of %d size in: RGlibRGIS2DataStream!\n",varHeader.ItemNum,itemSize);
		return (DBFault);
		}

/**************************************************************
*                                                             *
* Point template                                              *
*                                                             *
**************************************************************/

	if (tmplPntIO != (DBVPointIO *) NULL)
		{
		DBObjRecord *pntRec;

		if (fieldPTR == (DBObjTableField *) NULL)
			{
			for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
				{
				layerRec = gridIO->Layer (layerID);
				strncpy (varHeader.Date,layerRec->Name (),MFDateStringLength - 1);
				for (itemID = 0;itemID < varHeader.ItemNum;++itemID)
					{
					pntRec = tmplPntIO->Item (itemID);
					if ((varHeader.DataType == MFByte) || (varHeader.DataType == MFShort) || (varHeader.DataType == MFInt))
						{
						if (gridIO->Value (layerRec,tmplPntIO->Coordinate (pntRec),&intValue) == false) intValue = varHeader.Missing.Int;
						switch (varHeader.DataType)
							{
							case MFByte:  ((char *)  data) [itemID] = (char)  intValue;	break;
							case MFShort: ((short *) data) [itemID] = (short) intValue;	break;
							case MFInt:   ((int *)   data) [itemID] = (short) intValue;	break;
						 	}
						}
					else
						{
						if (gridIO->Value (layerRec,tmplPntIO->Coordinate (pntRec),&floatValue) == false) floatValue = varHeader.Missing.Float;
						switch (varHeader.DataType)
							{
							case MFFloat:  ((float *)  data) [itemID] = (float)  floatValue;	break;
							case MFDouble: ((double *) data) [itemID] = (double) floatValue;	break;
							}
						}
					}
				if ((DBInt) fwrite (&varHeader,sizeof (MFVarHeader_t),1,outFile) != 1)
					{ perror ("Error: Writing record header in: RGlibRGIS2DataStream ()\n");	ret = DBFault; break; }
				if ((DBInt) fwrite (data,itemSize,varHeader.ItemNum,outFile) != varHeader.ItemNum)
					{ perror ("Error: Writing data in: RGlibRGIS2DataStream ()\n");				ret = DBFault; break; }
				}
			}
		else
			{
			for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
				{
				layerRec = gridIO->Layer (layerID);
				strncpy (varHeader.Date,layerRec->Name (),MFDateStringLength - 1);
				for (itemID = 0;itemID < varHeader.ItemNum;++itemID)
					{
					pntRec = tmplPntIO->Item (itemID);
					gridRec = gridIO->GridItem (layerRec,tmplPntIO->Coordinate (pntRec));
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
					{ perror ("Error: Writing record header in: RGlibRGIS2DataStream ()\n");	ret = DBFault; break; }
				if ((DBInt) fwrite (data,itemSize,varHeader.ItemNum,outFile) != varHeader.ItemNum)
					{ perror ("Error: Writing data in: RGlibRGIS2DataStream ()\n");				ret = DBFault; break; }
				}
			}
		delete tmplPntIO;
		}

/**************************************************************
*                                                             *
* Grid Template (default when no template coverage is given.  *
*                                                             *
**************************************************************/
	else if (tmplGrdIO != (DBGridIO *) NULL)
		{
		DBPosition pos;
		DBCoordinate coord;

		if (fieldPTR == (DBObjTableField *) NULL)
			{
			for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
				{
				layerRec = gridIO->Layer (layerID);
				strncpy (varHeader.Date,layerRec->Name (),MFDateStringLength - 1);
				for (pos.Row = 0;pos.Row < tmplGrdIO->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < tmplGrdIO->ColNum ();++pos.Col)
						{
						itemID = pos.Row * tmplGrdIO->ColNum () + pos.Col;
						if ((varHeader.DataType == MFByte) || (varHeader.DataType == MFShort) || (varHeader.DataType == MFInt))
							{
							if (tmplGrdIO != gridIO)
								{
								tmplGrdIO->Pos2Coord (pos,coord);
								if (gridIO->Value (layerRec,coord,&intValue) == false) intValue = varHeader.Missing.Int;
								}
							else
								{
								if (gridIO->Value (layerRec,pos,&intValue) == false) intValue = varHeader.Missing.Int;
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
							if (tmplGrdIO != gridIO)
								{
								tmplGrdIO->Pos2Coord (pos,coord);
								if (gridIO->Value (layerRec,coord,&floatValue) == false) floatValue = varHeader.Missing.Float;
								}
							else
								{
								if (gridIO->Value (layerRec,pos,&floatValue) == false) floatValue = varHeader.Missing.Float;
								}
							switch (varHeader.DataType)
								{
								case MFFloat:  ((float *)  data) [itemID] = (float)  floatValue;	break;
								case MFDouble: ((double *) data) [itemID] = (double) floatValue;	break;
								}
							}
						}
				if ((DBInt) fwrite (&varHeader,sizeof (MFVarHeader_t),1,outFile) != 1)
					{ perror ("Error: Writing record header in: RGlibRGIS2DataStream ()\n");	ret = DBFault; break; }
				if ((DBInt) fwrite (data,itemSize,varHeader.ItemNum,outFile) != varHeader.ItemNum)
					{ perror ("Error: Writing data in: RGlibRGIS2DataStream ()\n");				ret = DBFault; break; }
				}
			}
		else
			{
			for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
				{
				layerRec = gridIO->Layer (layerID);
				strncpy (varHeader.Date,layerRec->Name (),MFDateStringLength - 1);
				for (pos.Row = 0;pos.Row < tmplGrdIO->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < tmplGrdIO->ColNum ();++pos.Col)
						{
						itemID = pos.Row * tmplGrdIO->ColNum () + pos.Col;
						if (tmplGrdIO != gridIO)
							{
							tmplGrdIO->Pos2Coord (pos,coord);
							gridRec = gridIO->GridItem (layerRec,coord);
							}
						else gridRec = gridIO->GridItem (layerRec,pos);
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
					{ perror ("Error: Writing record header in: RGlibRGIS2DataStream ()\n");	ret = DBFault; break; }
				if ((DBInt) fwrite (data,itemSize,varHeader.ItemNum,outFile) != varHeader.ItemNum)
					{ perror ("Error: Writing data in: RGlibRGIS2DataStream ()\n");				ret = DBFault; break; }
				}
			}
		if (tmplGrdIO != gridIO) delete tmplGrdIO;
		}

/**************************************************************
*                                                             *
* Network Template                                            *
*                                                             *
**************************************************************/
	else if (tmplNetIO != (DBNetworkIO *) NULL)
		{
		DBObjRecord *cellRec;

		if (fieldPTR == (DBObjTableField *) NULL)
			{
			for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
				{
				layerRec = gridIO->Layer (layerID);
				strncpy (varHeader.Date,layerRec->Name (),MFDateStringLength - 1);
				for (itemID = 0;itemID < varHeader.ItemNum;++itemID)
					{
					cellRec = tmplNetIO->Cell (itemID);
					if ((varHeader.DataType == MFByte) || (varHeader.DataType == MFShort) || (varHeader.DataType == MFInt))
						{
						if (gridIO->Value (layerRec,tmplNetIO->Center (cellRec),&intValue) == false) intValue = varHeader.Missing.Int;
						switch (varHeader.DataType)
							{
							case MFByte:  ((char *)  data) [itemID] = (char)  intValue;	break;
							case MFShort: ((short *) data) [itemID] = (short) intValue;	break;
							case MFInt:   ((int *)   data) [itemID] = (short) intValue;	break;
						 	}
						}
					else
						{
						if (gridIO->Value (layerRec,tmplNetIO->Center (cellRec),&floatValue) == false) floatValue = varHeader.Missing.Float;
						switch (varHeader.DataType)
							{
							case MFFloat:  ((float *)  data) [itemID] = (float)  floatValue;	break;
							case MFDouble: ((double *) data) [itemID] = (double) floatValue;	break;
							}
						}
					}
				if ((DBInt) fwrite (&varHeader,sizeof (MFVarHeader_t),1,outFile) != 1)
					{ perror ("Error: Writing record header in: RGlibRGIS2DataStream ()\n");	ret = DBFault; break; }
				if ((DBInt) fwrite (data,itemSize,varHeader.ItemNum,outFile) != varHeader.ItemNum)
					{ perror ("Error: Writing data in: RGlibRGIS2DataStream ()\n");				ret = DBFault; break; }
				}
			}
		else
			{
			for (layerID = 0;layerID < gridIO->LayerNum ();++layerID)
				{
				layerRec = gridIO->Layer (layerID);
				strncpy (varHeader.Date,layerRec->Name (),MFDateStringLength - 1);
				for (itemID = 0;itemID < varHeader.ItemNum;++itemID)
					{
					cellRec = tmplNetIO->Cell (itemID);
					gridRec = gridIO->GridItem (layerRec,tmplNetIO->Center (cellRec));
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
					{ perror ("Error: Writing record header in: RGlibRGIS2DataStream ()\n");	ret = DBFault; break; }
				if ((DBInt) fwrite (data,itemSize,varHeader.ItemNum,outFile) != varHeader.ItemNum)
					{ perror ("Error: Writing data in: RGlibRGIS2DataStream ()\n");				ret = DBFault; break; }
				}
			}
		delete tmplNetIO;
		}

	free (data);
	delete gridIO;
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
			DBVPointIO *pntIO = new DBVPointIO (tmplData);

			itemTable->AddField (idField);
			itemTable->AddField (dateField);

			while (MFVarReadHeader (&header,inFile))
				{
				if (header.ItemNum != pntIO->ItemNum ())
					{ fprintf (stderr,"Error: Datastream inconsistency %d %d!\n",header.ItemNum,pntIO->ItemNum ()); return (DBFault); }
				if (data == (void *) NULL)
					{
					itemSize = MFVarItemSize (header.DataType);
					if ((data = (void *) realloc (data,header.ItemNum * itemSize)) == (void *) NULL)
						{ perror ("Memory allocation error in: RGlibDataStream2RGIS ()"); return (DBFault); }
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
					{ fprintf (stderr,"Error: Data stream read in: RGlibDataStream2rgis ()"); return (DBFault); }
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
			delete pntIO;
			} break;
		case DBTypeGridContinuous:
		case DBTypeGridDiscrete:
			{
			DBGridIO *gridIO = new DBGridIO (outData);

			while (MFVarReadHeader (&header,inFile))
				{
				if (header.ItemNum != gridIO->RowNum () * gridIO->ColNum ())
					{ fprintf (stderr,"Error: Datastream inconsistency!\n"); return (DBFault); }
				if (layerID == 0)
					{
					itemSize = MFVarItemSize (header.DataType);
					if ((data = (void *) realloc (data,header.ItemNum * itemSize)) == (void *) NULL)
						{ perror ("Memory allocation error in: RGlibDataStream2RGIS ()"); return (DBFault); }
					record = gridIO->Layer (layerID);
					gridIO->RenameLayer (header.Date);
					}
				else record = gridIO->AddLayer (header.Date);

				switch (header.DataType)
					{
					case MFByte:
					case MFShort:
					case MFInt:		gridIO->MissingValue (record,header.Missing.Int); break;
					case MFFloat:
					case MFDouble:	gridIO->MissingValue (record,header.Missing.Float); break;
					}
				if ((int) fread (data,itemSize,header.ItemNum,inFile) != header.ItemNum)
					{ fprintf (stderr,"Error: Data stream read in: RGlibDataStream2rgis ()"); return (DBFault); }

				for (pos.Row = 0;pos.Row < gridIO->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < gridIO->ColNum ();++pos.Col)
						{
						switch (header.DataType)
							{
							case MFByte:	val = (DBFloat) (((char *)   data) [pos.Row * gridIO->ColNum () + pos.Col]); break;
							case MFShort:	val = (DBFloat) (((short *)  data) [pos.Row * gridIO->ColNum () + pos.Col]); break;
							case MFInt:		val = (DBFloat) (((int *)    data) [pos.Row * gridIO->ColNum () + pos.Col]); break;
							case MFFloat:	val = (DBFloat) (((float *)  data) [pos.Row * gridIO->ColNum () + pos.Col]); break;
							case MFDouble:	val = (DBFloat) (((double *) data) [pos.Row * gridIO->ColNum () + pos.Col]); break;
							}
						gridIO->Value (record,pos,val);
						}
				layerID++;
				}
			gridIO->RecalcStats ();
			} break;
		case DBTypeNetwork:
			{
			DBInt cellID;
			DBGridIO *gridIO   = new DBGridIO (outData);
			DBNetworkIO *netIO = new DBNetworkIO (tmplData);

			while (MFVarReadHeader (&header,inFile))
				{
				if (header.ItemNum != netIO->CellNum ())
					{ fprintf (stderr,"Error: Datastream inconsistency!\n"); return (DBFault); }
				if (layerID == 0)
					{
					itemSize = MFVarItemSize (header.DataType);
					if ((data = (void *) realloc (data,header.ItemNum * itemSize)) == (void *) NULL)
						{ perror ("Memory allocation error in: RGlibDataStream2RGIS ()"); return (DBFault); }
					record = gridIO->Layer (layerID);
					gridIO->RenameLayer (header.Date);
					}
				else record = gridIO->AddLayer (header.Date);
				if ((int) fread (data,itemSize,header.ItemNum,inFile) != header.ItemNum)
					{ fprintf (stderr,"Error: Data stream read in: RGlibDataStream2rgis ()"); delete netIO; return (DBFault); }

				for (pos.Row = 0;pos.Row < gridIO->RowNum ();++pos.Row)
					for (pos.Col = 0;pos.Col < gridIO->ColNum ();++pos.Col)
						gridIO->Value (record,pos,gridIO->MissingValue ());

				for (cellID = 0;cellID < netIO->CellNum ();++cellID)
					{
					pos = netIO->CellPosition (netIO->Cell (cellID));

					switch (header.DataType)
						{
						case MFByte:	val = (DBFloat) (((char *)   data) [cellID]); break;
						case MFShort:	val = (DBFloat) (((short *)  data) [cellID]); break;
						case MFInt:		val = (DBFloat) (((int *)    data) [cellID]); break;
						case MFFloat:	val = (DBFloat) (((float *)  data) [cellID]); break;
						case MFDouble:	val = (DBFloat) (((double *) data) [cellID]); break;
						}
					gridIO->Value (record,pos,val);
					}
				layerID++;
				}
			gridIO->RecalcStats ();
			} break;
		}
	return (DBSuccess);
	}
