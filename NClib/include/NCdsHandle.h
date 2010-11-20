#ifndef _NCGdsHandle_H
#define _NCGdsHandle_H

#include<NCtable.h>
#include<NCtime.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include<udunits.h>

typedef struct NCGreference_s
{
	size_t  Num;
	int    *Idx;
	double *Weight;
} NCGreference_t;

void     NCGreferenceInitialize  (NCGreference_t *);
void     NCGreferenceClear       (NCGreference_t *);

typedef struct NCGdsHandle_s
{
/* Begin dsHandle */
	NCGdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
} NCGdsHandle_t;

NCGstate       NCGdsHandleDefine        (NCGdsHandle_t *dsHandle, int *ncid, size_t n);
void           NCGdsHandleClear         (NCGdsHandle_t *dsHandle);

NCGdsHandle_t *NCGdsHandleOpen          (const char *);
NCGdsHandle_t *NCGdsHandleOpenById      (int);
NCGdsHandle_t *NCGdsHandleOpenByIds     (int *, size_t);
NCGstate       NCGdsHandleClose         (NCGdsHandle_t *);
NCGdsHandle_t *NCGdsHandleCreate        (const char *, const char *, int, NCGtimeStep, utUnit *, double, double);

int            NCGdsHandleGetLNum       (const NCGdsHandle_t *);
int            NCGdsHandleGetTNum       (const NCGdsHandle_t *);
NCGstate       NCGdsHandleLoadCache     (NCGdsHandle_t *, size_t, size_t, size_t, size_t);
NCGstate       NCGdsHandleSaveCache     (NCGdsHandle_t *, size_t, size_t);
bool           NCGdsHandleGetFloat      (const NCGdsHandle_t *, NCGreference_t *, double *);
NCGstate       NCGdsHandleSetFloat      (NCGdsHandle_t *, size_t *, double);
NCGstate       NCGdsHandleSetFill       (NCGdsHandle_t *, size_t *);
NCGstate       NCGdsHandleGetTime       (const NCGdsHandle_t *, size_t, utUnit *, double *);
int            NCGdsHandleGetTimeStep   (const NCGdsHandle_t *, size_t, utUnit *, double *);
int            NCGdsHandleGetTLayerID   (const NCGdsHandle_t *, utUnit *, double);
NCGstate       NCGdsHandleGetUnitConv   (const NCGdsHandle_t *,const utUnit *, double *, double *);
NCGstate       NCGdsHandleReference     (const NCGdsHandle_t *,const NCGcoordinate_t *,NCGreference_t *);
NCGstate       NCGdsHandleSetLevelUnit  (const NCGdsHandle_t *, const char *);
NCGstate       NCGdsHandleSetVarUnit    (const NCGdsHandle_t *, const char *);
NCGstate       NCGdsHandleSetVarAttribs (const NCGdsHandle_t *, double, double, double, double, double);
NCGstate       NCGdsHandleUpdateRanges  (const NCGdsHandle_t *);

typedef struct NCGdsHandleGeo_s
{
/* Begin dsHandle */
	NCGdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCGprojection Projection;
	NCGregion_t   Extent;
/* End   dsHandleGeo */
} NCGdsHandleGeo_t;

NCGstate NCGdsHandleGeoDefine (NCGdsHandleGeo_t *, int *, size_t);
void     NCGdsHandleGeoClear  (NCGdsHandleGeo_t *);

typedef struct NCGdsHandleGLayout_s
{
/* Begin dsHandle */
	NCGdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCGprojection Projection;
	NCGregion_t   Extent;
/* End   dsHandleGeo */
/* Begin dsHandleGLayout */
	int    *GVarIds;
	nc_type GType;
	size_t  ColNum,   RowNum;
	double *XCoords, *YCoords;
/* End   dsHandleGLayout */
} NCGdsHandleGLayout_t;

NCGstate NCGdsHandleGLayoutDefine (NCGdsHandleGLayout_t *, int *, size_t);
// Fills out the GLayout structure. Allocates memory for XCoords
// and YCoords arrays. Both arrays will have an extra element
// (i.e. "ColNum + 1" and "RowNum + 1" respectively).
void     NCGdsHandleGLayoutClear  (NCGdsHandleGLayout_t *);

int NCGdsHandleGLayoutColByCoord (NCGdsHandleGLayout_t *gLayout, double x);
// Returns the column position of coordinate x in a grid layout, when 
// x is in the grid extent. Otherwise it will return NCGfailed.
int NCGdsHandleGLayoutRowByCoord (NCGdsHandleGLayout_t *gLayout, double y);
// Returns the row position of coordinate y in a grid layout, when 
// y is in the grid extent. Otherwise it will return NCGfailed.

typedef struct NCGdsHandleGrid_s
{
/* Begin dsHandle */
	NCGdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCGprojection Projection;
	NCGregion_t   Extent;
/* End   dsHandleGeo */
/* Begin dsHandleGLayout */
	int    *GVarIds;
	nc_type GType;
	size_t  ColNum,   RowNum;
	double *XCoords, *YCoords;
/* End   dsHandleGLayout */
/* Begin dsHandleGrid */
	size_t *NCindex, *NCoffset;
	bool    DoLUnit,  DoTUnit;
	utUnit *LUnits,  *TUnits;
	int    *LVarIds, *TVarIds;
	double *Levels,  *Times;
	size_t  LNum,     TNum;
	NCGtimeStep TSmode;
	bool    Climatology;
/* End   dsHandleGrid */
} NCGdsHandleGrid_t;

NCGstate NCGdsHandleGridDefine (NCGdsHandleGrid_t *,int *, size_t);
void     NCGdsHandleGridClear  (NCGdsHandleGrid_t *);
NCGstate NCGdsHandleGridGetTime      (const NCGdsHandleGrid_t *, size_t, utUnit *, double *);
int      NCGdsHandleGridGetTimeStep  (const NCGdsHandleGrid_t *, size_t, utUnit *, double *);
int      NCGdsHandleGridGetTLayerID  (const NCGdsHandleGrid_t *, utUnit *, double );

typedef struct NCGdsHandleGCont_s
{
/* Begin dsHandle */
	NCGdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCGprojection Projection;
	NCGregion_t   Extent;
/* End   dsHandleGeo */
/* Begin dsHandleGLayout */
	int    *GVarIds;
	nc_type GType;
	size_t  ColNum,   RowNum;
	double *XCoords, *YCoords;
/* End   dsHandleGLayout */
/* Begin dsHandleGrid */
	size_t *NCindex, *NCoffset;
	bool    DoLUnit,  DoTUnit;
	utUnit *LUnits,  *TUnits;
	int    *LVarIds, *TVarIds;
	double *Levels,  *Times;
	size_t  LNum,     TNum;
	NCGtimeStep TSmode;
	bool    Climatology;
/* End   dsHandleGrid */
/* Begin dsHandleGCont */
	NCGscale ScaleMode;
	bool    DoGUnit;
	utUnit  GUnit;
	double  Scale,      Offset;
	union { int Int; double Float; } FillValue, MissingVal;
	int    *MeanIds, *MinIds, *MaxIds, *StdIds;
	double *Data, *AuxData;
	size_t *ObsNum;
/* End   dsHandleGCont */
} NCGdsHandleGCont_t;

NCGstate NCGdsHandleGContDefine    (NCGdsHandleGCont_t *gCont, int *ncids, size_t n);
void     NCGdsHandleGContClear     (NCGdsHandleGCont_t *);

NCGstate NCGdsHandleGContReference (const NCGdsHandleGCont_t *, const NCGcoordinate_t *, NCGreference_t *);
NCGstate NCGdsHandleGContLoadCache (NCGdsHandleGCont_t *, size_t, size_t, size_t, size_t);
NCGstate NCGdsHandleGContSaveCache (NCGdsHandleGCont_t *, size_t, size_t);
bool     NCGdsHandleGContGetFloat  (const NCGdsHandleGCont_t *, NCGreference_t *, double *);
NCGstate NCGdsHandleGContSetFloat  (NCGdsHandleGCont_t *, size_t, size_t,   double);
NCGstate NCGdsHandleGContSetFill   (NCGdsHandleGCont_t *, size_t, size_t);
NCGstate NCGdsHandleGContCLStats   (NCGdsHandleGCont_t *, size_t, size_t);
NCGstate NCGdsHandleGContUpdateRanges (const NCGdsHandleGCont_t *);

typedef struct NCGdsHandleGDisc_s
{
/* Begin dsHandle */
	NCGdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCGprojection Projection;
	NCGregion_t   Extent;
/* End   dsHandleGeo */
/* Begin dsHandleGLayout */
	int    *GVarIds;
	nc_type GType;
	size_t  ColNum,   RowNum;
	double *XCoords, *YCoords;
/* End   dsHandleGLayout */
/* Begin dsHandleGrid */
	size_t *NCindex, *NCoffset;
	bool    DoLUnit,  DoTUnit;
	utUnit *LUnits,  *TUnits;
	int    *LVarIds, *TVarIds;
	double *Levels,  *Times;
	size_t  LNum,     TNum;
	NCGtimeStep TSmode;
	bool    Climatology;
/* End   dsHandleGrid */
} NCGdsHandleGDisc_t;

NCGstate NCGdsHandleGDiscDefine    (NCGdsHandleGDisc_t *, int *, size_t);
void     NCGdsHandleGDiscClear     (NCGdsHandleGDisc_t *);

typedef struct NCGdsHandleNetwork_s
{
/* Begin dsHandle */
	NCGdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCGprojection Projection;
	NCGregion_t   Extent;
/* End   dsHandleGeo */
/* Begin dsHandleGLayout */
	int    *GVarIds;
	nc_type GType;
	size_t  ColNum,   RowNum;
	double *XCoords, *YCoords;
/* End   dsHandleGLayout */
	int    *Data;
	struct
	{
		NCGtable_t *Table;
		NCGfield_t *NameFld;
		NCGfield_t *RowFld;
		NCGfield_t *ColFld;
		NCGfield_t *OrderFld;
		NCGfield_t *SymbolFld;
		NCGfield_t *LengthFld;
		NCGfield_t *AreaFld;
	} Basins;
	struct
	{
		NCGtable_t *Table;
		NCGfield_t *RowFld;
		NCGfield_t *ColFld;
		NCGfield_t *ToCellFld;
		NCGfield_t *FromCellFld;
		NCGfield_t *BasinFld;
		NCGfield_t *NCellsFld;
		NCGfield_t *TravelFld;
		NCGfield_t *OrderFld;
		NCGfield_t *LengthFld;
		NCGfield_t *AreaFld;
	} Cells;
} NCGdsHandleNetwork_t;

NCGstate NCGdsHandleNetworkDefine (NCGdsHandleNetwork_t *, int);
void     NCGdsHandleNetworkClear  (NCGdsHandleNetwork_t *);
NCGstate NCGdsHandleNetReference (const NCGdsHandleNetwork_t *, const NCGcoordinate_t *, NCGreference_t *);
#define NCGdsHandleNetworkCellIdByRowCol(net,row,col) ((row<0)||(row>=net->RowNum)||(col<0)||(col>=net->ColNum)?NCGfailed:net->Data[row*net->ColNum+col])

// #define NCGdsHandleNetworkGetCellRow(dshn,cell) (dshn->Cells.RowFld-> 

typedef struct NCGdsHandleVector_s
{
/* Begin dsHandle */
	NCGdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCGprojection Projection;
	NCGregion_t   Extent;
/* End   dsHandleGeo */
/* Begin dsHandleVector */
	size_t  ItemNum;
/* End   dsHandleVector */
} NCGdsHandleVector_t;

NCGstate NCGdsHandleVectorDefine (NCGdsHandleVector_t *, int);
void     NCGdsHandleVectorClear  (NCGdsHandleVector_t *);

typedef struct NCGdsHandleVPoint_s
{
/* Begin dsHandle */
	NCGdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCGprojection Projection;
	NCGregion_t   Extent;
/* End   dsHandleGeo */
/* Begin dsHandleVector */
	size_t  ItemNum;
/* End   dsHandleVector */
/* Begin dsHandleVPoint */
	double *XCoords;
	double *YCoords;	
/* End   dsHandleVPoint */
} NCGdsHandleVPoint_t;

NCGstate NCGdsHandleVPointDefine (NCGdsHandleVPoint_t *, int);
void     NCGdsHandleVPointClear  (NCGdsHandleVPoint_t *);

NCGstate NCGdsHandleVPointReference (const NCGdsHandleVPoint_t *, const NCGcoordinate_t *, NCGreference_t *);

#if defined(__cplusplus)
}
#endif

#endif
