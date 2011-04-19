/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDgrdDateLayers.C

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

static DBInt modifyDate (DBObjData *dbData, int timeStep,
								int year, int month, int day, int hour, int minute,
								int interval)
	{
	DBInt layerID;
	DBDate stepDate;
	DBGridIF *gridIF = new DBGridIF (dbData);
	DBObjRecord *layerRec;
	DBDate date;
	//change this!

	switch (timeStep)
		{
		case DBTimeStepYear:   stepDate.Set (interval);         break;
		case DBTimeStepMonth:  stepDate.Set (0,interval);       break;
		case DBTimeStepDay:    stepDate.Set (0,0,interval);     break;
		case DBTimeStepHour:   stepDate.Set (0,0,0,interval);   break;
		case DBTimeStepMinute: stepDate.Set (0,0,0,0,interval); break;
		}
	if (year == DBDefaultMissingIntVal) //if the user hasnt entered a specific year
		{
		if (((stepDate.Year   () > 0) && (gridIF->LayerNum () > 1)) ||
		    ((stepDate.Month  () > 0) && (gridIF->LayerNum () > (12 / interval))) ||
		    ((stepDate.Day    () > 0) && (gridIF->LayerNum () > (365 / interval))) ||
		    ((stepDate.Hour   () > 0) && (gridIF->LayerNum () > (365 * 24 / interval))) ||
		    ((stepDate.Minute () > 0) && (gridIF->LayerNum () > 365 * 24 * 60 / interval)))
			{
			//todo: handle this better!
			printf("(Climatology)There seems to be more than a year's worth of layers!\n");
			printf("(Climatology)I suggest you define the year, or change the step interval\n");
			delete gridIF;
			return (CMfailed);
			}
		}
	//printf("Labeling Layers");
	//this sets the first layer to contain all of the fields even if a field is undefined.
	//I do this so (for example)the user can compare day 1 of every month of a year with day 2 of
	//every moth of a year and so on.. the first layer will indicate everything down to the
	//minute, and the next layers will omit anything that isnt different.

	date.Set (year,month,day,hour,minute);

	for (layerID = 0;layerID < gridIF->LayerNum ();++layerID)
		{
		layerRec = gridIF->Layer (layerID);
		gridIF->RenameLayer (layerRec,date.Get ());
		date = date + (stepDate);
		}
	delete gridIF;
	return (DBSuccess);
	}

int main(int argc, char* argv[])

	{
	int argPos, argNum = argc, ret, verbose = false;
	int startYear    = DBDefaultMissingIntVal;
	int startMonth   = DBDefaultMissingIntVal;
	int startDay     = DBDefaultMissingIntVal;
	int startHour    = DBDefaultMissingIntVal;
	int startMinute  = DBDefaultMissingIntVal;
	int timeInterval = 1; // this is defined by the -n field
	int shadeSet     = DBDataFlagDispModeContGreyScale;
	bool changeShadeSet = false;
	DBObjData *dbData;
	DBInt timeStep = DBFault;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-e","--step"))
			{
			const char *timeStepStr [] = { "year", "month", "day", "hour", "minute", (char *) NULL };
			int timeStepCodes [] = { DBTimeStepYear,
											 DBTimeStepMonth,
											 DBTimeStepDay,
											 DBTimeStepHour,
											 DBTimeStepMinute };
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Time step variable step is not defined!\n"); return (CMfailed); }
			if ((timeStep = CMoptLookup (timeStepStr,argv[argPos],true)) == DBFault)
				{ printf("Invalid timeStep %s\n",argv[argPos]); return DBFault; }
			timeStep = timeStepCodes [timeStep];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-y","--year"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing Year!\n");         return (CMfailed); }
			if (sscanf (argv[argPos],"%d",&startYear) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid year!\n");         return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-m","--month"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing Month!\n");        return (CMfailed); }
			if (sscanf (argv[argPos],"%d",&startMonth) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid Month!\n");        return (CMfailed); }
			startMonth--;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-d","--day"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing Day!\n");          return (CMfailed); }
			if (sscanf (argv[argPos],"%d",&startDay) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid day!\n");          return (CMfailed); }
			startDay--;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-r","--hour"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing Hour!\n");         return (CMfailed); }
			if (sscanf (argv[argPos],"%d",&startHour) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid hour!\n");         return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-i","--minute"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing Minute!\n");       return (CMfailed); }
			if (sscanf (argv[argPos],"%d",&startMinute) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid minute!\n");       return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-n","--number"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing Number!\n");       return (CMfailed); }
			if (sscanf (argv[argPos],"%d",&timeInterval) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid interval!\n");     return (CMfailed); }
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
				{ CMmsgPrint (CMmsgUsrError,"Missing shadeset!\n");     return (CMfailed); }
			if ((shadeSet = CMoptLookup (shadeSets,argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError,"Invalid shadeset!\n");     return (CMfailed); }
			shadeSet = shadeCodes [shadeSet];
			changeShadeSet = true;
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			CMmsgPrint (CMmsgInfo,"%s [options] <input grid> <output grid>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -y,--year      [beginning year]\n");
			CMmsgPrint (CMmsgInfo,"     -m,--month     [beginning month]\n");
			CMmsgPrint (CMmsgInfo,"     -d,--day       [beginning day]\n");
			CMmsgPrint (CMmsgInfo,"     -r,--hour      [beginning hour]\n");
			CMmsgPrint (CMmsgInfo,"     -i,--minute    [beginning minute]\n");
			CMmsgPrint (CMmsgInfo,"     -e,--step      [year|month|day|hour|minute]\n");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset  [standard|grey|blue|blue-to-red|elevation]\n");
			CMmsgPrint (CMmsgInfo,"     -n,--number    [number of intervals]\n");
			CMmsgPrint (CMmsgInfo,"     -V,--verbose\n");
			CMmsgPrint (CMmsgInfo,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
			argPos++;
			}

	switch (timeStep)
		{
		case DBFault: CMmsgPrint (CMmsgUsrError,"Missing time step!\n"); return (CMfailed);
		case DBTimeStepMinute:	if (startMinute == DBDefaultMissingIntVal) startMinute = 0;
		case DBTimeStepHour:		if (startHour   == DBDefaultMissingIntVal) startHour   = 0;
		case DBTimeStepDay:		if (startDay    == DBDefaultMissingIntVal) startDay    = 0;
		case DBTimeStepMonth:	if (startMonth  == DBDefaultMissingIntVal) startMonth  = 0;
		default:	break;
		}

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }
	if (verbose) RGlibPauseOpen (argv[0]);

	dbData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? dbData->Read (argv [1]) : dbData->Read (stdin);
	if ((ret == DBFault) ||
		 ((dbData->Type () != DBTypeGridContinuous) &&
		  (dbData->Type () != DBTypeGridDiscrete))) { delete dbData; return (CMfailed); }
	if (changeShadeSet && (dbData->Type () == DBTypeGridContinuous))
		{
		dbData->Flags (DBDataFlagDispModeContShadeSets,DBClear);
		dbData->Flags (shadeSet, DBSet);
		}
	if (modifyDate(dbData,timeStep,startYear,startMonth,startDay,startHour,startMinute,timeInterval)
		  	== DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? dbData->Write (argv [2]) : dbData->Write (stdout);

	delete dbData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
