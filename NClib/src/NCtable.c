#include <NCtable.h>

void NCGtableClose(NCGtable_t *tbl)
	{
	register int i;
	if (tbl == (NCGtable_t *) NULL) return;
	for (i = 0; i < tbl->NFields; i++)
	{
		if (tbl->Fields [i].Data != (void *) NULL) free (tbl->Fields [i].Data);
		if (tbl->Fields [i].Name != (char *) NULL) free (tbl->Fields [i].Name);
	}
	if (tbl->Name != (char *) NULL) free(tbl->Name);
	if (tbl->Fields != (NCGfield_t *) NULL) free(tbl->Fields); // Allocated with realloc
	free(tbl);
}

static int _NCGtableVarLen(int ncid, int varid, int dimid, int *dimids)
{
	int status, ndims, i, len = 1, dimlen;

	if((status = nc_inq_varndims (ncid,varid,&ndims))  != NC_NOERR)
	{ NCGprintNCError (status,"_NCGtableVarCheck"); return (NCGfailed); }
	if((status = nc_inq_vardimid (ncid,varid,dimids)) != NC_NOERR)
	{ NCGprintNCError (status,"_NCGtableVarCheck"); return (NCGfailed); }
	if(dimids[0] != dimid) return (NCGfailed);
	for(i = 1; i < ndims; i++)
	{
		if((status = nc_inq_dimlen(ncid,dimids [i],&dimlen)) != NC_NOERR)
		{ NCGprintNCError (status,"_NCGtableVarCheck"); return (NCGfailed); }
		len = len * dimlen;
	}
	return (len);
}

NCGtable_t *NCGtableOpen(int ncid, char *tablename)
{
	int status, *dimids, dimid, i = 0, j = 0;
	int ndims, nvars, nrecords;
	size_t len;
	char varname [NC_MAX_NAME];
	nc_type type;
	NCGfield_t *field = (NCGfield_t *) NULL;
	NCGtable_t *tbl   = (NCGtable_t *) NULL;

	if((status = nc_inq_dimid  (ncid,tablename,&dimid)) != NC_NOERR)
	{ NCGprintNCError (status,"NCGtableOpen"); return ((NCGtable_t *) NULL); }
	if((status = nc_inq_dimlen (ncid,dimid,&nrecords))  != NC_NOERR)
	{ NCGprintNCError (status,"NCGtableOpen"); return ((NCGtable_t *) NULL); }
	if((status = nc_inq_nvars  (ncid,&nvars))           != NC_NOERR)
	{ NCGprintNCError (status,"NCGtableOpen"); return ((NCGtable_t *) NULL); }
	if(nc_inq_ndims(ncid,&ndims) != NC_NOERR)
	{ NCGprintNCError (status,"NCGtableOpen"); return ((NCGtable_t *) NULL); }
	if ((dimids = (int *) malloc(sizeof(int) * ndims)) == (int *) NULL)
	{ perror("Memory allocation error in: NCGtableOpen ()"); return (NCGtable_t *) NULL; }

	if ((tbl = (NCGtable_t *) malloc(sizeof(NCGtable_t))) == (NCGtable_t *) NULL)
	{ perror("Memory allocation error in: NCGtableOpen ()"); free (dimids); return ((NCGtable_t *) NULL); }
	if ((tbl->Name = (char *) malloc (strlen(tablename) + 1)) == (char *) NULL)
	{ perror("Memory allocation error in: NCGtableOpen ()"); free (dimids); free (tbl); return ((NCGtable_t *) NULL); }
	strcpy(tbl->Name,tablename);
	tbl->NFields  = 0;
	tbl->Fields   = (NCGfield_t *) NULL;

	for(i = 0; i < nvars; i++)
	{
		if ((status = nc_inq_vartype(ncid,i,&type)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGtableOpen"); free (dimids); NCGtableClose (tbl); return ((NCGtable_t *) NULL); }
		if ((len = _NCGtableVarLen(ncid,i,dimid,dimids)) == NCGfailed) continue;
		if ((type != NC_CHAR) && (len > 1)) continue;

		if((status = nc_inq_varname(ncid,i,varname)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGtableOpen"); free (dimids); NCGtableClose (tbl); return ((NCGtable_t *) NULL); }

		if ((tbl->Fields = (NCGfield_t *) realloc (tbl->Fields, sizeof (NCGfield_t) * (tbl->NFields + 1))) == (NCGfield_t *) NULL)
		{ perror ("Error allocating memory in: NCGtableOpen ()"); free (dimids); NCGtableClose (tbl); return ((NCGtable_t *) NULL); }
		field = tbl->Fields + tbl->NFields;
		tbl->NFields += 1;
		field->NRecords = nrecords;
		field->Data = (void *) NULL;
		field->Name = (char *) NULL;
		field->Type = type;
		field->Len  = len;

		if ((field->Name = malloc (strlen (varname) + 1)) == (char *) NULL)
		{ perror ("Error allocating memory in: NCGtableOpen ()"); free (dimids); NCGtableClose (tbl); return ((NCGtable_t *) NULL); }
		strcpy (field->Name,varname);

		if (nc_get_att_double (ncid,i,NCGnameVAScaleFactor,&field->Scale)      != NC_NOERR) field->Scale      = 1.0;
		if (nc_get_att_double (ncid,i,NCGnameVAAddOffset,  &field->Offset)     != NC_NOERR) field->Offset     = 0.0;

		switch(field->Type)
		{
			case NC_CHAR:
				if ((field->Data = (void *) malloc(field->NRecords * field->Len * sizeof (char)))   == (void *) NULL)
				{ perror ("Error allocating memory in: NCGtableOpen ()"); free (dimids); NCGtableClose (tbl); return ((NCGtable_t *) NULL); }
				if((status = nc_get_var_text(ncid,i,(char *) (field->Data))) != NC_NOERR)
				{ NCGprintNCError (status,"NCGtableOpen"); free (dimids); NCGtableClose (tbl); return ((NCGtable_t *) NULL); }
				break;
			case NC_BYTE:
			case NC_SHORT:
			case NC_INT:
				if ((field->Data = (void *) malloc(field->NRecords * field->Len * sizeof (int)))   == (void *) NULL)
				{ perror ("Error allocating memory in: NCGtableOpen ()"); free (dimids); NCGtableClose (tbl); return ((NCGtable_t *) NULL); }
				if((status = nc_get_var_int(ncid,i,(int *) (field->Data))) != NC_NOERR)
				{ NCGprintNCError (status,"NCGtableOpen"); free (dimids); NCGtableClose (tbl); return ((NCGtable_t *) NULL); }
				if (nc_get_att_int    (ncid,i,NCGnameVAFillValue,  &field->FillValue.Int)  != NC_NOERR) field->FillValue.Int  = INT_NOVALUE;
				if (nc_get_att_double (ncid,i,NCGnameVAMissingVal, &field->MissingVal)     != NC_NOERR) field->MissingVal     = FLOAT_NOVALUE;
				break;
			case NC_FLOAT:
			case NC_DOUBLE:
				if ((field->Data = (void *) malloc(field->NRecords * field->Len * sizeof (double))) == (void *) NULL)
				{ perror ("Error allocating memory in: NCGtableOpen ()"); free (dimids); NCGtableClose (tbl); return ((NCGtable_t *) NULL); }
				if((status = nc_get_var_double(ncid,i,(double *) (field->Data))) != NC_NOERR)
				{ NCGprintNCError (status,"NCGtableOpen"); free (dimids); NCGtableClose (tbl); return ((NCGtable_t *) NULL); }
				if (nc_get_att_double (ncid,i,NCGnameVAFillValue,  &field->FillValue.Float)  != NC_NOERR) field->FillValue.Float = FLOAT_NOVALUE;
				if (nc_get_att_double (ncid,i,NCGnameVAMissingVal, &field->MissingVal)       != NC_NOERR) field->MissingVal      = FLOAT_NOVALUE;
				break;
			default:        field->Data = (void *) NULL; break;
		}
		if(GetDebug()) fprintf(stderr,"Loaded: %s(dimid: %d)\n",field->Name,dimid);
	}

	if(GetDebug())
	{
		fprintf(stderr,"Dim: %d Name: %s Cols: %d Rows: %d\n",dimid,tbl->Name,tbl->NFields,field->NRecords);
		for(i = 0; i < tbl->NFields; i++)
		{
			field = tbl->Fields + i;
			fprintf(stderr,"\tField: %d Name: %s ",i,field->Name);
			switch(field->Type)
			{
				case NC_CHAR:
					if(field->Len == 1)
					{
						fprintf(stderr,"Type: char\n");
						for(j = 0; j < 5; j++) fprintf (stderr,"\t\t%d %c\n",j,((char *) (field->Data)) [j]);
					}
					else
					{
						fprintf(stderr,"Type: string\n");
						for(j = 0; j < 5; j++) fprintf (stderr,"\t\t%d %s\n",j,((char *) (field->Data)) + j * field->Len);
					}
					break;
				case NC_BYTE:
				case NC_SHORT:
				case NC_INT:
					fprintf(stderr,"Type: int\n");
					for(j = 0; j < 5; j++) fprintf(stderr,"\t\t%d %i\n",j,((int *)    (field->Data)) [j]);
					break;
				case NC_FLOAT:
				case NC_DOUBLE:
					fprintf(stderr,"Type: double\n");
					for(j = 0; j < 5; j++) fprintf(stderr,"\t\t%d %f\n",j,((double *) (field->Data)) [j]);
					break;
				default: break;
			}
		}
	}
	return (tbl);
}

void NCGtableExportAscii(NCGtable_t *tbl, FILE *file)
{
	int i,j, intVal;
	double floatVal;
	NCGfield_t *field;

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
						if (NCGfieldGetInt   (field, i, &intVal))  fprintf(file,"%i",intVal);   else fprintf (file,"-"); break;
					case NC_FLOAT:
					case NC_DOUBLE:
						if (NCGfieldGetFloat (field,i, &floatVal)) fprintf(file,"%f",floatVal); else fprintf (file,"-"); break;
				}
			}
			fprintf(file,"\n");
		}
}

NCGfield_t *NCGtableAddField (NCGtable_t *tbl, char *name, nc_type type)
{
	int i;
	NCGfield_t *field;

	if ((tbl->Fields = (NCGfield_t *) realloc(tbl->Fields, sizeof (NCGfield_t) * (tbl->NFields + 1))) == (NCGfield_t *) NULL)
	{ perror ("Memory allocation error in: NCGtableAddField ()"); return ((NCGfield_t *) NULL); }
	field = tbl->Fields + tbl->NFields;
	field->NRecords = tbl->NFields > 1 ? tbl->Fields [tbl->NFields - 1].NRecords : 0;
	field->Data = (void *) NULL;
	field->Name = (char *) NULL;
	field->Type = type;
	field->Scale = 1.0;
	field->Offset = 0.0;
	tbl->NFields += 1;
	if ((field->Name = (char *) malloc(strlen (name) + 1)) == (char *) NULL)
	{ perror ("Memory allocation error in: NCGtableAddField ()"); return ((NCGfield_t *) NULL); }
	strcpy(field->Name,name);

	switch (field->Type)
	{
		default:
		case NC_CHAR:
			if ((field->Data = (void *) malloc (field->NRecords * sizeof (char)))   == (void *) NULL)
			{ perror ("Memory allocation error in: NCGtableAddField ()"); return ((NCGfield_t *) NULL); }
			for (i = 0; i < field->NRecords; i++) ((char *)   field->Data) [i] = '\0';
			break;
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:
			if ((field->Data = (void *) malloc (field->NRecords * sizeof (int)))    == (void *) NULL)
			{ perror ("Memory allocation error in: NCGtableAddField ()"); return ((NCGfield_t *) NULL); }
			field->MissingVal = INT_NOVALUE;
			field->FillValue.Int = INT_NOVALUE;
			for (i = 0; i < field->NRecords; i++) ((int *)    field->Data) [i] = INT_NOVALUE;
			break;
		case NC_FLOAT:
		case NC_DOUBLE:
			if ((field->Data = (void *) malloc (field->NRecords * sizeof (double))) == (void *) NULL)
			{ perror ("Memory allocation error in: NCGtableAddField ()"); return ((NCGfield_t *) NULL); }
			field->MissingVal = FLOAT_NOVALUE;
			field->FillValue.Float = FLOAT_NOVALUE;
			for (i = 0; i < field->NRecords; i++) ((double *) field->Data) [i] = FLOAT_NOVALUE;
			break;
	}
	return (field);
}

NCGstate NCGtableCommit(int ncid, NCGtable_t *tbl)
{
	NCGstate ret = NCGsucceeded;
	int j;
	for(j = 1; j < tbl->NFields; j++)
		if (NCGtableCommitField (ncid,tbl->Name,tbl->Fields + j) != NCGsucceeded) ret = NCGfailed;
	return (ret);
}

NCGstate NCGtableCommitField (int ncid, char *tablename, NCGfield_t *field)
{
	int status, dimid, varid;

	if (nc_inq_varid(ncid,field->Name,&varid) != NC_NOERR)
	{
		if(field->Len > 1) return (NCGfailed);
		if((status = nc_inq_dimid(ncid,tablename,&dimid)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGtableCommitField"); return (NCGfailed);}
		if((status = nc_redef(ncid) != NC_NOERR))
		{ NCGprintNCError (status,"NCGtableCommitField"); return (NCGfailed);}
		if ((status = nc_def_var(ncid,field->Name,field->Type,1,&dimid,&varid)) != NC_NOERR)
		{ NCGprintNCError (status,"NCGtableCommitField"); return (NCGfailed);}
		if((status = nc_enddef(ncid) != NC_NOERR))
		{ NCGprintNCError (status,"NCGtableCommitField"); return (NCGfailed);}
	}
	switch (field->Type)
	{
		default:
		case NC_CHAR:
			if((status = nc_put_var_text(ncid,varid,(char *) field->Data)) != NC_NOERR)
			{ NCGprintNCError (status,"NCGtableCommitField"); return (NCGfailed);}
			break;
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:
			if((status = nc_put_var_int(ncid,varid, (int *)  field->Data)) != NC_NOERR)
			{ NCGprintNCError (status,"NCGtableCommitField"); return (NCGfailed);}
			break;
		case NC_FLOAT:
		case NC_DOUBLE:
			if((status = nc_put_var_double(ncid,varid,(double *) field->Data)) != NC_NOERR)
			{ NCGprintNCError (status,"NCGtableCommitField"); return (NCGfailed);}
			break;
	}
	return (NCGsucceeded);
}

NCGfield_t *NCGtableGetFieldByName (NCGtable_t *table,const char *name)
{
	int i;

	for (i = 0;i < table->NFields;++i) if (strcmp (table->Fields [i].Name,name) == 0) return (table->Fields + i);
	return ((NCGfield_t *) NULL);
}

NCGfield_t *NCGtableGetFieldById (NCGtable_t *table,int id)
{
	if ((id >= 0) && (id < table->NFields)) return (table->Fields + id);
	return ((NCGfield_t *) NULL);
}

bool NCGfieldGetInt (NCGfield_t *field,int id, int *val)
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
			if (NCGmathEqualValues (field->Scale,1.0) && ((int) field->Offset == 0)) { *val = intVal; return (true); }
			floatVal = (double) intVal * field->Scale + field->Offset;
			break;
		case NC_FLOAT:
		case NC_DOUBLE:
			if ((id < 0) && (id >= field->NRecords)) goto NOVALUE;
			floatVal = ((double *) field->Data) [id];
			if (NCGmathEqualValues (floatVal, field->FillValue.Float)) goto NOVALUE;
			floatVal = floatVal * field->Scale + field->Offset;
			break;
	}
	if (NCGmathEqualValues (floatVal,field->MissingVal)) goto NOVALUE; 
	*val = (int) floatVal;
	return (true);

NOVALUE:
	*val = INT_NOVALUE;
	return (false);
}

NCGstate NCGfieldSetInt (NCGfield_t *field,int id, int setVal)
{
	if ((id < 0) && (id >= field->NRecords)) return (NCGfailed);
	if(setVal == INT_NOVALUE) NCGfieldSetFill (field, id);
	switch (field->Type)
	{
		default:
		case NC_CHAR:  return (NCGfailed);
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:    ((int *)    field->Data) [id] = (int) ((setVal - field->Offset) / field->Scale);  break;
		case NC_FLOAT:
		case NC_DOUBLE: ((double *) field->Data) [id] = ((double) setVal - field->Offset) / field->Scale; break;
	}
	return (NCGsucceeded);
}

bool NCGfieldGetFloat (NCGfield_t *field,int id, double *val)
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
			if (NCGmathEqualValues (floatVal, field->FillValue.Float)) goto NOVALUE;
			floatVal = floatVal * field->Scale + field->Offset;
			break;
	}
	if (NCGmathEqualValues (floatVal, field->MissingVal)) goto NOVALUE;
	*val = floatVal;
	return (true);

NOVALUE:
	*val = FLOAT_NOVALUE;
	return (false);
}

NCGstate NCGfieldSetFloat (NCGfield_t *field,int id, double setVal)
{
	if ((id < 0) && (id >= field->NRecords)) return (NCGfailed);
	switch (field->Type)
	{
		default:
		case NC_CHAR:  return (NCGfailed);
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:    ((int *)    field->Data) [id] = (int) ((setVal - field->Offset) / field->Scale); break;
		case NC_FLOAT:
		case NC_DOUBLE: ((double *) field->Data) [id] = (setVal - field->Offset) / field->Scale; break;
	}
	return (NCGsucceeded);
}

NCGstate NCGfieldSetFill (NCGfield_t *field,int id)
{
	if ((id < 0) && (id >= field->NRecords)) return (NCGfailed);
	switch (field->Type)
	{
		default:
		case NC_CHAR:  return (NCGfailed);
		case NC_BYTE:
		case NC_SHORT:
		case NC_INT:    ((int *)    field->Data) [id] = field->FillValue.Int;   break;
		case NC_FLOAT:
		case NC_DOUBLE: ((double *) field->Data) [id] = field->FillValue.Float; break;
	}
	return (NCGsucceeded);
}
