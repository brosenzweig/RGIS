/******************************************************************************

GHAAS RiverGIS Plot Utility V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

RGPDrawNetwork.C

balazs.fekete@unh.edu

*******************************************************************************/

#include<cm.h>
#include<rgisPlot.H>
#include<DBio.H>

#define RGPWidthConstant 15.0

DBInt RGPDrawNetwork (DBInt mode, DBInt *entryNum, DBObjData *netData)

	{
	DBInt ret, recID, i, basinID, legendNum = 0, buffLen;
	DBInt lineStyle, lineColor, lineWidth, foreground, width;
	DBInt symbolMode, scaleMode, legendMode, legendPos;
	char charBuffer [RGPBufferSIZE], errorMsg [RGPBufferSIZE], legendTitle [RGPBufferSIZE], *ptr;
	const char *symbolModes [] = { "default", "basin", "symbol", "custom", NULL };
	const char *scaleModes  [] = { "linear", "log10", NULL };
	const char *legendModes  [] = { "on", "off", NULL };
	const char *legendPositions  [] = { "ll", "lr", "ul","ur", NULL };
	float xCoord[4], yCoord[4], minValue, maxValue, wValue, *lValues;
	float llX, llY, urX, urY, lSpacing, lPosX, lPosY;
	float tWidth, tHeight;
	float charSize;
	DBCoordinate fromCoord, toCoord;
	DBObjRecord *cellRec, *basinRec;
	DBObjTable *cellTable = netData->Table (DBrNCells);
	DBObjTableField *widthFLD;
	DBNetworkIO *netIO = new DBNetworkIO (netData);

	cpgqls (&lineStyle);
	cpgqlw (&lineWidth);
	cpgqci (&lineColor);
	
	do	{
		RGPPrintMessage (mode,entryNum,"Symbol mode [default|basin|symbol|uniform|custom]:");
		if (fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL) { ret = DBFault; goto Stop; }
		if ((strlen (charBuffer) > 0) && (charBuffer [strlen (charBuffer) - 1] == '\n'))
			charBuffer [strlen (charBuffer) - 1] = '\0';
		if ((symbolMode = CMoptLookup (symbolModes,charBuffer,true)) != DBFault) break;
		sprintf (errorMsg,"Invalid Symbol Mode [%s]",charBuffer);
		if (RGPPrintError (mode,*entryNum,errorMsg)) { ret = DBFault; goto Stop; }
		} while (true);
	do	{
		RGPPrintMessage (mode,entryNum,"Width field [field name]:");
		if (fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL) { ret = DBFault; goto Stop; }
		if ((strlen (charBuffer) > 0) && (charBuffer [strlen (charBuffer) - 1] == '\n'))
			charBuffer [strlen (charBuffer) - 1] = '\0';
		if ((widthFLD = cellTable->Field (charBuffer)) != (DBObjTableField *) NULL)	break;
		sprintf (errorMsg,"Invalid width field [%s]",charBuffer);
		if (RGPPrintError (mode,*entryNum,errorMsg)) { ret = DBFault; goto Stop; }
		} while (true);
	do	{
		RGPPrintMessage (mode,entryNum,"Scale mode,Minimum, Maximum [linear|log,min,max]:");
		while (fgets (charBuffer,sizeof (charBuffer) - 1,stdin) == (char *) NULL);
		if ((strlen (charBuffer) > 0) && (charBuffer [strlen (charBuffer) - 1] == '\n'))
			charBuffer [strlen (charBuffer) - 1] = '\0';
		for (i = 0;i < (int) strlen (charBuffer);++i) if (charBuffer [i] == ',') break;
		if (i < (int) strlen (charBuffer))
			{
			charBuffer [i] = '\0';
			if ((scaleMode = CMoptLookup (scaleModes,charBuffer,true)) != DBFault)
				{
				if ((sscanf (charBuffer + i + 1,"%f,%f",&minValue, &maxValue) == 2) && (fabs (maxValue - minValue) > 0.0)) break;
				if (RGPPrintError (mode,*entryNum,"Width range input error")) { ret = DBFault; goto Stop; }
				}
			sprintf (errorMsg,"Invalid scaling mode [%s]",charBuffer);
			if (RGPPrintError (mode,*entryNum,errorMsg)) { ret = DBFault; goto Stop; }
			}
		sprintf (errorMsg,"Invalid scaling entry [%s]",charBuffer);
		if (RGPPrintError (mode,*entryNum,errorMsg)) { ret = DBFault; goto Stop; }
		} while (true);
	do	{
		RGPPrintMessage (mode,entryNum,"Legend display [on|off]:");
		if (fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL) { ret = DBFault; goto Stop; }
		if ((strlen (charBuffer) > 0) && (charBuffer [strlen (charBuffer) - 1] == '\n'))
			charBuffer [strlen (charBuffer) - 1] = '\0';
		if ((legendMode = CMoptLookup (legendModes,charBuffer,true)) != DBFault) break;
		sprintf (errorMsg,"Invalid legend display mode [%s]",charBuffer);
		if (RGPPrintError (mode,*entryNum,errorMsg)) { ret = DBFault; goto Stop; }
		} while (true);
	if (legendMode == 0)
		do	{
			RGPPrintMessage (mode,entryNum,"Legend position,title, and values [ll|lr|ul|ur,title,value1,value2,...,valueN]:");
			while (fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL);
			if ((strlen (charBuffer) > 0) && (charBuffer [strlen (charBuffer) - 1] == '\n'))
				charBuffer [strlen (charBuffer) - 1] = '\0';
			buffLen = strlen (charBuffer) - 1;
			for (i = 0;i < buffLen; ++i) if (charBuffer [i] == ',') break;
			if (i < buffLen)
				{
				charBuffer [i] = '\0';
				if ((legendPos = CMoptLookup (legendPositions,charBuffer,true)) != DBFault)
					{
					ptr = charBuffer + i + 1;
					for (i = i + 1;i < buffLen; ++i) if (charBuffer [i] == ',') break;
					if (i < buffLen)
						{
						charBuffer [i] = '\0';
						strcpy (legendTitle,ptr);
						do	{
							ptr = charBuffer + i + 1;
							for (i = i + 1;i < buffLen;++i) if (charBuffer [i] == ',') break;
							charBuffer [i] = '\0';
							if (sscanf (ptr,"%f",&wValue) == 1)
								{
								if (wValue <= minValue) continue;
								lValues = legendNum > 0 ? (float *) realloc (lValues,(legendNum + 1) * sizeof (float)) :
																  (float *) calloc (1,sizeof (float));
								if (lValues  == (float *) NULL)
									{ perror ("Memory Allocation Error in: RGPDrawNetwork ()"); return (DBFault); }
								lValues[legendNum++] = wValue;
								}
							else	break;
							} while (true);
						if (legendNum > 0) break;
						if (RGPPrintError (mode,*entryNum,"Legend values input error")) { ret = DBFault; goto Stop; }
						}
					else
						{
						sprintf (errorMsg,"Invalid legend title [%s]",charBuffer);
						if (RGPPrintError (mode,*entryNum,errorMsg)) { ret = DBFault; goto Stop; }
						}
					}
				else
					{
					sprintf (errorMsg,"Invalid legend position [%s]",charBuffer);
					if (RGPPrintError (mode,*entryNum,errorMsg)) { ret = DBFault; goto Stop; }
					}
				}
			sprintf (errorMsg,"Invalid legend values entry [%s]",charBuffer);
			if (RGPPrintError (mode,*entryNum,errorMsg)) { ret = DBFault; goto Stop; }
			} while (true);

	if (minValue > maxValue) { wValue = minValue; minValue = maxValue; maxValue = wValue; }
	cpgsls (1);
	basinID = DBFault;
	width = DBFault;
	for (recID = 0;recID < netIO->CellNum ();++recID)
		{
		cellRec = netIO->Cell (recID);
		wValue = widthFLD->Float (cellRec);
		if (CMmathEqualValues (wValue,widthFLD->FloatNoData ())) continue;
		if (wValue <= minValue) continue;
		if (wValue > maxValue) wValue = maxValue; 

		switch (scaleMode)
			{
			default:
			case 0: 	wValue = (wValue - minValue) / (maxValue - minValue); break;
			case 1:	wValue = log10 (wValue - minValue) / log10 (maxValue - minValue);	break;
			}
		wValue = wValue * RGPWidthConstant;
	
		width = (int) (ceil ((double) wValue));
		if (width < 1) width = 1;
		cpgslw (width);

		if (basinID != netIO->CellBasinID (cellRec))
			{
			basinRec = netIO->Basin (cellRec);
			basinID = basinRec->RowID ();
			switch (symbolMode)
				{
				default:
				case 1: foreground = netIO->Color(basinRec); break;
				case 2: foreground = netIO->ItemForeground (basinRec); break;
				case 3: foreground = 1; break;
				}
			cpgsci (foreground);
			}

		fromCoord = netIO->Center (cellRec);
		toCoord = fromCoord + netIO->Delta (cellRec);
		xCoord [0] = fromCoord.X;
		yCoord [0] = fromCoord.Y;
		xCoord [1] = toCoord.X;
		yCoord [1] = toCoord.Y;
		cpgline (2,xCoord,yCoord);
		}
	if (legendMode == 0)
		{
		cpgqwin (&llX,&urX,&llY,&urY);
		cpgqch (&charSize);
		cpgsch (1.0);

		sprintf (charBuffer,DBMathFloatAutoFormat (lValues[legendNum - 1] * 100.0),lValues[legendNum - 1]);
		cpgqtxt (llX,llY,0.0,1.0,charBuffer,xCoord,yCoord);
		tWidth  = xCoord[2] - xCoord[1];
		tHeight = yCoord[1] - yCoord[0];
		lSpacing = tHeight * 1.5;
		tWidth = tWidth + 3.0 *lSpacing;

		lPosX = lPosY = lSpacing;
		lPosX = (legendPos & 0x01)           == 0x01 ? urX - lPosX - lSpacing - tWidth          : llX + lPosX;
		lPosY = ((legendPos >> 0x01) & 0x01) == 0x01 ? urY - lPosY - (legendNum + 1) * lSpacing : llY + lPosY;
		cpgsci (0);
		cpgrect (lPosX,lPosX + lSpacing + tWidth,lPosY,lPosY + (legendNum + 1) *lSpacing);
		xCoord [0] = lPosX + 0.5 * lSpacing;
		xCoord [1] = lPosX + 2.5 * lSpacing;
		cpgsci (1);
		for (i = 0;i < legendNum;++i)
			{
			switch (scaleMode)
				{
				default:
				case 0: 	wValue = (lValues [i] - minValue) / (maxValue - minValue); break;
				case 1:	wValue = log10 (lValues [i] - minValue) / log10 (maxValue - minValue);	break;
				}
			wValue = wValue * RGPWidthConstant;
			width = (int) (ceil ((double) wValue));
			if (width < 1) width = 1;
			cpgslw (width);

			yCoord [0] = yCoord [1] = lPosY + ((float) i + 0.5) * lSpacing;
			cpgline (2,xCoord,yCoord);
//			sprintf (charBuffer,DBMathFloatAutoFormat (lValues[legendNum - 1] * 100.0),lValues[i]);
			sprintf (charBuffer,"%4.0f",lValues[i]);
			cpgslw (1);
			cpgptxt (xCoord [1] + 0.5 * lSpacing,yCoord[0] - 0.5 * tHeight,0.0,0.0,charBuffer);
			}
		yCoord [0] = lPosY + ((float) i + 0.5) * lSpacing;
		cpgptxt (lPosX + 0.5 * (lSpacing + tWidth),yCoord[0] - 0.5 * tHeight,0.0,0.5,legendTitle);
		cpgsch (charSize);
		}

Stop:
	cpgsls (lineStyle);
	cpgslw (lineWidth);
	cpgsci (lineColor);
	if (legendNum > 0) free (lValues);
	delete netIO;
	return (ret);
	}
