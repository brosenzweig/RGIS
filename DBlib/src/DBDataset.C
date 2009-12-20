/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

DBDataset.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>

int DBObjMetaEntry::Read (FILE *file,DBInt swap)

	{
	if (DBObject::Read (file,swap) != DBSuccess) return (DBFault);
	if (fread ((char *) this + sizeof (DBObject),sizeof (DBObjMetaEntry) - sizeof (DBObject),1,file) != 1)
		{ perror ("File Reading Error in: DBObjMetaEntry::Read ()"); return (DBFault); }
	if (swap) Swap ();
	return (DBSuccess);
	}
	
int DBObjMetaEntry::Write (FILE *file)

	{
	if (DBObject::Write (file) != DBSuccess) return (DBFault);
	if (fwrite ((char *) this + sizeof (DBObject),sizeof (DBObjMetaEntry) - sizeof (DBObject),1,file) != 1)
		{ perror ("File Reading Error in: DBObjMetaEntry::Write ()"); return (DBFault); }
	return (DBSuccess);
	}
	
int DBObjSubject::Read (FILE *file,DBInt swap)

	{
	if (DBObject::Read (file,swap) != DBSuccess) return (DBFault);
	if (fread ((char *) this + sizeof (DBObject),sizeof (DBObjSubject) - sizeof (DBObject),1,file) != 1)
		{ perror ("File Reading Error in: DBObjSubject::Read ()"); return (DBFault); }
	if (swap) Swap ();
	return (DBSuccess);
	}
	
int DBObjSubject::Write (FILE *file)

	{
	if (DBObject::Write (file) != DBSuccess) return (DBFault);
	if (fwrite ((char *) this + sizeof (DBObject),sizeof (DBObjSubject) - sizeof (DBObject),1,file) != 1)
		{ perror ("File Reading Error in: DBObjSubject::Write ()"); return (DBFault); }
	return (DBSuccess);
	}
	
int DBObjGeoDomain::Read (FILE *file,DBInt swap)

	{
	if (DBObject::Read (file,swap) != DBSuccess) return (DBFault);
	if (fread ((char *) this + sizeof (DBObject),sizeof (DBObjGeoDomain) - sizeof (DBObject),1,file) != 1)
		{ perror ("File Reading Error in: DBObjGeoDomain::Read ()"); return (DBFault); }
	if (swap) Swap ();
	return (DBSuccess);
	}
	
int DBObjGeoDomain::Write (FILE *file)

	{
	if (DBObject::Write (file) != DBSuccess) return (DBFault);
	if (fwrite ((char *) this + sizeof (DBObject),sizeof (DBObjGeoDomain) - sizeof (DBObject),1,file) != 1)
		{ perror ("File Reading Error in: DBObjGeoDomain::Write ()"); return (DBFault); }
	return (DBSuccess);
	}
	
int DBObjProject::Read (FILE *file,DBInt swap)

	{
	if (DBObject::Read (file,swap) != DBSuccess) return (DBFault);
	if (fread ((char *) this + sizeof (DBObject),sizeof (DBObjProject) - sizeof (DBObject),1,file) != 1)
		{ perror ("File Reading Error in: DBObjProject::Read ()"); return (DBFault); }
	return (DBSuccess);
	}
	
int DBObjProject::Write (FILE *file)

	{
	if (DBObject::Write (file) != DBSuccess) return (DBFault);
	if (fwrite ((char *) this + sizeof (DBObject),sizeof (DBObjProject) - sizeof (DBObject),1,file) != 1)
		{ perror ("File Reading Error in: DBObjProject::Write ()"); return (DBFault); }
	return (DBSuccess);
	}

DBDataset::DBDataset (char *modulName,char *metaDB)
	{
	DBInt i;
	extern char *_DBPredefinedSubjetList [];
	extern char *_DBPredefinedGeoDomainList [];

	MetaLIST 		= new DBObjectLIST<DBObjMetaEntry>	("Meta Data List");
	SubjectLIST		= new DBObjectLIST<DBObjSubject>		("Subject List");
	GeoDomainLIST	= new DBObjectLIST<DBObjGeoDomain>	("Geo Domain List");
	DataLIST			= new DBObjectLIST<DBObjData>			("Data List");

	strncpy (MetaFileNameSTR,metaDB,sizeof (MetaFileNameSTR) - 1);

	if (Read (MetaFileNameSTR) == DBFault)
		{
		for (i = 0;_DBPredefinedSubjetList [i] != (char *) NULL;++i)
			SubjectLIST->Add (new DBObjSubject (_DBPredefinedSubjetList [i]));
		for (i = 0;_DBPredefinedGeoDomainList [i] != (char *) NULL;++i)
			GeoDomainLIST->Add (new DBObjGeoDomain (_DBPredefinedGeoDomainList [i]));
		}
	}

DBDataset::~DBDataset ()
	{
	DBObjData *data;

	for (data = DataLIST->First ();data != (DBObjData *) NULL;data = DataLIST->Next ())
		if (strlen (data->FileName ()) == 0) RemoveData (data);
	Write (MetaFileNameSTR);
	delete DataLIST;
	delete MetaLIST;
	delete SubjectLIST;
	delete GeoDomainLIST;
	}

int DBDataset::Read (const char *fileName)

	{
	FILE *file;
	DBInt byteOrder, id, swap;

	if ((file = fopen (fileName,"r")) == NULL) return (DBFault);
	
	if (fread (&byteOrder,sizeof (DBInt),1,file) != 1)
		{
		perror ("File Reading Error in: DBDataset::Read ()");
		fclose (file);
		return (DBFault);
		}
	
	swap = byteOrder != 1 ? true : false;
	if (SubjectLIST->Read (file,swap) == DBFault) return (DBFault);
	for (id = 0;id < SubjectLIST->ItemNum ();++id)
		if (SubjectLIST->ReadItem (file,id,swap) == DBFault) return (DBFault);
	if (GeoDomainLIST->Read (file,swap) == DBFault) return (DBFault);
	for (id = 0;id < GeoDomainLIST->ItemNum ();++id)
		if (GeoDomainLIST->ReadItem (file,id,swap) == DBFault) return (DBFault);
	if (MetaLIST->Read (file,swap) == DBFault) return (DBFault);
	for (id = 0;id < MetaLIST->ItemNum ();++id)
		if (MetaLIST->ReadItem (file,id,swap) == DBFault) return (DBFault);
	fclose (file);	
	return (DBSuccess);
	}

int DBDataset::Write (const char *fileName)

	{
	FILE *file;
	DBInt byteOrder = 1,id;

	if ((file = fopen (fileName,"w")) == NULL)
		{ perror ("File Opening Error in: DBDataset::SaveChanges ()"); return (DBFault); }
	if (fwrite (&byteOrder,sizeof (DBInt),1,file) != 1)
		{
		perror ("File Writiing Error in: DBDataset::Write (const char *fileName)");
		fclose (file);
		return (DBFault);
		}
	if (SubjectLIST->Write (file) == DBFault) return (DBFault);
	for (id = 0;id < SubjectLIST->ItemNum ();++id)
		if (SubjectLIST->WriteItem (file,id) == DBFault) return (DBFault);
	if (GeoDomainLIST->Write (file) == DBFault) return (DBFault);
	for (id = 0;id < GeoDomainLIST->ItemNum ();++id)
		if (GeoDomainLIST->WriteItem (file,id) == DBFault) return (DBFault);
	if (MetaLIST->Write (file) == DBFault) return (DBFault);
	for (id = 0;id < MetaLIST->ItemNum ();++id)
		if (MetaLIST->WriteItem (file,id) == DBFault) return (DBFault);
	fclose (file);	
	return (DBSuccess);
	}

void DBDataset::Data 	(DBObjData *data)

	{
	DBObjSubject *subjOBJ;
	DBObjGeoDomain *domainOBJ;
	
	if (DataLIST->Item (data->Name ()) == NULL)
		{ DataLIST->Add (data); if(data->Type () != DBTypeTable) ExtentVAR.Expand (data->Extent ()); }
	if (MetaLIST->Item (data->Name ()) == NULL)
		{
		MetaLIST->Add (new DBObjMetaEntry (data));
		if (strlen (data->Document (DBDocSubject)) > 0)
			{
			if ((subjOBJ = SubjectLIST->Item (data->Document (DBDocSubject))) == NULL)
				SubjectLIST->Add (subjOBJ = new DBObjSubject (data->Document (DBDocSubject)));
			subjOBJ->OccuranceInc ();
			}
		if (strlen (data->Document (DBDocGeoDomain)) > 0)
			{
			if ((domainOBJ = GeoDomainLIST->Item (data->Document (DBDocGeoDomain))) == NULL)
				GeoDomainLIST->Add (domainOBJ = new DBObjGeoDomain (data->Document (DBDocGeoDomain)));
			domainOBJ->OccuranceInc ();
			}
		}
	}

void DBDataset::RemoveData 	(DBObjData *remData)

	{
	DBRegion initRegion;
	DBObjData *data;
	DBObjMetaEntry *metaEntry;

	DataLIST->Remove (remData);

	ExtentVAR = initRegion;
	for (data = DataLIST->First ();data != (DBObjData *) NULL;data = DataLIST->Next ())
		{
		ExtentVAR.Expand (data->Extent ());
		if (data->LinkedData () == remData) data->LinkedData ((DBObjData *) NULL);
		}
	metaEntry = MetaLIST->Item (remData->Name ());
	if (strlen (remData->FileName ()) == 0) MetaLIST->Delete (metaEntry);
	}

void DBDataset::RemoveMetaEntry (DBObjMetaEntry *metaEntry)

	{
	DBObjSubject 	*subjOBJ   = SubjectLIST->Item (metaEntry->Subject ());
	DBObjGeoDomain *domainOBJ = GeoDomainLIST->Item (metaEntry->GeoDomain ());
	MetaLIST->Remove (metaEntry);
	subjOBJ->OccuranceDec ();
	domainOBJ->OccuranceDec ();
	}
