/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

UIMenu.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <Xm/CascadeB.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/RowColumn.h>
#include <UI.H>

#define UIMENU_SEPARATOR	0x01
#define UIMENU_SUBMENU		0x02
#define UIMENU_FUNCTION		0x03

void _UIHelpCallback (Widget,char *, XmAnyCallbackStruct *);

static void _UINotImplementedCBK (Widget widget,char *text,XmAnyCallbackStruct *callData)
         {
         widget = widget; text = text; callData = callData;
          UIMessage ((char *) "This menu option is not implemented");
          }

UIMenuItem::UIMenuItem ()

	{
	Type	 = 0;
	Label	 = NULL;
	Action = (void *) NULL;
	VisibleVAR   = UIMENU_NORULE;
	SensitiveVAR = UIMENU_NORULE;
	HelpFile = NULL;
	}

UIMenuItem::UIMenuItem (DBUnsigned visible,DBUnsigned sensitive)

	{
	Type   = UIMENU_SEPARATOR;
	Label  = NULL;
	Action = (void *) NULL;
	VisibleVAR	= visible;
	SensitiveVAR = sensitive;
	HelpFile = NULL;
	}

UIMenuItem::UIMenuItem (char *label,DBUnsigned visible,DBUnsigned sensitive,UIMenuItem subMenu [])

	{
	Type   = UIMENU_SUBMENU;
	Label  = label;
	Action = (void *) subMenu;
	VisibleVAR	= visible;
	SensitiveVAR = sensitive;
	HelpFile = NULL;
	}

UIMenuItem::UIMenuItem (char *label,DBUnsigned visible,DBUnsigned sensitive,void (*function) (Widget,void *,XmAnyCallbackStruct *),char *helpFile)

	{
	Type   = UIMENU_FUNCTION;
	Label  = label;
	Action = (void *) function != NULL ? (void *) function : (void *) _UINotImplementedCBK;
	VisibleVAR	= visible;
	SensitiveVAR = sensitive;
	HelpFile = helpFile;
	}

static void _UIMenuButtonCBK (Widget widget,XtPointer *menuData,XmAnyCallbackStruct *callData)

	{
	UIMenuItem *menuBut;

	XtVaGetValues (widget,XmNuserData, &menuBut, NULL);
	switch (callData->event->xbutton.button)
		{
		default:	menuBut->CallMenu (widget,menuData,callData); break;
		case 3:	menuBut->CallHelp ();	break;
		}
	}

void UIMenuItem::CreateButton (Widget menuWidget,
										 UIMenuItem *menuBut,
										 void *menuData)
	{
	Arg wargs [2];
	int argNum = 0, menuNum;
	char mnemonic[2];
	Widget subMenu;
	XmString string;

	switch (menuBut->Type)
		{
		case UIMENU_FUNCTION:
			string = XmStringCreate (menuBut->Label,UICharSetNormal);
			menuBut->Button = XtVaCreateWidget ("UIFunctionButton",xmPushButtonGadgetClass, menuWidget,
														XmNuserData,			menuBut,
														XmNlabelString,		string,
														NULL);
			XtAddCallback (menuBut->Button,XmNactivateCallback, (XtCallbackProc) _UIMenuButtonCBK,(XtPointer) menuData);
			XmStringFree (string);
			break;
		case UIMENU_SUBMENU:
			argNum = 0;
			subMenu = XmCreatePulldownMenu (menuWidget,(char *) "GHAASPullDownMenu",wargs,argNum);

			mnemonic[0] = menuBut->Label [0];
			mnemonic[1] = '\0';
			string = XmStringCreate (menuBut->Label,UICharSetBold);
			menuBut->Button = XtVaCreateWidget ("UISubmenuButton",xmCascadeButtonWidgetClass,menuWidget,
															XmNlabelString,		string,
															XmNsubMenuId,	subMenu,
															XmNmnemonic,	XStringToKeysym( mnemonic ),
															NULL);
			XmStringFree (string);
			if (menuBut->Action != NULL)
				for (menuNum = 0; ((UIMenuItem *) menuBut->Action) [menuNum].Type != 0; ++menuNum)
					menuBut->CreateButton (subMenu,((UIMenuItem *) menuBut->Action) + menuNum,menuData);
			break;
		case UIMENU_SEPARATOR:
			menuBut->Button = 	 XmCreateSeparatorGadget (menuWidget,(char *) "GHAASMenuSeparator", NULL,0);
			break;
		default: fprintf (stderr,"Not valid Button type: in UIMenuItem::CreateButton ()\n"); return;
		}
	if (menuBut->VisibleVAR == UIMENU_NORULE)		XtManageChild	(menuBut->Button);
	XtSetSensitive (menuBut->Button,menuBut->SensitiveVAR == UIMENU_NORULE);
	}

void UIMenuItem::Sensitive (unsigned groups,int sensitive)

	{
	int menuNum;

	switch (Type)
		{
		case UIMENU_SUBMENU:
			for (menuNum = 0; ((UIMenuItem *) Action) [menuNum].Type != 0; ++menuNum)
				((UIMenuItem *) Action) [menuNum].Sensitive (groups,sensitive);
		default:
			if (sensitive)
				{ if ((SensitiveVAR & groups) == SensitiveVAR) XtSetSensitive (Button,true); }
			else
				{ if ((SensitiveVAR & groups) == groups)	XtSetSensitive (Button,false); }
			break;
		}
	}

void UIMenuItem::Visible (unsigned groups, int visible)

	{
	int menuNum;

	switch (Type)
		{
		case UIMENU_SUBMENU:
			for (menuNum = 0; ((UIMenuItem *) Action) [menuNum].Type != 0; ++menuNum)
				((UIMenuItem *) Action) [menuNum].Visible (groups,visible);
		default:
			if (visible)
				{ if ((VisibleVAR & groups) == VisibleVAR) XtManageChild (Button); }
			else
				{ if ((VisibleVAR & groups) == groups) XtUnmanageChild (Button);}
			break;
		}
	}

void UIMenuItem::CallMenu (Widget widget,void *data,XmAnyCallbackStruct *callData) const

	{ (*((XtCallbackProc) Action)) (widget,data,callData); }

void UIMenuDisplayHelp (char *helpFile)

	{
	if (helpFile == (char *) NULL) UIMessage ((char *) "Sorry, No Help! You are on your own!");
	else
		{
		char cmndString [2048];
		if (getenv ("GHAAS_BROWSER") == NULL) { UIMessage ((char *) "You Don't have Web Browser Configured!"); return; }
		sprintf (cmndString,getenv ("GHAAS_BROWSER"),helpFile);
		system (cmndString);
		}
	}

void UIMenuItem::CallHelp () const { UIMenuDisplayHelp (HelpFile); }
