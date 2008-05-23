/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

MFDomain.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <cm.h>
#include <MF.h>

void MFDomainFree (MFDomain_t *domain) {
	int objID;

	if (domain->Objects != (MFObject_t *) NULL) {
		for (objID = 0;objID < domain->ObjNum;++objID) {
			if (domain->Objects [objID].DLinks != (size_t *) NULL) free (domain->Objects [objID].DLinks);
			if (domain->Objects [objID].ULinks != (size_t *) NULL) free (domain->Objects [objID].ULinks);
		}
		free (domain->Objects);
	}
	free (domain);	
}

MFDomain_t *MFDomainGet (FILE *inFile) {
	int objID, i;
	MFDomain_t *domain;

	if ((domain = (MFDomain_t *) calloc (1,sizeof (MFDomain_t))) == (MFDomain_t *) NULL) return ((MFDomain_t *) NULL);
	domain->Objects = (MFObject_t *) NULL;

	if (fread (domain,sizeof (MFDomain_t) - sizeof (MFObject_t *),1,inFile) != 1) {
		CMmsgPrint (CMmsgSysError,"File Reading Error in: %s:%d\n",__FILE__,__LINE__);
		MFDomainFree (domain);
		return ((MFDomain_t *) NULL);
	}
	if (domain->Swap != 1) {
		MFSwapHalfWord (&(domain->Type));		
		MFSwapWord (&(domain->ObjNum));
	}
	if ((domain->Objects = (MFObject_t *) calloc (domain->ObjNum,sizeof (MFObject_t))) == (MFObject_t *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d\n",__FILE__,__LINE__);
		MFDomainFree (domain);
		return ((MFDomain_t *) NULL);
	}
	for (objID = 0;objID < domain->ObjNum;++objID) {
		domain->Objects [objID].DLinks = (size_t *) NULL;
		domain->Objects [objID].ULinks = (size_t *) NULL;
	}
	for (objID = 0;objID < domain->ObjNum;++objID) {
		if (fread (domain->Objects + objID,sizeof (MFObject_t) - 2 * sizeof (MFObject_t *),1,inFile) != 1) {
			CMmsgPrint (CMmsgSysError,"File Reading Error in: %s:%d\n",__FILE__,__LINE__);
			MFDomainFree (domain);
			return ((MFDomain_t *) NULL);
		}
		if (domain->Swap != 1) {
			MFSwapWord (&(domain->Objects [objID].ID));		
			MFSwapHalfWord (&(domain->Objects [objID].DLinkNum));		
			MFSwapHalfWord (&(domain->Objects [objID].ULinkNum));		
			MFSwapWord (&(domain->Objects [objID].XCoord));		
			MFSwapWord (&(domain->Objects [objID].YCoord));		
			MFSwapWord (&(domain->Objects [objID].Lon));		
			MFSwapWord (&(domain->Objects [objID].Lat));		
			MFSwapWord (&(domain->Objects [objID].Area));		
			MFSwapWord (&(domain->Objects [objID].Length));		
		}
		if (domain->Objects [objID].DLinkNum > 0) {
			domain->Objects [objID].DLinks = (size_t *) calloc (domain->Objects [objID].DLinkNum,sizeof (size_t *));
			if (domain->Objects [objID].DLinks == (size_t *) NULL) {
				CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d\n",__FILE__,__LINE__);
				MFDomainFree (domain);
				return ((MFDomain_t *) NULL);
			}
			if (fread (domain->Objects [objID].DLinks,sizeof (size_t),domain->Objects [objID].DLinkNum,inFile) == domain->Objects [objID].DLinkNum) {
				if (domain->Swap != 1)
					for (i = 0;i < domain->Objects [objID].DLinkNum; ++i)
						MFSwapWord (domain->Objects [objID].DLinks + i);
			}
			else {
				CMmsgPrint (CMmsgSysError,"File Reading Error in: %s:%d\n",__FILE__,__LINE__);
				MFDomainFree (domain);
				return ((MFDomain_t *) NULL);
			}
		}
		if (domain->Objects [objID].ULinkNum > 0) {
			domain->Objects [objID].ULinks = (size_t *) calloc (domain->Objects [objID].ULinkNum,sizeof (size_t));
			if (domain->Objects [objID].ULinks == (size_t *) NULL) {
				CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d\n",__FILE__,__LINE__);
				MFDomainFree (domain);
				return ((MFDomain_t *) NULL);
			}
			if (fread (domain->Objects [objID].ULinks,sizeof (size_t),domain->Objects [objID].ULinkNum,inFile) == domain->Objects [objID].ULinkNum) {
				if (domain->Swap != 1)
					for (i = 0;i < domain->Objects [objID].ULinkNum; ++i)
						MFSwapWord (domain->Objects [objID].ULinks + i);
			}
			else {
				CMmsgPrint (CMmsgSysError,"File Reading Error in: %s:%d\n",__FILE__,__LINE__);
				MFDomainFree (domain);
				return ((MFDomain_t *) NULL);
			}
		}
	}
	return (domain);
}	

int MFDomainWrite (MFDomain_t *domain,FILE *outFile) {
	int objID;

	domain->Swap = 1;
	if (fwrite (domain,sizeof (MFDomain_t) - sizeof (MFObject_t *),1,outFile) != 1) {
		CMmsgPrint (CMmsgSysError,"File Writing Error in: %s:%d\n",__FILE__,__LINE__);
		return (CMfailed);
	}

	for (objID = 0;objID < domain->ObjNum;++objID) {
		if (fwrite (domain->Objects + objID,sizeof (MFObject_t) - 2 * sizeof (MFObject_t *),1,outFile) != 1) {
			CMmsgPrint (CMmsgSysError,"File Writng Error in: %s:%d\n",__FILE__,__LINE__);
			return (CMfailed);
		}
		if (domain->Objects [objID].DLinkNum > 0) {
			if (fwrite (domain->Objects [objID].DLinks,sizeof (size_t),domain->Objects [objID].DLinkNum,outFile) != domain->Objects [objID].DLinkNum) {
				CMmsgPrint (CMmsgSysError,"File Writing Error in: %s:%d\n",__FILE__,__LINE__);
				return (CMfailed);
			}
		}
		if (domain->Objects [objID].ULinkNum > 0) {
			if (fwrite (domain->Objects [objID].ULinks,sizeof (size_t),domain->Objects [objID].ULinkNum,outFile) != domain->Objects [objID].ULinkNum) {
				CMmsgPrint (CMmsgSysError,"File Writing Error in: %s:%d\n",__FILE__,__LINE__);
				return (CMfailed);
			}
		}
	}
	return (CMsucceeded);
}
