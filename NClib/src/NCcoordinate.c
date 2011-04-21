#include <cm.h>
#include <NC.h>

void NCcoordinateAdd (NCcoordinate_t *coord0,NCcoordinate_t *coord1,NCcoordinate_t *result)
	{
	if ((coord0 == (NCcoordinate_t *) NULL) || (coord1 == (NCcoordinate_t *) NULL) || (result == (NCcoordinate_t *) NULL))
		{ CMmsgPrint (CMmsgAppError, "Null Coordinate Pointer in: %s %d",__FILE__,__LINE__); return; }
	result->X = coord0->X + coord1->X;
	result->Y = coord0->Y + coord1->Y;
	}

void NCcoordinateAddConst (NCcoordinate_t *coord0,double constant,NCcoordinate_t *result)
	{
	if ((coord0 == (NCcoordinate_t *) NULL) || (result == (NCcoordinate_t *) NULL))
		{ CMmsgPrint (CMmsgAppError, "Null Coordinate Pointer in: %s %d",__FILE__,__LINE__); return; }
	result->X = coord0->X + constant;
	result->Y = coord0->Y + constant;
	}

void NCcoordinateSubtract (NCcoordinate_t *coord0,NCcoordinate_t *coord1,NCcoordinate_t *result)
	{
	if ((coord0 == (NCcoordinate_t *) NULL) || (coord1 == (NCcoordinate_t *) NULL) || (result == (NCcoordinate_t *) NULL))
		{ CMmsgPrint (CMmsgAppError, "Null Coordinate Pointer in: %s %d",__FILE__,__LINE__); return; }
	result->X = coord0->X - coord1->X;
	result->Y = coord0->Y - coord1->Y;
	}

void NCcoordinateSubtractConst (NCcoordinate_t *coord0,double constant,NCcoordinate_t *result)
	{
	if ((coord0 == (NCcoordinate_t *) NULL) || (result == (NCcoordinate_t *) NULL))
		{ CMmsgPrint (CMmsgAppError, "Null Coordinate Pointer in: %s %d",__FILE__,__LINE__); return; }
	result->X = coord0->X - constant;
	result->Y = coord0->Y - constant;
	}

void NCcoordinateMultiply (NCcoordinate_t *coord0,NCcoordinate_t *coord1,NCcoordinate_t *result)
	{
	if ((coord0 == (NCcoordinate_t *) NULL) || (coord1 == (NCcoordinate_t *) NULL) || (result == (NCcoordinate_t *) NULL))
		{ CMmsgPrint (CMmsgAppError, "Null Coordinate Pointer in: %s %d",__FILE__,__LINE__); return; }
	result->X = coord0->X * coord1->X;
	result->Y = coord0->Y * coord1->Y;
	}

void NCcoordinateMultiplyConst (NCcoordinate_t *coord0,double constant,NCcoordinate_t *result)
	{
	if ((coord0 == (NCcoordinate_t *) NULL) || (result == (NCcoordinate_t *) NULL))
		{ CMmsgPrint (CMmsgAppError, "Null Coordinate Pointer in: %s %d",__FILE__,__LINE__); return; }
	result->X = coord0->X * constant;
	result->Y = coord0->Y * constant;
	}

void NCcoordinateDivide (NCcoordinate_t *coord0,NCcoordinate_t *coord1,NCcoordinate_t *result)
	{
	if ((coord0 == (NCcoordinate_t *) NULL) || (coord1 == (NCcoordinate_t *) NULL) || (result == (NCcoordinate_t *) NULL))
		{ CMmsgPrint (CMmsgAppError, "Null Coordinate Pointer in: %s %d",__FILE__,__LINE__); return; }
	result->X = coord0->X / coord1->X;
	result->Y = coord0->Y / coord1->Y;
	}

void NCcoordinateDivideConst (NCcoordinate_t *coord0,double constant,NCcoordinate_t *result)
	{
	if ((coord0 == (NCcoordinate_t *) NULL) || (result == (NCcoordinate_t *) NULL))
		{ CMmsgPrint (CMmsgAppError, "Null Coordinate Pointer in: %s %d",__FILE__,__LINE__); return; }
	result->X = coord0->X / constant;
	result->Y = coord0->Y / constant;
	}
