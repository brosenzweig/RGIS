/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

UIPatterns.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <UI.H>

#define UIWidth 8
#define UIHeight 8

static char _UITile00 [] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static char _UITile01 [] = {0x45, 0xaa, 0x11, 0x88, 0x55, 0x22, 0x11, 0xaa};
static char _UITile02 [] = {0x44, 0xaa, 0x11, 0x88, 0x55, 0x22, 0x11, 0xaa};
static char _UITile03 [] = {0xff, 0x04, 0x04, 0x04, 0xff, 0x20, 0x20, 0x20};
static char _UITile04 [] = {0x03, 0x66, 0xf6, 0x93, 0x30, 0x66, 0x6f, 0x39};
static char _UITile05 [] = {0x92, 0x49, 0x64, 0x92, 0x19, 0x24, 0x46, 0x89};
static char _UITile06 [] = {0xa8, 0x45, 0x2a, 0x51, 0x8a, 0x54, 0xa2, 0x15};
static char _UITile07 [] = {0xdf, 0xb5, 0xda, 0xbf, 0xfd, 0x5b, 0xad, 0xfb};
static char _UITile08 [] = {0x6f, 0x24, 0x42, 0x6f, 0xf6, 0x42, 0x24, 0xf6};
static char _UITile09 [] = {0x00, 0x7e, 0x42, 0x5a, 0x5a, 0x42, 0x7e, 0x00};
static char _UITile10 [] = {0x00, 0xee, 0xaa, 0xee, 0x00, 0xee, 0xaa, 0xee};

static char *_UIPatterns [] = { _UITile00,
				_UITile01,
				_UITile02,
				_UITile03,
				_UITile04,
				_UITile05,
				_UITile06,
				_UITile07,
				_UITile08,
				_UITile09,
				_UITile10};

Pixmap UIPattern (int style,int foreground,int background)

	{
	Pixmap pixmap;
	if (style >= (int) (sizeof (_UIPatterns) / sizeof (char *))) return ((Pixmap) NULL);

	pixmap = XCreatePixmapFromBitmapData (XtDisplay (UITopLevel ()),XtWindow (UITopLevel ()),
					      _UIPatterns [style],UIWidth,UIHeight,foreground,background,
					      DefaultDepth (XtDisplay (UITopLevel ()),DefaultScreen (XtDisplay (UITopLevel ())) ));

	return (pixmap);
	}	
