#ifndef _NCdsHandle_H
#define _NCdsHandle_H

#include<NCtable.h>
#include<NCtime.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include<udunits.h>

typedef struct NCreference_s
{
	size_t  Num;
	int    *Idx;
	double *Weight;
} NCreference_t;

void     NCreferenceInitialize  (NCreference_t *);
void     NCreferenceClear       (NCreference_t *);

typedef struct NCdsHandle_s
{
/* Begin dsHandle */
	NCdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
} NCdsHandle_t;

NCstate       NCdsHandleDefine        (NCdsHandle_t *dsHandle, int *ncid, size_t n);
void           NCdsHandleClear         (NCdsHandle_t *dsHandle);

NCdsHandle_t *NCdsHandleOpen          (const char *);
NCdsHandle_t *NCdsHandleOpenById      (int);
NCdsHandle_t *NCdsHandleOpenByIds     (int *, size_t);
NCstate       NCdsHandleClose         (NCdsHandle_t *);
NCdsHandle_t *NCdsHandleCreate        (const char *, const char *, int, NCtimeStep, utUnit *, double, double);

int            NCdsHandleGetLNum       (const NCdsHandle_t *);
int            NCdsHandleGetTNum       (const NCdsHandle_t *);
NCstate       NCdsHandleLoadCache     (NCdsHandle_t *, size_t, size_t, size_t, size_t);
NCstate       NCdsHandleSaveCache     (NCdsHandle_t *, size_t, size_t);
bool           NCdsHandleGetFloat      (const NCdsHandle_t *, NCreference_t *, double *);
NCstate       NCdsHandleSetFloat      (NCdsHandle_t *, size_t *, double);
NCstate       NCdsHandleSetFill       (NCdsHandle_t *, size_t *);
NCstate       NCdsHandleGetTime       (const NCdsHandle_t *, size_t, utUnit *, double *);
int            NCdsHandleGetTimeStep   (const NCdsHandle_t *, size_t, utUnit *, double *);
int            NCdsHandleGetTLayerID   (const NCdsHandle_t *, utUnit *, double);
NCstate       NCdsHandleGetUnitConv   (const NCdsHandle_t *,const utUnit *, double *, double *);
NCstate       NCdsHandleReference     (const NCdsHandle_t *,const NCcoordinate_t *,NCreference_t *);
NCstate       NCdsHandleSetLevelUnit  (const NCdsHandle_t *, const char *);
NCstate       NCdsHandleSetVarUnit    (const NCdsHandle_t *, const char *);
NCstate       NCdsHandleSetVarAttribs (const NCdsHandle_t *, double, double, double, double, double);
NCstate       NCdsHandleUpdateRanges  (const NCdsHandle_t *);

typedef struct NCdsHandleGeo_s
{
/* Begin dsHandle */
	NCdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCprojection Projection;
	NCregion_t   Extent;
/* End   dsHandleGeo */
} NCdsHandleGeo_t;

NCstate NCdsHandleGeoDefine (NCdsHandleGeo_t *, int *, size_t);
void     NCdsHandleGeoClear  (NCdsHandleGeo_t *);

typedef struct NCdsHandleGLayout_s
{
/* Begin dsHandle */
	NCdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCprojection Projection;
	NCregion_t   Extent;
/* End   dsHandleGeo */
/* Begin dsHandleGLayout */
	int    *GVarIds;
	nc_type GType;
	size_t  ColNum,   RowNum;
	double *XCoords, *YCoords;
/* End   dsHandleGLayout */
} NCdsHandleGLayout_t;

NCstate NCdsHandleGLayoutDefine (NCdsHandleGLayout_t *, int *, size_t);
// Fills out the GLayout structure. Allocates memory for XCoords
// and YCoords arrays. Both arrays will have an extra element
// (i.e. "ColNum + 1" and "RowNum + 1" respectively).
void     NCdsHandleGLayoutClear  (NCdsHandleGLayout_t *);

int NCdsHandleGLayoutColByCoord (NCdsHandleGLayout_t *gLayout, double x);
// Returns the column position of coordinate x in a grid layout, when 
// x is in the grid extent. Otherwise it will return NCfailed.
int NCdsHandleGLayoutRowByCoord (NCdsHandleGLayout_t *gLayout, double y);
// Returns the row position of coordinate y in a grid layout, when 
// y is in the grid extent. Otherwise it will return NCfailed.

typedef struct NCdsHandleGrid_s
{
/* Begin dsHandle */
	NCdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCprojection Projection;
	NCregion_t   Extent;
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
	NCtimeStep TSmode;
	bool    Climatology;
/* End   dsHandleGrid */
} NCdsHandleGrid_t;

NCstate NCdsHandleGridDefine (NCdsHandleGrid_t *,int *, size_t);
void     NCdsHandleGridClear  (NCdsHandleGrid_t *);
NCstate NCdsHandleGridGetTime      (const NCdsHandleGrid_t *, size_t, utUnit *, double *);
int      NCdsHandleGridGetTimeStep  (const NCdsHandleGrid_t *, size_t, utUnit *, double *);
int      NCdsHandleGridGetTLayerID  (const NCdsHandleGrid_t *, utUnit *, double );

typedef struct NCdsHandleGCont_s
{
/* Begin dsHandle */
	NCdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCprojection Projection;
	NCregion_t   Extent;
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
	NCtimeStep TSmode;
	bool    Climatology;
/* End   dsHandleGrid */
/* Begin dsHandleGCont */
	NCscale ScaleMode;
	bool    DoGUnit;
	utUnit  GUnit;
	double  Scale,      Offset;
	union { int Int; double Float; } FillValue, MissingVal;
	int    *MeanIds, *MinIds, *MaxIds, *StdIds;
	double *Data, *AuxData;
	size_t *ObsNum;
/* End   dsHandleGCont */
} NCdsHandleGCont_t;

NCstate NCdsHandleGContDefine    (NCdsHandleGCont_t *gCont, int *ncids, size_t n);
void     NCdsHandleGContClear     (NCdsHandleGCont_t *);

NCstate NCdsHandleGContReference (const NCdsHandleGCont_t *, const NCcoordinate_t *, NCreference_t *);
NCstate NCdsHandleGContLoadCache (NCdsHandleGCont_t *, size_t, size_t, size_t, size_t);
NCstate NCdsHandleGContSaveCache (NCdsHandleGCont_t *, size_t, size_t);
bool     NCdsHandleGContGetFloat  (const NCdsHandleGCont_t *, NCreference_t *, double *);
NCstate NCdsHandleGContSetFloat  (NCdsHandleGCont_t *, size_t, size_t,   double);
NCstate NCdsHandleGContSetFill   (NCdsHandleGCont_t *, size_t, size_t);
NCstate NCdsHandleGContCLStats   (NCdsHandleGCont_t *, size_t, size_t);
NCstate NCdsHandleGContUpdateRanges (const NCdsHandleGCont_t *);

typedef struct NCdsHandleGDisc_s
{
/* Begin dsHandle */
	NCdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCprojection Projection;
	NCregion_t   Extent;
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
	NCtimeStep TSmode;
	bool    Climatology;
/* End   dsHandleGrid */
} NCdsHandleGDisc_t;

NCstate NCdsHandleGDiscDefine    (NCdsHandleGDisc_t *, int *, size_t);
void     NCdsHandleGDiscClear     (NCdsHandleGDisc_t *);

typedef struct NCdsHandleNetwork_s
{
/* Begin dsHandle */
	NCdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCprojection Projection;
	NCregion_t   Extent;
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
		NCtable_t *Table;
		NCfield_t *NameFld;
		NCfield_t *RowFld;
		NCfield_t *ColFld;
		NCfield_t *OrderFld;
		NCfield_t *SymbolFld;
		NCfield_t *LengthFld;
		NCfield_t *AreaFld;
	} Basins;
	struct
	{
		NCtable_t *Table;
		NCfield_t *RowFld;
		NCfield_t *ColFld;
		NCfield_t *ToCellFld;
		NCfield_t *FromCellFld;
		NCfield_t *BasinFld;
		NCfield_t *NCellsFld;
		NCfield_t *TravelFld;
		NCfield_t *OrderFld;
		NCfield_t *LengthFld;
		NCfield_t *AreaFld;
	} Cells;
} NCdsHandleNetwork_t;

NCstate NCdsHandleNetworkDefine (NCdsHandleNetwork_t *, int);
void     NCdsHandleNetworkClear  (NCdsHandleNetwork_t *);
NCstate NCdsHandleNetReference (const NCdsHandleNetwork_t *, const NCcoordinate_t *, NCreference_t *);
#define NCdsHandleNetworkCellIdByRowCol(net,row,col) ((row<0)||(row>=net->RowNum)||(col<0)||(col>=net->ColNum)?NCfailed:net->Data[row*net->ColNum+col])

// #define NCdsHandleNetworkGetCellRow(dshn,cell) (dshn->Cells.RowFld-> 

typedef struct NCdsHandleVector_s
{
/* Begin dsHandle */
	NCdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCprojection Projection;
	NCregion_t   Extent;
/* End   dsHandleGeo */
/* Begin dsHandleVector */
	size_t  ItemNum;
/* End   dsHandleVector */
} NCdsHandleVector_t;

NCstate NCdsHandleVectorDefine (NCdsHandleVector_t *, int);
void     NCdsHandleVectorClear  (NCdsHandleVector_t *);

typedef struct NCdsHandleVPoint_s
{
/* Begin dsHandle */
	NCdataType DataType;
	int        *NCIds;
	size_t      NCnum;
/* End   dsHandle */
/* Begin dsHandleGeo */
	NCprojection Projection;
	NCregion_t   Extent;
/* End   dsHandleGeo */
/* Begin dsHandleVector */
	size_t  ItemNum;
/* End   dsHandleVector */
/* Begin dsHandleVPoint */
	double *XCoords;
	double *YCoords;	
/* End   dsHandleVPoint */
} NCdsHandleVPoint_t;

NCstate NCdsHandleVPointDefine (NCdsHandleVPoint_t *, int);
void     NCdsHandleVPointClear  (NCdsHandleVPoint_t *);

NCstate NCdsHandleVPointReference (const NCdsHandleVPoint_t *, const NCcoordinate_t *, NCreference_t *);

#if defined(__cplusplus)
}
#endif

#endif
