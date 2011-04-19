/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

UIInit.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <unistd.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/CascadeB.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawingA.h>
#include <UI.H>

#define UICopyRightText "[Copyright (1994-2011) UNH - CCNY/CUNY]"

static Widget _UITopLevel;
static UIMenuItem	*_UIMainMenu;
static DBDataset	*_UIDataset;
static XtAppContext _UIApplicationContext;

XtAppContext UIApplicationContext () { return (_UIApplicationContext); }
Widget UITopLevel () { return (_UITopLevel); }
DBDataset *UIDataset () { return (_UIDataset); }

class UIResData
	{
	public:
		XmFontList LargeFontList;
		XmFontList NormalFontList;
		XmFontList SmallFontList;
		XmFontList ItalicFontList;
		XmFontList FixedFontList;
	};

static class UIResData _UIResData;

static XtResource _UIResources [] = {
{(char *) "UILargeFontList",	XmCFontList,XmRFontList, sizeof (XmFontList), XtOffset (UIResData *,LargeFontList), NULL},
{(char *) "UINormalFontList",	XmCFontList,XmRFontList, sizeof (XmFontList), XtOffset (UIResData *,NormalFontList),NULL},
{(char *) "UISmallFontList",	XmCFontList,XmRFontList, sizeof (XmFontList), XtOffset (UIResData *,SmallFontList), NULL},
{(char *) "UIItalicFontList",	XmCFontList,XmRFontList, sizeof (XmFontList), XtOffset (UIResData *,ItalicFontList),NULL},
{(char *) "UIFixedFontList",	XmCFontList,XmRFontList, sizeof (XmFontList), XtOffset (UIResData *,FixedFontList),NULL}};

XmFontList UILargeFontList ()		{ return (_UIResData.LargeFontList); }
XmFontList UINormalFontList ()	{ return (_UIResData.NormalFontList); }
XmFontList UISmallFontList ()		{ return (_UIResData.SmallFontList); }
XmFontList UIItalicFontList ()	{ return (_UIResData.ItalicFontList); }
XmFontList UIFixedFontList ()		{ return (_UIResData.FixedFontList); }

static void _UIMenuHelpButtonCBK (Widget widget,char *helpFile,XmAnyCallbackStruct *callData)

	{
	widget = widget; callData = callData;
	void UIMenuDisplayHelp (char *); UIMenuDisplayHelp (helpFile);
	}

void UIDataset (char *icon, char *ghaasMetaDB)

	{
	char metaFileName [DBDataFileNameLen];

	if (ghaasMetaDB == (char *) NULL)
		{
		if (getenv ("GHAASMetaDB") == NULL)
				sprintf (metaFileName,  "%s/GHAASMetadb", getenv ("HOME"));
		else	strcpy (metaFileName, getenv ("GHAASMetaDB"));
		ghaasMetaDB = metaFileName;
		}
	_UIDataset = new DBDataset (icon,ghaasMetaDB);
	}

Widget UIInitialize (char *title,char *icon,char *helpFile, UIMenuItem mainMenu [],void *data,int *argc, char *argv [],
                     int reqWidth,int reqHeight,bool spin, bool progress)
	{
	int i, foreground, background;
	Dimension lWidth;
	char applicationTitle [256];
	Widget mainForm, menuBar, helpButton, ccnyLogo, unhLogo, ghaasLogo, globeWidget, subForm;
	Pixmap pixmap;
	XmString string;
	void _UIColorInitialize (Widget);
	void _UIGlobeInitialize (Widget,int,Pixel,DBInt);

	sprintf (applicationTitle,"%s %s", title, UICopyRightText);
	_UITopLevel = XtVaAppInitialize (&_UIApplicationContext,"ghaas",(XrmOptionDescRec *) NULL,0,argc,argv,NULL,
												XmNminWidth,					reqWidth,
												XmNmaxWidth,					reqWidth,
												XmNminHeight,					reqHeight,
												XmNmaxHeight,					reqHeight,
												XmNwidth,						reqWidth,
												XmNheight,						reqHeight,
												XmNiconName,					title,
												XmNtitle,						applicationTitle,
												XmNkeyboardFocusPolicy,		XmPOINTER,
												NULL);
	XtGetApplicationResources (_UITopLevel,&_UIResData,_UIResources,XtNumber (_UIResources),NULL,0);

	_UIMainMenu = mainMenu;

	string = XmStringCreate (applicationTitle,UICharSetNormal);
	mainForm = XtVaCreateManagedWidget ("GHAASMainForm", xmFormWidgetClass, _UITopLevel,
													XmNdialogTitle,			string,
													NULL);
	XmStringFree (string);
	XtVaGetValues (mainForm,XmNforeground, &foreground,XmNbackground, &background,NULL);
	if (icon != NULL)
		{
		pixmap = XmGetPixmap(XtScreen(UITopLevel ()),icon,foreground,background);
		XtVaSetValues (_UITopLevel,XmNiconPixmap,	pixmap,NULL);
		}
	menuBar = XtVaCreateManagedWidget ("GHAASMainMenu", xmRowColumnWidgetClass, mainForm,
													XmNtopAttachment,		XmATTACH_FORM,
													XmNleftAttachment,	XmATTACH_FORM,
													XmNrightAttachment,	XmATTACH_FORM,
													XmNrowColumnType,		XmMENU_BAR,
													NULL);
	if (helpFile != (char *) NULL)
		{
		string = XmStringCreate ((char *) "Help",UICharSetBold);
		helpButton = XtVaCreateManagedWidget ("GHAASMainHelp",xmCascadeButtonWidgetClass,menuBar,
											XmNlabelString,	string,
											NULL);
		XmStringFree (string);
		XtAddCallback (helpButton,XmNactivateCallback, (XtCallbackProc) _UIMenuHelpButtonCBK,(XtPointer) helpFile);
		}
	XtVaSetValues (menuBar, XmNmenuHelpWidget, helpButton, NULL);

	for (i = 0;_UIMainMenu [i].Type != 0;++i)
		_UIMainMenu [i].CreateButton (menuBar,_UIMainMenu + i,data);

	XtRealizeWidget (_UITopLevel);
	_UIColorInitialize (_UITopLevel);

	pixmap	= XmGetPixmap(XtScreen(UITopLevel()),(char *) "GHAASccny.xpm",foreground,background);
	ccnyLogo = XtVaCreateManagedWidget ("GHAASMainUNHLogo",xmLabelWidgetClass,mainForm,
	                                    XmNlabelType,        XmPIXMAP,
	                                    XmNlabelPixmap,      pixmap,
	                                    XmNmarginWidth,      0,
	                                    XmNmarginHeight,     0,
	                                    XmNleftAttachment,   XmATTACH_FORM,
	                                    XmNleftOffset,       5,
	                                    XmNbottomAttachment, XmATTACH_FORM,
	                                    XmNbottomOffset,     5,
	                                    NULL);
	pixmap	= XmGetPixmap(XtScreen(UITopLevel()),(char *) "GHAASunh.xpm",foreground,background);
	unhLogo = XtVaCreateManagedWidget ("GHAASMainUNHLogo",xmLabelWidgetClass,mainForm,
	                                    XmNlabelType,        XmPIXMAP,
	                                    XmNlabelPixmap,      pixmap,
	                                    XmNmarginWidth,      0,
	                                    XmNmarginHeight,     0,
	                                    XmNleftAttachment,   XmATTACH_FORM,
	                                    XmNleftOffset,       5,
	                                    XmNbottomAttachment, XmATTACH_WIDGET,
	                                    XmNbottomWidget,     ccnyLogo,
	                                    XmNbottomOffset,     5,
	                                    NULL);
	pixmap = XmGetPixmap(XtScreen(UITopLevel()),(char *) "GHAASlogo.xpm",foreground,background);
	ghaasLogo = XtVaCreateManagedWidget ("GHAASMainUNHLogo",xmLabelWidgetClass,mainForm,
	                                    XmNlabelType,        XmPIXMAP,
	                                    XmNlabelPixmap,      pixmap,
	                                    XmNrightAttachment,  XmATTACH_FORM,
	                                    XmNrightOffset,      5,
	                                    XmNbottomAttachment, XmATTACH_FORM,
	                                    XmNbottomOffset,     5,
	                                    NULL);
	string = XmStringCreate ((char *) "Global Hydrological Archive and Analysis System",UICharSetNormal);
	XtVaCreateManagedWidget ("GHAASLabel",xmLabelWidgetClass,mainForm,
	                                    XmNlabelString,      string,
	                                    XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
	                                    XmNtopWidget,        ghaasLogo,
	                                    XmNleftAttachment,   XmATTACH_WIDGET,
	                                    XmNleftWidget,       ccnyLogo,
	                                    XmNrightAttachment,  XmATTACH_WIDGET,
	                                    XmNrightWidget,      ghaasLogo,
	                                    XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
	                                    XmNbottomWidget,     ghaasLogo,
	                                    NULL);
	XmStringFree (string);
	subForm = XtVaCreateManagedWidget ("GHAASMainForm",xmFormWidgetClass,mainForm,
	                                    XmNtopAttachment,    XmATTACH_WIDGET,
	                                    XmNtopWidget,        menuBar,
	                                    XmNtopOffset,        5,
	                                    XmNleftAttachment,   XmATTACH_WIDGET,
	                                    XmNleftWidget,       ccnyLogo,
	                                    XmNleftOffset,       5,
	                                    XmNrightAttachment,  XmATTACH_FORM,
	                                    XmNrightOffset,      10,
	                                    XmNbottomAttachment, XmATTACH_WIDGET,
	                                    XmNbottomWidget,     ghaasLogo,
	                                    XmNbottomOffset,     5,
	                                    XmNshadowType,       XmSHADOW_IN,
	                                    XmNshadowThickness,  1,
	                                    NULL);
	XtVaGetValues (ccnyLogo,XmNwidth, &lWidth, NULL);
	globeWidget = XtVaCreateManagedWidget ("UIMainGlobeWidget",xmDrawingAreaWidgetClass,mainForm,
	                                    XmNtopAttachment,    XmATTACH_WIDGET,
	                                    XmNtopOffset,        5,
	                                    XmNtopWidget,        menuBar,
	                                    XmNleftAttachment,   XmATTACH_FORM,
	                                    XmNleftOffset,       5,
	                                    XmNrightAttachment,  XmATTACH_WIDGET,
	                                    XmNrightWidget,      subForm,
	                                    XmNrightOffset,      5,
	                                    XmNheight,           lWidth,
	                                    XmNwidth,            lWidth,
	                                    NULL);
	_UIGlobeInitialize (globeWidget,lWidth, background,spin);

	UILoop ();
	UIPauseDisplay (progress);
	return (subForm);
	}

void UIMenuSensitive (unsigned group,int sensitive)

	{ int i; for (i = 0;_UIMainMenu [i].Type != 0;++i) _UIMainMenu [i].Sensitive (group,sensitive); }

void UIMenuVisible (unsigned group,int sensitive)

	{ int i; for (i = 0;_UIMainMenu [i].Type != 0;++i) _UIMainMenu [i].Visible (group,sensitive); }
