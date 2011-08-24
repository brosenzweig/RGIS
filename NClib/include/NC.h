#ifndef _NC_H
#define _NC_H

#include <stdlib.h>
#include <netcdf.h>
#include <NCcore.h>
#include <NCnames.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum { NCscaleNone, NCscaleLinear, NCscaleLogarithmic } NCscale;

typedef enum { NCsizeShortString =   8,
               NCsizeString      =  64,
               NCsizeLongString  = 128,
               NCsizeText        = 256,
               NCsizeLongText    = 512 } NCsize;

typedef enum { NCdirE  = (0x01 << 0x00),
               NCdirSE = (0x01 << 0x01),
               NCdirS  = (0x01 << 0x02),
               NCdirSW = (0x01 << 0x03),
               NCdirW  = (0x01 << 0x04),
               NCdirNW = (0x01 << 0x05),
               NCdirN  = (0x01 << 0x06),
               NCdirNE = (0x01 << 0x07) } NCdir;

#define NCNetworkOppositeDirection(dir) (((dir >> 0x04) | (dir << 0x04)) & 0xff)

typedef struct NCcoordinate_s
{
	double X;
	double Y;
} NCcoordinate_t;

void NCcoordinateAdd           (NCcoordinate_t *, NCcoordinate_t *, NCcoordinate_t *);
void NCcoordinateAddConst      (NCcoordinate_t *, double, NCcoordinate_t *);
void NCcoordinateSubtract      (NCcoordinate_t *, NCcoordinate_t *, NCcoordinate_t *);
void NCcoordinateSubtractConst (NCcoordinate_t *, double, NCcoordinate_t *);
void NCcoordinateMultiply      (NCcoordinate_t *, NCcoordinate_t *, NCcoordinate_t *);
void NCcoordinateMultiplyConst (NCcoordinate_t *, double, NCcoordinate_t *);
void NCcoordinateDivide        (NCcoordinate_t *, NCcoordinate_t *, NCcoordinate_t *);
void NCcoordinateDivideConst   (NCcoordinate_t *, double, NCcoordinate_t *);

typedef struct NCregion_s
{
	NCcoordinate_t LowerLeft;
	NCcoordinate_t UpperRight;
} NCregion_t;

void NCregionInitialize        (NCregion_t *);
void NCregionExpand            (NCregion_t *, NCregion_t *,     NCregion_t *);
void NCregionExpandCoord       (NCregion_t *, NCcoordinate_t *, NCregion_t *);
void NCregionUnion             (NCregion_t *, NCregion_t *,     NCregion_t *);
bool NCregionTestCoord         (NCregion_t *, NCcoordinate_t *);
bool NCregionTestRegion        (NCregion_t *, NCregion_t *);

typedef enum { NCtypeUndefined = NCundefined,
	            NCtypeGCont     = 1,
               NCtypeGDisc     = 2,
               NCtypeNetwork   = 3,
	            NCtypePoint     = 4,
               NCtypeLine      = 5,
               NCtypePolygon   = 6 } NCdataType;

NCdataType NCdataGetType (int ncid);
// Reads the data type from netCDF data file. If data type global attribute exists,
// it reads that attributes and translate to NCdataType numerical code, otherwise
// it defaults to NCtypeContGrid.

char *NCnameTypeString (NCdataType dataType);
// Returns data type as character string 

char    *NCdataGetTextAttribute (int ncid, int varid, const char *name);
// Checks the length of the named attribute and allocates memory before retrieving
// the attribute content. Returns NULL when the attribute does not exist, or a character
// pointer to the allocated memory with the content of the attribute.
NCstate NCdataSetTextAttribute (int ncid, int varid, const char *name,const char *text);
// Sets the named text attributes.

NCstate NCdataCopyAttributes (int source_ncid, int source_varid, int dest_ncid, int dest_varid, bool overwrite);
// Copies all attributes of a given variable from the source file to the destination variable.
NCstate NCdataCopyAllAttributes (int source_ncid, int dest_ncid, bool overwrite);
// Copies all attributes (global and variable) from source to destination.

typedef enum { NCprojNoCoordinates = NCundefined, NCprojSpherical, NCprojCartesian } NCprojection;

NCprojection NCdataGetProjection (int ncid);
// If projection global attribute the function reads its content and translates to NCprojection code.
// Otherwise, test the name of the horizontal dimension (Xdim) and returns "NCprojCartesian", if the
// horizontal dimension is "x_coord" or NCproSpherical when the horizontal dimension is either lon or longitude.

char *NCnameProjString (NCprojection);
// Returns projection name.

int NCdataGetXDimId      (int ncid);
// Returns the identifier of the horizontal dimension ("x_coord", "lon" or "longitude").
int NCdataGetXVarId      (int ncid);
// Returns the identifier of the horizontal variable  ("x_coord", "lon" or "longitude").
int NCdataGetYDimId      (int ncid);
// Returns the identifier of the vertical   dimension ("y_coord", "lat" or "latitude").
int NCdataGetYVarId      (int ncid);
// Returns the identifier of the vertical   variable  ("y_coord", "lat" or "latitude").
int NCdataGetCDimId      (int ncid);
// Returns the identifier of the coordinate dimension ("coord").
int NCdataGetLVarId      (int ncid);
// Returns the identifier of the level      variable  ("level").
int NCdataGetTVarId      (int ncid);
// Returns the identifier of the time       variable  ("time").

double *NCdataGetVector (int ncid, int dimid, int varid, size_t *len);
// Allocates memory and reads the vector values from NetCDF file. Returns the vector array and its length.
#define NCdataGetXCoords(ncid,len) NCdataGetVector (ncid,NCdataGetXDimId(ncid),NCdataGetXVarId(ncid),len)
#define NCdataGetYCoords(ncid,len) NCdataGetVector (ncid,NCdataGetYDimId(ncid),NCdataGetYVarId(ncid),len)
int     NCdataGetCoreVarId   (int ncid);
// Returns the variable identifier of the grid variable by searching for the first variable that has both
// horizontal ("x_coord, "lon" or "longitude") and vertical ("y_coord", "lat", "latitude") dimensions.
#define NCprintNCError(status,func) fprintf (stderr, "NCError \"%s\" in:  %s ()\n", nc_strerror(status), func)

NCstate NCfileCreate        (char *, int);
int      NCfileVarAdd        (int, const char *, nc_type, nc_type, nc_type);
NCstate NCfileVarClone      (int, int);
NCstate NCfileSetTimeUnit   (int, const char *);
NCstate NCfileSetLevelUnit  (int, const char *);
NCstate NCfileSetVarUnit    (int, const char *);
NCstate NCfileSetTransform  (int, double, double);
NCstate NCfileSetMissingVal (int, double);
NCstate NCfileSetValidRange (int, double, double);

char   **NCfileList         (const char *, size_t *);
void     NCfileListFree     (char **, size_t);

NCstate NCGridContSampling (int, int);

#if defined(__cplusplus)
}
#endif

#endif
