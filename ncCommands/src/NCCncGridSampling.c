#include <string.h>
#include <cm.h>
#include <NC.h>

int main (int argc,char *argv [])
{
	int argPos, argNum = argc, tblLayout;
	int tblLayouts [2];
	const char *layoutTexts [] = { "vertical", "horizontal", (char *) NULL};
	char *sample = (char *) NULL;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-S","--sample"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "%s: Missing sampling coverage!",CMprgName (argv [0])); return (CMfailed); }
			sample = argv [argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-L","--layout"))
			{
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError, "Missing layout!"); return (CMfailed); }
			if ((tblLayout = CMoptLookup (layoutTexts, argv [argPos],true)) == CMfailed)
				{ CMmsgPrint (CMmsgUsrError, "%s: Invalid table layout [%s]!",CMprgName (argv [0]), argv [argPos]); return (CMfailed); }
			tblLayout = tblLayouts [tblLayout];
			if ((argNum = CMargShiftLeft(argPos,argv,argc)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			fprintf (stdout,"%s [options] <ncgis grid> <ascii table>", CMprgName(argv [0]));
			fprintf (stdout,"     -S,--sampling");
			fprintf (stdout,"     -L,--layout [vertical|horizontal]");
			fprintf (stdout,"     -h,--help");
			return (NCsucceeded);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError, "Unknown option: %s!",argv [argPos]); return (CMfailed); }
		argPos++;
		}
	if (sample == (char *) NULL)
		{ CMmsgPrint (CMmsgUsrError, "%s: Missing template!",CMprgName (argv [0])); return (CMfailed); }

	if (argNum < 2)
		{ CMmsgPrint (CMmsgUsrError, "%s: Too few arguments!", CMprgName (argv [0])); return (CMfailed); }
/*
	if ((objGrid = NCObjDataOpen (argv [1],true)) == (NCObjData_t *) NULL) return (CMfailed);
	if ((objGrid->DType != NCTypeContGrid) && (objGrid->DType != NCTypeDiscGrid))
	{
		CMmsgPrint (CMmsgUsrError, "%s: Non-grid input coverage!", CMprgName (argv [0]));
		NCObjDataFree (objGrid);
		return (CMfailed);
	}
	if ((objData = NCObjDataOpen (sample,true)) == (NCObjData_t *) NULL)
	{
		NCObjDataFree (objGrid);
		return (CMfailed);
	}
	if ((objData->DType != NCTypePoint)    && (objData->DType != NCTypeNetwork))
	{
		CMmsgPrint (CMmsgUsrError, "%s: Invalid sampling coverage!", CMprgName (argv [0]));
		NCObjDataFree (objGrid);
		NCObjDataFree (objData);
		return (CMfailed);
	}
	objTable = NCGridContPointSampling (objGrid,objData,tblLayout);

	NCObjDataFree (objGrid);
	NCObjDataFree (objData);
*/
	return (CMsucceeded);
}
