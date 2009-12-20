/******************************************************************************

GHAAS RiverGIS V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

rgis.H

balazs.fekete@unh.edu

*******************************************************************************/

#include <UI.H>
#include <RG.H>

#define RGISDataGroup					(UIMENU_GROUP01)
#define RGISGeoDataGroup 				(UIMENU_GROUP02 | RGISDataGroup)

#define RGISSymbolGroup					(UIMENU_GROUP03)
#define RGISVectorGroup					(UIMENU_GROUP04 | RGISSymbolGroup | RGISGeoDataGroup) 
#define RGISPointGroup					(UIMENU_GROUP05 | RGISVectorGroup)
#define RGISLineGroup 					(UIMENU_GROUP06 | RGISVectorGroup)
#define RGISPolyGroup 					(UIMENU_GROUP07 | RGISVectorGroup)
#define RGISGridGroup 					(UIMENU_GROUP08 | RGISGeoDataGroup)
#define RGISDiscreteGroup 				(UIMENU_GROUP09 | RGISGridGroup | RGISSymbolGroup)
#define RGISContinuousGroup 			(UIMENU_GROUP10 | RGISGridGroup)
#define RGISNetworkGroup 				(UIMENU_GROUP11 | RGISGeoDataGroup | RGISSymbolGroup)
#define RGISTableGroup 					(UIMENU_GROUP12 | RGISDataGroup)

#define RGISLinkedDataGroup			(UIMENU_GROUP13)
#define RGISLinkedGeoDataGroup		(UIMENU_GROUP14 | RGISLinkedDataGroup)
#define RGISLinkedVectorGroup			(UIMENU_GROUP15 | RGISLinkedGeoDataGroup) 
#define RGISLinkedPointGroup			(UIMENU_GROUP16 | RGISLinkedVectorGroup)
#define RGISLinkedLineGroup			(UIMENU_GROUP17 | RGISLinkedVectorGroup)
#define RGISLinkedPolyGroup			(UIMENU_GROUP18 | RGISLinkedVectorGroup)
#define RGISLinkedGridGroup			(UIMENU_GROUP19 | RGISLinkedGeoDataGroup) 
#define RGISLinkedDiscreteGroup		(UIMENU_GROUP20 | RGISLinkedGridGroup)
#define RGISLinkedContinuousGroup	(UIMENU_GROUP21 | RGISLinkedGridGroup)
#define RGISLinkedNetworkGroup		(UIMENU_GROUP22 | RGISLinkedGeoDataGroup)
#define RGISLinkedTableGroup			(UIMENU_GROUP23 | RGISLinkedDataGroup)

class RGISControlMode
	{
		Widget ButtonWDT;
		char *NameSTR;
		DBInt DataTypeVAL;
		DBUnsigned ModeVAL;
		DBInt EditFLAG;
	public:
		RGISControlMode (char *name,DBInt dataType,DBUnsigned mode,DBInt edit)
			{
			ButtonWDT = (Widget) NULL;
			NameSTR = name;
			DataTypeVAL = dataType;
			ModeVAL = mode;
			EditFLAG = edit;
			}
		void ShowButton (Widget, DBObjData *,DBUnsigned);
		int CreateButton (Widget,XtCallbackProc);
	};

class RGISPanel
	{
	private:
		Widget FormWGT;
	public:
		RGISPanel (Widget, Widget);
		Widget Form () { return (FormWGT); };
	};

class RGISVecPanel : public RGISPanel
	{
	private:
		Widget RowColumnWGT;
		Widget ActiveToggleWGT;
		void Initialize ();
	public:
		RGISVecPanel (Widget, Widget);
		Widget Load (DBObjData *);
	};

class RGISGrdPanel : public RGISPanel
	{
	private:
		Widget LayerListWGT;
		Widget DynamicToggleWGT;
		Widget AnnotateToggleWGT;
		void Initialize ();
	public:
		RGISGrdPanel (Widget, Widget);
		Widget Load (DBObjData *);
	};

class RGISNetPanel	: public RGISPanel
	{
	private:
		Widget ColorMenuWGT;
		Widget FlowDirToggleWGT;
		Widget ActiveToggleWGT;
	public:
		RGISNetPanel (Widget, Widget);
		Widget Load (DBObjData *);
	};

class RGISTblPanel : public RGISPanel

	{
	private:
	public:
		RGISTblPanel (Widget, Widget);
		Widget Load (DBObjData *);
	};

class RGISGrpPanel : public RGISPanel

	{
	private:
		Widget GroupListWGT;
		Widget SaveButtonWGT;
		Widget LoadButtonWGT;
		Widget DeleteButtonWGT;
		DBObjectLIST<DBObjTableField> *GroupLIST;
	public:
		RGISGrpPanel (Widget, Widget);
		Widget Load (DBObjData *);
		void Add (DBObjTableField *);
		void Remove (DBObjTableField *);
		void Groups (DBObjectLIST<DBObjTableField> *groups) { GroupLIST = groups; }
		void Current (DBObjTableField *group) { GroupLIST->Item (group->RowID (),True); }
		DBObjTableField *Current () const { return (GroupLIST->Item ()); }
	};

class RGISWorkspace
	{
	private:
		DBInt DataTypeVAR, LinkedTypeVAR;
		XmString DataNameString;
		Widget DataList;
		Widget CurrentDataLabel;
		Widget VersionLabel;
		Widget UserModeMenu;
		Widget SelectModeMenu;
		Widget GeoDomainLabel;
		Widget DataTypeLabel;
		Widget SubjectLabel;
		Widget LinkedDataLabel;
		Widget LinkedTypeLabel;
		Widget DisplayToggle;
		Widget SmartSortToggle;
		Widget Pages [2];
		
		RGISVecPanel	*VecPNL;
		RGISGrdPanel	*GrdPNL;
		RGISNetPanel	*NetPNL;
		RGISTblPanel	*TblPNL;
		RGISGrpPanel	*GrpPNL;
		DBInt UserModeNum, SelectModeNum;
		RGISControlMode *UserModeARR, *SelectModeARR;
		char SubjectSTR [DBStringLength];
		char DomainSTR  [DBStringLength];
	public:
		RGISWorkspace ()	{ DataTypeVAR = DBFault; LinkedTypeVAR = DBFault; }
		~RGISWorkspace ();
		void Initialize (Widget);
		void CurrentData (DBObjData *);
		void CurrentDataName ();
		void ControlPage (DBInt);
		char *Subject () { return (SubjectSTR); }
		char *Domain ()  { return (DomainSTR); }
	};

DBInt _RGISARCInfoImport (DBObjData *);
void RGISAuxFieldSelectCBK (Widget,Widget,XmAnyCallbackStruct *);
DBInt RGISFieldCompare (DBObjTable *,char *,char *,char *,DBInt);
DBInt RGISFieldCalculate (DBObjTable *,char *,char *,char *,DBInt);

