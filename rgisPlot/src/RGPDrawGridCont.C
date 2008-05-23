/******************************************************************************

GHAAS RiverGIS Plot Utility V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

RGPDrawGridCont.C

balazs.fekete@unh.edu

*******************************************************************************/

#include<cm.h>
#include<rgisPlot.H>
#include<DBio.H>

static RGPColorMapEntry _RGPGreyColors [] = {
		{ 255, 	255,	255},
		{ 255, 	255,	255},	
		{ 32,		32,	32}};

static RGPColorMapEntry _RGPBlueColors [] = {
		{ 240,	240,	240},
		{ 255, 	255,	255},	
		{ 0, 		255,	255},	
		{ 0, 		0,		255},	
		{ 0, 		0,		165},	
		{ 0,		0,		100}};	

static RGPColorMapEntry _RGPRedColors [] = {
		{ 240,	240,	240},
		{ 255, 	255,	255},	
		{ 255, 	0,		0},	
		{ 160, 	0,		0},	
		{ 80, 	0,	 	0}};	

static RGPColorMapEntry _RGPBlueRedColors [] = {
		{ 240,	240,	240},
		{ 0,		0,		100},	
		{ 0, 		0,		255},	
		{ 0, 		255,	255},	
		{ 255, 	255,	255},	
		{ 255, 	0,		0},	
		{ 200, 	0,		0},	
		{ 60, 	0,	 	0}};	

static RGPColorMapEntry _RGPElevationColors [] = {
		{ 0,		240,	255},
		{ 0,		92,	0},	
		{ 0,		255,	0},	
		{ 255,	255,	0},	
		{ 200,	200,	112},	
		{ 120,	40,	0},
		{ 60,		0,		0}};

#define RGPColorNum(colorList) (int) (sizeof (colorList) / sizeof (RGPColorMapEntry))
			
		

DBInt RGPDrawGridContinuous (DBInt mode, DBInt *entryNum, DBObjData *grdData)

	{
	DBInt r, g, b;
	DBInt ret, shadeNum, colorNum, scaleMode = 0;
	DBPosition pos;
	char charBuffer [RGPBufferSIZE], errorMsg [RGPBufferSIZE];
	const char *colorMaps [] = { "default", "grey", "blue", "red", "blue-red", "elevation", "custom", NULL };
	char colorMap;
	const char *scaleModes [] = { "linear", "logarithmic", "square-root", NULL };
	RGPColorMapEntry *customColors;
	DBFloat value;
	float *array, translation [6], min, max, unit = 1.0;
	DBRegion extent = grdData->Extent ();
	DBObjRecord *layerRec;
	DBGridIO *gridIO = new DBGridIO (grdData);
	
	do {
		RGPPrintMessage (mode,entryNum,"Grid Layer:");
		if (fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL) { ret = DBFault; goto Stop; }
		if ((strlen (charBuffer) > 0) && (charBuffer [strlen (charBuffer) - 1] == '\n'))
			charBuffer [strlen (charBuffer) - 1] = '\0';
		if ((layerRec = gridIO->Layer (charBuffer)) != NULL) break;
		sprintf (errorMsg,"Invalid grid layer [%s]",charBuffer);
		if (RGPPrintError (mode,*entryNum,errorMsg)) { ret = DBFault; goto Stop;}
 		} while (true);
		
	do	{
		RGPPrintMessage (mode,entryNum,"Colormap [default|grey|blue|red-blue|elevation|custom]:");
		if (fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL) { ret = DBFault; goto Stop; }
		if ((strlen (charBuffer) > 0) && (charBuffer [strlen (charBuffer) - 1] == '\n'))
			charBuffer [strlen (charBuffer) - 1] = '\0';
		if ((colorMap = CMoptLookup (colorMaps,charBuffer,true)) != DBFault) break;
		sprintf (errorMsg,"Invalid colormap [%s]",charBuffer);
		if (RGPPrintError (mode,*entryNum,errorMsg)) { ret = DBFault; goto Stop; }
		} while (true);
	switch (colorMap)
		{
		default:
		case 0:
			switch (grdData->Flags () & DBDataFlagDispModeFlags)
				{
				case DBDataFlagDispModeContStandard:
				case DBDataFlagDispModeContGreyScale:
					shadeNum = RGPSetColorMap (_RGPGreyColors,		RGPColorNum (_RGPGreyColors));		break;
				case DBDataFlagDispModeContBlueScale:
					shadeNum = RGPSetColorMap	(_RGPBlueColors,		RGPColorNum (_RGPBlueColors));		break;
				case DBDataFlagDispModeContBlueRed:
					shadeNum = RGPSetColorMap (_RGPBlueRedColors,	RGPColorNum (_RGPBlueRedColors));	break;
				case DBDataFlagDispModeContElevation:
					shadeNum = RGPSetColorMap (_RGPElevationColors,	RGPColorNum (_RGPElevationColors));	break;	
				}
			break;
		case 1:	shadeNum = RGPSetColorMap (_RGPGreyColors,		RGPColorNum (_RGPGreyColors));		break;
		case 2:	shadeNum = RGPSetColorMap (_RGPBlueColors,		RGPColorNum (_RGPBlueColors));		break;
		case 3:	shadeNum = RGPSetColorMap (_RGPRedColors,			RGPColorNum (_RGPRedColors));			break;
		case 4:	shadeNum = RGPSetColorMap (_RGPBlueRedColors,	RGPColorNum (_RGPBlueRedColors));	break;
		case 5:	shadeNum = RGPSetColorMap (_RGPElevationColors,	RGPColorNum (_RGPElevationColors));	break;
		case 6:
			colorNum = 0;
			if ((customColors = (RGPColorMapEntry *) calloc (1,sizeof (RGPColorMapEntry))) == (RGPColorMapEntry *) NULL)
				{ perror ("Memory Allocation Error in: RGPDrawGridContinuous ()"); delete gridIO; return (DBFault); }
			do	{
				RGPPrintMessage (mode,entryNum,"Background Shade [red,green,blue]:");
				if (fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL) { ret = DBFault; goto Stop; }
				if (sscanf (charBuffer,"%d,%d,%d",&r,&g,&b) == 3)
					{
					customColors [colorNum].Red = r;
					customColors [colorNum].Green = g;
					customColors [colorNum].Blue = b;
					colorNum++;
					break;
					}
				else	if (RGPPrintError (mode,*entryNum,"Background Color Entry Error")) { ret = DBFault; goto Stop; }
				} while (true);
			do	{
				RGPPrintMessage (mode,entryNum,"Color Shade [red,green,blue]:");
				if (fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL) { ret = DBFault; goto Stop; }
				if (sscanf (charBuffer,"%d,%d,%d",&r,&g,&b) == 3)
					{
					if ((customColors = (RGPColorMapEntry *) realloc (customColors,(colorNum + 1) * sizeof (RGPColorMapEntry))) == (RGPColorMapEntry *) NULL)
						{ perror ("Memory Allocation Error in: RGPDrawGridContinuous ()"); delete gridIO; return (DBFault); }
					customColors [colorNum].Red = r;
					customColors [colorNum].Green = g;
					customColors [colorNum].Blue = b;
					colorNum++;
					}
				else
					{
					if (colorNum < 3)
						{ if (RGPPrintError (mode,*entryNum,"Color Shade Entry Error")) { ret = DBFault; goto Stop; }}
					else	break;
					}
				} while (true);
			shadeNum = RGPSetColorMap (customColors,colorNum);
			free (customColors);
			break;
		}
	if (shadeNum == DBFault)	{ RGPPrintError (mode,*entryNum,"Colormap Initialization Error") ; ret = DBFault; goto Stop; }
	do	{
		RGPPrintMessage (mode,entryNum,"Scale mode [linear|logarithmic]:");
		if (fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL) { ret = DBFault; goto Stop; }
		if ((strlen (charBuffer) > 0) && (charBuffer [strlen (charBuffer) - 1] == '\n'))
			charBuffer [strlen (charBuffer) - 1] = '\0';
		if ((scaleMode = CMoptLookup (scaleModes,charBuffer,true)) != DBFault) break;
		sprintf (errorMsg,"Invalid scale mode [%s]",charBuffer);
		if (RGPPrintError (mode,*entryNum,errorMsg)) { ret = DBFault; goto Stop; }
		} while (true);
	do	{
		RGPPrintMessage (mode,entryNum,"Value range [minimum,maximum]:");
		if (fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL) { ret = DBFault; goto Stop; }
		if (sscanf (charBuffer,"%f,%f",&min,&max) == 2)	break;
		else	if (RGPPrintError (mode,*entryNum,"Value range input error")) { ret = DBFault; goto Stop; }
		} while (true);
	if ((array = (float *) calloc (gridIO->RowNum () * gridIO->ColNum (),sizeof (float))) == (float *) NULL)
		{ perror ("Memory allocation error in: RGPDrawGridContinuous ()"); ret = DBFault; goto Stop; }
	for (pos.Row = 0;pos.Row < gridIO->RowNum ();++pos.Row)
		for (pos.Col = 0;pos.Col < gridIO->ColNum ();++pos.Col)
			if (gridIO->Value (layerRec,pos,&value))
					{
					value = value > min ? value : min;
					value = value < max ? value : max;
					switch (scaleMode)
						{
						default:
						case 0:	array [pos.Row * gridIO->ColNum () + pos.Col] = value; break;
						case 1:	array [pos.Row * gridIO->ColNum () + pos.Col] = log10 (value); break;
						case 2:	array [pos.Row * gridIO->ColNum () + pos.Col] = sqrt (value); break;
						}
					}
			else 	array [pos.Row * gridIO->ColNum () + pos.Col] = min - (max - min) / (float) shadeNum;

	translation [0] = extent.LowerLeft.X - gridIO->CellWidth () / 2.0;
	translation [1] = gridIO->CellWidth ();
	translation [2] = 0.0;
	translation [3] = extent.LowerLeft.Y - gridIO->CellHeight () / 2.0;
	translation [4] = 0.0;
	translation [5] = gridIO->CellHeight ();
	RGPPrintMessage (mode,entryNum,"Unit:");
	if ((fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL) || (sscanf (charBuffer,"%f",&unit) != 1))
		{ RGPPrintError (mode,*entryNum,"Unit input error"), ret = DBFault; goto Stop; }
	
	RGPPrintMessage (mode,entryNum,"Unit label:");
	if (fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL)
		{ RGPPrintError (mode,*entryNum,"Unit label input error"); ret = DBFault; goto Stop; }
	if ((strlen (charBuffer) > 0) && (charBuffer [strlen (charBuffer) - 1] == '\n'))
			charBuffer [strlen (charBuffer) - 1] = '\0';

/*	cpgsitf (scaleMode); */
	switch (scaleMode)
		{
		default:
		case 0:
			min = min - (max - min) / (float) shadeNum;
			max = max + (max - min) / (float) shadeNum;
			cpgwedg ("BI",0.0,5.0,min * unit,max * unit,charBuffer);
			cpgimag (array,pos.Col,pos.Row,1,pos.Col,1,pos.Row,min,max,translation);
			break;
		case 1:
			cpgwedg ("BI",0.0,5.0,log10 (min * unit),log10 (max * unit),charBuffer);
			cpgimag (array,pos.Col,pos.Row,1,pos.Col,1,pos.Row,log10 (min),log10 (max),translation);
			break;
		case 2:
			cpgwedg ("BI",0.0,5.0,sqrt (min * unit),sqrt (max * unit),charBuffer);
			cpgimag (array,pos.Col,pos.Row,1,pos.Col,1,pos.Row,sqrt (min),sqrt (max),translation);
			break;
		}

Stop:
	delete gridIO;
	return (ret);
	}
