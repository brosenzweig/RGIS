#include<NCdsHandle.h>

NCstate NCdsHandleNetworkDefine (NCdsHandleNetwork_t *net, int ncid)
{
	int status;
	if (NCdataGetType (ncid) != NCtypeNetwork) 
	{ fprintf (stderr,"Invalid network in: NCdsHandleNetworkDefine ()\n"); return (NCfailed); }

	if (NCdsHandleGLayoutDefine ((NCdsHandleGLayout_t *) net, &ncid, 1) == NCfailed) return (NCfailed);
	
	net->Data = (int *) NULL; net->Basins.Table = net->Cells.Table = (NCtable_t *) NULL;
	if ((net->Data = (int *) calloc (net->ColNum * net->RowNum,sizeof (int))) == (int *) NULL)
	{
		perror ("Memory allocation error in: NCdsHandleNetworkDefine ()");
		NCdsHandleNetworkClear (net);
		return (NCfailed);
	}
	if ((status = nc_get_var_int (ncid,net->GVarIds [0],net->Data)) != NC_NOERR)
	{
		NCprintNCError (status, "NCdsHandleNetworkDefine");
		NCdsHandleNetworkClear (net);
		return (NCfailed);
	}

	if ((net->Basins.Table = NCtableOpen (ncid, NCnameTBItems)) == (NCtable_t *) NULL) { NCdsHandleNetworkClear (net); return (NCfailed); }
	if ((net->Cells.Table  = NCtableOpen (ncid, NCnameTBCells)) == (NCtable_t *) NULL) { NCdsHandleNetworkClear (net); return (NCfailed); }

	if (((net->Basins.NameFld    = NCtableGetFieldByName (net->Basins.Table,NCnameTBItems))     == (NCfield_t *) NULL) ||
	    ((net->Basins.RowFld     = NCtableGetFieldByName (net->Basins.Table,NCnameBSNRow))      == (NCfield_t *) NULL) ||
	    ((net->Basins.ColFld     = NCtableGetFieldByName (net->Basins.Table,NCnameBSNCol))      == (NCfield_t *) NULL) ||
	    ((net->Basins.OrderFld   = NCtableGetFieldByName (net->Basins.Table,NCnameBSNOrder))    == (NCfield_t *) NULL) ||
	    ((net->Basins.SymbolFld  = NCtableGetFieldByName (net->Basins.Table,NCnameBSNSymbol))   == (NCfield_t *) NULL) ||
	    ((net->Basins.LengthFld  = NCtableGetFieldByName (net->Basins.Table,NCnameBSNLength))   == (NCfield_t *) NULL) ||
	    ((net->Basins.AreaFld    = NCtableGetFieldByName (net->Basins.Table,NCnameBSNArea))     == (NCfield_t *) NULL) ||
	    ((net->Cells.RowFld      = NCtableGetFieldByName (net->Cells.Table, NCnameCLSRow))      == (NCfield_t *) NULL) ||
	    ((net->Cells.ColFld      = NCtableGetFieldByName (net->Cells.Table, NCnameCLSCol))      == (NCfield_t *) NULL) ||
	    ((net->Cells.ToCellFld   = NCtableGetFieldByName (net->Cells.Table, NCnameCLSToCell))   == (NCfield_t *) NULL) ||
	    ((net->Cells.FromCellFld = NCtableGetFieldByName (net->Cells.Table, NCnameCLSFromCell)) == (NCfield_t *) NULL) ||
	    ((net->Cells.BasinFld    = NCtableGetFieldByName (net->Cells.Table, NCnameCLSBasinId))  == (NCfield_t *) NULL) ||
	    ((net->Cells.NCellsFld   = NCtableGetFieldByName (net->Cells.Table, NCnameCLSNCells))   == (NCfield_t *) NULL) ||
	    ((net->Cells.TravelFld   = NCtableGetFieldByName (net->Cells.Table, NCnameCLSTravel))   == (NCfield_t *) NULL) ||
	    ((net->Cells.OrderFld    = NCtableGetFieldByName (net->Cells.Table, NCnameCLSOrder))    == (NCfield_t *) NULL) ||
	    ((net->Cells.LengthFld   = NCtableGetFieldByName (net->Cells.Table, NCnameCLSLength))   == (NCfield_t *) NULL) ||
	    ((net->Cells.AreaFld     = NCtableGetFieldByName (net->Cells.Table, NCnameCLSArea))     == (NCfield_t *) NULL))
	{
		fprintf (stderr,"Corrupt networkd data in: NCdsHandleNetworkDefine ()\n");
		NCdsHandleNetworkClear (net);
		return (NCfailed);
	}
	return (NCsucceeded);
}

void NCdsHandleNetworkClear (NCdsHandleNetwork_t *net)
{
	NCdsHandleGLayoutClear ((NCdsHandleGLayout_t *) net);

	if (net->Data != (int *) NULL) free (net->Data);
	if (net->Basins.Table != (NCtable_t *) NULL) NCtableClose (net->Basins.Table);
	if (net->Cells.Table  != (NCtable_t *) NULL) NCtableClose (net->Cells.Table);
}


