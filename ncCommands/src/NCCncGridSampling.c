#include <cm.h>
#include <NC.h>

int main (int argc,char *argv [])
{
	int argPos, argNum = argc, tblLayout;
	int tblLayouts [2];
	char *layoutTexts [] = { "vertical", "horizontal", (char *) NULL};
	char *sample = (char *) NULL;

	for (argPos = 1;argPos < argNum; )
	{
		if (NCcmArgTest (argv [argPos],"-S","--sample"))
		{
			NCcmArgShiftLeft (argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum))
			{
				CMmsgPrint (CMmsgUsrError, "%s: Missing sampling coverage!",NCcmProgName (argv [0]));
				return (NCfailed);
			}
			sample = argv [argPos];
			NCcmArgShiftLeft (argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest (argv [argPos],"-L","--layout"))
		{
			NCcmArgShiftLeft (argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { CMmsgPrint (CMmsgUsrError, "Missing layout!"); return (NCfailed); }
			if ((tblLayout = NCcmStringLookup (layoutTexts, argv [argPos],true)) == NCfailed)
			{
				CMmsgPrint (CMmsgUsrError, "%s: Invalid table layout [%s]!",NCcmProgName (argv [0]), argv [argPos]);
				return (NCfailed);
			}
			tblLayout = tblLayouts [tblLayout];
			NCcmArgShiftLeft (argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest (argv [argPos],"-h","--help"))
		{
			fprintf (stdout,"%s [options] <ncgis grid> <ascii table>", NCcmProgName(argv [0]));
			fprintf (stdout,"     -S,--sampling");
			fprintf (stdout,"     -L,--layout [vertical|horizontal]");
			fprintf (stdout,"     -h,--help");
			return (NCsucceeded);
		}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
		{
			CMmsgPrint (CMmsgUsrError, "Unknown option: %s!",argv [argPos]);
			return (NCfailed);
		}
		argPos++;
		}
	if (sample == (char *) NULL)
	{
		CMmsgPrint (CMmsgUsrError, "%s: Missing template!",NCcmProgName (argv [0]));
		return (NCfailed);
	}

	if (argNum < 2)
	{
		CMmsgPrint (CMmsgUsrError, "%s: Too few arguments!", NCcmProgName (argv [0]));
		return (NCfailed);
	}
/*
	if ((objGrid = NCObjDataOpen (argv [1],true)) == (NCObjData_t *) NULL) return (NCfailed);
	if ((objGrid->DType != NCTypeContGrid) && (objGrid->DType != NCTypeDiscGrid))
	{
		CMmsgPrint (CMmsgUsrError, "%s: Non-grid input coverage!", NCcmProgName (argv [0]));
		NCObjDataFree (objGrid);
		return (NCfailed);
	}
	if ((objData = NCObjDataOpen (sample,true)) == (NCObjData_t *) NULL)
	{
		NCObjDataFree (objGrid);
		return (NCfailed);
	}
	if ((objData->DType != NCTypePoint)    && (objData->DType != NCTypeNetwork))
	{
		CMmsgPrint (CMmsgUsrError, "%s: Invalid sampling coverage!", NCcmProgName (argv [0]));
		NCObjDataFree (objGrid);
		NCObjDataFree (objData);
		return (NCfailed);
	}
	objTable = NCGridContPointSampling (objGrid,objData,tblLayout);

	NCObjDataFree (objGrid);
	NCObjDataFree (objData);
*/
	return (NCsucceeded);
}
