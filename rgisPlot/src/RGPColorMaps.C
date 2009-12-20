/******************************************************************************

GHAAS RiverGIS Plot Utility V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

RGPColorMaps.C

balazs.fekete@unh.edu

*******************************************************************************/

#include<rgisPlot.H>

#define RGPStartColorIndex 32
#define RGPColorShadeNum 0x7f

DBInt RGPSetColorMap (RGPColorMapEntry *colorMap,DBInt colorNum)

	{
	DBInt color, shade, shadeNum = RGPColorShadeNum;
	float red, red0, red1, green, green0, green1, blue, blue0, blue1, mColor;

	if (colorNum < 3) return (DBFault);

	red	= (float) colorMap [0].Red / 255.0;
	green = (float) colorMap [0].Green / 255.0;
	blue	= (float) colorMap [0].Blue / 255.0;
	cpgscr (RGPStartColorIndex,red, green, blue);

	for (shade = 0;shade < shadeNum;++shade)
		{
		color = (DBInt) (floor ((double) (colorNum - 2) * (double) shade / (double) (shadeNum - 1)));
		red0		= (float) colorMap [color + 1].Red		/ 255.0;
		green0	= (float) colorMap [color + 1].Green 	/ 255.0;
		blue0		= (float) colorMap [color + 1].Blue 	/ 255.0;
		red1		= (float) colorMap [color + 2].Red		/ 255.0;
		green1	= (float) colorMap [color + 2].Green	/ 255.0;
		blue1		= (float) colorMap [color + 2].Blue		/ 255.0;
		mColor = shade - ((float) color * (float) (shadeNum - 1) / (float) (colorNum - 2));

		red    = red0	 + (red1 - red0)		* (float) mColor / ((float) shadeNum / (float) (colorNum - 2));
		green  = green0 + (green1 - green0)	* (float) mColor / ((float) shadeNum / (float) (colorNum - 2));
		blue   = blue0  + (blue1  - blue0)	* (float) mColor / ((float) shadeNum / (float) (colorNum - 2));
		cpgscr (RGPStartColorIndex + 1 + shade, red, green, blue);
		}
	red	= (float) colorMap [colorNum - 1].Red / 255.0;
	green = (float) colorMap [colorNum - 1].Green / 255.0;
	blue	= (float) colorMap [colorNum - 1].Blue / 255.0;
	cpgscr (RGPStartColorIndex + 1 + shade,red, green, blue);
	
	cpgscir (RGPStartColorIndex,RGPStartColorIndex + shadeNum + 1);
	return (shadeNum + 2);
	}

static RGPColorMapEntry _RGPStandardColors [] = {
		{ 0, 		0, 	0},		/* Black	*/
		{ 255,	0, 	0},		/* Red	*/
		{ 0, 		255,	0},		/* Green	*/
		{ 0, 		0, 	255},		/* Blue	*/
		{ 0, 		255,	255},		/* Cyan	*/
		{ 255,	0, 	255},		/* Magenta	*/
		{ 255,	255,	0},		/* Yellow	*/
		{ 255,	165,	0},		/* Orange	*/
		{ 50,		205,	50},		/* LimeGreen	*/
		{ 0,		250,	154},		/* MediumSpringGreen */
		{ 72,		61,	139},		/* DarkSlateBlue	*/
		{ 123,	104,	238},		/* MediumSlateBlue	*/
		{ 165,	42,	42},		/* Brown	*/
		{ 105,	105,	105},		/* DimGray	*/
		{ 201,	201,	201}};	/* LightGray	*/

void RGPInitPenColors ()

	{
	DBInt color;
	float red, green, blue;

	for (color = 0;color < 	(DBInt) (sizeof (_RGPStandardColors) / sizeof (RGPColorMapEntry));color++)
		{
		red   = (float) _RGPStandardColors [color].Red		/ 255.0;
		green = (float) _RGPStandardColors [color].Green	/ 255.0;
		blue  = (float) _RGPStandardColors [color].Blue		/ 255.0;
		cpgscr (color + 1,red, green, blue);
		} 
	}
