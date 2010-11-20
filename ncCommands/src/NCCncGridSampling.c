#include<NC.h>

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
				fprintf (stderr,"%s: Missing sampling coverage!\n",NCcmProgName (argv [0]));
				return (NCfailed);
			}
			sample = argv [argPos];
			NCcmArgShiftLeft (argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest (argv [argPos],"-L","--layout"))
		{
			NCcmArgShiftLeft (argPos,argv,argc); argNum--;
			if (NCcmArgCheck(argv,argPos,argNum)) { fprintf (stderr,"Missing layout!\n"); return (NCfailed); }
			if ((tblLayout = NCcmStringLookup (layoutTexts, argv [argPos],true)) == NCfailed)
			{
				fprintf (stderr,"%s: Invalid table layout [%s]!\n",NCcmProgName (argv [0]), argv [argPos]);
				return (NCfailed);
			}
			tblLayout = tblLayouts [tblLayout];
			NCcmArgShiftLeft (argPos,argv,argc); argNum--;
			continue;
		}
		if (NCcmArgTest (argv [argPos],"-h","--help"))
		{
			fprintf (stdout,"%s [options] <ncgis grid> <ascii table>\n", NCcmProgName(argv [0]));
			fprintf (stdout,"     -S,--sampling\n");
			fprintf (stdout,"     -L,--layout [vertical|horizontal]\n");
			fprintf (stdout,"     -h,--help\n");
			return (NCsucceeded);
		}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
		{
			fprintf (stderr,"Unknown option: %s!\n",argv [argPos]);
			return (NCfailed);
		}
		argPos++;
		}
	if (sample == (char *) NULL)
	{
		fprintf (stderr,"%s: Missing template!\n",NCcmProgName (argv [0]));
		return (NCfailed);
	}

	if (argNum < 2)
	{
		fprintf (stderr,"%s: Too few arguments!\n", NCcmProgName (argv [0]));
		return (NCfailed);
	}
/*
	if ((objGrid = NCObjDataOpen (argv [1],true)) == (NCObjData_t *) NULL) return (NCfailed);
	if ((objGrid->DType != NCTypeContGrid) && (objGrid->DType != NCTypeDiscGrid))
	{
		fprintf (stderr,"%s: Non-grid input coverage!\n", NCcmProgName (argv [0]));
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
		fprintf (stderr,"%s: Invalid sampling coverage!\n", NCcmProgName (argv [0]));
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
