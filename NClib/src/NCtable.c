#include <string.h>
#include <cm.h>
#include <NCtable.h>
#include <NCstdlib.h>

void NCtableClose(NCtable_t *tbl)
	{
	register int i;
	if (tbl == (NCtable_t *) NULL) return;
	for (i = 0; i < tbl->NFields; i++)
	{
		if (tbl->Fields [i].Data != (void *) NULL) free (tbl->Fields [i].Data);
		if (tbl->Fields [i].Name != (char *) NULL) free (tbl->Fields [i].Name);
	}
	if (tbl->Name != (char *) NULL) free(tbl->Name);
	if (tbl->Fields != (NCfield_t *) NULL) free(tbl->Fields); // Allocated with realloc
	free(tbl);
}

static int _NCtableVarLen(int ncid, int varid, int dimid, int *dimids)
{
	int status, ndims, i, len = 1;
	size_t dimlen;

	if((status = nc_inq_varndims (ncid,varid,&ndims))  != NC_NOERR)
	{ NCprintNCError (status,"_NCtableVarCheck"); return (NCfailed); }
	if((status = nc_inq_vardimid (ncid,varid,dimids)) != NC_NOERR)
	{ NCprintNCError (status,"_NCtableVarCheck"); return (NCfailed); }
	if(dimids[0] != dimid) return (NCfailed);
	for(i = 1; i < ndims; i++)
	{
		if((status = nc_inq_dimlen(ncid,dimids [i],&dimlen)) != NC_NOERR)
		{ NCprintNCError (status,"_NCtableVarCheck"); return (NCfailed); }
		len = len * dimlen;
	}
	return (len);
}

NCtable_t *NCtableOpen(int ncid, char *tablename)
{
	int status, *dimids, dimid, i = 0, j = 0;
	int ndims, nvars;
	size_t nrecords;
	size_t len;
	char varname [NC_MAX_NAME];
	nc_type type;
	NCfield_t *field = (NCfield_t *) NULL;
	NCtable_t *tbl   = (NCtable_t *) NULL;

	if((status = nc_inq_dimid  (ncid,tablename,&dimid)) != NC_NOERR)
	{ NCprintNCError (status,"NCtableOpen"); return ((NCtable_t *) NULL); }
	if((status = nc_inq_dimlen (ncid,dimid,&nrecords))  != NC_NOERR)
	{ NCprintNCError (status,"NCtableOpen"); return ((NCtable_t *) NULL); }
	if((status = nc_inq_nvars  (ncid,&nvars))           != NC_NOERR)
	{ NCprintNCError (status,"NCtableOpen"); return ((NCtable_t *) NULL); }
	if(nc_inq_ndims(ncid,&ndims) != NC_NOERR)
	{ NCprintNCError (status,"NCtableOpen"); return ((NCtable_t *) NULL); }
	if ((dimids = (int *) malloc(sizeof(int) * ndims)) == (int *) NULL)
	{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return (NCtable_t *) NULL; }

	if ((tbl = (NCtable_t *) malloc(sizeof(NCtable_t))) == (NCtable_t *) NULL)
	{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); free (dimids); return ((NCtable_t *) NULL); }
	if ((tbl->Name = (char *) malloc (strlen(tablename) + 1)) == (char *) NULL)
	{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); free (dimids); free (tbl); return ((NCtable_t *) NULL); }
	strcpy(tbl->Name,tablename);
	tbl->NFields  = 0;
	tbl->Fields   = (NCfield_t *) NULL;

	for(i = 0; i < nvars; i++)
	{
		if ((status = nc_inq_vartype(ncid,i,&type)) != NC_NOERR)
		{ NCprintNCError (status,"NCtableOpen"); free (dimids); NCtableClose (tbl); return ((NCtable_t *) NULL); }
		if ((len = _NCtableVarLen(ncid,i,dimid,dimids)) == NCfailed) continue;
		if ((type != NC_CHAR) && (len > 1)) continue;

		if((status = nc_inq_varname(ncid,i,varname)) != NC_NOERR)
		{ NCprintNCError (status,"NCtableOpen"); free (dimids); NCtableClose (tbl); return ((NCtable_t *) NULL); }

		if ((tbl->Fields = (NCfield_t *) realloc (tbl->Fields, sizeof (NCfield_t) * (tbl->NFields + 1))) == (NCfield_t *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Error allocating memory in: %s %d",__FILE__,__LINE__); free (dimids); NCtableClose (tbl); return ((NCtable_t *) NULL); }
		field = tbl->Fields + tbl->NFields;
		tbl->NFields += 1;
		field->NRecords = nrecords;
		field->Data = (void *) NULL;
		field->Name = (char *) NULL;
		field->Type = type;
		field->Len  = len;

		if ((field->Name = malloc (strlen (varname) + 1)) == (char *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Error allocating memory in: %s %d",__FILE__,__LINE__); free (dimids); NCtableClose (tbl); return ((NCtable_t *) NULL); }
		strcpy (field->Name,varname);

		if (nc_get_att_double (ncid,i,NCnameVAScaleFactor,&field->Scale)      != NC_NOERR) field->Scale      = 1.0;
		if (nc_get_att_double (ncid,i,NCnameVAAddOffset,  &field->Offset)     != NC_NOERR) field->Offset     = 0.0;

		switch(field->Type)
		{
			case NC_CHAR:
				if ((field->Data = (void *) malloc(field->NRecords * field->Len * sizeof (char)))   == (void *) NULL)
				{ CMmsgPrint (CMmsgSysError, "Error allocating memory in: %s %d",__FILE__,__LINE__); free (dimids); NCtableClose (tbl); return ((NCtable_t *) NULL); }
				if((status = nc_get_var_text(ncid,i,(char *) (field->Data))) != NC_NOERR)
				{ NCprintNCError (status,"NCtableOpen"); free (dimids); NCtableClose (tbl); return ((NCtable_t *) NULL); }
				break;
			case NC_BYTE:
			case NC_SHORT:
			case NC_INT:
				if ((field->Data = (void *) malloc(field->NRecords * field->Len * sizeof (int)))   == (void *) NULL)
				{ CMmsgPrint (CMmsgSysError, "Error allocating memory in: %s %d",__FILE__,__LINE__); free (dimids); NCtableClose (tbl); return ((NCtable_t *) NULL); }
				if((status = nc_get_var_int(ncid,i,(int *) (field->Data))) != NC_NOERR)
				{ NCprintNCError (status,"NCtableOpen"); free (dimids); NCtableClose (tbl); return ((NCtable_t *) NULL); }
				if (nc_get_att_int    (ncid,i,NCnameVAFillValue,  &field->FillValue.Int)  != NC_NOERR) field->FillValue.Int  = INT_NOVALUE;
				if (nc_get_att_double (ncid,i,NCnameVAMissingVal, &field->MissingVal)     != NC_NOERR) field->MissingVal     = FLOAT_NOVALUE;
				break;
			case NC_FLOAT:
			case NC_DOUBLE:
				if ((field->Data = (void *) malloc(field->NRecords * field->Len * sizeof (double))) == (void *) NULL)
				{ CMmsgPrint (CMmsgSysError, "Error allocating memory in: %s %d",__FILE__,__LINE__); free (dimids); NCtableClose (tbl); return ((NCtable_t *) NULL); }
				if((status = nc_get_var_double(ncid,i,(double *) (field->Data))) != NC_NOERR)
				{ NCprintNCError (status,"NCtableOpen"); free (dimids); NCtableClose (tbl); return ((NCtable_t *) NULL); }
				if (nc_get_att_double (ncid,i,NCnameVAFillValue,  &field->FillValue.Float)  != NC_NOERR) field->FillValue.Float = FLOAT_NOVALUE;
				if (nc_get_att_double (ncid,i,NCnameVAMissingVal, &field->MissingVal)       != NC_NOERR) field->MissingVal      = FLOAT_NOVALUE;
				break;
			default:        field->Data = (void *) NULL; break;
		}
		if(GetDebug()) CMmsgPrint (CMmsgUsrError, "Loaded: %s(dimid: %d)\n",field->Name,dimid);
	}

	if(GetDebug())
	{
		CMmsgPrint (CMmsgUsrError, "Dim: %d Name: %s Cols: %d Rows: %d\n",dimid,tbl->Name,tbl->NFields,field->NRecords);
		for(i = 0; i < tbl->NFields; i++)
		{
			field = tbl->Fields + i;
			CMmsgPrint (CMmsgUsrError, "\tField: %d Name: %s ",i,field->Name);
			switch(field->Type)
			{
				case NC_CHAR:
					if(field->Len == 1)
					{
						CMmsgPrint (CMmsgUsrError, "Type: char\n");
						for(j = 0; j < 5; j++) CMmsgPrint (CMmsgUsrError, "\t\t%d %c\n",j,((char *) (field->Data)) [j]);
					}
					else
					{
						CMmsgPrint (CMmsgUsrError, "Type: string\n");
						for(j = 0; j < 5; j++) CMmsgPrint (CMmsgUsrError, "\t\t%d %s\n",j,((char *) (field->Data)) + j * field->Len);
					}
					break;
				case NC_BYTE:
				case NC_SHORT:
				case NC_INT:
					CMmsgPrint (CMmsgUsrError, "Type: int\n");
					for(j = 0; j < 5; j++) CMmsgPrint (CMmsgUsrError, "\t\t%d %i\n",j,((int *)    (field->Data)) [j]);
					break;
				case NC_FLOAT:
				case NC_DOUBLE:
					CMmsgPrint (CMmsgUsrError, "Type: double\n");
					for(j = 0; j < 5; j++) CMmsgPrint (CMmsgUsrError, "\t\t%d %f\n",j,((double *) (field->Data)) [j]);
					break;
				default: break;
			}
		}
	}
	return (tbl);
}

void NCtableExportAscii(NCtable_t *tbl, FILE *file)
{
	int i,j, intVal;
	double floatVal;
	NCfield_t *field;

	for(j = 0; j < tbl->NFields; j++) { if (j > 0) fprintf(file,"\t"); fprintf(file,"\"%s\"",tbl->Fields [j].Name); }
	fprintf (file,"\n");

	if (tbl->NFields > 0)
		for(i = 0; i < tbl->Fields [0].NRecords; i++)
		{
			for(j = 0; j < tbl->NFields; j++)
			{
				if(j > 0) fprintf(file,"\t");
				field = tbl->Fields + j;
				switch(field->Type)
				{
					default:
					case NC_CHAR:
						if(field->Len == 1) fprintf(file,"%c",((char *) field->Data) [i]);
						else fprintf(file,"\"%s\"",((char *) field->Data) + i * field->Len);
						break;
					case NC_BYTE:
					case NC_SHORT:
					case NC_INT:
						if (NCfieldGetInt   (field, i, &intVal))  fprintf(file,"%i",intVal);   else fprintf (file,"-"); break;
					case NC_FLOAT:
					case NC_DOUBLE:
						if (NCfieldGetFloat (field,i, &floatVal)) fprintf(file,"%f",floatVal); else fprintf (file,"-"); break;
				}
			}
			fprintf(file,"\n");
		}
}

NCfield_t *NCtableAddField (NCtable_t *tbl, char *name, nc_type type)
{
	int i;
	NCfield_t *field;

	if ((tbl->Fields = (NCfield_t *) realloc(tbl->Fields, sizeof (NCfield_t) * (tbl->NFields + 1))) == (NCfield_t *) NULL)
	{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return ((NCfield_t *) NULL); }
	field = tbl->Fields + tbl->NFields;
	field->NRecords = tbl->NFields > 1 ? tbl->Fields [tbl->NFields - 1].NRecords : 0;
	field->Data = (void *) NULL;
	field->Name = (char *) NULL;
	field->Type = type;
	field->Scale = 1.0;
	field->Offset = 0.0;
	tbl->NFields += 1;
	if ((field->Name = (char *) malloc(strlen (name) + 1)) == (char *) NULL)
	{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return ((NCfield_t *) NULL); }
	strcpy(field->Name,name);

	switch (field->Type)
	{
		default:
		case NC_CHAR:
			if ((field->Data = (void *) malloc (field->NRecords * sizeof (char)))   == (void *) NULL)
			{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return ((NCfield_t *) NULL); }
			for (i = 0; i < field->NRecords; i++) ((char *)   field->Data) [i] = '\0';
			break;
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:
			if ((field->Data = (void *) malloc (field->NRecords * sizeof (int)))    == (void *) NULL)
			{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return ((NCfield_t *) NULL); }
			field->MissingVal = INT_NOVALUE;
			field->FillValue.Int = INT_NOVALUE;
			for (i = 0; i < field->NRecords; i++) ((int *)    field->Data) [i] = INT_NOVALUE;
			break;
		case NC_FLOAT:
		case NC_DOUBLE:
			if ((field->Data = (void *) malloc (field->NRecords * sizeof (double))) == (void *) NULL)
			{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return ((NCfield_t *) NULL); }
			field->MissingVal = FLOAT_NOVALUE;
			field->FillValue.Float = FLOAT_NOVALUE;
			for (i = 0; i < field->NRecords; i++) ((double *) field->Data) [i] = FLOAT_NOVALUE;
			break;
	}
	return (field);
}

NCstate NCtableCommit(int ncid, NCtable_t *tbl)
{
	NCstate ret = NCsucceeded;
	int j;
	for(j = 1; j < tbl->NFields; j++)
		if (NCtableCommitField (ncid,tbl->Name,tbl->Fields + j) != NCsucceeded) ret = NCfailed;
	return (ret);
}

NCstate NCtableCommitField (int ncid, char *tablename, NCfield_t *field)
{
	int status, dimid, varid;

	if (nc_inq_varid(ncid,field->Name,&varid) != NC_NOERR)
	{
		if(field->Len > 1) return (NCfailed);
		if((status = nc_inq_dimid(ncid,tablename,&dimid)) != NC_NOERR)
		{ NCprintNCError (status,"NCtableCommitField"); return (NCfailed);}
		if((status = nc_redef(ncid) != NC_NOERR))
		{ NCprintNCError (status,"NCtableCommitField"); return (NCfailed);}
		if ((status = nc_def_var(ncid,field->Name,field->Type,1,&dimid,&varid)) != NC_NOERR)
		{ NCprintNCError (status,"NCtableCommitField"); return (NCfailed);}
		if((status = nc_enddef(ncid) != NC_NOERR))
		{ NCprintNCError (status,"NCtableCommitField"); return (NCfailed);}
	}
	switch (field->Type)
	{
		default:
		case NC_CHAR:
			if((status = nc_put_var_text(ncid,varid,(char *) field->Data)) != NC_NOERR)
			{ NCprintNCError (status,"NCtableCommitField"); return (NCfailed);}
			break;
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:
			if((status = nc_put_var_int(ncid,varid, (int *)  field->Data)) != NC_NOERR)
			{ NCprintNCError (status,"NCtableCommitField"); return (NCfailed);}
			break;
		case NC_FLOAT:
		case NC_DOUBLE:
			if((status = nc_put_var_double(ncid,varid,(double *) field->Data)) != NC_NOERR)
			{ NCprintNCError (status,"NCtableCommitField"); return (NCfailed);}
			break;
	}
	return (NCsucceeded);
}

NCfield_t *NCtableGetFieldByName (NCtable_t *table,const char *name)
{
	int i;

	for (i = 0;i < table->NFields;++i) if (strcmp (table->Fields [i].Name,name) == 0) return (table->Fields + i);
	return ((NCfield_t *) NULL);
}

NCfield_t *NCtableGetFieldById (NCtable_t *table,int id)
{
	if ((id >= 0) && (id < table->NFields)) return (table->Fields + id);
	return ((NCfield_t *) NULL);
}

bool NCfieldGetInt (NCfield_t *field,int id, int *val)
{
	int    intVal;
	double floatVal;

	switch (field->Type)
	{
		default:
		case NC_CHAR: goto NOVALUE;
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:
			if ((id < 0) && (id >= field->NRecords)) goto NOVALUE;
			intVal = ((int *) field->Data) [id];
			if (intVal == field->FillValue.Int)      goto NOVALUE;
			if (NCmathEqualValues (field->Scale,1.0) && ((int) field->Offset == 0)) { *val = intVal; return (true); }
			floatVal = (double) intVal * field->Scale + field->Offset;
			break;
		case NC_FLOAT:
		case NC_DOUBLE:
			if ((id < 0) && (id >= field->NRecords)) goto NOVALUE;
			floatVal = ((double *) field->Data) [id];
			if (NCmathEqualValues (floatVal, field->FillValue.Float)) goto NOVALUE;
			floatVal = floatVal * field->Scale + field->Offset;
			break;
	}
	if (NCmathEqualValues (floatVal,field->MissingVal)) goto NOVALUE; 
	*val = (int) floatVal;
	return (true);

NOVALUE:
	*val = INT_NOVALUE;
	return (false);
}

NCstate NCfieldSetInt (NCfield_t *field,int id, int setVal)
{
	if ((id < 0) && (id >= field->NRecords)) return (NCfailed);
	if(setVal == INT_NOVALUE) NCfieldSetFill (field, id);
	switch (field->Type)
	{
		default:
		case NC_CHAR:  return (NCfailed);
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:    ((int *)    field->Data) [id] = (int) ((setVal - field->Offset) / field->Scale);  break;
		case NC_FLOAT:
		case NC_DOUBLE: ((double *) field->Data) [id] = ((double) setVal - field->Offset) / field->Scale; break;
	}
	return (NCsucceeded);
}

bool NCfieldGetFloat (NCfield_t *field,int id, double *val)
{
	int      intVal;
	double   floatVal;

	switch (field->Type)
	{
		default:
		case NC_CHAR: goto NOVALUE;
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:
			if ((id < 0) && (id >= field->NRecords)) goto NOVALUE;
			intVal = ((int *) field->Data) [id];
			if (intVal == field->FillValue.Int)      goto NOVALUE;
			floatVal = (double) intVal * field->Scale + field->Offset;
			break;
		case NC_FLOAT:
		case NC_DOUBLE:
			if ((id < 0) && (id >= field->NRecords)) goto NOVALUE;
			floatVal = ((double *) field->Data) [id];
			if (NCmathEqualValues (floatVal, field->FillValue.Float)) goto NOVALUE;
			floatVal = floatVal * field->Scale + field->Offset;
			break;
	}
	if (NCmathEqualValues (floatVal, field->MissingVal)) goto NOVALUE;
	*val = floatVal;
	return (true);

NOVALUE:
	*val = FLOAT_NOVALUE;
	return (false);
}

NCstate NCfieldSetFloat (NCfield_t *field,int id, double setVal)
{
	if ((id < 0) && (id >= field->NRecords)) return (NCfailed);
	switch (field->Type)
	{
		default:
		case NC_CHAR:  return (NCfailed);
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:    ((int *)    field->Data) [id] = (int) ((setVal - field->Offset) / field->Scale); break;
		case NC_FLOAT:
		case NC_DOUBLE: ((double *) field->Data) [id] = (setVal - field->Offset) / field->Scale; break;
	}
	return (NCsucceeded);
}

NCstate NCfieldSetFill (NCfield_t *field,int id)
{
	if ((id < 0) && (id >= field->NRecords)) return (NCfailed);
	switch (field->Type)
	{
		default:
		case NC_CHAR:  return (NCfailed);
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:    ((int *)    field->Data) [id] = field->FillValue.Int;   break;
		case NC_FLOAT:
		case NC_DOUBLE: ((double *) field->Data) [id] = field->FillValue.Float; break;
	}
	return (NCsucceeded);
}
