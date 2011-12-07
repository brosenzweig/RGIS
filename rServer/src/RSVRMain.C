/******************************************************************************

GHAAS RiverGIS GIS Server V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

RSRVMain.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <rServer.H>

int main (int argc, char *argv [])
{
   int argPos, argNum = argc, ret = CMfailed, port=CMfailed, socket=CMfailed;
   
   for (argPos = 1;argPos < argNum;) {
		if (CMargTest(argv[argPos],"-p","--port")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			if (port != CMfailed) CMmsgPrint (CMmsgUsrError,"Skipping port redefinition!");
			else {
			}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest(argv[argPos],"-s","--socket")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			if (socket != CMfailed) CMmsgPrint (CMmsgUsrError,"Skipping socket redefinition!");
			else {
			}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
Help:	if (CMargTest(argv[argPos],"-h","--help")) {
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) < argPos) break;
			CMmsgPrint (CMmsgUsrError,"%s [options]",CMprgName(argv[0]));
			CMmsgPrint (CMmsgUsrError,"  -p, --port");
			CMmsgPrint (CMmsgUsrError,"  -s, --socket");
			CMmsgPrint (CMmsgUsrError,"  -h,--help");
			ret = CMsucceeded;
			goto Stop;
		}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1)) {
			CMmsgPrint (CMmsgUsrError,"Unknown option: %s!",argv [argPos]);
			goto Stop;
		}
      argPos++;
	}
	if (argNum > 1) { CMmsgPrint (CMmsgUsrError,"Extra arguments!"); return (CMfailed); }

   {
      DBObjData *data;
   }
   
   ret = CMsucceeded;
   
Stop:
	return (ret);
}
