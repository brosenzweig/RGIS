/******************************************************************************

GHAAS RiverGIS Libarary V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

RGlibPause.C

balazs.fekete@unh.edu

*******************************************************************************/

#include<DB.H>

static const char *_RGlibPrefixText = (char *) NULL;

static int _RGlibPrevProgress = 0;

static int _RGlibPause (int progress)

	{
	int i;

	if (progress < _RGlibPrevProgress) return (false);
	fprintf (stderr,"\r|");
	for (i = 0;i < progress / 10;i++) fprintf (stderr,"*");
	for (     ;i < 10;i++) fprintf (stderr," ");
	fprintf (stderr,"| %2d|",progress);
	return (false);
	}

void RGlibPauseOpen (const char *text)

	{
	_RGlibPrefixText = text;
	_RGlibPrevProgress = 0;
	fprintf (stderr,"%s",text);
	DBPauseSetFunction (_RGlibPause);
	}

void RGlibPauseClose ()

	{
	_RGlibPrefixText = (char *) NULL;
	DBPauseSetFunction ((int (*) (int)) NULL);
	fprintf (stderr,"Done\n");
	}
