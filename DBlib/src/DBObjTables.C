/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBObjTables.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>

DBInt DBTableFieldIsString	(const DBObject *obj)

	{
	DBObjTableField *tableField = (DBObjTableField *) obj;
	return (tableField->Type () == DBTableFieldString ? true : false);
	}

DBInt DBTableFieldIsInteger(const DBObject *obj)

	{
	DBObjTableField *tableField = (DBObjTableField *) obj;
	return (tableField->Type () == DBTableFieldInt ? true : false);
	}

DBInt DBTableFieldIsFloat	(const DBObject *obj)

	{
	DBObjTableField *tableField = (DBObjTableField *) obj;
	return (tableField->Type () == DBTableFieldFloat ? true : false);
	}

DBInt DBTableFieldIsCategory (const DBObject *obj)

	{
	DBObjTableField *tableField = (DBObjTableField *) obj;
	return ((tableField->Type () == DBTableFieldInt) ||
			  (tableField->Type () == DBTableFieldString) ? true : false);
	}

DBInt DBTableFieldIsSimple (const DBObject *obj)

	{
	DBObjTableField *tableField = (DBObjTableField *) obj;
	return ((tableField->Type () == DBTableFieldInt) ||
			  (tableField->Type () == DBTableFieldString) ||
			  (tableField->Type () == DBTableFieldFloat)  ? true : false);
	}

DBInt DBTableFieldIsNumeric(const DBObject *obj)

	{
	DBObjTableField *tableField = (DBObjTableField *) obj;
	return ((tableField->Type () == DBTableFieldInt) ||
			  (tableField->Type () == DBTableFieldFloat) ? true : false);
	}

DBInt DBTableFieldIsAxisVal(const DBObject *obj)

	{
	DBObjTableField *tableField = (DBObjTableField *) obj;
	return ((tableField->Type () == DBTableFieldInt) ||
			  (tableField->Type () == DBTableFieldFloat) ||
			  (tableField->Type () == DBTableFieldDate) ? true : false);
	}

DBInt DBTableFieldIsDate 	(const DBObject *obj)

	{
	DBObjTableField *tableField = (DBObjTableField *) obj;
	return (tableField->Type () == DBTableFieldDate ? true : false);
	}

DBInt DBTableFieldIsVisible (const DBObject *obj)

	{
	DBObjTableField *tableField = (DBObjTableField *) obj;
	switch (tableField->Type ())
		{
		case DBTableFieldString:
		case DBTableFieldInt:
		case DBTableFieldFloat:
		case DBTableFieldDate:	return (strlen (tableField->Format ()) > 0 ? true : false);
		default:	return (false);
		}
	}

DBInt DBTableFieldIsOptional (const DBObject *obj)

	{
	DBObjTableField *tableField = (DBObjTableField *) obj;
	switch (tableField->Type ())
		{
		case DBTableFieldString:
		case DBTableFieldInt:
		case DBTableFieldFloat:
		case DBTableFieldDate:	return (tableField->Required () ? false : true);
		default:	return (false);
		}
	}

DBInt DBTableFieldMatch (const DBObjTableField *field0, const DBObjRecord *record0, const DBObjTableField *field1, const DBObjRecord *record1)

	{
	char *string0, *string1;

	if ((field0 != (DBObjTableField *) NULL) && (field1 != (DBObjTableField *) NULL))
		switch (field0->Type ())
			{
			case DBTableFieldString:
				string0 = field0->String (record0);
				string1 = field1->String (record1);

				while (strlen (string0) > 0) { if ((string0 [0] != ' ') && (string0 [0] != '\t')) break; else string0++; }
				while (strlen (string1) > 0) { if ((string1 [0] != ' ') && (string1 [0] != '\t')) break; else string1++; }
				if (strcmp (string0,string1) == 0) return (true);
				return (false);
			case DBTableFieldInt:
				if (field0->Int (record0) == field1->Int (record1)) return (true);
			default:
				return (false);
			}
	else	if ((field0 == (DBObjTableField *) NULL) && (field1 != (DBObjTableField *) NULL))
		switch (field1->Type ())
			{
			case DBTableFieldString:
				string0 = record0->Name ();
				string1 = field1->String (record1);

				while (strlen (string0) > 0) { if ((string0 [0] != ' ') && (string0 [0] != '\t')) break; else string0++; }
				while (strlen (string1) > 0) { if ((string1 [0] != ' ') && (string1 [0] != '\t')) break; else string1++; }
				if (strcmp (string0,string1) == 0) return (true);
				return (false);
			case DBTableFieldInt:
				if (record0->RowID () + 1 == field1->Int (record1))  return (true);
			default:
				return (false);
			}
	else if ((field0 != (DBObjTableField *) NULL) && (field1 == (DBObjTableField *) NULL))
		switch (field0->Type ())
			{
			case DBTableFieldString:
				string0 = record0->Name ();
				string1 = record0->Name ();

				while (strlen (string0) > 0) { if ((string0 [0] != ' ') && (string0 [0] != '\t')) break; else string0++; }
				while (strlen (string1) > 0) { if ((string1 [0] != ' ') && (string1 [0] != '\t')) break; else string1++; }
				if (strcmp (string0,string1) == 0) return (true);
				return (false);
			case DBTableFieldInt:
				if (field0->Int (record0) == record1->RowID () + 1) return (true);
			default:
				return (false);
			}
	if (record0->RowID () == record1->RowID ()) return (true);
	return (false);
	}

void DBObjTableField::Swap ()

	{
	DBByteOrderSwapHalfWord (&StartByteVAR);
	DBByteOrderSwapHalfWord (&LengthVAR);
	switch (TypeVAR)
		{
		case DBTableFieldInt:
		switch (LengthVAR)
			{
			case 2: ShortPROP.Swap ();		break;
			case 4: IntPROP.Swap ();		break;
			}
			break;
		case DBTableFieldFloat:
			switch (LengthVAR)
				{
				case 4: Float4PROP.Swap ();	break;
				case 8: FloatPROP.Swap (); 	break;
				}
			break;
		case DBTableFieldDate: 		DatePROP.Swap ();  break;
		case DBTableFieldTableRec:
		case DBTableFieldDataRec:	DBByteOrderSwapWord (&RecordPROP);	break;
		}
	}

void DBObjTableField::Initialize (DBInt type,const char *format, DBUnsigned length,DBInt required)

	{
	TypeVAR = type;
	StartByteVAR = DBFault;
	LengthVAR = length;
	switch (type)
		{
		case DBTableFieldString:
			sprintf (FormatSTR + 1,"%ds",length);
			FormatSTR [0] = '%';
			break;
		case DBTableFieldInt:
			switch (length)
				{
				case sizeof (DBByte):	BytePROP.NoData	(DBDefaultMissingByteVal);		break;
				case sizeof (DBShort):	ShortPROP.NoData	(DBDefaultMissingIntVal);		break;
				case sizeof (DBInt):		IntPROP.NoData		(DBDefaultMissingIntVal);		break;
				}
			strncpy (FormatSTR,format,DBStringLength);
			break;
		case DBTableFieldFloat:
			switch (length)
				{
				case sizeof (DBFloat4):	Float4PROP.NoData (DBDefaultMissingFloatVal);	break;
				case sizeof (DBFloat):	FloatPROP.NoData  (DBDefaultMissingFloatVal);	break;
				}
			strncpy (FormatSTR,format,DBStringLength);
			break;
		case DBTableFieldDate:
			sprintf (FormatSTR + 1,"%ds",length);
			FormatSTR [0] = '%';
			Length (sizeof (DBDate));
			break;
		}
	RequiredVAR = required;
	}

void DBObjTableField::Type (DBInt type)

	{
	if (type != TypeVAR)
		switch (type)
			{
			case DBTableFieldString:	Format ("%s");		Length (DBStringLength);		break;
			case DBTableFieldInt:
				Format ("%8d");
				Length (sizeof (DBShort));
				IntNoData (DBDefaultMissingIntVal);
				break;
			case DBTableFieldFloat:
				Format ("%8.3f");
				Length (sizeof (DBFloat4));
				FloatNoData  (DBDefaultMissingFloatVal);
				break;
			case DBTableFieldDate:
				Format ("%10s");
				Length (sizeof (DBDate));
				break;
			}
	TypeVAR = type;
	}

void DBObjTableField::Length (DBInt length)

	{
	switch (TypeVAR)
		{
		case DBTableFieldInt:
			{
			DBInt intNoData;

			intNoData = IntNoData ();
			LengthVAR = length;
			IntNoData (intNoData);
			}	break;
		case DBTableFieldFloat:
			{
			DBFloat floatNoData;

			floatNoData = FloatNoData ();
			LengthVAR = length;
			FloatNoData (floatNoData);
			}	break;
		default: LengthVAR = length;	break;
		}
	}

DBObjTableField::DBObjTableField (DBObjTableField &field) : DBObject (field)

	{
	TypeVAR = field.TypeVAR;
	UnitTypeVAR = field.UnitTypeVAR;
	UnitVAR = field.UnitVAR;
	RequiredVAR = field.RequiredVAR;

	StartByteVAR = field.StartByteVAR;
	LengthVAR = field.LengthVAR;
	strcpy (FormatSTR,field.FormatSTR);
	Flags (field.Flags ());
	switch (TypeVAR)
		{
		case DBTableFieldInt:
			switch (LengthVAR)
				{
				case sizeof (DBByte): 	BytePROP		= field.BytePROP;		break;
				case sizeof (DBShort):	ShortPROP	= field.ShortPROP;	break;
				default:						IntPROP		= field.IntPROP;		break;
				}
			break;
		case DBTableFieldFloat:
			switch (LengthVAR)
				{
				case sizeof (DBFloat4): Float4PROP	= field.Float4PROP;	break;
				default: 					FloatPROP	= field.FloatPROP; 	break;
				}
			break;
		case DBTableFieldDate: 		DatePROP = field.DatePROP;  			break;
		case DBTableFieldTableRec:
		case DBTableFieldDataRec:	RecordPROP = field.RecordPROP;		break;
		}
	}

void DBObjTableField::String (DBObjRecord *record, const char *value)

	{
	int i;

	if (record == (DBObjRecord *) NULL) return;
	strncpy ((char *) record->Data () + StartByte (),value,Length () - 1);
	((char*) record->Data ()) [StartByte () + Length () - 1] = '\0';
	for (i = Length () - 2;i >= 0;--i)
		if (((char*) record->Data ()) [StartByte () + i] != ' ') break;
		else ((char*) record->Data ()) [StartByte () + i] = '\0';
	}

char *DBObjTableField::String (const DBObjRecord *record) const

	{
	static char retString [DBStringLength];

	if (record == (DBObjRecord *) NULL) return ((char *) NULL);
	switch (Type ())
		{
		case DBTableFieldString:
			return ((char *) record->Data () + StartByte ());
		case DBTableFieldInt:
			{
			DBInt ret = Int (record);
			if (ret == IntNoData ())	return ((char *) "");
			sprintf (retString,Format (),ret);	return (retString);
			}
		case DBTableFieldFloat:
			{
			DBFloat ret = Float (record);
			if (CMmathEqualValues (ret,FloatNoData ()))	return ((char *) "");
			sprintf (retString,Format (),ret);	return (retString);
			}
		case DBTableFieldDate:
			{
			DBDate date = Date (record);
			strncpy (retString,date.Get (),sizeof (retString));
			} return (retString);
		case DBTableFieldTableRec:
		case DBTableFieldDataRec:
			{
			DBObjRecord *refRecord = Record (record);
			return (refRecord->Name ());
			}
		default:	return (NULL);
		}
	}

void DBObjTableField::Int (DBObjRecord  *record,DBInt value)

	{
	if (record == (DBObjRecord *) NULL) return;
	switch (Type ())
		{
		case DBTableFieldInt:
			switch (Length ())
				{
				case sizeof (DBInt):
					memcpy ((char *) record->Data () + StartByte (),&value, Length ());
					break;
				case sizeof (DBShort):
					{
					DBShort shortVAR = (DBShort) value;
					memcpy ((char *) record->Data () + StartByte (),&shortVAR, Length ());
					} break;
				case sizeof (DBByte):
					{
					DBByte byteVAR = (DBByte) value;
					*((DBByte *) record->Data () + StartByte ()) = byteVAR;
					} break;
				default:
					CMmsgPrint (CMmsgAppError, "Invalid Data Length [%s %d] in: %s %d",Name (),Length (),__FILE__,__LINE__);
					break;
				} break;
		case DBTableFieldFloat:
			Float (record,(DBFloat) value); break;
		default:
			CMmsgPrint (CMmsgAppError, "Invalid Data Type [%s] in: %s %d",Name (),__FILE__,__LINE__);
			break;
		}
	}

DBInt DBObjTableField::Int (const DBObjRecord *record) const

	{
	if (record == (DBObjRecord *) NULL) return (IntPROP.NoData ());
	switch (Type ())
		{
		case DBTableFieldString:
				DBInt intVAR;
				if (sscanf (String (record),"%d",&intVAR) == 1) return (intVAR);
				else return (DBDefaultMissingIntVal);
		case DBTableFieldInt:
			switch (Length ())
				{
				case sizeof (DBInt):
					{
					DBInt		intVAR;
					memcpy (&intVAR,	(char *) record->Data () + StartByte (),	Length ());
					return (intVAR);
					}
				case sizeof (DBShort):
					{
					DBShort	shortVAR;
					memcpy (&shortVAR,(char *) record->Data () + StartByte (),	Length ());
					return ((DBInt) shortVAR);
					}
				case sizeof (DBByte):
					{
					DBByte	byteVAR;
					byteVAR = *((DBByte *) record->Data () + StartByte ());
					return ((DBInt) byteVAR);
					}
				} break;
		case DBTableFieldFloat:	return ((DBInt) Float (record));
		}
	CMmsgPrint (CMmsgAppError, "Bogus Data Field [%s] in: %s %d",Name (),__FILE__,__LINE__);
	return (IntPROP.NoData ());
	}

void DBObjTableField::Float (DBObjRecord *record,DBFloat value)

	{
	if (record == (DBObjRecord *) NULL) return;
	switch (Type ())
		{
		case DBTableFieldInt:
			Int (record,(DBInt) value);
			break;
		case DBTableFieldFloat:
			switch (Length ())
				{
				case sizeof (DBFloat):
					memcpy ((char *) record->Data () + StartByte (),&value,		Length ());
					break;
				case sizeof (DBFloat4):
					{
					DBFloat4 float4VAR = (DBFloat4) value;
					memcpy ((char *) record->Data () + StartByte (),&float4VAR,	Length ());
					} break;
				default:
					CMmsgPrint (CMmsgAppError, "Invalid Data length [%s %d] in: %s %d",Name (),Length (),__FILE__,__LINE__);
					break;
				}
			break;
		default:
			CMmsgPrint (CMmsgAppError, "Invalid Data Type [%s] in: %s %d",Name (),__FILE__,__LINE__);
			break;
		}
	}

DBFloat DBObjTableField::Float (const DBObjRecord *record) const

	{
	if (record == (DBObjRecord *) NULL) return (FloatPROP.NoData ());
	switch (Type ())
		{
		case DBTableFieldInt:	return ((DBFloat) Int (record));
		case DBTableFieldFloat:
			switch (Length ())
				{
				case sizeof (DBFloat):
					{
					DBFloat  floatVAR;
					memcpy (&floatVAR,  (char *) record->Data () + StartByte (),	Length ());
					return (floatVAR);
					}
				case sizeof (DBFloat4):
					{
					DBFloat4 float4VAR;
					memcpy (&float4VAR, (char *) record->Data () + StartByte (),	Length ());
					return ((DBFloat) float4VAR);
					}
				default:
					CMmsgPrint (CMmsgAppError, "[%s] Invalid field length in: %s %d",Name (),__FILE__,__LINE__);
					break;
				}
		default:
			CMmsgPrint (CMmsgAppError, "[%s] Invalid field type in: %s %d",Name (),__FILE__,__LINE__);
			break;
		}
	return (FloatPROP.NoData ());
	}

void DBObjTableField::Date (DBObjRecord  *record,DBDate value)

	{ memcpy ((char *) record->Data () + StartByte (),&value,sizeof (DBDate)); }

DBDate DBObjTableField::Date (const DBObjRecord *record) const

	{
	DBDate   value;

	if (record == (DBObjRecord *) NULL) return (DatePROP.NoData ());
	switch (Type ())
		{
		case DBTableFieldString:
		case DBTableFieldInt:    value.Set (String (record)); break;
		case DBTableFieldDate:   memcpy (&value,(char *) record->Data () + StartByte (),sizeof (DBDate)); break;
		}
	return (value);
	}

void DBObjTableField::Record (DBObjRecord  *record,DBObjRecord  *value)

	{
	DBAddress address = (DBAddress) (((char *) value) - (char *) NULL);
	memcpy ((char *) record->Data () + StartByte (),&address,sizeof (DBAddress));
	}

DBObjRecord *DBObjTableField::Record (const DBObjRecord *record) const

	{
	DBAddress   value;
	memcpy (&value,(char *) record->Data () + StartByte (),sizeof (DBAddress));
	return ((DBObjRecord *) ((char *) NULL + value));
	}

void DBObjTableField::Coordinate (DBObjRecord  *record,DBCoordinate value)

	{ memcpy ((char *) record->Data () + StartByte (),&value,sizeof (DBCoordinate)); }

DBCoordinate DBObjTableField::Coordinate (const DBObjRecord *record) const

	{
	DBCoordinate   value;
	memcpy (&value,(char *) record->Data () + StartByte (),sizeof (DBCoordinate)); return (value);
	}

void DBObjTableField::Region (DBObjRecord  *record,DBRegion value)

	{ memcpy ((char *) record->Data () + StartByte (),&value,sizeof (DBRegion)); }

DBRegion DBObjTableField::Region (const DBObjRecord *record) const

	{
	DBRegion   value;
	memcpy (&value,(char *) record->Data () + StartByte (),sizeof (DBRegion)); return (value);
	}

void DBObjTableField::Position (DBObjRecord  *record,DBPosition pos)

	{
	class
		{
		private:
		public:
			DBUShort Row;
			DBUShort Col;
		} uPos;

	if (Length () == sizeof (uPos))
		{
		uPos.Row = (DBUShort) pos.Row;
		uPos.Col = (DBUShort) pos.Col;
		memcpy ((char *) record->Data () + StartByte (),&uPos,sizeof (uPos));
		}
	else
		memcpy ((char *) record->Data () + StartByte (),&pos,sizeof (DBPosition));
	}

DBPosition DBObjTableField::Position (const DBObjRecord *record) const

	{
	class
		{
		private:
		public:
			DBUShort Row;
			DBUShort Col;
		} uPos;
	DBPosition pos;

	if (Length () == sizeof (uPos))
		{
		memcpy (&uPos,(char *) record->Data () + StartByte (),sizeof (uPos));
		pos.Row = (DBInt) uPos.Row;
		pos.Col = (DBInt) uPos.Col;
		}
	else
		memcpy (&pos,(char *) record->Data () + StartByte (),sizeof (DBPosition));
	return (pos);
	}

DBInt DBObjTableField::FormatWidth () const

	{
	DBInt i, width;
	char formatString [DBStringLength];

	switch (Type ())
		{
		case DBTableFieldString:  		return (Length ());
		case DBTableFieldInt:
		case DBTableFieldFloat:
		case DBTableFieldDate:
			strcpy (formatString,Format ());
			for (i = 0;(i < (DBInt) strlen (formatString)) && formatString [i] != '.';++i);
			if (i < ((DBInt) strlen (formatString)))	formatString [i] = '\0';
			else formatString [i - 1] = '\0';
			sscanf (formatString + 1,"%d",&width);
			return (width);
		default: 	return (DBFault);
		}
	}

void DBObjTableField::FormatWidth (DBInt width)

	{
	char formatString [DBStringLength];

	switch (Type ())
		{
		case DBTableFieldInt:
			sprintf (formatString + 1,"%dd",width);
			formatString [0] = '%';
			Format (formatString);
			return;
		case DBTableFieldFloat:
			if (width - 3 < FormatDecimals ())	FormatDecimals (width - 3);
			sprintf (formatString + 1,"%d.%df",width,FormatDecimals ());
			formatString [0] = '%';
			Format (formatString);
		case DBTableFieldDate:
			sprintf (formatString + 1,"%ds",width);
			formatString [0] = '%';
			return;
		default: return;
		}
	}

DBInt DBObjTableField::FormatDecimals () const

	{
	DBInt i, decimals;
	char formatString [DBStringLength];

	if (Type () != DBTableFieldFloat) return (DBFault);
	strcpy (formatString,Format ());
	for (i = 0;(i < (DBInt) strlen (formatString)) && (formatString [i] != '.');++i);
	if (i < ((DBInt) strlen (formatString) - 2))
		{
		formatString [strlen (formatString) - 1] = '\0';
		sscanf (formatString + i + 1,"%d",&decimals);
		}
	else	decimals = 0;
	return (decimals);
	}

void DBObjTableField::FormatDecimals (DBInt decimals)

	{
	DBInt width;
	char formatString [DBStringLength];

	if (Type () != DBTableFieldFloat) return;
	width = FormatWidth ();
	sprintf (formatString + 1,"%d.%df",width,decimals);
	formatString [0] = '%';
	Format (formatString);
	}

DBObjTable::DBObjTable (DBObjTable &tableObj) : DBObjectLIST<DBObjRecord> (*((DBObjectLIST<DBObjRecord> *) &tableObj))

	{
	RecordLengthVAR = tableObj.RecordLengthVAR;
	FieldPTR  = new DBObjectLIST<DBObjTableField> (*(tableObj.FieldPTR));
	MethodPTR = new DBObjectLIST<DBObjRecord> (*(tableObj.MethodPTR));
	}

void DBObjTable::AddField (DBObjTableField *field)

	{
	DBInt recID;
	DBObjRecord *record;
	field->StartByte (RecordLengthVAR);
	FieldPTR->Add (field);
	RecordLengthVAR += field->Length ();
	for (recID = 0;recID < ItemNum ();recID++)
		{
		record = Item (recID);
		record->Realloc (RecordLengthVAR);
		switch (field->Type ())
			{
			case DBTableFieldString:	field->String (record,"");					break;
			case DBTableFieldInt:		field->Int 	(record,field->IntNoData ());	break;
			case DBTableFieldFloat:		field->Float(record,field->FloatNoData ());	break;
			case DBTableFieldDate:		field->Date (record,field->DateNoData ());	break;
			}
		}
	}

void DBObjTable::RedefineField (DBObjTableField *field, DBObjTableField *newField)

	{
	DBInt recID, fieldID;
	DBObjRecord *record, *oldRecord;
	DBObjTableField *tmpField;

	newField->StartByte (field->StartByte ());
	if (field->Length () != newField->Length ())
		{
		RecordLengthVAR += (newField->Length () - field->Length ());
		for (fieldID = field->RowID () + 1;fieldID < FieldNum ();++fieldID)
			{
			tmpField = Field (fieldID);
			tmpField->StartByte (tmpField->StartByte () + newField->Length () - field->Length ());
			}
		}
	for (recID = 0;recID < ItemNum ();recID++)
		{
		record = Item (recID);
		oldRecord = new DBObjRecord (*record);
		if (field->Length () != newField->Length ())
			{
			record->Realloc (RecordLengthVAR);
			if (newField->StartByte () + newField->Length () < RecordLengthVAR)
				memcpy (((char *) record->Data ()) + newField->StartByte () + newField->Length (),
			  	        ((char *) oldRecord->Data ()) + field->StartByte () + field->Length (),
						  RecordLengthVAR - (newField->StartByte () + newField->Length ()));
			}
		switch (newField->Type ())
			{
			case DBTableFieldString:	newField->String (record,field->String	(oldRecord));	break;
			case DBTableFieldInt:		newField->Int    (record,field->Int		(oldRecord));	break;
			case DBTableFieldFloat:		newField->Float  (record,field->Float	(oldRecord));	break;
			case DBTableFieldDate:		newField->Date   (record,field->Date	(oldRecord));	break;
			case DBTableFieldCoord:		newField->Coordinate (record,field->Coordinate  (oldRecord));	break;
			case DBTableFieldRegion:	newField->Region (record,field->Region (oldRecord));	break;
			case DBTableFieldPosition:	newField->Position(record,field->Position(oldRecord));break;
			case DBTableFieldTableRec:
			case DBTableFieldDataRec:  newField->Record (record,field->Record (oldRecord));	break;
			}
		delete oldRecord;
		}
	if (strcmp (field->Name (),newField->Name ()) != 0); field->Name (newField->Name ());
	if (field->Type () != newField->Type ()) field->Type (newField->Type ());
	if (field->FormatWidth () != newField->FormatWidth ()) field->FormatWidth (newField->FormatWidth ());
	if ((field->Type () == DBTableFieldFloat) && (field->FormatDecimals () != field->FormatDecimals ()))
		field->FormatDecimals (newField->FormatDecimals ());
	if (field->Length () != newField->Length ()) field->Length (newField->Length ());
	}

void DBObjTable::DeleteField (DBObjTableField *delField)

	{
	DBInt i, recID, fieldID;
	DBObjRecord *record;
	DBObjTableField *field;
	unsigned char *data;

	for (fieldID = 0;fieldID < FieldPTR->ItemNum ();++fieldID)
		{
		field = FieldPTR->Item (fieldID);
		if (field->StartByte () > delField->StartByte ())
			field->StartByte (field->StartByte () - delField->Length ());
		}
	for (recID = 0;recID < ItemNum ();++recID)
		{
		record = Item (recID);
		data = (unsigned char * ) record->Data () + delField->StartByte ();
		for (i = 0;i < RecordLengthVAR - delField->StartByte () - delField->Length ();++i)
			data [i] = data [i + delField->Length ()];
		record->Realloc (RecordLengthVAR - delField->Length ());
		}
	FieldPTR->Remove (delField);
	RecordLengthVAR -= delField->Length ();
	delete delField;
	}

void DBObjTable::DeleteAllFields ()

	{
	DBObjRecord *record;

	FieldPTR->DeleteAll ();
	RecordLengthVAR = 0;
	for (record = First ();record != (DBObjRecord *) NULL;record = Next ())
		record->Realloc (RecordLengthVAR);
	}

static DBObjectLIST<DBObjTableField> *_DBObjTableSortFields;

static int _DBObjTableListSort (const DBObjRecord **obj0,const DBObjRecord **obj1)

	{
	DBInt ret = 0;
	DBObjTableField *field;
	DBDate date0, date1;

	for ( field = _DBObjTableSortFields->First ();
		 	field != (DBObjTableField *) NULL;
			field = _DBObjTableSortFields->Next ())
		{
		switch (field->Type ())
			{
			case DBTableFieldString:	ret = strcmp (field->String (*obj0),field->String (*obj1)); break;
			case DBTableFieldInt:		ret = field->Int  (*obj0) - field->Int(*obj1);					break;
			case DBTableFieldFloat:
				if (field->Float(*obj0) - field->Float(*obj1) > 0.0) 			ret = 1;
				else if (field->Float(*obj0) - field->Float(*obj1) < 0.0)	ret = -1;
				else	ret = 0;
				break;
			case DBTableFieldDate:
				date0 = field->Date (*obj0);	date1 = field->Date (*obj1);
				if (date0 > date1) ret = 1;
				else if (date0 < date1) ret = -1;
				else ret = 0;
				break;
			case DBTableFieldTableRec:	ret = strcmp ((*obj0)->Name (),(*obj1)->Name ());	break;
			case DBTableFieldDataRec:	ret = (*obj0)->RowID () - (*obj1)->RowID ();			break;
			}
		if ((field->Flags () & DBObjectFlagSortReversed) == DBObjectFlagSortReversed) ret *= -1;
		if (ret != 0) return (ret);
		}
	if (ret == 0) ret = (*obj0)->ListPos () - (*obj1)->ListPos ();
	return (ret);
	}

void DBObjTable::ListSort (DBObjectLIST<DBObjTableField> *fields)

	{ _DBObjTableSortFields = fields; ListSort (_DBObjTableListSort); }

