/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBInfoDB.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <ctype.h>
#include <DB.H>

#define INFO_DATE_TYPE			1      /* INFO item types */
#define INFO_CHARACTER_TYPE	2
#define INFO_INTEGER_TYPE		3
#define INFO_NUMBER_TYPE		4
#define INFO_BINARY_TYPE		5
#define INFO_FLOATING_TYPE		6
#define INFO_RECNO_TYPE			99

class ARCDirRecord
	{
	private:
		char	InternalName [32];	/* user file name */
		char	ExternalName [8];		/* INFO file name */
		short	NumberItems;			/* number of items */
		short	RecordLength;			/* logical record length */
		char	Level0Password[4];	/* level 0 password */
		char	Level1Password[4];	/* level 1 password */
		char	Level2Password[4];	/* level 2 password */
		char	Level3Password[4];	/* level 3 password */
		short	PrintWidth;				/* report output width */
		short	EmptyFlag;				/* directory record empty flag */
		int	NumberRecords;			/* number of records */
		short	Filler1;					/* not used */
		short	FileType;				/* directory entry type */
		short	Filler2;					/* ? */
		char	Filler3[4];				/* ? */
		char	ExternalFlag[2];		/* external data file flag */
		char	Filler4[300];			/* ? and not used */
		void Swap ()
			{
			DBByteOrderSwapHalfWord (&NumberItems);
			DBByteOrderSwapHalfWord (&RecordLength);
			DBByteOrderSwapHalfWord (&PrintWidth);
			DBByteOrderSwapHalfWord (&EmptyFlag);
			DBByteOrderSwapWord (&NumberRecords);
			DBByteOrderSwapHalfWord (&Filler1);
			DBByteOrderSwapHalfWord (&FileType);
			DBByteOrderSwapHalfWord (&Filler2);
			}
	public:
		int Read (FILE *file,DBInt swap)
			{
			int i;
			if (fread (this,sizeof (ARCDirRecord),1,file) != 1) return (DBFault);
			for (i = sizeof (InternalName) - 1;i >= 0;--i)
				InternalName [i] = InternalName [i] == ' ' ? '\0' : tolower (InternalName [i]);
			for (i = sizeof (ExternalName) - 1;i >= 0;--i)
				ExternalName [i] = ExternalName [i] == ' ' ? '\0' : tolower (ExternalName [i]);
			if (swap) Swap ();
			return (DBSuccess);
			}
		char *GetInternalName()	{ return (InternalName); }
		char *GetExternalName()	{ return (ExternalName); }
		int 	GetEmptyFlag ()	{ return (EmptyFlag); }
		int	GetFieldNum ()		{ return (NumberItems); }
	};

class ARCNitRecord
	{
	private:
		char	ItemNameSTR[16];		/* item name */
		short	ItemWidthVAR;			/* internal item width */
		short	KeyTypeVAR;				/* key type */
		short	ItemPositionVAR;		/* item position */
		short	ProtectionLevelVAR;	/* item protection level */
		short	Filler1VAR;				/* ? */
		short	OutputWidthVAR;		/* item output width */
		short	NumberDecimalsVAR;	/* number of decimal places */
		short	ItemTypeVAR;			/* item type */
		short	ReadOnlyFlagVAR;		/* read only flag */
		short	KeyLevelVAR;			/* key level */
		short	IndexNumberVAR;		/* item index number */
		short	OccuranceCountVAR;	/* number of occurrences */
		short	Filler2VAR;				/* ? */
		char	AlternateNameSTR[16];/* item alternate name */
		char	Filler3STR[16];		/* ? */
		short	ConcordLevelVAR;		/* concordance level */
		short	AlternateRuleVAR;		/* item naming rule */
		char	Filler4STR[32];		/* ? */
		short	ConcordCaseVAR;		/* concordance case */
		short	ConcordNumbersVAR;	/* concordance numbers */
		short	ItemNumberVAR;			/* logical item number */
		char	Filler5STR[28];		/* not used */
		void Swap ()
			{
			DBByteOrderSwapHalfWord (&ItemWidthVAR);
			DBByteOrderSwapHalfWord (&KeyTypeVAR);
			DBByteOrderSwapHalfWord (&ItemPositionVAR);
			DBByteOrderSwapHalfWord (&ProtectionLevelVAR);
			DBByteOrderSwapHalfWord (&Filler1VAR);
			DBByteOrderSwapHalfWord (&OutputWidthVAR);
			DBByteOrderSwapHalfWord (&NumberDecimalsVAR);
			DBByteOrderSwapHalfWord (&ItemTypeVAR);
			DBByteOrderSwapHalfWord (&ReadOnlyFlagVAR);
			DBByteOrderSwapHalfWord (&KeyLevelVAR);
			DBByteOrderSwapHalfWord (&IndexNumberVAR);
			DBByteOrderSwapHalfWord (&OccuranceCountVAR);
			DBByteOrderSwapHalfWord (&Filler2VAR);
			DBByteOrderSwapHalfWord (&ConcordLevelVAR);
			DBByteOrderSwapHalfWord (&ConcordNumbersVAR);
			DBByteOrderSwapHalfWord (&ItemNumberVAR);
			}
	public:
		DBInt Read (FILE *file,DBInt swap)
			{
			if (fread (this,sizeof (ARCNitRecord),1,file) != 1) return (DBFault);
			if (swap) Swap ();
			return (DBSuccess);
			}
		DBObjTableField *MakeField (FILE *file,DBInt swap)
			{
			char name [sizeof (ItemNameSTR) + 1],format [DBStringLength];
			int i;
			
			if (Read (file,swap) == DBFault) return ((DBObjTableField *) NULL);
			strncpy (name,ItemNameSTR,sizeof (ItemNameSTR)); name [sizeof (ItemNameSTR)] = '\0';
			for (i = sizeof (ItemNameSTR);i > 0;--i) name [i] = name [i] == ' ' ? '\0' : tolower (name [i]);
			switch (ItemTypeVAR)
				{
				case INFO_CHARACTER_TYPE:
					format [0] = '%'; sprintf (format + 1,"%ds",ItemWidthVAR);
					return (new DBObjTableField (name,DBTableFieldString,format,ItemWidth () + 1));
				case INFO_INTEGER_TYPE:
					format [0] = '%'; sprintf (format + 1,"%dd",OutputWidthVAR);
					return (new	DBObjTableField (name,DBTableFieldInt,format,sizeof (DBInt)));
				case INFO_RECNO_TYPE:
					format [0] = '%'; sprintf (format + 1,"%dd",OutputWidthVAR);
					return (new	DBObjTableField (name,DBTableFieldInt,format,ItemWidth ()));
				case INFO_BINARY_TYPE:
					format [0] = '%'; sprintf (format + 1,"%dd",OutputWidthVAR);
					return (new	DBObjTableField (name,DBTableFieldInt,format,ItemWidth ()));
				case INFO_NUMBER_TYPE:
					format [0] = '%'; sprintf (format + 1,"%d.%df",OutputWidthVAR,NumberDecimalsVAR);
					return (new	DBObjTableField (name,DBTableFieldFloat,format,sizeof (DBFloat)));
				case INFO_FLOATING_TYPE:
					format [0] = '%'; sprintf (format + 1,"%d.%df",OutputWidthVAR,NumberDecimalsVAR);
					return (new	DBObjTableField (name,DBTableFieldFloat,format,ItemWidth ()));
				case INFO_DATE_TYPE:	
					return (new	DBObjTableField (name,DBTableFieldDate,DBHiddenField,sizeof (DBDate)));
				}
			return ((DBObjTableField *) NULL);
			}
		char *Name () { return (ItemNameSTR); }
		DBUnsigned ItemWidth  () { return (ItemWidthVAR); }
		DBInt ItemPosition () { return (ItemPositionVAR - 1); }
		DBInt ItemType () { return (ItemTypeVAR); }
	};

char *DBInfoFileName (const char *workSpace,const  char *infoName)

	{
	FILE *inFile;
	static char fileName [DBDataFileNameLen];
	DBInt swap = DBByteOrder () == DBByteOrderLITTLE ? true : false;
	DBInt notFound = true;
	ARCDirRecord arcDirRecord;
	
	sprintf (fileName,"%s/info/arcdr9",workSpace);
	if (access (fileName,R_OK) == DBFault) sprintf (fileName,"%s/info/arc.dir",workSpace);
	if ((inFile = fopen (fileName,"r")) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (NULL); }
	
	while (arcDirRecord.Read (inFile,swap) == DBSuccess)
		if ((strcmp (arcDirRecord.GetInternalName (),infoName) == 0) &&
			 (arcDirRecord.GetEmptyFlag () == false)) { notFound = false; break; }
	fclose (inFile);
	if (notFound) return (NULL);
	
	sprintf (fileName,"%s/info/",workSpace);
	strncpy (fileName + strlen (fileName),arcDirRecord.GetExternalName (),8);
	return (fileName);
	}

char *DBInfoFileName (const char *coverage,DBInt type)

	{
	char workSpace [DBDataFileNameLen], infoName [32];
	int i;
	
	for (i = strlen (coverage);i > 0;--i) if (coverage [i] == '/') break;
	strncpy (workSpace,coverage,i);
	workSpace [i] = '\0';
	switch (type)
		{
		case DBTypeVectorPoint:		sprintf (infoName,"%s.pat",coverage + i + 1); break;
		case DBTypeVectorLine:		sprintf (infoName,"%s.aat",coverage + i + 1); break;
		case DBTypeVectorPolygon:	sprintf (infoName,"%s.pat",coverage + i + 1); break;
		case DBTypeGridDiscrete:	sprintf (infoName,"%s.vat",coverage + i + 1); break;
		default: CMmsgPrint (CMmsgAppError, "Wrong Data Type in: %s %d",__FILE__,__LINE__); return (NULL);
		}
	return (DBInfoFileName (workSpace,infoName));
	}

DBInt DBInfoGetFields (DBObjTable *table,const char *infoName)

	{
	FILE *inFile;
	char fileName [DBDataFileNameLen];
	DBInt swap = DBByteOrder () == DBByteOrderLITTLE ? true : false;
	ARCNitRecord arcNitRecord;
	DBObjTableField *field;
	
	if (infoName == NULL) return (DBFault);
	
	sprintf (fileName,"%snit",infoName);
	if (access (fileName,R_OK) == DBFault) sprintf (fileName,"%s.nit",infoName);

	if ((inFile = fopen (fileName,"r")) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }

	while ((field = arcNitRecord.MakeField (inFile,swap)) != (DBObjTableField *) NULL)
		if (table->Field (field->Name ()) == (DBObjTableField *) NULL) table->AddField (field);
	return (DBSuccess);
	}

static void _DBInfoGetField (DBObjTableField *field,DBObjRecord *record,ARCNitRecord *arcNitRecord,void *infoBuffer,DBInt swap)

	{
	char format [DBStringLength];
	
	switch (field->Type ())
		{
		case DBTableFieldString:
			field->String (record,(char *) ((char *) infoBuffer + arcNitRecord->ItemPosition ()));
			break;
		case DBTableFieldInt:
			{
			DBInt intVAR = field->IntNoData ();
			format [0] = '%'; sprintf (format + 1,"%dd",arcNitRecord->ItemWidth ());
			switch (arcNitRecord->ItemType ())
				{
				case INFO_INTEGER_TYPE:
					sscanf ((char *) infoBuffer + arcNitRecord->ItemPosition (),format,&intVAR); break;
				case INFO_RECNO_TYPE:
				case INFO_BINARY_TYPE:
					switch (arcNitRecord->ItemWidth ())
						{
						case sizeof (DBInt):
							memcpy (&intVAR,(char *) infoBuffer + arcNitRecord->ItemPosition (),sizeof (DBInt));
							if (swap)	DBByteOrderSwapWord (&intVAR);
							break;
						case sizeof (DBShort):
							{
							DBShort shortVAR;
							memcpy (&shortVAR,(char *) infoBuffer + arcNitRecord->ItemPosition (),sizeof (DBShort));
							if (swap)	DBByteOrderSwapHalfWord (&shortVAR);
							intVAR = shortVAR;
							break;
							}
						default:
							CMmsgPrint (CMmsgAppError, "Invalid Item Width in: %s %d",__FILE__,__LINE__);	break;
						}
					break;
				default: CMmsgPrint (CMmsgAppError, "Invalid Info Field in: %s %d",__FILE__,__LINE__);	break;
				}
			field->Int (record,intVAR);
			break;
			}
		case DBTableFieldFloat:
			{
			DBFloat floatVAR = field->FloatNoData ();
			format [0] = '%'; sprintf (format + 1,"%df",arcNitRecord->ItemWidth ());
			switch (arcNitRecord->ItemType ())
				{
				case INFO_NUMBER_TYPE:
					sscanf ((char *) infoBuffer + arcNitRecord->ItemPosition (),format,&floatVAR);
					break;
				case INFO_FLOATING_TYPE:
					switch (arcNitRecord->ItemWidth ())
						{
						case sizeof (DBFloat):
							memcpy (&floatVAR,(char *) infoBuffer + arcNitRecord->ItemPosition (),sizeof (DBFloat));
							if (swap)	DBByteOrderSwapLongWord (&floatVAR);
							break;
						case sizeof (DBFloat4):
							{
							DBFloat4 float4VAR;
							memcpy (&float4VAR,(char *) infoBuffer + arcNitRecord->ItemPosition (),sizeof (DBFloat4));
							if (swap)	DBByteOrderSwapWord (&float4VAR);
							floatVAR = float4VAR;
							break;
							}
						default: CMmsgPrint (CMmsgAppError, "Invalid Item Width in: %s %d",__FILE__,__LINE__);	break;
						}
					break;
				default:
					CMmsgPrint (CMmsgAppError, "Invalid Info Field in: %s %d",__FILE__,__LINE__);	break;
				}
			field->Float (record,floatVAR);	
			break;
			}
		case DBTableFieldDate:
			break;
		default: break;
		}
	}

DBInt DBInfoGetTable (DBObjTable *table,const char *infoName)

	{
	FILE *inFile;
	char *buffer, name [DBStringLength > 80 ? DBStringLength : 80];
	char fileName [DBDataFileNameLen];
	DBInt swap = DBByteOrder (DBByteOrderLITTLE), external;
	DBInt fieldNum, i, j, sep;
	DBUnsigned recordLen = 0;
	ARCNitRecord *arcNitRecord = NULL;
	DBObjTableField **fields;
	DBObjRecord *record;

	sprintf (fileName,"%snit",infoName);
	if (access (fileName,R_OK) == DBFault) sprintf (fileName,"%s.nit",infoName);
	if ((inFile = fopen (fileName,"r")) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in:  %s %d",__FILE__,__LINE__); return (DBFault); }

	fieldNum = 0;
	for (arcNitRecord = (ARCNitRecord *) malloc (sizeof (ARCNitRecord));
			arcNitRecord [fieldNum].Read (inFile,swap) == DBSuccess;
			arcNitRecord = (ARCNitRecord *) realloc (arcNitRecord,(fieldNum + 1) * sizeof (ARCNitRecord)))
			{
			if (arcNitRecord == (ARCNitRecord *) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory (Re)allocation Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
			recordLen = recordLen > arcNitRecord [fieldNum].ItemPosition () + arcNitRecord [fieldNum].ItemWidth () ?
							recordLen : arcNitRecord [fieldNum].ItemPosition () + arcNitRecord [fieldNum].ItemWidth ();
			fieldNum++;
			}
	recordLen = (recordLen & 0x01) == 0 ? recordLen : ((recordLen >> 0x01) + 1) << 0x01;
	fclose (inFile);
	if ((fields = (DBObjTableField **) calloc (sizeof (DBObjTableField *),fieldNum)) == (DBObjTableField **) NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	for (i = 0;i < fieldNum;++i)
		{
		strncpy (name,arcNitRecord [i].Name (),16); name [16] = '\0';
		for (j = strlen (name);j > 0;--j) name [j] = name [j] == ' ' ? '\0' : tolower (name [j]);
		fields [i] = table->Field (name);
		}
	
	sprintf (fileName,"%sdat",infoName);
	if (access (fileName,R_OK) == DBFault) sprintf (fileName,"%s.dat",infoName);
	external = DBFileSize (fileName) == 80 ? true : false;
	
	if ((inFile = fopen (fileName,"r")) == (FILE *) NULL)
		{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }

	if (external)
		{
		if (fread (name,80,1,inFile) != 1)
			{ CMmsgPrint (CMmsgSysError, "File Reading Error in: %s %d",__FILE__,__LINE__); fclose (inFile); return (DBFault); }
		fclose (inFile);
		for (i = sizeof (name) - 1;(i >= 0) && (name [i] == ' ');--i) name [i] = '\0';
		sep = 0; for (i = strlen (name); (i >= 0) && (sep < 2);--i) if (name [i] == '/') ++sep;
		sep = 0;	for (j = strlen (fileName);(j >= 0) && (sep < 2);--j) if (fileName [j] == '/') ++sep;
		strcpy (fileName + j + 2,name + i + 2);
		
		if ((inFile = fopen (fileName,"r")) == (FILE *) NULL)
			{ CMmsgPrint (CMmsgSysError, "File Opening Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
		}
	
	if ((buffer = (char *) calloc (recordLen,sizeof (char))) == NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__); fclose (inFile); return (DBFault); }

	for (record = table->First ();fread (buffer,recordLen,1,inFile) == 1;record = table->Next ())
		{
		if (record == (DBObjRecord *) NULL)
			sprintf (name,"Table Record: %d",table->ItemNum () + 1); table->Add (name); record = table->Item ();

		if (record == (DBObjRecord *) NULL)
			{ CMmsgPrint (CMmsgAppError, "Null Record in: %s %d",__FILE__,__LINE__); return (DBFault); }

		for (i = 0;i < fieldNum;++i)
			if (fields [i] != NULL) _DBInfoGetField (fields [i],record,arcNitRecord + i,buffer,swap);
		}
	free (arcNitRecord);
	free (fields);
	free (buffer);
	return (DBSuccess);						
	}
