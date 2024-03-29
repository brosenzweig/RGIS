/******************************************************************************

GHAAS RiverGIS Plot Utility V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGPDrawVecPoint.C

balazs.fekete@unh.edu

*******************************************************************************/

#include<cm.h>
#include<rgisPlot.H>
#include<DBif.H>

DBInt RGPDrawVecPoint (DBInt mode, DBInt *entryNum, DBObjData *pntData)

	{
	DBInt ret, pntID, pntColor, pntSize, legendNum, i, buffLen;
	DBInt symbolMode = 0, legendMode = 0, legendPos = 0;
	char charBuffer [RGPBufferSIZE], errorMsg [RGPBufferSIZE], legendTitle [RGPBufferSIZE], *ptr;
	const char *symbolModes [] = { "default", "custom", NULL };
	const char *legendModes [] = { "on", "off", NULL };
	const char *legendPositions [] = { "ll", "lr", "ul","ur", NULL };
	float *xCoord = (float *) NULL, *yCoord = (float *) NULL;
	float llX, llY, urX, urY, lSpacing, lPosX, lPosY;
	float tWidth, tHeight;
	float charSize;
	DBCoordinate coord;
	DBObjTable  *symbols;
	DBObjRecord *symRec;
	DBVPointIF *pntIF = new DBVPointIF (pntData);
	DBObjRecord *record;
	DBObjTableField *symbolIDFld;
	DBObjTableField *foregroundFld;
	DBObjTableField *backgroundFld;
	DBObjTableField *styleFld;

	if ((symbols = pntData->Table (DBrNSymbols)) == (DBObjTable *) NULL)
	{ CMmsgPrint (CMmsgUsrError, "Corrupt point coverage"); return (DBFault); }
	if ((symbolIDFld   = symbols->Field (DBrNSymbolID))   == (DBObjTableField *) NULL)
	{ CMmsgPrint (CMmsgUsrError, "Corrupt point coverage"); return (DBFault); }
	if ((foregroundFld = symbols->Field (DBrNForeground)) == (DBObjTableField *) NULL)
	{ CMmsgPrint (CMmsgUsrError, "Corrupt point coverage"); return (DBFault); }
	if ((backgroundFld = symbols->Field (DBrNBackground)) == (DBObjTableField *) NULL)
	{ CMmsgPrint (CMmsgUsrError, "Corrupt point coverage"); return (DBFault); }
	if ((styleFld      = symbols->Field (DBrNStyle))      == (DBObjTableField *) NULL)
	{ CMmsgPrint (CMmsgUsrError, "Corrupt point coverage"); return (DBFault); }

	do	{
		RGPPrintMessage (mode,entryNum,"Symbol mode [default|custom]:");
		if (fgets (charBuffer,sizeof (charBuffer) - 2,stdin) == (char *) NULL) { ret = DBFault; goto Stop; }
		if ((strlen (charBuffer) > 0) && (charBuffer [strlen (charBuffer) - 1] == '\n'))
			charBuffer [strlen (charBuffer) - 1] = '\0';
		if ((symbolMode = CMoptLookup (symbolModes,charBuffer,true)) != DBFault) break;
		sprintf (errorMsg,"Invalid Symbol Mode [%s]",charBuffer);
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
		{
		RGPPrintMessage (mode,entryNum,"Legend position,title [ll|lr|ul|ur,title]:");
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
				strcpy (legendTitle,ptr);
				}
			else
				{
				sprintf (errorMsg,"Invalid legend position [%s]",charBuffer);
				if (RGPPrintError (mode,*entryNum,errorMsg)) { ret = DBFault; goto Stop; }
				}
			}
		}

	switch (symbolMode)
		{
		default:
		case 0:	break;
		case 1:	break;
		}
	cpgqci (&pntColor);
	cpgqlw (&pntSize);

	if ((xCoord = (float *) calloc (pntIF->ItemNum (),sizeof (float))) == (float *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Memory Reallocation Error in: %s %d",__FILE__,__LINE__); delete pntIF; return (DBFault); }
	if ((yCoord = (float *) calloc (pntIF->ItemNum (),sizeof (float))) == (float *) NULL)
		{ CMmsgPrint (CMmsgAppError, "Memory Reallocation Error in: %s %d",__FILE__,__LINE__); free (xCoord); delete pntIF; return (DBFault); }

	cpgslw (3);
	for (pntID = 0;pntID < pntIF->ItemNum (); ++pntID)
		{
		record = pntIF->Item (pntID);
		cpgsci (pntIF->ItemForeground (record));
		coord = pntIF->Coordinate (record);
		xCoord [pntID] = coord.X;
		yCoord [pntID] = coord.Y;
		cpgpt1 (xCoord [pntID],yCoord [pntID],-1);
		}
	if (legendMode == 0)
		{
		legendNum = symbols->ItemNum ();
		cpgqwin (&llX,&urX,&llY,&urY);
		cpgqch (&charSize);
		cpgsch (1.0);

		sprintf (charBuffer,"Moderalt szoveg");
		cpgqtxt (llX,llY,0.0,1.0,charBuffer,xCoord,yCoord);
		tWidth  = xCoord[2] - xCoord[1];
		tHeight = yCoord[1] - yCoord[0];
		lSpacing = tHeight * 1.5;
		tWidth = tWidth + lSpacing;

		lPosX = lPosY = lSpacing;
		lPosX = (legendPos & 0x01)           == 0x01 ? urX - lPosX - lSpacing - tWidth          : llX + lPosX;
		lPosY = ((legendPos >> 0x01) & 0x01) == 0x01 ? urY - lPosY - (legendNum + 1) * lSpacing : llY + lPosY;
		cpgsci (0);
		cpgrect (lPosX,lPosX + lSpacing + tWidth,lPosY,lPosY + (legendNum + 1) *lSpacing);

		xCoord [0] = lPosX + lSpacing;
		cpgsci (1);
		symbols->ListSort (symbolIDFld);
		i = 0;
		for (symRec = symbols->First ();symRec != (DBObjRecord *) NULL;symRec = symbols->Next ())
			{
			yCoord [0] = lPosY + ((float) i + 0.5) * lSpacing;
			cpgslw (1);
			cpgsci (1);
			cpgptxt (xCoord [0] + 0.5 * lSpacing, yCoord[0] - 0.5 * tHeight,0.0,0.0,symRec->Name ());
			cpgsci (foregroundFld->Int (symRec));
			cpgpt (1,xCoord,yCoord,-6);
			i++;
			}
		cpgsci (1);
		yCoord [0] = lPosY + ((float) i + 0.5) * lSpacing;
		cpgptxt (lPosX + 0.5 * (lSpacing + tWidth),yCoord[0] - 0.5 * tHeight,0.0,0.5,legendTitle);
		cpgsch (charSize);
		}

Stop:
	cpgsci (pntColor);
	cpgslw (pntSize);
	if (xCoord != (float *) NULL) free (xCoord);
	if (yCoord != (float *) NULL) free (yCoord);
	delete pntIF;
	return (ret);
	}
