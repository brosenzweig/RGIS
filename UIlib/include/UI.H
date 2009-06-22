#include <DB.H>
#include <DBio.H>
#include <Xm/Xm.h>

#define UICharSetNormal ((char *) "normal")
#define UICharSetItalic ((char *) "italic")
#define UICharSetBold 	((char *) "bold")

#define UIDataSymbolLineMode 0x01
#define UIDataSymbolFillMode 0x02

#define UIMENU_NORULE	 ((DBUnsigned) 0x00)

#define UIMENU_GROUP01   ((DBUnsigned)(0x01 << 0x00))
#define UIMENU_GROUP02   ((DBUnsigned)(0x01 << 0x01))
#define UIMENU_GROUP03   ((DBUnsigned)(0x01 << 0x02))
#define UIMENU_GROUP04   ((DBUnsigned)(0x01 << 0x03))
#define UIMENU_GROUP05   ((DBUnsigned)(0x01 << 0x04))
#define UIMENU_GROUP06   ((DBUnsigned)(0x01 << 0x05))
#define UIMENU_GROUP07   ((DBUnsigned)(0x01 << 0x06))
#define UIMENU_GROUP08   ((DBUnsigned)(0x01 << 0x07))
#define UIMENU_GROUP09	 ((DBUnsigned)(0x01 << 0x08))
#define UIMENU_GROUP10   ((DBUnsigned)(0x01 << 0x09))
#define UIMENU_GROUP11   ((DBUnsigned)(0x01 << 0x0a))
#define UIMENU_GROUP12   ((DBUnsigned)(0x01 << 0x0b))
#define UIMENU_GROUP13   ((DBUnsigned)(0x01 << 0x0c))
#define UIMENU_GROUP14   ((DBUnsigned)(0x01 << 0x0d))
#define UIMENU_GROUP15   ((DBUnsigned)(0x01 << 0x0e))
#define UIMENU_GROUP16   ((DBUnsigned)(0x01 << 0x0f))
#define UIMENU_GROUP17   ((DBUnsigned)(0x01 << 0x10))
#define UIMENU_GROUP18   ((DBUnsigned)(0x01 << 0x11))
#define UIMENU_GROUP19   ((DBUnsigned)(0x01 << 0x12))
#define UIMENU_GROUP20   ((DBUnsigned)(0x01 << 0x13))
#define UIMENU_GROUP21   ((DBUnsigned)(0x01 << 0x14))
#define UIMENU_GROUP22   ((DBUnsigned)(0x01 << 0x15))
#define UIMENU_GROUP23   ((DBUnsigned)(0x01 << 0x16))
#define UIMENU_GROUP24   ((DBUnsigned)(0x01 << 0x17))
#define UIMENU_GROUP25   ((DBUnsigned)(0x01 << 0x18))
#define UIMENU_GROUP26   ((DBUnsigned)(0x01 << 0x19))
#define UIMENU_GROUP27   ((DBUnsigned)(0x01 << 0x1a))
#define UIMENU_GROUP28   ((DBUnsigned)(0x01 << 0x1b))
#define UIMENU_GROUP29   ((DBUnsigned)(0x01 << 0x1c))
#define UIMENU_GROUP30   ((DBUnsigned)(0x01 << 0x1d))
#define UIMENU_GROUP31   ((DBUnsigned)(0x01 << 0x1e))
#define UIMENU_GROUP32   ((DBUnsigned)(0x01 << 0x1f))

#define UIColorStandard		0x00
#define UIColorGreyScale	0x01
#define UIColorBlueScale	0x02
#define UIColorRedScale		0x03
#define UIColorElevation	0x04
#define UIColorTrueColor	0x05

#define UIStandardWhite		0x00
#define UIStandardBlack		0x01
#define UIStandardRed		0x02
#define UIStandardGreen		0x03
#define UIStandardBlue		0x04
#define UIStandardCyan		0x05
#define UIStandardMagenta	0x06
#define UIStandardYellow	0x07
#define UIStandardGrey		0x14

#define UIGraphTimeSeries	0x01
#define UIGraphXYPlot		0x02

#define UIFault -1

#define UISymbolMarker		0x01
#define UISymbolLine			0x02
#define UISymbolShade		0x03
#define UISymbolStick		0x04

#define UIMenuNotImplemented (void (*) (Widget,void *,XmAnyCallbackStruct *)) NULL

void	 UIAuxSetBooleanTrueCBK (Widget,int *,XmAnyCallbackStruct *);
void	 UIAuxSetBooleanFalseCBK (Widget,int *,XmAnyCallbackStruct *);
void	 UIAuxSetIntegerCBK (Widget,DBInt,XmAnyCallbackStruct *);
void	 UIAuxSetToggleCBK (Widget,DBInt *,XmToggleButtonCallbackStruct *);
void   UIAuxObjectSelectCBK (Widget,Widget,XmAnyCallbackStruct *);
void   UIAuxFileSelectCBK (Widget,Widget,XmAnyCallbackStruct *);
void   UIAuxDirSelectCBK (Widget,Widget,XmAnyCallbackStruct *);
void   UIAuxSetDefaultButtonEH (Widget,void *,XEvent *,Boolean *);
void   UIAuxSetLabelString (Widget,char *,char *);
void   UIAuxSetLabelString (Widget,char *);
char  *UIAuxGetLabelString (Widget,char *);
char  *UIAuxGetLabelString (Widget);

class UIColorSet
	{
	public:
		Pixel *Colors;
		DBInt ColorNum;
	};
typedef void (*UIMenuCBK) (Widget,void *,XmAnyCallbackStruct *);
class UIMenuItem
	{
	private:
		char *Label;
		void *Action;
		char *HelpFile;
		Widget Button;
	public:
		DBInt Type;
		DBUnsigned VisibleVAR;
		DBUnsigned SensitiveVAR;
		UIMenuItem ();
		UIMenuItem (DBUnsigned,DBUnsigned);
		UIMenuItem (char *,DBUnsigned,DBUnsigned,UIMenuItem []);
		UIMenuItem (char *,DBUnsigned,DBUnsigned,UIMenuCBK,char *);
		void CreateButton (Widget,UIMenuItem *,void *);
		void Sensitive (unsigned, int);
		void Visible (unsigned, int);
		void CallMenu (Widget widget,void *data,XmAnyCallbackStruct *callData) const;
		void CallHelp () const;
	};

#define UIDataXYSeries		0x01
#define UIDataTimeSeries	0x02

class UIDataSeries : public DBObject
	{
	private:
		DBInt TypeVAR;
		DBInt SymbolVAR;
		DBInt ObsNumVAR;
		void *DataPTR;
		DBRegion		XYRangeVAR;
		DBObsRange	ObsRangeVAR;
		DBFloat NoDataVAR;
		void Initialize () { XYRangeVAR.Initialize (); ObsRangeVAR.Initialize (); }
	public:
		UIDataSeries () : DBObject ("UIDataSeries",sizeof (UIDataSeries))
			{
			TypeVAR = UIDataXYSeries;
			SymbolVAR = DBFault;
			ObsNumVAR = 0;
			DataPTR = (void *) NULL;
			Initialize ();
			}
		UIDataSeries (char *,DBCoordinate *,DBInt,DBInt);
		UIDataSeries (char *,DBObservation *,DBInt,DBInt);
		UIDataSeries (char *,DBObjTable *,DBObjTableField *,DBObjTableField *,DBObjTableField *,char *,DBInt);
		~UIDataSeries () { free (DataPTR); }
		DBInt Type () const { return (TypeVAR); }
		DBInt Symbol () const { return (SymbolVAR);  }
		DBInt ObsNum () const { return (ObsNumVAR); }
		DBFloat NoData () { return (NoDataVAR); }
		DBCoordinate XYData  (DBInt i) const { return (((DBCoordinate *) DataPTR) [i]); }
		DBObservation	ObsData (DBInt i) const { return (((DBObservation *) DataPTR) [i]); }
		DBRegion		XYRange () { return (XYRangeVAR); }
		DBObsRange	ObsRange () { return (ObsRangeVAR); }
		DBFloat MinX () { return (XYRangeVAR.LowerLeft.X); }
		DBFloat MaxX () { return (XYRangeVAR.UpperRight.Y); }
		DBFloat MinY ()
			{
			if (TypeVAR == UIDataTimeSeries)
					return (ObsRangeVAR.MinValue ());
			else	return (XYRangeVAR.LowerLeft.Y);
			}
		DBFloat MaxY ()
			{
			if (TypeVAR == UIDataTimeSeries)
					return (ObsRangeVAR.MaxValue ());
			else	return (XYRangeVAR.UpperRight.Y);
			}
	};

class UIXYGraph : public DBObjectLIST<UIDataSeries>
	{
	private:
		Widget MainWGT;
		Widget LabelWGT;
		Widget ScaleWGT;
		Widget ScrollBarWGT;
		Widget DrawingAreaWGT;
		Widget YAxisWGT;
		DBInt ViewPercentVAR;
		DBInt ViewOffsetVAR;
		DBRegion		XYRangeVAR;
		DBObsRange	ObsRangeVAR;
		DBCoordinate 	XYStepVAR;
		DBObservation	ObsStepVAR;
		char XFormatSTR [DBStringLength];
		char YFormatSTR [DBStringLength];
		GC Gc;
		void Initialize (Widget,char *,Arg wargs [],int argNum,XtCallbackProc,XtPointer);
		void Initialize (Widget parent,char *title,Arg wargs [],int argNum)
			{ Initialize (parent,title,wargs, argNum,(XtCallbackProc) NULL,(XtPointer) NULL); }
	public:
		UIXYGraph ();
		UIXYGraph (Widget parent,char *title,Arg wargs [],int argNum) : DBObjectLIST<UIDataSeries> ("UIXYGraph",sizeof (UIXYGraph))
			{ Initialize (parent,title, wargs, argNum); }
		UIXYGraph (Widget parent,char *title,Arg wargs [],int argNum,XtCallbackProc callbackProc,XtPointer callData) : DBObjectLIST<UIDataSeries> ("UIXYGraph",sizeof (UIXYGraph))
			{ Initialize (parent,title, wargs, argNum, callbackProc,callData); }
		~UIXYGraph () { XtReleaseGC (DrawingAreaWGT,Gc); XtDestroyWidget (MainWGT); }
		Widget MainWidget () const { return (MainWGT); }
		void ChangeTitle (char *title)	{ UIAuxSetLabelString (LabelWGT,title,UICharSetBold); }
		void SetViewPercent (DBInt);
		DBInt ViewPercent ()	const	{ return (ViewPercentVAR); }
		void SetViewOffset  (DBInt);
		DBInt ViewOffset ()	const	{ return (ViewOffsetVAR); }
		void Draw (int clear) { DrawYAxis (clear); DrawSeries (clear); };
		void DrawSeries (int);
		void DrawYAxis (int);
		void Add (UIDataSeries *);
		void Remove (UIDataSeries *);
		void SetRange ();
	};

#define UIXYGraphShellStr ((char *) "UIXYGraphShellString")

class UIXYGraphShell : public DBObject
	{
	private:
		Widget DShell;
		Widget TSDataFieldWGT;
		Widget XAxisButtonWGT, 	XAxisFieldWGT;
		Widget YAxisButtonWGT, 	YAxisFieldWGT;
		Widget SeriesListWGT;
		Widget AddButtonWGT, 	RemoveButtonWGT;
		UIXYGraph *Graph;
		DBObjData *DataPTR, *RelDataPTR;
		DBObjRecord *RecordPTR;
		DBObjTableField *RelFLD;
		DBObjTableField *JoinFLD;
		DBObjTableField *XAxisFLD;
		DBObjectLIST<DBObjTableField> *YFieldsPTR;
	public:
		UIXYGraphShell () : DBObject (UIXYGraphShellStr,sizeof (UIXYGraphShell))
			{ Graph = (UIXYGraph *) NULL;RelFLD = JoinFLD = (DBObjTableField *) NULL; }
		UIXYGraphShell (DBObjData *);
		~UIXYGraphShell ();
		void Configure (DBObjRecord *);
		void Configure () { Configure (RecordPTR); }
		void SetData ();
		void SetSeries ();
		void SetList (int);
		void AddSeries ();
		void RemoveSeries ();
		void Update (DBObjRecord *);
		void Raise ();
	};

class UI2DView : public DBObject
	{
	private:
		DBInt InputModeVAR;
		DBRegion RequiredEXT, ViewEXT, ActiveEXT;
		XImage	*Image;
		Region	DrawRegion, FullRegion;
		Cursor	ActiveCursor, RegenCursor;
		Pixel		Background;
		Widget	DShell;
		Widget	MainFormW;
		Widget	HorScrollBarW;
		Widget	VerScrollBarW;
		Widget	ScaleW;
		Widget	DrawingAreaW;
		Widget	ZoomToggle;
		Widget	PaneToggle;
		Widget	UserToggle;
		Widget	MeshOptionW;
		double	MultipX, MultipY, TransX, TransY;
		double	MultipU, MultipV, TransU, TransV;
		void SetExtent (DBRegion);
		void DrawPoints		(DBObjData *,GC);
		void DrawPointObject(DBVPointIO *,DBObjRecord *,GC);
		void DrawLines		(DBObjData *,GC);
		void DrawLineObject(DBVLineIO *,DBObjRecord *,GC);
		void DrawPolygons	(DBObjData *,GC);
		void DrawPolyObject(DBVPolyIO *,DBObjRecord *,GC);
		void DrawVectorAnnotations (DBObjData *,GC);
		void DrawVectorAnnotation  (DBVectorIO *,DBObjRecord *,GC);
		void DrawNetwork	(DBObjData *,GC);
		void DrawNetworkCell(DBNetworkIO *,DBObjRecord *,DBUnsigned,DBUnsigned,GC);
		void DrawNetworkCellBox(DBNetworkIO *,DBObjRecord *,GC);
		void DrawGrid		(DBObjData *,GC);
		DBInt MaxVertexNumVAR;
		XPoint *PointARR;
	public:
		UI2DView ();
		~UI2DView ();
		int Map2Window (DBFloat,DBFloat,short *,short *);
		int Window2Map (short,short,DBFloat *,DBFloat *);
		DBRegion Extent () const { return (ViewEXT); }
		int DrawMesh		();
		void Size ();
		void Set (DBRegion);
		void Set () { Set (RequiredEXT); }
		void SetActiveExtent (DBRegion);
		void SetActiveExtent () { SetActiveExtent (ViewEXT); }
		void SetUserToggleMode (DBInt);
		int Width ()  { return (Image->width); }
		int Height () { return (Image->height); }
		int InputMode () { return (InputModeVAR); }
		void InputMode (int);
		void Draw ();
		void Draw (DBRegion);
		void Clear ();
		Widget DrawingArea () const { return (DrawingAreaW); }
	};

typedef void (*UI2DViewUserFunction) (DBObjData *,UI2DView *,XEvent *);
void UI2DViewChangeUserFunction (UI2DViewUserFunction);
UI2DViewUserFunction UI2DViewGetUserFunction ();
void UI2DViewChangeUserData (DBObjData *);
DBObjData *UI2DViewGetUserData ();

UI2DView *UI2DViewFirst();
UI2DView *UI2DViewNext ();
void UI2DViewRedrawAll ();
void UI2DViewRedrawAll (DBRegion);
DBObjectLIST<UI2DView> *UI2DViewList ();

class UITableField : public DBObject

	{
	private:
		Widget FieldButtonWGT;
		Dimension WidthVAR;
		DBObjTableField *FieldPTR;
	public:
		UITableField () : DBObject ("Noname",sizeof (UITableField))
			{ FieldButtonWGT = (Widget) NULL; FieldPTR = (DBObjTableField *) NULL; WidthVAR = 0; }
		UITableField (Widget, DBObjTableField *);
		~UITableField ();
		void Width (DBObjRecord *);
		DBInt Width () { return (WidthVAR); }
		DBObjTableField *Field () { return (FieldPTR); }
	};

class UITableRecord : public DBObject

	{
	private:
		Widget RecordLeftMenuBarWGT;
		Widget RecordRightMenuBarWGT;
		DBObjectLIST<UITableField> *FieldsPTR;
		DBObjRecord *RecordPTR;
	public:
		UITableRecord () : DBObject ("Noname",sizeof (UITableRecord))
			{
			RecordLeftMenuBarWGT = RecordRightMenuBarWGT = (Widget) NULL;
			FieldsPTR = (DBObjectLIST<UITableField> *) NULL;
			RecordPTR = (DBObjRecord *) NULL;
			perror ("Invalid Constructor call: UITableRecord ()");
			}
		UITableRecord (Widget,Widget, DBInt, DBInt, DBObjectLIST<UITableField> *,DBInt);
		~UITableRecord ()
			{ XtDestroyWidget (RecordLeftMenuBarWGT); XtDestroyWidget (RecordRightMenuBarWGT);}
		void AddField (UITableField *,DBInt);
		DBObjRecord *Record () { return (RecordPTR); }
		void Draw (UITableField *);
		void Draw ();
		void Draw (DBObjRecord *record) { RecordPTR = record; Draw (); };
	};

class UITable : public DBObjectLIST<UITableRecord>

	{
	private:
		Dimension RecordIDWidthVAR, NameWidthVAR;
		Widget DShellWGT;
		Dimension BaseHeightVAR, CellHeightVAR;
		int StartRowVAR, ResizingVAR;
		Widget RecordRowColWGT;
		Widget LeftRowColWGT;
		Widget RightRowColWGT;
		Widget VerScrollBarWGT;
		Widget FieldMenuBarWGT;
		Widget RecordIDButtonWGT, NameButtonWGT;
		DBObjectLIST<UITableField> *FieldsPTR;
		DBObjData  *DataPTR;
		DBObjTable *TablePTR;
	public:
		UITable () : DBObjectLIST<UITableRecord> ("",sizeof (UITable))
			{ DShellWGT = (Widget) NULL; FieldsPTR = (DBObjectLIST<UITableField> *) NULL; }
		UITable (DBObjData *,DBObjTable *);
		~UITable () { XtDestroyWidget (DShellWGT); }
		DBInt	RecordIDWidth () { return (RecordIDWidthVAR); }
		void	NameWidth (DBObjRecord *,XmFontList);
		DBInt	NameWidth () { return (NameWidthVAR); }
		void Draw (DBInt,DBObjTableField *, DBObjRecord *);
		void Draw (DBInt row,DBObjRecord *record)
			{	UITableRecord *uiRecord = Item (row);	uiRecord->Draw (record); }
		void Draw (DBObjTableField *,DBObjRecord *);
		void Draw (DBObjRecord *);
		void Draw (DBInt);
		void Draw () { Draw (StartRowVAR); }
		void Resize (int);
		int StartRow ()	const { return (StartRowVAR); }
		int RecordNum ()	const { return (TablePTR->ItemNum ()); }
		DBObjTable *Table () const { return (TablePTR); }
		DBObjData *Data () const { return (DataPTR); }
		void AddField (DBObjTableField *);
		void AddRecord (DBObjRecord *);
		void Raise () const;
	};
char *UITableName (const DBObjData *,const DBObjTable *);
class UIAttribView : public DBObject

	{
	private:
		Widget DShellWGT, ItemNameWGT, NamesRowCol, FieldsRowCol;
		DBObjData  *DataPTR;
		DBObjTable *ItemTable;
		void DrawField (char *,char *);
	public:
		UIAttribView (DBObjData *);
		~UIAttribView () { XtDestroyWidget (DShellWGT); }
		void Draw (DBObjRecord *);
		void NewField (char *,const char *);
		DBObjData *Data () const { return (DataPTR); }
		void Raise () const;
	};

#define UIAttribViewSTR "UIAttribView"

Widget UIInitialize (char *,char *,char *,UIMenuItem [],void *,int *argc, char *argv [],int,int,bool,bool);
Widget UITopLevel (void);
XtAppContext UIApplicationContext ();
XmFontList UILargeFontList ();
XmFontList UINormalFontList ();
XmFontList UISmallFontList ();
XmFontList UIItalicFontList ();
XmFontList UIFixedFontList ();

void UIDataset (char *,char *);
DBDataset *UIDataset (void);
char *UIProjectFile ();
void UIProjectFile (const char *);
char *UIDatasetSubject ();
char *UIDatasetGeoDomain ();
DBObjMetaEntry *UIDatasetMetaData (char *,char *,DBInt);
DBObjMetaEntry *UIDatasetMetaData ();
DBObjData *UIDatasetOpenData (char *,char *,DBInt);
DBObjData *UIDatasetOpenData ();
DBObjData *UIDatasetSelectData (DBObjData *);
DBObjData *UIDatasetSelectData ();

void   UIMenuSensitive (unsigned,int);
void   UIMenuVisible (unsigned,int);

Widget UIDialogForm (char *,int);
Widget UIDialogForm (char *);
Widget UIDialogFormGetMainForm (Widget);
Widget UIDialogFormGetOkButton (Widget);
Widget UIDialogFormGetCancelButton (Widget);
void   UIDialogFormPopup (Widget);
void 	 UIDialogFormPopdown (Widget);
void   UIDialogFormTitle (Widget,char *);

int	 UIColorSet (int);
char  *UIColorSetName (int);
Pixel	 UIColor (int,int);
char  *UIStandardColorName (int);
int	 UIColorNum (int);

void	 UILoopAddProc		(void (*) (void *),void *);
void	 UILoopRemoveProc	(void (*) (void *),void *);
int	 UILoop ();
void	 UILoopStopCBK (Widget,void *,XmAnyCallbackStruct *);

void	 UIMessage (char *);
int	 UIYesOrNo (char *);

typedef int (*UISelectCondFunc) (const DBObject *);
Widget UISelectionCreate (char *);
char  *UISelection (Widget,char *,int,int);
char  *UISelectObject (Widget,DBObjectLIST<DBObject> *);
char  *UISelectObject (Widget,DBObjectLIST<DBObject> *,UISelectCondFunc);
Widget UIFileSelectionCreate (char *,char *,char *,int);
char  *UIFileSelection (Widget,int);
char  *UIGetString (char *,int);
void   UIPauseDisplay (bool);
void   UIPauseDialogOpen (char *,DBInt);
void   UIPauseDialogOpen (char *);
int    UIPause (int);
void   UIPauseDialogClose ();

#define	UINumberInputModeLinear			0x00
#define	UINumberInputModeLogarithmic	0x01

Widget 	UINumberInputCreate (Widget, char *, Arg *,int argNum,DBFloat,DBFloat,DBFloat, DBInt, char *);
void	 	UINumberInputResize (Widget,Dimension,Dimension, Dimension);
Widget 	UINumberInputGetScale (Widget);
void		UINumberInputSetValue (Widget,DBFloat);
DBFloat	UINumberInputGetValue (Widget);

DBInt  UIDataHeaderForm (DBObjData *data);
DBInt  UIDataPropertiesForm (DBObjData *data);
DBInt  UIRelateData (DBObjData *,DBObjRecord *);
int    UISymbolEdit (DBObjTable *,int);
DBObjTableField *UITableFieldEdit (DBObjTableField *);
DBObjTableField *UITableFieldEdit ();
DBInt UITableRedefineFields (DBObjTable *);

Pixmap UIPattern (int,int,int);
Pixmap UIMarker (int,int,int);

void UIEquationBuilder ();

