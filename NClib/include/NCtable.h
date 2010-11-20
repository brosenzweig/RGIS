#ifndef _NCGtable_H
#define _NCGtable_H

#include<NCmath.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct NCGfield_s
{
   nc_type Type;
   char   *Name;
	size_t  Len;
	size_t  NRecords;
	void   *Data;
	NCGscale ScaleMode;
	double  Scale;
	double  Offset;
	union { int Int; double Float; } FillValue;
	double  MissingVal;
} NCGfield_t;

typedef struct NCGtable_s
{
   char *Name;
   NCGfield_t *Fields;
   int NFields;
} NCGtable_t;

void NCGtableClose(NCGtable_t *tbl);
// Free all the memory used by the open table
NCGtable_t *NCGtableOpen(int ncid, char *tablename);
// reads the given 'ncid' file, and finds the given table.
// Returns NULL if table was not found, or if any error occurs.
// Otherwise returns the table.
void NCGtableExportAscii(NCGtable_t *tbl, FILE *file);
// prints out the entire table to the specified filehandle.
//NCGcolumn_t *NCGtableAddColString(int ncid, NCGtable_t *tbl, char *name);
NCGfield_t *NCGtableAddField (NCGtable_t *tbl, char *name, nc_type type);
// Creates a new column in the table with the given attributes and sets
// all values to NOVALUE. Returns a pointer to the new column.
NCGstate NCGtableCommit(int ncid, NCGtable_t *tbl);
// Overwrites everything in the 'ncid' with the elements in 'tbl'.
NCGstate NCGtableCommitField(int ncid, char *tablename, NCGfield_t *field);
// Grants the ability to commit specific field from table.
NCGfield_t *NCGtableGetFieldByName (NCGtable_t *table,const char *name);
NCGfield_t *NCGtableGetFieldById   (NCGtable_t *table,int id);

bool     NCGfieldGetInt   (NCGfield_t *field,int id, int *);
NCGstate NCGfieldSetInt   (NCGfield_t *field,int id, int  set);
bool     NCGfieldGetFloat (NCGfield_t *field,int id, double *);
NCGstate NCGfieldSetFloat (NCGfield_t *field,int id, double  set);
NCGstate NCGfieldSetFill  (NCGfield_t *field,int id);

#if defined(__cplusplus)
}
#endif

#endif /* _NCGtable_H */
