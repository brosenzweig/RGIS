/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

CMDrgis2domain.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>
#include <MF.h>

int main (int argc,char *argv [])

	{
	FILE *outFile;
	DBInt argPos, argNum = argc, ret;
	int objID, size;
	DBFloat lCorrection = 1.0;
	MFDomain_t *domain = (MFDomain_t *) NULL;
	DBCoordinate coord;
	DBObjRecord *objRec;
	DBObjData *data;

	for (argPos = 1;argPos < argNum; )
		{
		if (CMargTest (argv [argPos],"-l","--lengthcorrection"))
			{
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos)
				{ CMmsgPrint (CMmsgUsrError,"Missing length correction!\n");  return (CMfailed); }
			if (sscanf (argv [argPos],"%lf", &lCorrection) != 1)
				{ CMmsgPrint (CMmsgUsrError, "Invalid length correction!\n"); return (CMfailed); }
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) <= argPos) break;
			continue;
			}
		if (CMargTest (argv [argPos],"-h","--help"))
			{
			CMmsgPrint (CMmsgInfo,"%s [options] <input rgisdata> <output domain>\n",CMprgName(argv[0]));
			CMmsgPrint (CMmsgInfo,"     -l,--lengthcorrection\n");
			CMmsgPrint (CMmsgInfo,"     -h,--help\n");
			return (DBSuccess);
			}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1))
			{ CMmsgPrint (CMmsgUsrError,"Unknown option: %s!\n",argv [argPos]); return (CMfailed); }
		argPos++;
		}

	if (argNum > 3) { CMmsgPrint (CMmsgUsrError,"Extra arguments!\n"); return (CMfailed); }

	outFile = (argNum > 2) && (strcmp (argv [2],"-") != 0) ? fopen (argv [2],"w") : stdout;
	if (outFile == (FILE *) NULL)
		{ CMmsgPrint (CMmsgUsrError,"Output file Opening error in: %s\n",CMprgName(argv[0])); exit (DBFault); }

	data = new DBObjData ();
	ret = (argNum > 1) && (strcmp (argv [1],"-") != 0) ? data->Read (argv [1]) : data->Read (stdin);

	if ((domain = (MFDomain_t *) calloc (1,sizeof (MFDomain_t))) != (MFDomain_t *) NULL)
		{
		domain->Objects = (MFObject_t *) NULL;
		switch (data->Type ())
			{
			case DBTypeVectorPoint:
				{
				DBVPointIF *pntIF = new DBVPointIF (data);

				domain->ObjNum = pntIF->ItemNum ();
				if ((domain->Objects = (MFObject_t *) calloc (domain->ObjNum,sizeof (MFObject_t))) == (MFObject_t *) NULL)
					{ perror ("Memory Allocation Error"); MFDomainFree (domain); goto Stop; }
				for (objID = 0;objID < domain->ObjNum;++objID)
					{
					objRec = pntIF->Item (objID);
					coord = pntIF->Coordinate (objRec);
					domain->Objects [objID].ID = objRec->RowID ();
					domain->Objects [objID].DLinkNum = 0;
					domain->Objects [objID].ULinkNum = 0;
					domain->Objects [objID].DLinks = (size_t *) NULL;
					domain->Objects [objID].ULinks = (size_t *) NULL;
					domain->Objects [objID].XCoord = domain->Objects [objID].Lon = coord.X;
					domain->Objects [objID].YCoord = domain->Objects [objID].Lat = coord.Y;
					domain->Objects [objID].Area   = 0.0;
					domain->Objects [objID].Length = 0.0;
					}
				} break;
			case DBTypeGridContinuous:
			case DBTypeGridDiscrete:
				{
				} break;
			case DBTypeNetwork:
				{
				DBInt dir;
				DBObjRecord *nextCell;
				DBNetworkIF *netIF = new DBNetworkIF (data);
				domain->ObjNum = netIF->CellNum ();
				if ((domain->Objects = (MFObject_t *) calloc (domain->ObjNum,sizeof (MFObject_t))) == (MFObject_t *) NULL)
					{ perror ("Memory Allocation Error"); MFDomainFree (domain); goto Stop; }
				for (objID = 0;objID < domain->ObjNum;++objID)
					{
					domain->Objects [objID].DLinks = (size_t *) NULL;
					domain->Objects [objID].ULinks = (size_t *) NULL;
					}
				for (objID = 0;objID < domain->ObjNum;++objID)
					{
					objRec = netIF->Cell (objID);
					coord = netIF->Center (objRec);
					domain->Objects [objID].ID = objRec->RowID ();
					domain->Objects [objID].DLinkNum = 0;
					domain->Objects [objID].ULinkNum = 0;
					domain->Objects [objID].XCoord = domain->Objects [objID].Lon = coord.X;
					domain->Objects [objID].YCoord = domain->Objects [objID].Lat = coord.Y;
					domain->Objects [objID].Area   = netIF->CellArea   (objRec);
					domain->Objects [objID].Length = netIF->CellLength (objRec) *lCorrection;
					if ((nextCell = netIF->ToCell (objRec)) != (DBObjRecord *) NULL)
					{
						size = (domain->Objects [objID].DLinkNum + 1) * sizeof (size_t);
						if ((domain->Objects [objID].DLinks = (size_t *) realloc (domain->Objects [objID].DLinks,size)) == (size_t *) NULL)
							{ perror ("Memory Allocation Error"); MFDomainFree (domain);	goto Stop; }
						domain->Objects [objID].DLinks [domain->Objects [objID].DLinkNum] = nextCell->RowID ();
						domain->Objects [objID].DLinkNum++;
					}
					for (dir = 0;dir < 8;++dir)
						if ((nextCell = netIF->FromCell (objRec,0x01 << dir)) != (DBObjRecord *) NULL)
							{
							size = (domain->Objects [objID].ULinkNum + 1) * sizeof (size_t);
							if ((domain->Objects [objID].ULinks = (size_t *) realloc (domain->Objects [objID].ULinks,size)) == (size_t *) NULL)
								{ perror ("Memory Allocation Error"); MFDomainFree (domain);goto Stop; }
							domain->Objects [objID].ULinks [domain->Objects [objID].ULinkNum] = nextCell->RowID ();
							domain->Objects [objID].ULinkNum++;
							}
					}
				} break;
			}
		ret = MFDomainWrite (domain,outFile);
		}
Stop:
	if (outFile != stdout) fclose (outFile);
	return (ret);
	}
