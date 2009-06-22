/******************************************************************************

GHAAS User Interface library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

UIColorSets.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <UI.H>

#define _UIColorNum(colorList) (int) (sizeof (colorList) / sizeof (XColor))

static char *_UIStdColorNames [] = {(char *) "white",
				(char *) "Black",
				(char *) "Red",
				(char *) "Green",
				(char *) "Blue",
				(char *) "Cyan",
				(char *) "Magenta",
				(char *) "Yellow",
				(char *) "Orange",
				(char *) "LimeGreen",
				(char *) "MediumSpringGreen",
				(char *) "DarkSlateBlue",
				(char *) "MediumSlateBlue",
				(char *) "Brown",
				(char *) "DimGray",
				(char *) "LightGray" };

static XColor _UIStandardColors [] = {
		{ 0,	255,	255,	255,	DoRed | DoGreen | DoBlue,	0},	/* White	*/
		{ 0,	0, 	0, 	0, 	DoRed | DoGreen | DoBlue,	0},	/* Black	*/
		{ 0,	255,	0, 	0, 	DoRed | DoGreen | DoBlue,	0},	/* Red	*/
		{ 0,	0, 	255,	0, 	DoRed | DoGreen | DoBlue,	0},	/* Green	*/
		{ 0,	0, 	0, 	255,	DoRed | DoGreen | DoBlue,	0},	/* Blue	*/
		{ 0,	0, 	255,	255,	DoRed | DoGreen | DoBlue,	0},	/* Cyan	*/
		{ 0,	255,	0, 	255,	DoRed | DoGreen | DoBlue,	0},	/* Magenta	*/
		{ 0,	255,	255,	0,		DoRed | DoGreen | DoBlue,	0},	/* Yellow	*/
		{ 0,	255,	165,	0, 	DoRed | DoGreen | DoBlue,	0},	/* Orange	*/
		{ 0,	50,	205,	50,	DoRed | DoGreen | DoBlue,	0},	/* LimeGreen	*/
		{ 0,	0,		250,	154,	DoRed | DoGreen | DoBlue,	0},	/* MediumSpringGreen */
		{ 0,	72,	61,	139,	DoRed | DoGreen | DoBlue,	0},	/* DarkSlateBlue	*/
		{ 0,	123,	104,	238,	DoRed | DoGreen | DoBlue,	0},	/* MediumSlateBlue	*/
		{ 0,	165,	42,	42,	DoRed | DoGreen | DoBlue,	0},	/* Brown	*/
		{ 0,	105,	105,	105,	DoRed | DoGreen | DoBlue,	0},	/* DimGray	*/
		{ 0,	201,	201,	201,	DoRed | DoGreen | DoBlue,	0}};	/* LightGray	*/

static XColor _UIGreyColors [] = {
		{ 0,	32,	32,	32,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	84,	84,	84,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	108, 	108,	108,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	128, 	128,	128,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	142, 	142,	142,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	156, 	156,	156,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	170, 	170,	170,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	182, 	182,	182,	DoRed | DoGreen | DoBlue,	0},

		{ 0,	190, 	190,	190,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	200,	200,	200,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	212,	212,	212,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	220, 	220,	220,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	230, 	230,	230,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	236, 	236,	236,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	245, 	245,	245,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	253, 	253,	253,	DoRed | DoGreen | DoBlue,	0}};

static XColor _UIRedColors [] = {
		{ 0,	255, 	255,	255,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	255, 	200,	200,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	244, 	187,	187,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	236, 	174,	174,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	228, 	160,	160,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	219, 	147,	147,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	211, 	134,	134,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	203,	120,	120,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	195,	107,	107,	DoRed | DoGreen | DoBlue,	0},

		{ 0,	186, 	94,	94,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	178, 	80,	80,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	170, 	67,	67,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	162, 	54,	54,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	153, 	40,	40,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	145, 	27,	27,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	137, 	14,	14,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	128,	0,		0,		DoRed | DoGreen | DoBlue,	0}};

static XColor _UIBlueColors [] = {
		{ 0,	152, 	255,	255,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	142, 	238,	243,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	132, 	221,	230,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	122, 	204,	217,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	112, 	187,	205,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	102, 	170,	192,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	92,	153,	179,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	82,	136,	166,	DoRed | DoGreen | DoBlue,	0},

		{ 0,	71, 	119,	154,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	61, 	102,	141,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	51, 	85,	128,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	41, 	68,	115,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	31, 	51,	103,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	21, 	34,	90,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	11,	17,	77,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	0,		0,		64,	DoRed | DoGreen | DoBlue,	0}};

static XColor _UIElevationColors [] = {
		{ 0,	0,		92,	0,		DoRed | DoGreen | DoBlue,	0},
		{ 0,	0,		146,	0,		DoRed | DoGreen | DoBlue,	0},
		{ 0,	0,		200,	0,		DoRed | DoGreen | DoBlue,	0},
		{ 0,	0,		255,	0,		DoRed | DoGreen | DoBlue,	0},
		{ 0,	64,	255,	0,		DoRed | DoGreen | DoBlue,	0},
		{ 0,	129,	255,	0,		DoRed | DoGreen | DoBlue,	0},
		{ 0,	194,	255,	0,		DoRed | DoGreen | DoBlue,	0},
		{ 0,	181,	222,	78,	DoRed | DoGreen | DoBlue,	0},

		{ 0,	200,	200,	112,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	189,	172,	94,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	177,	142,	75,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	166,	112,	56,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	132,	82,	38,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	98,	52,	19,	DoRed | DoGreen | DoBlue,	0},
		{ 0,	63,	22,	0,		DoRed | DoGreen | DoBlue,	0},
		{ 0,	0,		0,		0,		DoRed | DoGreen | DoBlue,	0}};

class UIColorSetList
	{
	public:
		int ColorSet;
		char *Name;
	};

static UIColorSetList	 _UIColorSetList [] = {
				{UIColorStandard,   (char *) "Standard"},
				{UIColorBlueScale,  (char *) "Blues"},
				{UIColorRedScale,   (char *) "Reds"},
				{UIColorGreyScale,  (char *) "Greys"},
				{UIColorElevation,  (char *) "Elevation"},
				{UIColorTrueColor,  (char *) "True"}};

int UIColorSet (int i)

	{
	if (i < 0) return (UIFault);
	if ((unsigned) i < sizeof (_UIColorSetList) / sizeof (UIColorSetList))
		return (_UIColorSetList [i].ColorSet);
	return (UIFault);
	}

char *UIColorSetName (int i)

	{
	if (i < 0) return (NULL);
	if ((unsigned)i < sizeof (_UIColorSetList) / sizeof (UIColorSetList))
		return (_UIColorSetList [i].Name);
	return (NULL);
	}

void _UIColorInitialize (Widget shell)

	{
	int i;
	Colormap cmap;

	XtVaGetValues (shell,XmNcolormap,&cmap,NULL);

	for (i = 0;i < _UIColorNum (_UIStandardColors); ++i)
		{
		_UIStandardColors [i].red	 *= 256;
		_UIStandardColors [i].green *= 256;
		_UIStandardColors [i].blue	 *= 256;
		XAllocColor (XtDisplay (shell),cmap,_UIStandardColors + i);
		}

	for (i = 0;i < _UIColorNum (_UIGreyColors);++i)
		{
		_UIGreyColors [i].red	*= 256;
		_UIGreyColors [i].green	*= 256;
		_UIGreyColors [i].blue	*= 256;
		XAllocColor (XtDisplay (shell),cmap,_UIGreyColors + i);
		}

	for (i = 0;i < _UIColorNum (_UIBlueColors);++i)
		{
		_UIBlueColors [i].red	 *= 256;
		_UIBlueColors [i].green  *= 256;
		_UIBlueColors [i].blue	 *= 256;
		XAllocColor (XtDisplay (shell),cmap,_UIBlueColors + i);
		}

	for (i = 0;i < _UIColorNum (_UIRedColors);++i)
		{
		_UIRedColors [i].red	   *= 256;
		_UIRedColors [i].green  *= 256;
		_UIRedColors [i].blue	*= 256;
		XAllocColor (XtDisplay (shell),cmap,_UIRedColors + i);
		}

	for (i = 0;i < _UIColorNum (_UIElevationColors);++i)
		{
		_UIElevationColors [i].red		 *= 256;
		_UIElevationColors [i].green	 *= 256;
		_UIElevationColors [i].blue	 *= 256;
		XAllocColor (XtDisplay (shell),cmap,_UIElevationColors + i);
		}
/*
	for (r = 0;r < 6; ++r)
		for (g = 0;g < 6; ++g)
			for (b = 0;b < 6; ++b)
				{
				colorRequired.red	  = r * 13107;
				colorRequired.green = g * 13107;
				colorRequired.blue  = b * 13107;
				colorRequired.flags = DoRed | DoGreen | DoBlue;
										  XAllocColor (XtDisplay (shell),cmap,&colorRequired);
				_UIColorSet [32 + r * 36 + g * 6 + b] = (Pixel) colorRequired.pixel;
										  }
	for (i  = 0;i < 16; ++i)
		{
		colorRequired.red	 = colorRequired.green = colorRequired.blue	= i * 4369;
					 colorRequired.flags = DoRed | DoGreen | DoBlue;
		XAllocColor (XtDisplay (shell),cmap,&colorRequired);
		_UIColorSet [i + 16] = (Pixel) colorRequired.pixel;
		}
*/
	}

Pixel UIColor (int colorSet,int color)

	{
	if (color < 0) { fprintf (stderr,"Color out of Range in: UIColor ()\n"); return ((Pixel) NULL);}
	switch (colorSet)
		{
		case UIColorStandard:
			if (color < _UIColorNum(_UIStandardColors)) return (_UIStandardColors [color].pixel);
			else return ((Pixel) NULL);
		case UIColorElevation:
			if (color < _UIColorNum(_UIElevationColors)) return (_UIElevationColors [color].pixel);
			else return ((Pixel ) NULL);
		case UIColorGreyScale :
			if (color < _UIColorNum(_UIGreyColors)) return (_UIGreyColors [color].pixel);
			else return ((Pixel) NULL);
		case UIColorRedScale:
			if (color < _UIColorNum(_UIRedColors)) return (_UIRedColors [color].pixel);
			else return ((Pixel) NULL);
		case UIColorBlueScale:
			if (color < _UIColorNum(_UIBlueColors)) return (_UIBlueColors [color].pixel);
			else return ((Pixel) NULL);
		case UIColorTrueColor:
		default: fprintf (stderr,"Invalid colorset in: UIColor ()\n"); return ((Pixel) NULL);
		}
	}

char *UIStandardColorName (int color)

	{
	if ((color < 0) && (color >= _UIColorNum (_UIStandardColors)))
		{	fprintf (stderr,"Color out of Range in: UIStandardColorName ()\n"); return (NULL);}
	return (_UIStdColorNames [color]);
	}

int UIColorNum (int colorSet)

	{
	switch (colorSet)
		{
		case UIColorStandard:		return (_UIColorNum (_UIStandardColors));
		case UIColorElevation:		return (_UIColorNum (_UIElevationColors));
		case UIColorGreyScale:		return (_UIColorNum (_UIGreyColors));
		case UIColorBlueScale:		return (_UIColorNum (_UIBlueColors));
		case UIColorRedScale:		return (_UIColorNum (_UIRedColors));
		case UIColorTrueColor:
		default:			 fprintf (stderr,"Invalid colorset in: UIColorNum ()\n"); return (UIFault);
		}
	}
