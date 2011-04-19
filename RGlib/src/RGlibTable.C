/******************************************************************************

GHAAS RiverGIS Libarary V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGlibTable.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
#include <DBio.H>
#include <RG.H>

DBInt RGLibTableJoin (DBObjTable *itemTable, DBObjTableField *relateField,
						    DBObjTable *joinTable, DBObjTableField *joinField) 
	{	
	DBInt itemID, joinID, fieldID, fieldNum = 0;
	DBObjectLIST<DBObjTableField> *fields = joinTable->Fields ();
	DBObjRecord *itemRec, *joinRec;
	DBObjTableField *field, **newFields;
		
	if ((newFields = (DBObjTableField **) calloc (1,sizeof (DBObjTableField *))) == (DBObjTableField **) NULL)
		{ perror ("Memory Allocation Error in: RGLibTableJoin ()"); return (DBFault); }

	if ((newFields [fieldNum] = itemTable->Field (joinTable->Name ())) == (DBObjTableField *) NULL)
		{
		newFields [fieldNum] = new DBObjTableField (joinTable->Name (),DBTableFieldString,"%s",DBStringLength);
		itemTable->AddField (newFields [fieldNum]);
		}
	for (fieldID = 0;fieldID < fields->ItemNum (); fieldID++)
		{
		field = fields->Item (fieldID);
		if (DBTableFieldIsVisible (field))
			{
			fieldNum++;
			newFields = (DBObjTableField **) realloc (newFields,(fieldNum + 1) * sizeof (DBObjTableField *));
			if (newFields == (DBObjTableField **) NULL)
				{ perror ("Memory Allocation Error in: RGLibTableJoin ()"); return (DBFault); }
			if ((newFields [fieldNum] = itemTable->Field (field->Name ())) == (DBObjTableField *) NULL)
				{
				itemTable->AddField (newFields [fieldNum] = new DBObjTableField (*field));
				newFields [fieldNum]->Required (false);
				}
			}
		}

	for (itemID = 0;itemID < itemTable->ItemNum ();itemID++)
		{
		itemRec = itemTable->Item (itemID);
		DBPause (itemID * 100 / itemTable->ItemNum ());
		if ((itemRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
		for (joinID = 0;joinID < joinTable->ItemNum (); joinID++)
			{
			joinRec = joinTable->Item (joinID);
			if ((joinRec->Flags () & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
			if (DBTableFieldMatch (relateField,itemRec,joinField,joinRec))
				{
				fieldNum = 0;
				newFields [fieldNum]->String (itemRec,joinRec->Name ());
				for (fieldID = 0;fieldID < fields->ItemNum (); fieldID++)
					{
					field = fields->Item (fieldID);
					if (DBTableFieldIsVisible (field))
						{
						fieldNum++;
						if (newFields [fieldNum]->Required ()) continue;
						switch (newFields [fieldNum]->Type ())
							{
							default:
							case DBTableFieldString:
								newFields [fieldNum]->String (itemRec,field->String (joinRec));
								break;
							case DBTableFieldInt:
								newFields [fieldNum]->Int    (itemRec,field->Int    (joinRec));
								break;
							case DBTableFieldFloat:
								newFields [fieldNum]->Float  (itemRec,field->Float  (joinRec));
								break;
							case DBTableFieldDate:
								newFields [fieldNum]->Date   (itemRec,field->Date   (joinRec));
								break;
							}
						}
					}
				break;
				}
			}
		}
	free (newFields);
	return (itemID < itemTable->ItemNum () ? DBFault : DBSuccess);
	}
