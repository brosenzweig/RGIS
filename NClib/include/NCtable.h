#ifndef _NCtable_H
#define _NCtable_H

#include<NCmath.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct NCfield_s
{
   nc_type Type;
   char   *Name;
	size_t  Len;
	size_t  NRecords;
	void   *Data;
	NCscale ScaleMode;
	double  Scale;
	double  Offset;
	union { int Int; double Float; } FillValue;
	double  MissingVal;
} NCfield_t;

typedef struct NCtable_s
{
   char *Name;
   NCfield_t *Fields;
   int NFields;
} NCtable_t;

void NCtableClose(NCtable_t *tbl);
// Free all the memory used by the open table
NCtable_t *NCtableOpen(int ncid, char *tablename);
// reads the given 'ncid' file, and finds the given table.
// Returns NULL if table was not found, or if any error occurs.
// Otherwise returns the table.
void NCtableExportAscii(NCtable_t *tbl, FILE *file);
// prints out the entire table to the specified filehandle.
//NCcolumn_t *NCtableAddColString(int ncid, NCtable_t *tbl, char *name);
NCfield_t *NCtableAddField (NCtable_t *tbl, char *name, nc_type type);
// Creates a new column in the table with the given attributes and sets
// all values to NOVALUE. Returns a pointer to the new column.
NCstate NCtableCommit(int ncid, NCtable_t *tbl);
// Overwrites everything in the 'ncid' with the elements in 'tbl'.
NCstate NCtableCommitField(int ncid, char *tablename, NCfield_t *field);
// Grants the ability to commit specific field from table.
NCfield_t *NCtableGetFieldByName (NCtable_t *table,const char *name);
NCfield_t *NCtableGetFieldById   (NCtable_t *table,int id);

bool     NCfieldGetInt   (NCfield_t *field,int id, int *);
NCstate NCfieldSetInt   (NCfield_t *field,int id, int  set);
bool     NCfieldGetFloat (NCfield_t *field,int id, double *);
NCstate NCfieldSetFloat (NCfield_t *field,int id, double  set);
NCstate NCfieldSetFill  (NCfield_t *field,int id);

#if defined(__cplusplus)
}
#endif

#endif /* _NCtable_H */
