/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

CMDnetAccumulate.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBio.H>
#include <RG.H>

int main (int argc,char *argv [])

	{
	int argPos, argNum = argc, ret, verbose = false;
	float coeff      = 0.000001;
	char *title      = (char *) NULL, *subject = (char *) NULL;
	char *domain     = (char *) NULL, *version = (char *) NULL;
	char *netName    = (char *) NULL,  *stnName = (char *) NULL;
	char *tsName     = (char *) NULL;
	char *fields [5] = { (char *) NULL, (char *) NULL, (char *) NULL, (char *) NULL, (char *) NULL };
	DBInt shadeSet   = DBFault;
	bool areaMult    = true, correction = false, allowNegative = true;
	DBObjData *data, *netData, *grdData, *stnData = (DBObjData *) NULL, *tsData = (DBObjData *) NULL;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-n","--network"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing network!\n");      return (CMfailed); }
			netName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-a","--stations"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing monitoring stations!\n"); return (CMfailed); }
			stnName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-i","--timeseries"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing time series!\n");  return (CMfailed); }
			tsName = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-r","--relate"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing relate field!\n"); return (CMfailed); }
			fields [0] = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-x","--nextstation"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing next station field!\n"); return (CMfailed); }
			fields [1] = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-j","--join"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing join field!\n");   return (CMfailed); }
			fields [2] = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-e","--date"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing date field!\n");   return (CMfailed); }
			fields [3] = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-g","--discharge"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing discharge field!\n"); return (CMfailed); }
			fields [4] = argv [argPos];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-c","--correction"))
			{
			int corCodes [] = {	true, false };
			const char *corModes [] = {	"on", "off", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing correcition mode!\n"); return (CMfailed); }
			if ((correction = CMoptLookup (corModes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid correction mode!\n");  return (CMfailed); }
			correction = corCodes [correction];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-m","--mode"))
			{
			int modeCodes [] = {	true, false };
			const char *modes [] = {	"rate", "volume", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing accumulation mode!\n"); return (CMfailed); }
			if ((areaMult = CMoptLookup (modes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid accumulation mode!\n"); return (CMfailed); }
			areaMult = modeCodes [areaMult];
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-f","--coefficient"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing coefficient!\n");  return (CMfailed); }
			if (sscanf (argv [argPos],"%f",&coeff) != 1)
				{ CMmsgPrint (CMmsgUsrError,"Invalid coefficient\n");   return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-l","--negative"))
			{
			int negCodes [] = {	true, false };
			const char *modes [] = {	"allow", "eliminate", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing negative mode!\n"); return (CMfailed); }
			if ((allowNegative = CMoptLookup (modes,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid negative mode!\n"); return (CMfailed); }
			allowNegative = negCodes [allowNegative];
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
		if (CMargTest (argv [argPos],"-s","--shadeset"))
			{
			int shadeCodes [] = {	DBDataFlagDispModeContStandard,
											DBDataFlagDispModeContGreyScale,
											DBDataFlagDispModeContBlueScale,
											DBDataFlagDispModeContBlueRed,
											DBDataFlagDispModeContElevation };
			const char *shadeSets [] = {	"standard","grey","blue","blue-to-red","elevation", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing shadeset!\n");     return (CMfailed); }
			if ((shadeSet = CMoptLookup (shadeSets,argv [argPos],true)) == DBFault)
				{ CMmsgPrint (CMmsgUsrError,"Invalid shadeset!\n");     return (CMfailed); }
			shadeSet = shadeCodes [shadeSet];
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
			CMmsgPrint (CMmsgInfo,"%s [options] <input grid> <output grid>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -n,--network     [network coverage]\n");
			CMmsgPrint (CMmsgInfo,"     -a,--stations    [station coverage]\n");
			CMmsgPrint (CMmsgInfo,"     -i,--timeseries  [time series data]\n");
			CMmsgPrint (CMmsgInfo,"     -r,--relate      [relate field]\n");
			CMmsgPrint (CMmsgInfo,"     -j,--join        [join field]\n");
			CMmsgPrint (CMmsgInfo,"     -e,--date        [date field]\n");
			CMmsgPrint (CMmsgInfo,"     -x,--nextstation [next station field]\n");
			CMmsgPrint (CMmsgInfo,"     -g,--discharge   [discharge field]\n");
			CMmsgPrint (CMmsgInfo,"     -c,--correction  [on|off (default is off)]\n");
			CMmsgPrint (CMmsgInfo,"     -m,--mode        [rate|volume]\n");
			CMmsgPrint (CMmsgInfo,"     -f,--coefficient [conversion coefficient]\n");
			CMmsgPrint (CMmsgInfo,"     -l,--negative    [allow|eliminate]\n");
			CMmsgPrint (CMmsgInfo,"     -t,--title       [dataset title]\n");
			CMmsgPrint (CMmsgInfo,"     -u,--subject     [subject]\n");
			CMmsgPrint (CMmsgInfo,"     -d,--domain      [domain]\n");
			CMmsgPrint (CMmsgInfo,"     -v,--version     [version]\n");
			CMmsgPrint (CMmsgInfo,"     -s,--shadeset    [standard|grey|blue|blue-to-red|elevation]\n");
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

	if (netName == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Network is not specified\n"); return (CMfailed); }
	if ((stnName == (char *) NULL) && (tsName != (char *) NULL))
		{ CMmsgPrint (CMmsgUsrError,"Missing station coverage!\n"); return (CMfailed); }
	if ((stnName != (char *) NULL) && (tsName == (char *) NULL))
		{ CMmsgPrint (CMmsgUsrError,"Missing time series data!\n"); return (CMfailed); }

	netData = new DBObjData ();
	if ((netData->Read (netName) == DBFault) || (netData->Type () != DBTypeNetwork))
		{ delete netData; return (CMfailed); }
	if (stnName != (char *) NULL)
		{
		stnData = new DBObjData ();
		if (stnData->Read (stnName) == DBFault)
			{ delete netData; delete stnData; return (CMfailed); }
		}
	if (tsName != (char *) NULL)
		{
		tsData = new DBObjData ();
		if (tsData->Read (tsName) == DBFault)
			{ delete netData; delete stnData; delete tsData; return (CMfailed); }
		}

	grdData = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? grdData->Read (argv [1]) : grdData->Read (stdin);
	if ((ret == DBFault) || (grdData->Type () != DBTypeGridContinuous))
		{ delete netData; delete grdData; return (CMfailed); }

	if (title	== (char *) NULL)	title   = "Accumulated Grid";
	if (subject == (char *) NULL)	subject = grdData->Document (DBDocSubject);
	if (domain	== (char *) NULL)	domain  = netData->Document (DBDocGeoDomain);
	if (version == (char *) NULL) version = "0.01pre";	

	data = DBNetworkToGrid (netData,DBTypeGridContinuous);
	data->Name (title);
	data->Document (DBDocSubject,subject);
	data->Document (DBDocGeoDomain,domain);
	data->Document (DBDocVersion,version);
	if (shadeSet != DBFault)
		{
		data->Flags (DBDataFlagDispModeContShadeSets,DBClear);
		data->Flags (shadeSet,DBSet);
		}

	ret = RGlibNetworkAccumulate (netData,grdData,stnData,tsData,
											fields,(DBFloat) coeff, areaMult, correction, allowNegative,data);
	if (ret == DBSuccess)
		ret = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? data->Write (argv [2]) : data->Write (stdout);

	delete data; delete netData; delete grdData;
	if (verbose) RGlibPauseClose ();
	return (ret);
	}
