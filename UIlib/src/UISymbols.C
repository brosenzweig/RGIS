/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

UISymbols.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/Form.h>
#include <Xm/ScrolledW.h>
#include <Xm/List.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <UI.H>

#define UISymListName				"*UISymList"
#define UISymTextFieldName			"*UISymTextField"
#define UISymLoadNamesButtonName	"*UISymLoadNamesButton"

#define UISymForegroundMenuName	"*UISymForegroundMenu"
#define UISymBackgroundMenuName	"*UISymBackgroundMenu"

#define UISymMarkerMenuName		"*UISymMarkerMenu"
#define UISymLineMenuName			"*UISymLineMenu"
#define UISymShadeMenuName			"*UISymPatternMenu"

class UISymbol : public DBObjRecord

	{
	private:
		DBObjTableField *SymbolIDFLD;
		DBObjTableField *ForegroundFLD;
		DBObjTableField *BackgroundFLD;
		DBObjTableField *StyleFLD;
		DBObjRecord *OrigREC;
	public:
		UISymbol (DBObjRecord *record,DBObjTableField *symbolField,DBObjTableField *foreField,DBObjTableField *backField,DBObjTableField *styleField)
		: DBObjRecord (*record)
			{
			SymbolIDFLD = symbolField;
			ForegroundFLD = foreField;
			BackgroundFLD = backField;
			StyleFLD = styleField;
			OrigREC = record;
			}
		DBInt SymbolID () const { return (SymbolIDFLD->Int ((DBObjRecord *) this)); }
		DBInt Foreground () const { return (ForegroundFLD->Int ((DBObjRecord *) this)); }
		void  Foreground (DBInt color) {  ForegroundFLD->Int ((DBObjRecord *) this,color); }
		DBInt Background () const { return (BackgroundFLD->Int ((DBObjRecord *) this)); }
		void  Background (DBInt color) { BackgroundFLD->Int ((DBObjRecord *) this,color); }
		DBInt Style () const { return (StyleFLD->Int ((DBObjRecord *) this)); }
		void  Style (DBInt style) { StyleFLD->Int ((DBObjRecord *) this,style); }
		DBInt IsChanged ()
			{
			if (Foreground () != ForegroundFLD->Int (OrigREC)) return (true);
			if (Background () != BackgroundFLD->Int (OrigREC)) return (true);
			if (Style () != StyleFLD->Int (OrigREC)) return (true);
			if (strcmp (Name (),OrigREC->Name ()) != 0) return (true);
			return (false);
			}
		void Update ()
			{
			ForegroundFLD->Int (OrigREC,Foreground ());
			BackgroundFLD->Int (OrigREC,Background ());
			StyleFLD->Int (OrigREC,Style ());
			OrigREC->Name (Name ());
			}
	};

static char *_UISymSetButtonString (char *symString,int symbol)
   { sprintf (symString,"*UISymButton%d",symbol); return (symString); }

static void _UISymbolSetForegroundCBK (Widget widget,Widget text,XmAnyCallbackStruct *callData)

	{
	UISymbol *editSymbol;
	DBInt value;

	callData = callData;
	XtVaGetValues (widget,XmNuserData, &value, NULL);
	XtVaGetValues (text,XmNuserData, &editSymbol, NULL);
	editSymbol->Foreground (value);
	}

static void _UISymbolSetBackgroundCBK (Widget widget,Widget text,XmAnyCallbackStruct *callData)

	{
	UISymbol *editSymbol;
	DBInt value;

	callData = callData;
	XtVaGetValues (widget,XmNuserData, &value, NULL);
	XtVaGetValues (text,XmNuserData, &editSymbol, NULL);
	editSymbol->Background (value);
	}

static void _UISymbolSetStyleCBK (Widget widget,Widget text,XmAnyCallbackStruct *callData)

	{
	UISymbol *editSymbol;
	DBInt value;

	callData = callData;
	XtVaGetValues (widget,XmNuserData, &value, NULL);
	XtVaGetValues (text,XmNuserData, &editSymbol, NULL);
	editSymbol->Style (value);
	}

static void _UISymbolTextValueChangedCBK (Widget widget,Widget list,XmTextVerifyCallbackStruct *callData)

	{
	char *aString;
	UISymbol *editSymbol;
	int *itemPosList, itemPosCount;
	XmString xmString [1];

	if (callData->reason != XmCR_VALUE_CHANGED) return;
	if (!XmListGetSelectedPos (list,&itemPosList,&itemPosCount)) return;
	if (itemPosCount > 1) { XtFree ((char *) itemPosList); return; }

	XtVaGetValues (widget, XmNuserData,	&editSymbol, NULL);
	aString = XmTextFieldGetString (widget);
	editSymbol->Name (aString);
	xmString [0] = XmStringCreate (aString,UICharSetNormal);
	XtFree (aString);
	XmListReplaceItemsPos (list,xmString,itemPosCount,itemPosList [0]);
	XmListSelectPos (list,itemPosList [0],false);
	XmStringFree (xmString [0]);
	XtFree ((char *) itemPosList);
	}

static void _UISymbolListSelectCBK (Widget widget,Widget dShell,XmListCallbackStruct *callData)

	{
	int symbol = callData->item_position - 1;
	char symString [20];
	UISymbol **editSymbols;
	Widget menu, option;

	if (callData->reason != XmCR_BROWSE_SELECT) return;

	XtVaGetValues (widget,XmNuserData,	&editSymbols, NULL);

	option = XtNameToWidget (dShell,UISymForegroundMenuName);
	XtVaGetValues (option,XmNsubMenuId, &menu, NULL);
	_UISymSetButtonString (symString,editSymbols [symbol]->Foreground ());
	XtVaSetValues (option, XmNmenuHistory, XtNameToWidget (menu,symString), NULL);

	option = XtNameToWidget (dShell,UISymBackgroundMenuName);
	XtVaGetValues (option,XmNsubMenuId, &menu, NULL);
	_UISymSetButtonString (symString,editSymbols [symbol]->Background ());
	XtVaSetValues (option, XmNmenuHistory, XtNameToWidget (menu,symString), NULL);

	_UISymSetButtonString (symString,editSymbols [symbol]->Style ());
	if (XtIsManaged (option = XtNameToWidget (dShell,UISymMarkerMenuName)))
		{
		XtVaGetValues (option,XmNsubMenuId, &menu, NULL);
		XtVaSetValues (option, XmNmenuHistory, XtNameToWidget (menu,symString), NULL);
		}
	if (XtIsManaged (option = XtNameToWidget (dShell,UISymLineMenuName)))
		{
		XtVaGetValues (option,XmNsubMenuId, &menu, NULL);
		XtVaSetValues (option, XmNmenuHistory, XtNameToWidget (menu,symString), NULL);
		}
	if (XtIsManaged (option = XtNameToWidget (dShell,UISymShadeMenuName)))
		{
		XtVaGetValues (option,XmNsubMenuId, &menu, NULL);
		XtVaSetValues (option, XmNmenuHistory, XtNameToWidget (menu,symString), NULL);
		}
	XtVaSetValues (XtNameToWidget (dShell,UISymTextFieldName),XmNuserData, editSymbols [symbol], NULL);
	XmTextFieldSetString (XtNameToWidget (dShell,UISymTextFieldName),editSymbols [symbol]->Name ());
	}

static void _UISymbolLoadNamesCBK (Widget widget,Widget list,XmAnyCallbackStruct *callData)

	{
	int symbol, symNum, i, color, *selectPos, selectCount;
	FILE *file;
	char *fileName, buffer [1024], *token, delimit [3];
	static Widget select = NULL;
	UISymbol**editSymbols;
	XmString xmString [1];

	widget = widget; callData = callData;

	if (select == NULL)
		select = UIFileSelectionCreate ((char *) "Symbol Name file Selection",NULL,(char *) "*.txt",XmFILE_REGULAR);

	XtVaGetValues (list,XmNuserData, &editSymbols, NULL);
	XtVaGetValues (list,XmNitemCount, &symNum, NULL);

	if ((fileName =  UIFileSelection (select,true)) == NULL) return;

	if ((file =  fopen (fileName,"r")) == NULL)
		{ CMmsgPrint (CMmsgSysError, "File Openning Error in: %s %d",__FILE__,__LINE__); return; }

	sprintf (delimit,"%c%c",DBASCIISeparator,'\0');
	while (fgets (buffer,sizeof (buffer),file) != NULL)
		{
		token = strtok (buffer,delimit);
		sscanf (token,"%d",&symbol);
		for (i = 0;i < symNum; ++i)
			if (editSymbols [i]->SymbolID () == symbol)
				{
				if ((token = strtok ((char *) NULL,delimit)) != (char *) NULL)
					{
					if (token [0] == '\"' || token [0] == '\'')	{token [strlen (token) - 1] = '\0'; token++; }
					editSymbols [i]->Name (token);
					}
				if ((token = strtok ((char *) NULL,delimit)) != (char *) NULL)
					if (sscanf (token,"%d",&color) == 1)	editSymbols [i]->Foreground (color);
				if ((token = strtok ((char *) NULL,delimit)) != (char *) NULL)
					if (sscanf (token,"%d",&color) == 1)	editSymbols [i]->Background (color);
				xmString [0] = XmStringCreate (editSymbols [i]->Name (),UICharSetNormal);
				XmListReplaceItemsPos (list,xmString,1,i + 1);
				XmStringFree (xmString [0]);
				break;
				}
		}
	if (XmListGetSelectedPos (list,&selectPos,&selectCount))
		{
		XmTextFieldSetString (XtNameToWidget (XtParent (XtParent (XtParent (list))),UISymTextFieldName),editSymbols [selectPos [0]]->Name ());
		XtFree ((char *) selectPos);
		}
	}

int UISymbolEdit (DBObjTable *symbols,int symbolType)

	{
	int symNum;
	static int changed, save;
	static Widget dShell = NULL;
	XmString string;
	UISymbol **editSymbols;
	DBObjRecord *symRecord;

	if (dShell == NULL)
		{
		char symString [20];
		int depth;
		Pixel foreground, background;
		XGCValues xgcv;
		GC gc;
		Widget mainForm, scrolledW, list, label, button, text, foreMenu, backMenu, rowCol, symMenu;
		Pixmap markerPixmap, labelPixmap;

		dShell = UIDialogForm ((char *) "Symbol Edit");
		mainForm = UIDialogFormGetMainForm (dShell);
		scrolledW = XtVaCreateManagedWidget ("UISymScrolledWindow",xmScrolledWindowWidgetClass,mainForm,
													XmNtopAttachment,		XmATTACH_FORM,
													XmNleftAttachment,	XmATTACH_FORM,
													XmNrightAttachment,	XmATTACH_FORM,
													NULL);
		list = XtVaCreateManagedWidget (UISymListName + 1,xmListWidgetClass,scrolledW,
													XmNvisibleItemCount,	6,
													XmNselectionPolicy,	XmBROWSE_SELECT,
													XmNscrollBarDisplayPolicy,	XmSTATIC,
													NULL);
		XtAddCallback (list,XmNbrowseSelectionCallback,(XtCallbackProc) _UISymbolListSelectCBK,dShell);
		string = XmStringCreate ((char *) "Symbol",UICharSetNormal);
		label = XtVaCreateManagedWidget ("UISymbolLabel",xmLabelWidgetClass,mainForm,
													XmNtopAttachment,		XmATTACH_WIDGET,
													XmNtopWidget,			scrolledW,
													XmNleftAttachment,	XmATTACH_FORM,
													XmNlabelString,		string,
													NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Load Names",UICharSetNormal);
		button = XtVaCreateManagedWidget (UISymLoadNamesButtonName + 1,xmPushButtonWidgetClass,mainForm,
													XmNtopAttachment,		XmATTACH_WIDGET,
													XmNtopWidget,			label,
													XmNrightAttachment,	XmATTACH_FORM,
													XmNlabelString,		string,
													NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UISymbolLoadNamesCBK,list);
		XmStringFree (string);
		text = XtVaCreateManagedWidget (UISymTextFieldName + 1,xmTextFieldWidgetClass,mainForm,
													XmNtopAttachment,		XmATTACH_WIDGET,
													XmNtopWidget,			label,
													XmNleftAttachment,	XmATTACH_FORM,
													XmNrightAttachment,	XmATTACH_WIDGET,
													XmNrightWidget,		button,
													XmNrightOffset,		5,
													NULL);
		XtAddCallback (text,XmNvalueChangedCallback,(XtCallbackProc) _UISymbolTextValueChangedCBK,list);
		XtAddCallback (text,XmNvalueChangedCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&changed);

		foreMenu = XmCreatePulldownMenu (mainForm,(char *) "UISymForegroundMenuPane",NULL,0);
		backMenu = XmCreatePulldownMenu (mainForm,(char *) "UISymBackgroundMenuPane",NULL,0);

		for (symNum = 0;symNum <  UIColorNum (UIColorStandard);symNum++)
			{
			foreground = UIColor (UIColorStandard,symNum);
			_UISymSetButtonString (symString,symNum);
			string = XmStringCreate (UIStandardColorName(symNum),UICharSetNormal);
			button =  XtVaCreateManagedWidget (symString + 1,xmPushButtonWidgetClass,foreMenu,
													XmNuserData,			symNum,
													XmNforeground,			foreground,
													XmNlabelString,		string,
													NULL);
			XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UISymbolSetForegroundCBK,text);
			XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&changed);
			button =  XtVaCreateManagedWidget (symString + 1,xmPushButtonWidgetClass,backMenu,
													XmNuserData,			symNum,
													XmNforeground,			foreground,
													XmNlabelString,		string,
													NULL);
			XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UISymbolSetBackgroundCBK,text);
			XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&changed);
			XmStringFree (string);
			}

		string = XmStringCreate ((char *) "Foreground:",UICharSetNormal);
		foreMenu = XtVaCreateManagedWidget (UISymForegroundMenuName + 1,xmRowColumnWidgetClass,mainForm,
													XmNtopAttachment,			XmATTACH_WIDGET,
													XmNtopWidget,				text,
													XmNrightAttachment,		XmATTACH_FORM,
													XmNsubMenuId,				foreMenu,
													XmNlabelString,			string,
													XmNrowColumnType,			XmMENU_OPTION,
													XmNtraversalOn,			false,
													NULL);
		XmStringFree (string);
		string = XmStringCreate ((char *) "Background:",UICharSetNormal);
		backMenu = XtVaCreateManagedWidget (UISymBackgroundMenuName + 1,xmRowColumnWidgetClass,mainForm,
													XmNtopAttachment,			XmATTACH_WIDGET,
													XmNtopWidget,				foreMenu,
													XmNrightAttachment,		XmATTACH_FORM,
													XmNsubMenuId,				backMenu,
													XmNlabelString,			string,
													XmNrowColumnType,			XmMENU_OPTION,
													XmNtraversalOn,			false,
													NULL);
		rowCol = XtVaCreateManagedWidget ("UISymbolRowCol",xmRowColumnWidgetClass,mainForm,
													XmNtopAttachment,			XmATTACH_WIDGET,
													XmNtopWidget,				backMenu,
													XmNrightAttachment,		XmATTACH_FORM,
													XmNbottomAttachment,		XmATTACH_FORM,
													NULL);
		XtVaGetValues (mainForm,XmNforeground,	&foreground,XmNbackground,	&background,NULL);
		xgcv.foreground = foreground;
		xgcv.background = background;
		xgcv.fill_style = FillTiled;
		gc = XCreateGC (XtDisplay (UITopLevel ()),XtWindow (UITopLevel ()),GCForeground | GCBackground | GCFillStyle,&xgcv);

		symMenu = XmCreatePulldownMenu (rowCol,(char *) "UISymShadeMenuPane",NULL,0);
		depth = DefaultDepth (XtDisplay (UITopLevel ()),DefaultScreen (XtDisplay (UITopLevel ())));
		for (symNum = 0;(xgcv.tile = UIPattern (symNum,foreground,background)) != (Pixmap) NULL;symNum++)
			{
			labelPixmap = XCreatePixmap (XtDisplay (UITopLevel ()),XtWindow (UITopLevel ()),96,24,depth);
			XChangeGC (XtDisplay (UITopLevel ()),gc,GCTile,&xgcv);
			XFillRectangle (XtDisplay (UITopLevel ()),labelPixmap,gc,0,0,96,24);
			XFreePixmap (XtDisplay (UITopLevel ()),xgcv.tile);

			_UISymSetButtonString(symString,symNum);
			button = XtVaCreateManagedWidget (symString + 1,xmPushButtonGadgetClass,symMenu,
													XmNuserData,				symNum,
													XmNlabelType,				XmPIXMAP,
													XmNlabelPixmap,			labelPixmap,
													NULL);
			XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UISymbolSetStyleCBK,text);
			XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&changed);
			}
		string = XmStringCreate ((char *) "Shade:",UICharSetNormal);
		symMenu = XtVaCreateWidget (UISymShadeMenuName + 1,xmRowColumnWidgetClass,rowCol,
													XmNsubMenuId,				symMenu,
													XmNlabelString,			string,
													XmNrowColumnType,			XmMENU_OPTION,
													XmNtraversalOn,			false,
													NULL);
		XmStringFree (string);

		xgcv.foreground = background;
		xgcv.fill_style = FillSolid;
		XChangeGC (XtDisplay (UITopLevel ()),gc,GCForeground | GCFillStyle,&xgcv);
		symMenu = XmCreatePulldownMenu (rowCol,(char *) "UISymMarkerMenuPane",NULL,0);
		depth = DefaultDepth (XtDisplay (UITopLevel ()),DefaultScreen (XtDisplay (UITopLevel ())));
		for (symNum = 0;(markerPixmap = UIMarker (symNum,foreground,background)) != (Pixmap) NULL;symNum++)
			{
			labelPixmap = XCreatePixmap (XtDisplay (UITopLevel ()),XtWindow (UITopLevel ()),96,24,depth);
			XFillRectangle (XtDisplay (UITopLevel ()),labelPixmap,gc,0,0,96,24);
			XCopyArea (XtDisplay (UITopLevel ()),markerPixmap,labelPixmap,gc,0,0,11,11,42,6);
			XFreePixmap (XtDisplay (UITopLevel ()),markerPixmap);
			_UISymSetButtonString(symString,symNum);
			button = XtVaCreateManagedWidget (symString + 1,xmPushButtonGadgetClass,symMenu,
													XmNuserData,				symNum,
													XmNlabelType,				XmPIXMAP,
													XmNlabelPixmap,			labelPixmap,
													NULL);
			XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UISymbolSetStyleCBK,text);
			XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&changed);
			}
		string = XmStringCreate ((char *) "Marker:",UICharSetNormal);
		symMenu = XtVaCreateWidget (UISymMarkerMenuName + 1,xmRowColumnWidgetClass,rowCol,
													XmNsubMenuId,				symMenu,
													XmNlabelString,			string,
													XmNrowColumnType,			XmMENU_OPTION,
													XmNtraversalOn,			false,
													NULL);
		XmStringFree (string);

		symMenu = XmCreatePulldownMenu (rowCol,(char *) "UISymLineMenuPane",NULL,0);
		depth = DefaultDepth (XtDisplay (UITopLevel ()),DefaultScreen (XtDisplay (UITopLevel ())));
		for (symNum = 0;symNum < 12;symNum++)
			{
			labelPixmap = XCreatePixmap (XtDisplay (UITopLevel ()),XtWindow (UITopLevel ()),96,24,depth);
			xgcv.foreground = background;
			xgcv.fill_style = FillSolid;
			XChangeGC (XtDisplay (UITopLevel ()),gc,GCForeground | GCFillStyle,&xgcv);
			XFillRectangle (XtDisplay (UITopLevel ()),labelPixmap,gc,0,0,96,24);
			xgcv.line_width = symNum & 0x03;
			switch (symNum >> 0x02)
				{
				case 0:	xgcv.line_style = LineSolid;		break;
				case 1:	xgcv.line_style = LineOnOffDash;	break;
				case 2:	xgcv.line_style = LineDoubleDash;	break;
				}
			xgcv.foreground = foreground;
			xgcv.background = UIColor (UIColorStandard,0);
			XChangeGC (XtDisplay (UITopLevel ()),gc,GCForeground | GCBackground | GCLineWidth | GCLineStyle,&xgcv);
			XDrawLine (XtDisplay (UITopLevel ()),labelPixmap,gc,0,12,96,12);
			_UISymSetButtonString(symString,symNum);
			button = XtVaCreateManagedWidget (symString + 1,xmPushButtonGadgetClass,symMenu,
													XmNuserData,				symNum,
													XmNlabelType,				XmPIXMAP,
													XmNlabelPixmap,			labelPixmap,
													NULL);
			XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _UISymbolSetStyleCBK,text);
			XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&changed);
			}
		string = XmStringCreate ((char *) "Line:",UICharSetNormal);
		symMenu = XtVaCreateWidget (UISymLineMenuName + 1,xmRowColumnWidgetClass,rowCol,
													XmNsubMenuId,				symMenu,
													XmNlabelString,			string,
													XmNrowColumnType,			XmMENU_OPTION,
													XmNtraversalOn,			false,
													NULL);
		XmStringFree (string);
		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&save);
		}
	symbols->ListSort ();
	switch (symbolType)
		{
		case UISymbolMarker:		XtManageChild (XtNameToWidget (dShell,UISymMarkerMenuName));	break;
		case UISymbolLine:		XtManageChild (XtNameToWidget (dShell,UISymLineMenuName));		break;
		case UISymbolShade:		XtManageChild (XtNameToWidget (dShell,UISymShadeMenuName));		break;
		}

	if ((editSymbols = (UISymbol **) calloc (symbols->ItemNum (),sizeof (UISymbol *))) == NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory Allocation Error in: %s %d",__FILE__,__LINE__); return (DBFault); }
	symNum = 0;
	for (symRecord = symbols->First ();symRecord != NULL;symRecord = symbols->Next ())
		{
		editSymbols [symNum] = new UISymbol (symRecord,	symbols->Field (DBrNSymbolID),
																		symbols->Field (DBrNForeground),
																		symbols->Field (DBrNBackground),
																		symbols->Field (DBrNStyle));
		string = XmStringCreate (editSymbols [symNum]->Name (),UICharSetNormal);
		XmListAddItem (XtNameToWidget (dShell,UISymListName),string,++symNum);
		XmStringFree (string);
		}
	XtVaSetValues (XtNameToWidget (dShell,UISymListName),XmNuserData, editSymbols, NULL);
	XmListSelectPos (XtNameToWidget (dShell,UISymListName),1,true);
	XtSetSensitive (UIDialogFormGetOkButton (dShell),false);
	changed = save = false;

	UIDialogFormPopup (dShell);
	while (UILoop ())
		if (changed)
			{
			changed = false;
			for (symNum = 0;symNum < symbols->ItemNum ();++symNum)
				if (editSymbols [symNum]->IsChanged ()) break;
			XtSetSensitive (UIDialogFormGetOkButton (dShell),symNum == symbols->ItemNum () ? false : true);
			}

	UIDialogFormPopdown (dShell);

	switch (symbolType)
		{
		case UISymbolMarker:		XtUnmanageChild (XtNameToWidget (dShell,UISymMarkerMenuName));	break;
		case UISymbolLine:		XtUnmanageChild (XtNameToWidget (dShell,UISymLineMenuName));	break;
		case UISymbolShade:		XtUnmanageChild (XtNameToWidget (dShell,UISymShadeMenuName));	break;
		}
	if (save) for (symNum = 0;symNum < symbols->ItemNum ();++symNum) editSymbols [symNum]->Update ();

	XmListDeleteAllItems (XtNameToWidget (dShell,UISymListName));

	for (symNum = 0;symNum < symbols->ItemNum ();++symNum) delete editSymbols [symNum];
	free (editSymbols);
	symbols->ListReset ();
	return (DBSuccess);
	}
