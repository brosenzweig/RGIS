/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDnetDefragment.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <math.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	float climb = 0.0, maxBasin = HUGE_VAL;
	char *title  = (char *) NULL,		*subject = (char *) NULL;
	char *domain = (char *) NULL,		*version = (char *) NULL;
	char *elevName = (char *) NULL;
	bool save = false;
	DBObjData *grdData, *netData;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-e","--elevation"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing Elevation!\n");	return (CMfailed); }
			elevName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-c","--climb"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing climb coefficient!\n"); return (CMfailed); }
			if (sscanf (argv [argPos],"%f",&climb) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid climb coefficient\n");  return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-a","--maximum_basin"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing maximum basin size!\n"); return (CMfailed); }
			if (sscanf (argv [argPos],"%f",&maxBasin) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid maximum basin size\n");  return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-s","--savesteps"))
			{
			int saveStep;
			const char *modes [] = { "no", "yes", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing savesteps!\n");    return (CMfailed); }
			if ((saveStep = CMoptLookup (modes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid savesteps mode!\n"); return (CMfailed); }
			save = saveStep == 0 ? true : false;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-P","--planet"))
			{
			int planet;
			const char *planets [] = { "Earth", "Mars", "Venus", NULL };
			DBFloat radius [] = { 6371.2213, 6371.2213 * 0.53264, 6371.2213 * 0.94886 };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing planet!\n");       return (CMfailed); }
			if ((planet = CMoptLookup (planets,argv [argPos],true)) == DBFault)
				{
				if (sscanf (argv [argPos],"%lf",radius) != 1)
					{ CMmsgPrint (CMmsgUsrError,"Invalid planet!\n");	return (CMfailed); }
				planet = 0;
				}
			DBMathSetGlobeRadius (radius [planet]);
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-t","--title"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing title!\n");        return (CMfailed); }
			title = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-u","--subject"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing subject!\n");      return (CMfailed); }
			subject = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-d","--domain"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing domain!\n");       return (CMfailed); }
			domain  = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-v","--version"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing version!\n");      return (CMfailed); }
			version  = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-V","--verbose"))
			{
			verbose = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			CMmsgPrint (CMmsgInfo,"%s [options] <input network> <output network>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -e,--elevation   [elevation coverage]\n");
			CMmsgPrint (CMmsgInfo,"     -c,--climb       [climb coefficient]\n");
			CMmsgPrint (CMmsgInfo,"     -a,--maximum_basin [maximum basin size]\n");
			CMmsgPrint (CMmsgInfo,"     -P, --planet     [Earth|Mars|Venus|radius]\n");
			CMmsgPrint (CMmsgInfo,"     -t,--title       [dataset title]\n");
			CMmsgPrint (CMmsgInfo,"     -u,--subject     [subject]\n");
			CMmsgPrint (CMmsgInfo,"     -d,--domain      [domain]\n");
			CMmsgPrint (CMmsgInfo,"     -v,--version     [version]\n");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose\n");
			CMmsgPrint (CMmsgInfo,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	if (elevName == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Elevation is not specified\n"); return (CMfailed); }

	grdData = new DBObjData ();
	if ((grdData->Read (elevName) == DBFault) || (grdData->Type () != DBTypeGridContinuous))
		{ delete grdData; return (CMfailed); }

	netData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? netData->Read (argv [1]) : netData->Read (stdin);
	if ((ret == DBFault) || (netData->Type () != DBTypeNetwork))
		{ delete grdData; delete netData; return (CMfailed); }

	if (title	!= (char *) NULL) netData->Name (title);
	if (subject != (char *) NULL) netData->Document (DBDocSubject,   subject);
	if (domain	!= (char *) NULL) netData->Document (DBDocGeoDomain, domain);
	if (version != (char *) NULL) netData->Document (DBDocVersion,   version);

	if ((argNum > 2) && (strcmp (argv [2],"-") != 0)) netData->FileName (argv [2]);
	else save = false;

	if ((ret = RGlibNetworkDefragment (netData, grdData,climb, maxBasin, save)) == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? netData->Write (argv [2]) : netData->Write (stdout);

	delete netData; delete grdData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}

static DBInt   *_RGlibNetworkBasinIDX;
static DBFloat *_RGlibNetworkBasinElev;

static DBInt _RGlibNetworkDefragCompare (const void *leftID,const void *rightID)
	{
	DBFloat leftVal  = _RGlibNetworkBasinElev [*((DBInt *) leftID)];
	DBFloat rightVal = _RGlibNetworkBasinElev [*((DBInt *) rightID)];
	if (CMmathEqualValues (leftVal, rightVal) == true) return (0);
	return (leftVal < rightVal ? -1 : 1);
	}

DBInt RGlibNetworkDefragment (DBObjData *netData, DBObjData *elevData,DBFloat maxClimb, DBFloat maxBasin, bool save)
	{
	DBInt basin, basinID, *basinIDs, cBasinID, nBasinID, cellID, dir, prevDir, pourDir, cellNum, count;
	DBFloat elev, elev2, pourElev;
	DBPosition pos, cellPos, pourPos;
	DBCoordinate coord;
	DBObjRecord *layerRec, *cellRec, *pourCell, *mouthCell, *toCell, *fromCell;
	DBNetworkIF *netIF = new DBNetworkIF (netData);
	DBGridIF    *gridIF = new DBGridIF (elevData);

	if ((basinIDs  = (DBInt *)   calloc (netIF->BasinNum (),sizeof (DBInt)))   == (DBInt *)   NULL)
		{ perror ("Memory allocation erron in: RGlibNetworkDefragment ()"); return (CMfailed); }
	if ((_RGlibNetworkBasinIDX  = (DBInt *)   calloc (netIF->BasinNum (),sizeof (DBInt)))   == (DBInt *)   NULL)
		{ perror ("Memory allocation erron in: RGlibNetworkDefragment ()"); free (basinIDs); return (CMfailed); }
	if ((_RGlibNetworkBasinElev = (DBFloat *) calloc (netIF->BasinNum (),sizeof (DBFloat))) == (DBFloat *) NULL)
		{ perror ("Memory allocation erron in: RGlibNetworkDefragment ()"); free (basinIDs); free (_RGlibNetworkBasinIDX); return (CMfailed); }
	layerRec = gridIF->Layer (0);
	for (basin = 0;basin < netIF->BasinNum ();basin++)
		{
		mouthCell = netIF->MouthCell (netIF->Basin (basin));
		prevDir = netIF->CellDirection (mouthCell);
		if ((prevDir == 0x0) && (gridIF->Value (layerRec, netIF->Center (mouthCell), &pourElev) != false))
			{
			cellPos = netIF->CellPosition (mouthCell);
			for (dir = 0;dir < 8;dir++)
				{
				pos = cellPos;
				if      (((0x01 << dir) == DBNetDirNW) || ((0x01 << dir) == DBNetDirN) || ((0x01 << dir) == DBNetDirNE)) pos.Row++;
   			else if (((0x01 << dir) == DBNetDirSE) || ((0x01 << dir) == DBNetDirS) || ((0x01 << dir) == DBNetDirSW)) pos.Row--;
				if      (((0x01 << dir) == DBNetDirNE) || ((0x01 << dir) == DBNetDirE) || ((0x01 << dir) == DBNetDirSE)) pos.Col++;
				else if (((0x01 << dir) == DBNetDirNW) || ((0x01 << dir) == DBNetDirW) || ((0x01 << dir) == DBNetDirSW)) pos.Col--;
				if (netIF->Cell (pos) != (DBObjRecord *) NULL) continue;
				netIF->Pos2Coord (pos,coord);
				if (gridIF->Value (layerRec,coord,&elev) == false) elev = gridIF->Minimum ();
				if (pourElev > elev) { pourElev = elev; prevDir = 0x01 << dir; }
				}
			if (prevDir != 0x0) netIF->CellDirection (mouthCell,prevDir);
			}
		}

	if (maxClimb <= 0.0) maxClimb = gridIF->Maximum () - gridIF->Minimum ();
	do	{
		count = 0;
		for (basin = 0;basin < netIF->BasinNum ();basin++)
			{
			basinIDs [basin] = basin;
			_RGlibNetworkBasinIDX  [basin] = basin;
			mouthCell = netIF->MouthCell (netIF->Basin (basin));
			if (gridIF->Value (layerRec,netIF->Center (mouthCell),&elev) &&
			    (netIF->CellDirection (mouthCell) == 0x0))
				  _RGlibNetworkBasinElev [basin] = elev;
			else _RGlibNetworkBasinElev [basin] = gridIF->Minimum ();
			}
		qsort (_RGlibNetworkBasinIDX,netIF->BasinNum (),sizeof (DBInt),_RGlibNetworkDefragCompare);
		for (basin = 0;basin <  netIF->BasinNum ();basin++)
			{
			cBasinID  = _RGlibNetworkBasinIDX  [basin];
			if (basinIDs [_RGlibNetworkBasinIDX  [basin]] != cBasinID) printf ("Ezt nem ertem\n");
			pourElev = _RGlibNetworkBasinElev [cBasinID] + maxClimb;
			if (CMmathEqualValues (_RGlibNetworkBasinElev [cBasinID],gridIF->Minimum ())) continue;
			toCell = pourCell = mouthCell = netIF->MouthCell (netIF->Basin (cBasinID));
			if (netIF->CellDirection (mouthCell) != 0x0)                continue;
			if (netIF->BasinArea (netIF->Basin (cBasinID)) >= maxBasin) continue;

			pourDir = 0x0;
			cellNum  = mouthCell->RowID () + netIF->CellBasinCells (mouthCell);
			for (cellID = mouthCell->RowID ();cellID < cellNum;cellID++)
				{
				cellRec = netIF->Cell (cellID);
				if (gridIF->Value (layerRec, netIF->Center (cellRec), &elev) == false) continue;
				cellPos = netIF->CellPosition (cellRec);
				if (pourElev > elev)
					for (dir = 0;dir < 8;dir++)
						{
						pos = cellPos;
						if      (((0x01 << dir) == DBNetDirNW) || ((0x01 << dir) == DBNetDirN) || ((0x01 << dir) == DBNetDirNE)) pos.Row++;
   					else if (((0x01 << dir) == DBNetDirSE) || ((0x01 << dir) == DBNetDirS) || ((0x01 << dir) == DBNetDirSW)) pos.Row--;
						if      (((0x01 << dir) == DBNetDirNE) || ((0x01 << dir) == DBNetDirE) || ((0x01 << dir) == DBNetDirSE)) pos.Col++;
						else if (((0x01 << dir) == DBNetDirNW) || ((0x01 << dir) == DBNetDirW) || ((0x01 << dir) == DBNetDirSW)) pos.Col--;
						if (((fromCell = netIF->Cell (pos)) == (DBObjRecord *) NULL) ||
						    (((nBasinID = basinIDs [netIF->CellBasinID (fromCell) - 1]) != cBasinID) &&
							  (_RGlibNetworkBasinElev [nBasinID] <=  _RGlibNetworkBasinElev [cBasinID])))
							{
							netIF->Pos2Coord (pos,coord);
							if (gridIF->Value (layerRec,coord,&elev2) != false) elev = elev > elev2 ? elev : elev2;
							if (pourElev > elev)
								{
								pourElev = elev;
								pourDir  = 0x01 << dir;
								pourPos  = pos;
								toCell   = cellRec;
								}
							}
						}
				}
			if (pourDir == 0x0) continue;
			if ((pourCell = netIF->Cell (pourPos)) != (DBObjRecord *) NULL)
				{
				nBasinID = basinIDs [netIF->CellBasinID (pourCell) - 1];
				cellRec = netIF->MouthCell (netIF->Basin (nBasinID));
				if ((netIF->CellDirection (cellRec) == 0x0) &&
				    ((gridIF->Value (layerRec,netIF->Center (cellRec),&elev) != false) &&
					 (_RGlibNetworkBasinElev [cBasinID] < elev))) continue;
				}
			if (toCell != mouthCell)
				{
				if ((fromCell = netIF->ToCell (toCell)) != (DBObjRecord *) NULL)
					{
					prevDir = netIF->CellDirection (toCell);
					while (fromCell != mouthCell)
						{
						dir = (((prevDir >> 0x04) | (prevDir << 0x04)) & 0xff);
						cellRec = fromCell;
						fromCell = netIF->ToCell (cellRec);
						prevDir = netIF->CellDirection (cellRec);
						netIF->CellDirection (cellRec,dir);
						}
					dir = (((prevDir >> 0x04) | (prevDir << 0x04)) & 0xff);
					netIF->CellDirection (mouthCell,dir);
					}
				}
			netIF->CellDirection (toCell, pourDir);
			if (pourCell != (DBObjRecord *) NULL)
				for (basinID = 0;basinID < netIF->BasinNum ();++basinID)
					if (cBasinID == basinIDs [basinID]) basinIDs [basinID] = nBasinID;
			count++;
			}
		if (count > 0) { printf ("Building [%d]\n", count); netIF->Build (); if (save) netData->Write (netData->FileName ()); }
		} while (count > 0);
	free (basinIDs);
	free (_RGlibNetworkBasinIDX);
	free (_RGlibNetworkBasinElev);
	return (DBSuccess);
	}
