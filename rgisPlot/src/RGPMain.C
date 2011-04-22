/******************************************************************************

GHAAS RiverGIS Plot Utility V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RGPMain.C

balazs.fekete@unh.edu

*******************************************************************************/

#include<cm.h>
#include<rgisPlot.H>

void RGPPrintMessage (DBInt mode,DBInt *entryNum, const char *message)
	{ if (mode == 0) { printf ("%s",message); fflush (stdout); } (*entryNum)++; }

DBInt RGPPrintError (DBInt mode,DBInt entryNum, const char *message)
	{
	if (mode == 0) { printf ("%s\n",message); fflush (stdout); return (false); }
	else { CMmsgPrint (CMmsgUsrError,"%s in line: %d\n",message,entryNum); return (true); }
	}

int main (int argc,char **argv)

	{
	int argPos, argNum = argc;
	char charBuffer [RGPBufferSIZE], panelTitle [RGPBufferSIZE], *outFile = (char *) "rgisplot";
	int panelRow, panelCol, panelRowNum,panelColNum, defaultLW;
	DBInt dataNum, entryNum = 0;
	DBInt ret, mode = 0, device = 0, format = 0, layout = 0;
	float x0, y0, x1, y1, pWidth = -1.0, pHeight = -1.0;
	DBObjData *dbData;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-m","--mode"))
			{
			const char *modes [] = { "interactive", "batch", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing mode!");    return (CMfailed); }
			if ((mode = CMoptLookup (modes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid mode %s",argv [argPos]); goto Usage; }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-d","--device"))
			{
			const char *devices [] = { "screen", "file", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing device!");  return (CMfailed); }
			if ((device = CMoptLookup (devices,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid device %s",argv [argPos]); goto Usage; }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-p","--psize"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing psize!");   return (CMfailed); }
			if ((argv [argPos] == (char *) NULL) || (sscanf (argv [argPos],"%f,%f",&pWidth,&pHeight) != 2))
				{ CMmsgPrint (CMmsgUsrError,"Invalid page size %s",argv [argPos]); goto Usage; }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-f","--format"))
			{
			const char *formats [] = { "eps", "gif", "ppm", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing format!");  return (CMfailed); }
			if ((format = CMoptLookup (formats,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid format %s",argv [argPos]); goto Usage; }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-l","--layout"))
			{
			const char *layouts [] = { "portrait","landscape", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing layout!");  return (CMfailed); }
			if ((layout = CMoptLookup (layouts,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid layout %s",argv [argPos]); goto Usage; }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-o","--output"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing output!");  return (CMfailed); }
			if (argv [argPos] == (char *) NULL)
				{ CMmsgPrint (CMmsgUsrError,"Invalid output file"); goto Usage; }
			outFile = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			Usage:
			CMmsgPrint (CMmsgUsrError,"Usage: rgisPlot [-m <>] [-d <>] [-f <>] [-l <>] [-o <>] -h");
			CMmsgPrint (CMmsgUsrError,"                 -m, --mode <interactive | batch>");
			CMmsgPrint (CMmsgUsrError,"                 -d, --device <screen | file>");
			CMmsgPrint (CMmsgUsrError,"                 -p, --psize width,height");
			CMmsgPrint (CMmsgUsrError,"                 -f, --format <eps | gif>");
			CMmsgPrint (CMmsgUsrError,"                 -l, --layout <landscape | portrait>");
			CMmsgPrint (CMmsgUsrError,"                 -o, --output <filename>");
			argNum = CMargShiftLeft (argPos,argv,argNum);
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]); return (CMfailed); }
		argPos++;

		}

	switch (device)
		{
		case 0: cpgopen ("/XWINDOW");	break;
		case 1:
			{
			char *formatStrings [] = { (char *) "CPS", (char *) "GIF", (char *) "PPM" };
			sprintf (charBuffer,layout == 0 ? "%s/V%s" : "%s/%s", outFile, formatStrings [format]);
			cpgopen (charBuffer);
			} break;
		default: return (CMfailed);
		}
	cpgscrn (0,"WHITE",&ret);
	if ((pWidth > 0.0) && (pHeight > 0.0)) cpgpap (pWidth, pHeight / pWidth);

	do	{
		RGPPrintMessage (mode,&entryNum,"Panel Layout [horizontal,vertical]:");
		while (fgets (charBuffer,sizeof (charBuffer) - 1,stdin) == (char *) NULL);
		if (sscanf (charBuffer,"%d,%d",&panelColNum,&panelRowNum) == 2) break;
		else
			if (RGPPrintError (mode,entryNum,"Panel layout input error")) goto Stop;
		} while (true);

	RGPInitPenColors ();
	cpgsubp (panelColNum,panelRowNum);
	cpgqlw (&defaultLW);

	ret = DBSuccess;
	for (panelRow = 0;panelRow < panelRowNum;++panelRow)
		for (panelCol = 0;panelCol < panelColNum; ++panelCol)

			{
			cpgpanl (panelCol + 1,panelRow + 1);
			cpgsch (1.8);
			cpgvstd ();
			do	{
				sprintf (charBuffer,"Panel Title [%d,%d]:",panelRow,panelCol);
				RGPPrintMessage (mode,&entryNum,charBuffer);
				if (fgets (panelTitle,sizeof (panelTitle) - 1,stdin) != (char *) NULL)
					{
					if (panelTitle [strlen (panelTitle) - 1] == '\n')
						panelTitle [strlen (panelTitle) - 1] = '\0';
					if (strlen (panelTitle) > 0) break;
					}
				RGPPrintError (mode,entryNum,"Panel Title input error"); goto Stop;
				} while (true);

			dataNum = 0;
			do {
				RGPPrintMessage (mode,&entryNum,"Mapextent [X0,Y0,X1,Y1]:");
				if (fgets (charBuffer,sizeof (charBuffer) - 1,stdin) == (char *) NULL) continue;
				if (sscanf (charBuffer,"%f,%f,%f,%f",&x0,&y0,&x1,&y1) == 4)	break;
				else	if (RGPPrintError (mode,entryNum,"Mapextent input error")) goto Stop;
				} while (true);
			cpgwnad (x0,x1,y0,y1);

			do	{
				sprintf (charBuffer,"RiverGIS data file [%d]:",++dataNum);
				RGPPrintMessage (mode,&entryNum, charBuffer);
				if ((fgets (charBuffer,sizeof (charBuffer) - 1,stdin) != (char *) NULL) &&
					 (strlen (charBuffer) > 0) && charBuffer [0] != '\n')
					{
					if (charBuffer [strlen (charBuffer) - 1] == '\n') charBuffer [strlen (charBuffer) - 1] = '\0';
					dbData = new DBObjData ();
					if (dbData->Read (charBuffer) != DBSuccess) { dataNum--; continue; }
					switch (dbData->Type ())
						{
						case DBTypeVectorPoint:
							if ((ret = RGPDrawVecPoint (mode, &entryNum, dbData)) == DBFault) goto Stop;
							break;
						case DBTypeVectorLine:
							if ((ret = RGPDrawVecLine (mode, &entryNum, dbData)) == DBFault) goto Stop;
							break;
						case DBTypeVectorPolygon:
							break;
						case DBTypeGridContinuous:
								if ((ret = RGPDrawGridContinuous (mode,&entryNum,dbData)) == DBFault)	goto Stop;
							break;
						case DBTypeGridDiscrete:
							break;
						case DBTypeNetwork:
							if ((ret = RGPDrawNetwork (mode, &entryNum, dbData)) == DBFault) goto Stop;
							break;
						default: CMmsgPrint (CMmsgUsrError,"Invalid data type"); dataNum--; break;
						}
					delete dbData;
					}
				else	break;
				} while (true);
			cpgbox ("BCMTS",0.0,0,"BCNMTS",0.0,0);
			cpgslw (2);
			cpgsch (2.5);
			cpgmtxt ("T",1.5,0.5,0.5,panelTitle);
			cpgslw (defaultLW);
			}
Stop:
	cpgend ();
	return (ret);
	}
