#ifndef _NCG_H
#define _NCG_H

#include<NCcm.h>
#include<NCnames.h>
#include<netcdf.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum { NCGscaleNone, NCGscaleLinear, NCGscaleLogarithmic } NCGscale;

typedef enum { NCGsizeShortString =   8,
               NCGsizeString      =  64,
               NCGsizeLongString  = 128,
               NCGsizeText        = 256,
               NCGsizeLongText    = 512 } NCGsize;

typedef enum { NCGdirE  = (0x01 << 0x00),
               NCGdirSE = (0x01 << 0x01),
               NCGdirS  = (0x01 << 0x02),
               NCGdirSW = (0x01 << 0x03),
               NCGdirW  = (0x01 << 0x04),
               NCGdirNW = (0x01 << 0x05),
               NCGdirN  = (0x01 << 0x06),
               NCGdirNE = (0x01 << 0x07) } NCGdir;

#define NCGNetworkOppositeDirection(dir) (((dir >> 0x04) | (dir << 0x04)) & 0xff)

typedef struct NCGcoordinate_s
{
	double X;
	double Y;
} NCGcoordinate_t;

void NCGcoordinateAdd           (NCGcoordinate_t *, NCGcoordinate_t *, NCGcoordinate_t *);
void NCGcoordinateAddConst      (NCGcoordinate_t *, double, NCGcoordinate_t *);
void NCGcoordinateSubtract      (NCGcoordinate_t *, NCGcoordinate_t *, NCGcoordinate_t *);
void NCGcoordinateSubtractConst (NCGcoordinate_t *, double, NCGcoordinate_t *);
void NCGcoordinateMultiply      (NCGcoordinate_t *, NCGcoordinate_t *, NCGcoordinate_t *);
void NCGcoordinateMultiplyConst (NCGcoordinate_t *, double, NCGcoordinate_t *);
void NCGcoordinateDivide        (NCGcoordinate_t *, NCGcoordinate_t *, NCGcoordinate_t *);
void NCGcoordinateDivideConst   (NCGcoordinate_t *, double, NCGcoordinate_t *);

typedef struct NCGregion_s
{
	NCGcoordinate_t LowerLeft;
	NCGcoordinate_t UpperRight;
} NCGregion_t;

void NCGregionInitialize        (NCGregion_t *);
void NCGregionExpand            (NCGregion_t *, NCGregion_t *,     NCGregion_t *);
void NCGregionExpandCoord       (NCGregion_t *, NCGcoordinate_t *, NCGregion_t *);
void NCGregionUnion             (NCGregion_t *, NCGregion_t *,     NCGregion_t *);
bool NCGregionTestCoord         (NCGregion_t *, NCGcoordinate_t *);
bool NCGregionTestRegion        (NCGregion_t *, NCGregion_t *);

typedef enum { NCGtypeUndefined = NCGundefined,
	            NCGtypeGCont     = 1,
               NCGtypeGDisc     = 2,
               NCGtypeNetwork   = 3,
	            NCGtypePoint     = 4,
               NCGtypeLine      = 5,
               NCGtypePolygon   = 6 } NCGdataType;

NCGdataType NCGdataGetType (int ncid);
// Reads the data type from netCDF data file. If data type global attribute exists,
// it reads that attributes and translate to NCGdataType numerical code, otherwise
// it defaults to NCGtypeContGrid.

char *NCGnameTypeString (NCGdataType dataType);
// Returns data type as character string 

char    *NCGdataGetTextAttribute (int ncid, int varid, const char *name);
// Checks the length of the named attribute and allocates memory before retrieving
// the attribute content. Returns NULL when the attribute does not exist, or a character
// pointer to the allocated memory with the content of the attribute.
NCGstate NCGdataSetTextAttribute (int ncid, int varid, const char *name,const char *text);
// Sets the named text attributes.

NCGstate NCGdataCopyAttributes (int source_ncid, int source_varid, int dest_ncid, int dest_varid, bool overwrite);
// Copies all attributes of a given variable from the source file to the destination variable.
NCGstate NCGdataCopyAllAttributes (int source_ncid, int dest_ncid, bool overwrite);
// Copies all attributes (global and variable) from source to destination.

typedef enum { NCGprojNoCoordinates = NCGundefined, NCGprojSpherical, NCGprojCartesian } NCGprojection;

NCGprojection NCGdataGetProjection (int ncid);
// If projection global attribute the function reads its content and translates to NCGprojection code.
// Otherwise, test the name of the horizontal dimension (Xdim) and returns "NCGprojCartesian", if the
// horizontal dimension is "x_coord" or NCGproSpherical when the horizontal dimension is either lon or longitude.

char *NCGnameProjString (NCGprojection);
// Returns projection name.

int NCGdataGetXDimId      (int ncid);
// Returns the identifier of the horizontal dimension ("x_coord", "lon" or "longitude").
int NCGdataGetXVarId      (int ncid);
// Returns the identifier of the horizontal variable  ("x_coord", "lon" or "longitude").
int NCGdataGetYDimId      (int ncid);
// Returns the identifier of the vertical   dimension ("y_coord", "lat" or "latitude").
int NCGdataGetYVarId      (int ncid);
// Returns the identifier of the vertical   variable  ("y_coord", "lat" or "latitude").
int NCGdataGetCDimId      (int ncid);
// Returns the identifier of the coordinate dimension ("coord").
int NCGdataGetLVarId      (int ncid);
// Returns the identifier of the level      variable  ("level").
int NCGdataGetTVarId      (int ncid);
// Returns the identifier of the time       variable  ("time").

double *NCGdataGetVector (int ncid, int dimid, int varid, size_t *len);
// Allocates memory and reads the vector values from NetCDF file. Returns the vector array and its length.
#define NCGdataGetXCoords(ncid,len) NCGdataGetVector (ncid,NCGdataGetXDimId(ncid),NCGdataGetXVarId(ncid),len)
#define NCGdataGetYCoords(ncid,len) NCGdataGetVector (ncid,NCGdataGetYDimId(ncid),NCGdataGetYVarId(ncid),len)
int     NCGdataGetCoreVarId   (int ncid);
// Returns the variable identifier of the grid variable by searching for the first variable that has both
// horizontal ("x_coord, "lon" or "longitude") and vertical ("y_coord", "lat", "latitude") dimensions.
#define NCGprintNCError(status,func) fprintf (stderr, "NCError \"%s\" in:  %s ()\n", nc_strerror(status), func)

NCGstate NCGfileCreate        (char *, int);
int      NCGfileVarAdd        (int, const char *, nc_type, nc_type, nc_type);
NCGstate NCGfileVarClone      (int, int);
NCGstate NCGfileSetTimeUnit   (int, const char *);
NCGstate NCGfileSetLevelUnit  (int, const char *);
NCGstate NCGfileSetVarUnit    (int, const char *);
NCGstate NCGfileSetTransform  (int, double, double);
NCGstate NCGfileSetMissingVal (int, double);
NCGstate NCGfileSetValidRange (int, double, double);

char   **NCGfileList         (const char *, size_t *);
void     NCGfileListFree     (char **, size_t);

#if defined(__cplusplus)
}
#endif

#endif
