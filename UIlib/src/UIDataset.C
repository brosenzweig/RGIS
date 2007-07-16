/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

UIDataset.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <UI.H>

char *UIDatasetSubject ()

	{
	static Widget select = NULL;
	DBDataset *dataset;
	
	if (select == NULL) select = UISelectionCreate ("Subjects");
	dataset = UIDataset ();
	
	return (UISelectObject (select,(DBObjectLIST<DBObject> *) dataset->SubjectList ()));
	}

char *UIDatasetGeoDomain ()

	{
	static Widget select = NULL;
	DBDataset *dataset;
	
	if (select == NULL) select = UISelectionCreate ("Geographic Domains");
	dataset = UIDataset ();
	
	return (UISelectObject (select,(DBObjectLIST<DBObject> *) dataset->GeoDomainList ()));
	}

static char *_UIDatasetGeoDomain  = (char *) NULL;
static char *_UIDatasetSubject = (char *) NULL;
static int   _UIDatasetType = DBFault;

static DBInt _UIDatasetMetaCond (const DBObject *obj)

	{
	DBObjMetaEntry *metaObj = (DBObjMetaEntry *) obj;
	DBDataset *dataset = UIDataset ();
	if (dataset->Data (metaObj->Name ()) != (DBObjData *) NULL) return (false);
	if ((_UIDatasetSubject == (char *) NULL) && (_UIDatasetGeoDomain == (char *) NULL) && (_UIDatasetType == DBFault)) return (true);
	if ((_UIDatasetSubject == (char *) NULL) && (_UIDatasetGeoDomain != (char *) NULL) && (_UIDatasetType == DBFault) &&
		(strcmp (_UIDatasetGeoDomain,metaObj->GeoDomain ()) == 0)) return (true);
	if ((_UIDatasetSubject != (char *) NULL) && (_UIDatasetGeoDomain == (char *) NULL) && (_UIDatasetType == DBFault) && 
		(strcmp (_UIDatasetSubject,metaObj->Subject ()) == 0))	return (true);
	if ((_UIDatasetSubject != (char *) NULL) && (_UIDatasetGeoDomain != (char *) NULL) && (_UIDatasetType == DBFault) &&
		(strcmp (_UIDatasetGeoDomain,metaObj->GeoDomain ()) == 0) &&
		(strcmp (_UIDatasetSubject,metaObj->Subject ()) == 0))	return (true);
	if ((_UIDatasetSubject == (char *) NULL) && (_UIDatasetGeoDomain != (char *) NULL) && (_UIDatasetType == metaObj->Type ()) &&
		(strcmp (_UIDatasetGeoDomain,metaObj->GeoDomain ()) == 0)) return (true);
	if ((_UIDatasetSubject != (char *) NULL) && (_UIDatasetGeoDomain == (char *) NULL) && (_UIDatasetType == metaObj->Type ()) && 
		(strcmp (_UIDatasetSubject,metaObj->Subject ()) == 0))	return (true);
	if ((_UIDatasetSubject != (char *) NULL) && (_UIDatasetGeoDomain != (char *) NULL) && (_UIDatasetType == metaObj->Type ()) &&
		(strcmp (_UIDatasetGeoDomain,metaObj->GeoDomain ()) == 0) &&
		(strcmp (_UIDatasetSubject,metaObj->Subject ()) == 0))	return (true);
	return (false);
	}

DBObjMetaEntry *UIDatasetMetaData (char *subject,char *domain,DBInt type)

	{
	static Widget select = NULL;
	char *dataName;
	DBDataset *dataset;

	_UIDatasetGeoDomain = domain;
	_UIDatasetSubject = subject;
	_UIDatasetType = type;
 
	if (select == NULL) select = UISelectionCreate ("Meta Database");
	dataset = UIDataset ();
	
	dataName = UISelectObject (select,(DBObjectLIST<DBObject> *) dataset->MetaList (),_UIDatasetMetaCond);
	if (dataName != (char *) NULL)	return (dataset->Meta (dataName));
	return ((DBObjMetaEntry *) NULL);
	}

DBObjMetaEntry *UIDatasetMetaData ()

	{ return (UIDatasetMetaData ((char *) NULL,(char *) NULL,DBFault)); }

static DBObjData *_UIExcludeData = (DBObjData *) NULL;

static DBInt _UIDatasetDataCond (const DBObject *obj)

	{ if (_UIExcludeData != (DBObjData *) obj) return (true); else	return (false); }

DBObjData *UIDatasetSelectData (DBObjData *data)

	{
	char *dataName;
	static Widget select = NULL;
	DBDataset *dataset = UIDataset ();
	
	if (select == NULL) select = UISelectionCreate ("Dataset");

	_UIExcludeData = data;
	dataName = UISelectObject (select,(DBObjectLIST<DBObject> *) dataset->DataList (),_UIDatasetDataCond);
	_UIExcludeData = (DBObjData *) NULL;

	if (dataName != NULL)	return (dataset->Data (dataName));
	return ((DBObjData *) NULL);
	}

DBObjData *UIDatasetSelectData ()

	{ DBDataset *dataset = UIDataset (); return (UIDatasetSelectData (dataset->Data ())); }

DBObjData *UIDatasetOpenData (char *subject,char *geoDomain,DBInt type)

	{
	DBObjMetaEntry *metaEntry;
	
	if ((metaEntry = UIDatasetMetaData (subject,geoDomain,type)) != (DBObjMetaEntry *) NULL)
		{
		DBObjData *data = new DBObjData ();
		if (data->Read (metaEntry->FileName ()) == DBSuccess) return (data);
		}
	return ((DBObjData *) NULL);
	}

DBObjData *UIDatasetOpenData ()

	{ return (UIDatasetOpenData ((char *) NULL,(char *) NULL,DBFault)); }
