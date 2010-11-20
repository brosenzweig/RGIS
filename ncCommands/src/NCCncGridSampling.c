#include<NC.h>

int main (int argc,char *argv [])
{
	int argPos, argNum = argc, tblLayout;
	int tblLayouts [2];
	char *layoutTexts [] = { "vertical", "horizontal", (char *) NULL};
	char *sample = (char *) NULL;

	for (argPos = 1;argPos < argNum; )
	{
		if (NCGcmArgTest (argv [argPos],"-S","--sample"))
		{
			NCGcmArgShiftLeft (argPos,argv,argc); argNum--;
			if (NCGcmArgCheck(argv,argPos,argNum))
			{
				fprintf (stderr,"%s: Missing sampling coverage!\n",NCGcmProgName (argv [0]));
				return (NCGfailed);
			}
			sample = argv [argPos];
			NCGcmArgShiftLeft (argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest (argv [argPos],"-L","--layout"))
		{
			NCGcmArgShiftLeft (argPos,argv,argc); argNum--;
			if (NCGcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing layout!\n"); return (NCGfailed); }
			if ((tblLayout = NCGcmStringLookup (layoutTexts, argv [argPos],true)) == NCGfailed)
			{
				fprintf (stderr,"%s: Invalid table layout [%s]!\n",NCGcmProgName (argv [0]), argv [argPos]);
				return (NCGfailed);
			}
			tblLayout = tblLayouts [tblLayout];
			NCGcmArgShiftLeft (argPos,argv,argc); argNum--;
			continue;
		}
		if (NCGcmArgTest (argv [argPos],"-h","--help"))
		{
			fprintf (stdout,"%s [options] <ncgis grid> <ascii table>\n", NCGcmProgName(argv [0]));
			fprintf (stdout,"     -S,--sampling\n");
			fprintf (stdout,"     -L,--layout [vertical|horizontal]\n");
			fprintf (stdout,"     -h,--help\n");
			return (NCGsucceeded);
		}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
		{
			fprintf (stderr,"Unknown option: %s!\n",argv [argPos]);
			return (NCGfailed);
		}
		argPos++;
		}
	if (sample == (char *) NULL)
	{
		fprintf (stderr,"%s: Missing template!\n",NCGcmProgName (argv [0]));
		return (NCGfailed);
	}

	if (argNum < 2)
	{
		fprintf (stderr,"%s: Too few arguments!\n", NCGcmProgName (argv [0]));
		return (NCGfailed);
	}
/*
	if ((objGrid = NCGObjDataOpen (argv [1],true)) == (NCGObjData_t *) NULL) return (NCGfailed);
	if ((objGrid->DType != NCGTypeContGrid) && (objGrid->DType != NCGTypeDiscGrid))
	{
		fprintf (stderr,"%s: Non-grid input coverage!\n", NCGcmProgName (argv [0]));
		NCGObjDataFree (objGrid);
		return (NCGfailed);
	}
	if ((objData = NCGObjDataOpen (sample,true)) == (NCGObjData_t *) NULL)
	{
		NCGObjDataFree (objGrid);
		return (NCGfailed);
	}
	if ((objData->DType != NCGTypePoint)    && (objData->DType != NCGTypeNetwork))
	{
		fprintf (stderr,"%s: Invalid sampling coverage!\n", NCGcmProgName (argv [0]));
		NCGObjDataFree (objGrid);
		NCGObjDataFree (objData);
		return (NCGfailed);
	}
	objTable = NCGGridContPointSampling (objGrid,objData,tblLayout);

	NCGObjDataFree (objGrid);
	NCGObjDataFree (objData);
*/
	return (NCGsucceeded);
}
