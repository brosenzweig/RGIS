/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDgrdRenameLayers.C

balazs.fekete@unh.edu

*******************************************************************************/
/*
 run the program with no options to see the instructions!
 pfw@unh.edu
 oct 5, 2001
*/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main(int argc, char* argv[])

	{
	int argPos, argNum = argc, ret, verbose = false;
	int layerID;
	int shadeSet        = DBDataFlagDispModeContGreyScale;
	bool changeShadeSet = false;
	DBObjData *dbData;
	DBGridIF *gridIF;
	class RenameCLS
		{
		public:
			DBInt LayerID;
			char *LayerName;
			RenameCLS *Next;
			RenameCLS (DBInt layerID, char * layerName)
				{ LayerID = layerID; LayerName = layerName; Next = (RenameCLS *) NULL; }
			void AddLink (RenameCLS *renameCLS)
				{
				if (Next == (RenameCLS *) NULL)	Next = renameCLS;
				else Next->AddLink (renameCLS);
				}
			void DeleteLink ()
				{
				if (Next != (RenameCLS *) NULL) { Next->DeleteLink (); delete Next; }
				}
			void RenameLayer (DBGridIF *gridIF)
				{
				DBObjRecord *layerRec;
				if ((layerRec = gridIF->Layer (LayerID - 1)) != (DBObjRecord *) NULL) gridIF->RenameLayer (layerRec,LayerName);
				if (Next != (RenameCLS *) NULL) Next->RenameLayer (gridIF);
				}
		} *renameCLS = (RenameCLS *) NULL;
  
	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-r","--rename"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing layerID!"); return (CMfailed); }
			if (sscanf (argv[argPos],"%d",&layerID) != 1)
				{
				CMmsgPrint (CMmsgUsrError,"Invalid layerID!");
				if (renameCLS != (RenameCLS *) NULL) { renameCLS->DeleteLink (); delete renameCLS; }
				return (CMfailed);
				}
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{
				CMmsgPrint (CMmsgUsrError,"Missing layername!");
				if (renameCLS != (RenameCLS *) NULL) { renameCLS->DeleteLink (); delete renameCLS; }
				return (CMfailed);
				}
			if (renameCLS == (RenameCLS *) NULL) renameCLS = new RenameCLS (layerID,argv [argPos]);
			else renameCLS->AddLink (new RenameCLS (layerID,argv [argPos]));
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-s","--shadeset"))
			{
			int shadeCodes [] = {	DBDataFlagDispModeContStandard,
			                        DBDataFlagDispModeContGreyScale,
			                        DBDataFlagDispModeContBlueScale,
			                        DBDataFlagDispModeContBlueRed,
			                        DBDataFlagDispModeContElevation };
			const char *shadeSets [] = { "standard","grey","blue","blue-to-red","elevation", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing shadeset!");     return (CMfailed); }
			if ((shadeSet = CMoptLookup (shadeSets,argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid shadeset!");     return (CMfailed); }
			shadeSet = shadeCodes [shadeSet];
			changeShadeSet = true;
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input grid> <output grid>",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -r,--rename    [layerID layerName]");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset  [standard|grey|blue|blue-to-red|elevation]");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose");
			CMmsgPrint (CMmsgInfo,"     -h,--help");
			return (DBSuccess);
			}

		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{
			CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]);
			if (renameCLS != (RenameCLS *) NULL) { renameCLS->DeleteLink (); delete renameCLS; }
			return (CMfailed);
			}
		argPos++;
		}
  
	if (argNum > 3)
		{
		CMmsgPrint (CMmsgUsrError,"Extra arguments!");
		if (renameCLS != (RenameCLS *) NULL) { renameCLS->DeleteLink (); delete renameCLS; }
		return (CMfailed);
		}
	if (verbose) RGlibPauseOpen (argv[0]);

	dbData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? dbData->Read (argv [1]) : dbData->Read (stdin);
	if ((ret == DBFault) || ((dbData->Type () != DBTypeGridContinuous) && (dbData->Type () != DBTypeGridDiscrete)))
		{
		delete dbData;
		if (renameCLS != (RenameCLS *) NULL) { renameCLS->DeleteLink (); delete renameCLS; }
		return (CMfailed);
		}
	gridIF = new DBGridIF (dbData);

	if (renameCLS != (RenameCLS *) NULL) renameCLS->RenameLayer (gridIF);

	if (changeShadeSet && (dbData->Type () == DBTypeGridContinuous))
		{
		dbData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
		dbData->Flags (shadeSet, DBSet);
		}
	ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? dbData->Write (argv [2]) : dbData->Write (stdout);

	if (renameCLS != (RenameCLS *) NULL) { renameCLS->DeleteLink (); delete renameCLS; }

	delete gridIF;
	delete dbData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
