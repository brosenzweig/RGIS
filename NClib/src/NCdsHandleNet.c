#include<NCdsHandle.h>

NCGstate NCGdsHandleNetworkDefine (NCGdsHandleNetwork_t *net, int ncid)
{
	int status;
	if (NCGdataGetType (ncid) != NCGtypeNetwork) 
	{ fprintf (stderr,"Invalid network in: NCGdsHandleNetworkDefine ()\n"); return (NCGfailed); }

	if (NCGdsHandleGLayoutDefine ((NCGdsHandleGLayout_t *) net, &ncid, 1) == NCGfailed) return (NCGfailed);
	
	net->Data = (int *) NULL; net->Basins.Table = net->Cells.Table = (NCGtable_t *) NULL;
	if ((net->Data = (int *) calloc (net->ColNum * net->RowNum,sizeof (int))) == (int *) NULL)
	{
		perror ("Memory allocation error in: NCGdsHandleNetworkDefine ()");
		NCGdsHandleNetworkClear (net);
		return (NCGfailed);
	}
	if ((status = nc_get_var_int (ncid,net->GVarIds [0],net->Data)) != NC_NOERR)
	{
		NCGprintNCError (status, "NCGdsHandleNetworkDefine");
		NCGdsHandleNetworkClear (net);
		return (NCGfailed);
	}

	if ((net->Basins.Table = NCGtableOpen (ncid, NCGnameTBItems)) == (NCGtable_t *) NULL) { NCGdsHandleNetworkClear (net); return (NCGfailed); }
	if ((net->Cells.Table  = NCGtableOpen (ncid, NCGnameTBCells)) == (NCGtable_t *) NULL) { NCGdsHandleNetworkClear (net); return (NCGfailed); }

	if (((net->Basins.NameFld    = NCGtableGetFieldByName (net->Basins.Table,NCGnameTBItems))     == (NCGfield_t *) NULL) ||
	    ((net->Basins.RowFld     = NCGtableGetFieldByName (net->Basins.Table,NCGnameBSNRow))      == (NCGfield_t *) NULL) ||
	    ((net->Basins.ColFld     = NCGtableGetFieldByName (net->Basins.Table,NCGnameBSNCol))      == (NCGfield_t *) NULL) ||
	    ((net->Basins.OrderFld   = NCGtableGetFieldByName (net->Basins.Table,NCGnameBSNOrder))    == (NCGfield_t *) NULL) ||
	    ((net->Basins.SymbolFld  = NCGtableGetFieldByName (net->Basins.Table,NCGnameBSNSymbol))   == (NCGfield_t *) NULL) ||
	    ((net->Basins.LengthFld  = NCGtableGetFieldByName (net->Basins.Table,NCGnameBSNLength))   == (NCGfield_t *) NULL) ||
	    ((net->Basins.AreaFld    = NCGtableGetFieldByName (net->Basins.Table,NCGnameBSNArea))     == (NCGfield_t *) NULL) ||
	    ((net->Cells.RowFld      = NCGtableGetFieldByName (net->Cells.Table, NCGnameCLSRow))      == (NCGfield_t *) NULL) ||
	    ((net->Cells.ColFld      = NCGtableGetFieldByName (net->Cells.Table, NCGnameCLSCol))      == (NCGfield_t *) NULL) ||
	    ((net->Cells.ToCellFld   = NCGtableGetFieldByName (net->Cells.Table, NCGnameCLSToCell))   == (NCGfield_t *) NULL) ||
	    ((net->Cells.FromCellFld = NCGtableGetFieldByName (net->Cells.Table, NCGnameCLSFromCell)) == (NCGfield_t *) NULL) ||
	    ((net->Cells.BasinFld    = NCGtableGetFieldByName (net->Cells.Table, NCGnameCLSBasinId))  == (NCGfield_t *) NULL) ||
	    ((net->Cells.NCellsFld   = NCGtableGetFieldByName (net->Cells.Table, NCGnameCLSNCells))   == (NCGfield_t *) NULL) ||
	    ((net->Cells.TravelFld   = NCGtableGetFieldByName (net->Cells.Table, NCGnameCLSTravel))   == (NCGfield_t *) NULL) ||
	    ((net->Cells.OrderFld    = NCGtableGetFieldByName (net->Cells.Table, NCGnameCLSOrder))    == (NCGfield_t *) NULL) ||
	    ((net->Cells.LengthFld   = NCGtableGetFieldByName (net->Cells.Table, NCGnameCLSLength))   == (NCGfield_t *) NULL) ||
	    ((net->Cells.AreaFld     = NCGtableGetFieldByName (net->Cells.Table, NCGnameCLSArea))     == (NCGfield_t *) NULL))
	{
		fprintf (stderr,"Corrupt networkd data in: NCGdsHandleNetworkDefine ()\n");
		NCGdsHandleNetworkClear (net);
		return (NCGfailed);
	}
	return (NCGsucceeded);
}

void NCGdsHandleNetworkClear (NCGdsHandleNetwork_t *net)
{
	NCGdsHandleGLayoutClear ((NCGdsHandleGLayout_t *) net);

	if (net->Data != (int *) NULL) free (net->Data);
	if (net->Basins.Table != (NCGtable_t *) NULL) NCGtableClose (net->Basins.Table);
	if (net->Cells.Table  != (NCGtable_t *) NULL) NCGtableClose (net->Cells.Table);
}


