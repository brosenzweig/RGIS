/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

DBObjReadWrit.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>

int DBVarString::Read (FILE *file,int swap)

	{
	if (fread (&LengthVAR,sizeof (LengthVAR),1,file) != 1)
		{ perror ("File Reading Error in: DBVarString::Read ()"); return (DBFault); }
	if (swap) Swap ();
	if ((StringPTR = (DBAddress) ((char *) malloc (LengthVAR + 1) - (char *) NULL)) == (DBAddress) NULL)
		{ perror ("Memory Allocation Error in: DBVarString::Read ()"); return (DBFault); }
	if (fread ((char *) NULL + StringPTR,LengthVAR + 1,1,file) != 1)
		{ perror ("File Reading Error in: DBVarString::Read ()"); return (DBFault); }
	return (DBSuccess);
	}

int DBVarString::Write (FILE *file)

	{
	if (fwrite (&LengthVAR,sizeof (LengthVAR),1,file) != 1)
		{ perror ("File Writing Error in: DBVarString::Write ()"); return (DBFault); }
	if (fwrite ((char *) NULL + StringPTR,LengthVAR + 1,1,file) != 1)
		{ perror ("File Writing Error in: DBVarString::Write ()"); return (DBFault); }
	return (DBSuccess);
	}

int DBObject::Read (FILE *file,int swap)

	{
	if (NameSTR.Read (file,swap) != DBSuccess) return (DBFault);
	if (fread ((char *) this + sizeof (DBObjectHeader),sizeof (DBObject) - sizeof (DBObjectHeader) - sizeof (NameSTR),1,file) != 1)
		{ perror ("File Reading Error in: DBObject::Read ()"); return (DBFault); }
	if (swap) Swap ();
	return (DBSuccess);
	}

int DBObject::Write (FILE *file)

	{
	if (NameSTR.Write (file) != DBSuccess) return (DBFault);
	if (fwrite ((char *) this + sizeof (DBObjectHeader),sizeof (DBObject) - sizeof (DBObjectHeader) - sizeof (NameSTR),1,file) != 1)
		{ perror ("File Writing Error in: DBObject::Write ()"); return (DBFault); }
	return (DBSuccess);
	}

int DBObjRecord::Read (FILE *file,int swap)

	{
	if (DBObject::Read (file,swap) != DBSuccess) return (DBFault);

	if (fread ((char *) this + sizeof (DBObject),sizeof (DBObjRecord) - sizeof (DBObject) - sizeof (DBAddress),1,file) != 1)
		{ perror ("File Reading Error in: DBObjRecord::Read ()"); return (DBFault); }
	if (swap) Swap ();

	if ((DataPTR = (DBAddress) ((char *) malloc (LengthVAR) - (char *) NULL)) == (DBAddress) NULL)
		{ perror ("Memory Allocation Error in: DBObjRecord::Read ()"); return (DBFault); }
	if (fread ((char *) NULL + DataPTR,LengthVAR,1,file) != 1)
		{ perror ("File Reading Error in: DBObjRecord::Read ()"); return (DBFault); }
	if (swap && ItemSizeVAR > 0)
		{
		DBUnsigned i;
		void (*swapFunc) (void *);

		switch (ItemSizeVAR)
			{
			case 1:	swapFunc = (void (*) (void *)) NULL; break;
			case 2:	swapFunc = DBByteOrderSwapHalfWord; break;
			case 4:	swapFunc = DBByteOrderSwapWord; 		break;
			case 8:	swapFunc = DBByteOrderSwapLongWord; break;
			default:
				fprintf (stderr,"Invalid Item Size (Record %d %s) in: DBObjRecord::Swap ()",RowID (), Name ());
				return (DBFault);
			}
		if (swapFunc != (void (*) (void *)) NULL)
			for (i = 0;i < LengthVAR;i += ItemSizeVAR) (*swapFunc) ((char *) NULL + DataPTR + i);
		}
	return (DBSuccess);
	}

int DBObjRecord::Write (FILE *file)

	{
	if (DBObject::Write (file) != DBSuccess) return (DBFault);

	if (fwrite ((char *) this + sizeof (DBObject),sizeof (DBObjRecord) - sizeof (DBObject) - sizeof (DBAddress),1,file) != 1)
		{ perror ("File Writing Error in: DBObjRecord::Write ()"); return (DBFault); }
	if (fwrite ((char *) NULL + DataPTR,LengthVAR,1,file) != 1)
		{ perror ("File Writing Error in: DBObjRecord::Write ()"); return (DBFault); }
	return (DBSuccess);
	}

int DBObjTableField::Read (FILE *file,int swap)

	{
	if (DBObject::Read (file,swap) != DBSuccess) return (DBFault);
	if (fread ((char *) this + sizeof (DBObject),sizeof (DBObjTableField) - sizeof (DBObject),1,file) != 1)
		{ perror ("File Reading Error in: DBObjTableField::Read ()"); return (DBFault); }
	if (swap) Swap ();
	if ((strcmp (Name (),"Drain") == 0) && Required ()) Required (false);
	return (DBSuccess);
	}

int DBObjTableField::Write (FILE *file)

	{
	if (DBObject::Write (file) != DBSuccess) return (DBFault);
	if (fwrite ((char *) this + sizeof (DBObject),sizeof (DBObjTableField) - sizeof (DBObject),1,file) != 1)
		{ perror ("File Writing Error in: DBObjTableField::Write ()"); return (DBFault); }
	return (DBSuccess);
	}

int DBObjTable::Read (FILE *file,int swap)

	{
	DBInt id;
	DBDate date;
	DBCoordinate coord;
	DBRegion region;
	DBPosition pos;
	DBObjRecord *record;
	DBObjTableField *field;

	if (DBObjectLIST<DBObjRecord>::Read (file,swap) != DBSuccess) return (DBFault);
	if (FieldPTR->Read (file,swap) != DBSuccess)  return (DBFault);
	for (id = 0;id < FieldPTR->ItemNum ();++id)
		if (FieldPTR->ReadItem (file,id,swap) == DBFault) return (DBFault);

	RecordLengthVAR = 0;
	for (field = FieldPTR->First ();field != (DBObjTableField *) NULL;field = FieldPTR->Next ())
		RecordLengthVAR = RecordLengthVAR > field->StartByte () + field->Length () ?
								RecordLengthVAR : field->StartByte () + field->Length ();

	for (id = 0;id < ItemNum ();++id)
		{
		if (ReadItem (file,id,swap) == DBFault) return (DBFault);
		record = Item (id);
		if (swap == false) continue;
		for (field = FieldPTR->First ();field != (DBObjTableField *) NULL;field = FieldPTR->Next ())
			switch (field->Type ())
				{
				case DBTableFieldInt:
				case DBTableFieldFloat:
				case DBTableFieldTableRec:
				case DBTableFieldDataRec:
					switch (field->Length ())
						{
						case 2: DBByteOrderSwapHalfWord	((char *) record->Data () + field->StartByte ());	break;
						case 4: DBByteOrderSwapWord 		((char *) record->Data () + field->StartByte ());	break;
						case 8: DBByteOrderSwapLongWord	((char *) record->Data () + field->StartByte ());	break;
						}
					break;
				case DBTableFieldDate:     date   = field->Date (record);       date.Swap ();   field->Date (record,date);        break;
				case DBTableFieldCoord:    coord  = field->Coordinate (record); coord.Swap ();  field->Coordinate (record,coord); break;
				case DBTableFieldRegion:   region = field->Region (record);     region.Swap (); field->Region (record,region);    break;
				case DBTableFieldPosition: pos    = field->Position (record);   pos.Swap ();    field->Position (record,pos);     break;
				default: break;
				}
			}
	if (MethodPTR->Read (file,swap) != DBSuccess) return (DBFault);
	for (id = 0;id < MethodPTR->ItemNum ();++id) if (MethodPTR->ReadItem (file,id,swap) == DBFault) return (DBFault);
	return (DBSuccess);
	}

int DBObjTable::Write (FILE *file)

	{
	DBInt id;
	DBObjRecord *obj;
	DBObjRecord *record;
	DBObjTableField *field;

	if (DBObjectLIST<DBObjRecord>::Write (file) != DBSuccess) return (DBFault);
	if (FieldPTR->Write (file) != DBSuccess)  return (DBFault);
	for (id = 0;id < FieldPTR->ItemNum ();++id)
		if (FieldPTR->WriteItem (file,id) == DBFault) return (DBFault);

	for (id = 0;id < ItemNum ();++id)
		{
		record = Item (id);
		for (field = FieldPTR->First ();field != (DBObjTableField *) NULL;field = FieldPTR->Next ())
			switch (field->Type ())
				{
				case DBTableFieldTableRec:
				case DBTableFieldDataRec:
					if ((obj = field->Record (record)) != NULL) field->Record (record,(DBObjRecord *) obj->RowID ());
					else field->Record (record,(DBObjRecord *) DBFault);
					break;
				}
		if (WriteItem (file,id) == DBFault) return (DBFault);
		}
	if (MethodPTR->Write (file) != DBSuccess) return (DBFault);
	for (id = 0;id < MethodPTR->ItemNum ();++id) if (MethodPTR->WriteItem (file,id) == DBFault) return (DBFault);
	return (DBSuccess);
	}

int DBDataHeader::Read (FILE *file)

	{
	if (fread (this,sizeof (DBDataHeader),1,file) != 1)
		{ fprintf (stderr,"File Reading Error in: DBDataHeader::Read ()\n"); return (DBFault); }
	if (ByteOrderVAR != 1) { Swap (); return (true); }
	return (false);
	}

int DBDataHeader::Write (FILE *file)

	{
	MajorVAR = 2;
	MinorVAR = 0;
	if (fwrite (this,sizeof (DBDataHeader),1,file) != 1)
		{ fprintf (stderr,"File Writing Error in: DBDataHeader::Write ()\n"); return (DBFault); }
	return (DBSuccess);
	}

int DBObjData::Read (const char *fileName)

	{
	FILE *file;

	if ((file = fopen (fileName,"r")) == (FILE *) NULL)
		{
		perror ("File Opening Error in: DBObjData::Read ()");
		fprintf (stderr,"Filename: %s\n",fileName);
		return (DBFault);
		}
	if (Read (file) == DBFault) return (DBFault);
	FileName (fileName);
	return (DBSuccess);
	}

int DBObjData::Read (FILE *file)

	{
	int swap;

	DocsPTR->DeleteAll ();
	ArraysPTR->DeleteAll ();
	TablesPTR->DeleteAll ();
	if ((swap = DBDataHeader::Read (file)) == DBFault) return (DBFault);
	return (_Read (file,swap));
	}

int DBObjData::_Read (FILE *file,int swap)

	{
	DBInt id;
	DBObjRecord *docRec;

	if (DBObject::Read (file,swap) == DBFault)	return (DBFault);
	if (DocsPTR->Read (file,swap) == DBFault)		return (DBFault);
	for (id = 0;id < DocsPTR->ItemNum ();++id)
		{
		if (DocsPTR->ReadItem (file,id,swap) == DBFault) return (DBFault);
		docRec = DocsPTR->Item (id);
		if (((DBVarString *) docRec->Data ())->Read (file,swap) == DBFault) return (DBFault);
		}
	if (ArraysPTR->Read (file,swap) == DBFault)		return (DBFault);
	for (id = 0;id < ArraysPTR->ItemNum ();++id)
		if (ArraysPTR->ReadItem (file,id,swap) == DBFault) return (DBFault);
	TablesPTR->Read (file,swap);
	for (id = 0;id < TablesPTR->ItemNum ();++id)
		if (TablesPTR->ReadItem (file,id,swap) == DBFault) return (DBFault);
	return (BuildFields ());
	}

int DBObjData::Write (const char *fileName)

	{
	DBInt ret;
	FILE *file;

	if ((file = fopen (fileName,"w")) == (FILE *) NULL)
		{ perror ("File Opening Error in: DBObjData::Write ()"); return (DBFault); }
	ret = Write (file);
	fclose (file);
	return (ret);
	}

int DBObjData::Write (FILE *file)

	{
	RecalcExtent ();
	if (DBDataHeader::Write (file) == DBFault) return (DBFault);
	return (_Write (file));
	}

int DBObjData::_Write (FILE *file)

	{
	DBInt id, userFlags;
	DBObjRecord *docRec;

	userFlags = Flags () & DBDataFlagUserModeFlags;
	Flags (DBDataFlagUserModeFlags,DBClear);
	if (DBObject::Write (file) == DBFault)		return (DBFault);
	Flags (userFlags,DBSet);
	if (DocsPTR->Write (file) == DBFault)		return (DBFault);
	for (id = 0;id < DocsPTR->ItemNum ();++id)
		{
		if (DocsPTR->WriteItem (file,id) == DBFault) return (DBFault);
		docRec = DocsPTR->Item (id);
		if (((DBVarString *) docRec->Data ())->Write (file) == DBFault) return (DBFault);
		}
	if (ArraysPTR->Write (file) == DBFault)		return (DBFault);
	for (id = 0;id < ArraysPTR->ItemNum ();++id)
		if (ArraysPTR->WriteItem (file,id) == DBFault) return (DBFault);
	TablesPTR->Write (file);
	for (id = 0;id < TablesPTR->ItemNum ();++id)
		if (TablesPTR->WriteItem (file,id) == DBFault) return (DBFault);
	return (BuildFields ());
	}

int DBObjData::BuildFields ()

	{
	DBInt ret = DBSuccess;
	DBObjTable	*table, *refTable;
	DBObjRecord *tableRec;
	DBObjTableField *field;
	DBObjectLIST<DBObjTableField> *tableFLDs;

	for (table = TablesPTR->First ();table != (DBObjTable *) NULL;table = TablesPTR->Next ())
		{
		tableFLDs = table->Fields ();
		for (field = tableFLDs->First ();field != (DBObjTableField *) NULL;field = tableFLDs->Next ())
			switch (field->Type ())
				{
				case DBTableFieldTableRec:
					if ((refTable = TablesPTR->Item (field->RecordProp ())) == (DBObjTable *) NULL)
						{ perror ("Corrupt Dataset in: DBObjData::BuildFields ()"); ret = DBFault; continue; }
					for (tableRec = table->First ();tableRec != (DBObjRecord *) NULL;tableRec = table->Next ())
						if ((DBInt) (field->Record (tableRec) - (DBObjRecord *) NULL) != DBFault)
								field->Record (tableRec,refTable->Item  ((DBInt) ((char *) (field->Record (tableRec)) - (char *) NULL)));
						else	field->Record (tableRec,(DBObjRecord *) NULL);
					break;
				case DBTableFieldDataRec:
					for (tableRec = table->First ();tableRec != (DBObjRecord *) NULL;tableRec = table->Next ())
						if ((DBInt) (field->Record (tableRec) - (DBObjRecord *) NULL) != DBFault)
								field->Record (tableRec,ArraysPTR->Item ((DBInt) ((char *) (field->Record (tableRec)) - (char *) NULL)));
						else	field->Record (tableRec,(DBObjRecord *) NULL);
					break;
				}
		}
	return (ret);
	}
