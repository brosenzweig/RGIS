/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

RGISAnSampleGrid.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <rgis.H>
#include <RG.H>

void RGISAnalyseSingleSampleGridCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	widget = widget;	workspace = workspace; callData = callData;

	if (UIYesOrNo ((char *) "You are about to sample grid. Are You sure?"))
		{
		DBDataset *dataset = UIDataset ();
		DBObjData *dbData  = dataset->Data (), *grdData = dbData->LinkedData ();
		DBObjTable *itemTable = dbData->Table (DBrNItems);

		DBInt recID, fieldID, progress = 0, maxProgress;
		DBCoordinate coord;
		DBGridIO *gridIO = new DBGridIO (grdData);
		DBVPointIO  *pntIO = dbData->Type () == DBTypeVectorPoint ? new DBVPointIO (dbData)	 : (DBVPointIO *) NULL;
		DBNetworkIO *netIO = dbData->Type () == DBTypeNetwork		 ? new DBNetworkIO (dbData) : (DBNetworkIO *) NULL;
		DBObjTableField *newField;
		DBObjRecord *record;

		UIPauseDialogOpen ((char *) "Single Layer Grid Sampling");
		switch (grdData->Type ())
			{
			case DBTypeGridContinuous:
				{
				DBFloat value;
				if ((newField = itemTable->Field ((gridIO->Layer ())->Name ())) == (DBObjTableField *) NULL)
					{
					newField = new DBObjTableField ((gridIO->Layer ())->Name (),DBTableFieldFloat,"%10.3f",sizeof (DBFloat4));
					itemTable->AddField (newField);
					}
				maxProgress = itemTable->ItemNum ();
				for (recID = 0;recID < itemTable->ItemNum ();recID++)
					{
					record = itemTable->Item (recID);
					UIPause (progress * 100 / maxProgress); progress++;
					if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
					if (pntIO != (DBVPointIO *) NULL)
							coord = pntIO->Coordinate (record);
					else	coord = netIO->Center (netIO->MouthCell (record));

					if (gridIO->Value (coord,&value))
							newField->Float (record,value);
					else	newField->Float (record,newField->FloatNoData ());
					}
				} break;
			case DBTypeGridDiscrete:
				{
				DBObjTable *grdTable = grdData->Table (DBrNItems);
				DBObjectLIST<DBObjTableField> *fields = grdTable->Fields ();
				DBObjTableField *field;
				DBObjRecord *grdRec;

				if ((newField = itemTable->Field ((gridIO->Layer ())->Name ())) == (DBObjTableField *) NULL)
					{
					newField = new DBObjTableField ((gridIO->Layer ())->Name (),DBTableFieldString,"%s",DBStringLength);
					itemTable->AddField (newField);
					}
				maxProgress = ((fields->ItemNum () + 1) * itemTable->ItemNum ());
				for (recID = 0;recID < itemTable->ItemNum ();recID++)
					{
					record = itemTable->Item (recID);
					UIPause (progress * 100 / maxProgress); progress++;
					if ((record->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
					if (pntIO != (DBVPointIO *) NULL)
						coord = pntIO->Coordinate (record);
					else	coord = netIO->Center (netIO->MouthCell (record));
					if ((grdRec = gridIO->GridItem (coord)) != (DBObjRecord *) NULL)
						newField->String (record,grdRec->Name ());
					}
				for (fieldID = 0;fieldID < fields->ItemNum ();fieldID++)
					if (DBTableFieldIsVisible (field = fields->Item (fieldID)))
						{
						if ((newField = itemTable->Field (field->Name ())) == (DBObjTableField *) NULL)
							{
							itemTable->AddField (newField = new DBObjTableField (*field));
							newField->Required (false);
							}
						if (newField->Required ()) continue;
						for (recID = 0;recID < itemTable->ItemNum ();recID++)
							{
							record = itemTable->Item (recID);
							UIPause (progress * 100 / maxProgress); progress++;
							if (pntIO != (DBVPointIO *) NULL)
									coord = pntIO->Coordinate (record);
							else	coord = netIO->Center (netIO->MouthCell (record));

							if ((grdRec = gridIO->GridItem (coord)) != (DBObjRecord *) NULL)
								switch (field->Type ())
									{
									case DBTableFieldString:	newField->String (record,field->String (grdRec));	break;
									case DBTableFieldInt:		newField->Int (record,field->Int (grdRec));			break;
									case DBTableFieldFloat:		newField->Float (record,field->Float (grdRec));		break;
									case DBTableFieldDate:		newField->Date (record,field->Date (grdRec));		break;
									}
							}
						}
				} break;
			}
		if (pntIO != (DBVPointIO *) NULL)  delete pntIO;
		if (netIO != (DBNetworkIO *) NULL) delete netIO;
		delete gridIO;
		UIPauseDialogClose ();
		}
	}

void RGISAnalyseMultiSampleGridCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBInt ret;
	DBDataset *dataset  = UIDataset ();
	DBObjData *dbData  = dataset->Data ();
	DBObjData *grdData  = dbData->LinkedData ();
	DBObjData *tblData;

	widget = widget; callData = callData;

	tblData  = new DBObjData ("",DBTypeTable);
	tblData->Document (DBDocGeoDomain,dbData->Document (DBDocGeoDomain));
	tblData->Document (DBDocSubject,grdData->Document (DBDocSubject));

	if (UIDataHeaderForm (tblData) == false) { delete tblData; return; }

	UIPauseDialogOpen ((char *) "Sampling Grid");
	ret = RGlibGridSampling (dbData,grdData,tblData);
	UIPauseDialogClose ();
	if (ret == DBSuccess) workspace->CurrentData  (tblData);
	else delete tblData;
	}
