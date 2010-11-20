#include<NC.h>

void NCGcoordinateAdd (NCGcoordinate_t *coord0,NCGcoordinate_t *coord1,NCGcoordinate_t *result)
	{
	if ((coord0 == (NCGcoordinate_t *) NULL) || (coord1 == (NCGcoordinate_t *) NULL) || (result == (NCGcoordinate_t *) NULL))
		{ fprintf (stderr,"Null Coordinate Pointer in: NCGcoordinateAdd ()\n"); return; }
	result->X = coord0->X + coord1->X;
	result->Y = coord0->Y + coord1->Y;
	}

void NCGcoordinateAddConst (NCGcoordinate_t *coord0,double constant,NCGcoordinate_t *result)
	{
	if ((coord0 == (NCGcoordinate_t *) NULL) || (result == (NCGcoordinate_t *) NULL))
		{ fprintf (stderr,"Null Coordinate Pointer in: NCGcoordinateAdd ()\n"); return; }
	result->X = coord0->X + constant;
	result->Y = coord0->Y + constant;
	}

void NCGcoordinateSubtract (NCGcoordinate_t *coord0,NCGcoordinate_t *coord1,NCGcoordinate_t *result)
	{
	if ((coord0 == (NCGcoordinate_t *) NULL) || (coord1 == (NCGcoordinate_t *) NULL) || (result == (NCGcoordinate_t *) NULL))
		{ fprintf (stderr,"Null Coordinate Pointer in: NCGcoordinateSubtract ()\n"); return; }
	result->X = coord0->X - coord1->X;
	result->Y = coord0->Y - coord1->Y;
	}

void NCGcoordinateSubtractConst (NCGcoordinate_t *coord0,double constant,NCGcoordinate_t *result)
	{
	if ((coord0 == (NCGcoordinate_t *) NULL) || (result == (NCGcoordinate_t *) NULL))
		{ fprintf (stderr,"Null Coordinate Pointer in: NCGcoordinateSubtractConst ()\n"); return; }
	result->X = coord0->X - constant;
	result->Y = coord0->Y - constant;
	}

void NCGcoordinateMultiply (NCGcoordinate_t *coord0,NCGcoordinate_t *coord1,NCGcoordinate_t *result)
	{
	if ((coord0 == (NCGcoordinate_t *) NULL) || (coord1 == (NCGcoordinate_t *) NULL) || (result == (NCGcoordinate_t *) NULL))
		{ fprintf (stderr,"Null Coordinate Pointer in: NCGcoordinateMultiply ()\n"); return; }
	result->X = coord0->X * coord1->X;
	result->Y = coord0->Y * coord1->Y;
	}

void NCGcoordinateMultiplyConst (NCGcoordinate_t *coord0,double constant,NCGcoordinate_t *result)
	{
	if ((coord0 == (NCGcoordinate_t *) NULL) || (result == (NCGcoordinate_t *) NULL))
		{ fprintf (stderr,"Null Coordinate Pointer in: NCGcoordinateMultiplyConst ()\n"); return; }
	result->X = coord0->X * constant;
	result->Y = coord0->Y * constant;
	}

void NCGcoordinateDivide (NCGcoordinate_t *coord0,NCGcoordinate_t *coord1,NCGcoordinate_t *result)
	{
	if ((coord0 == (NCGcoordinate_t *) NULL) || (coord1 == (NCGcoordinate_t *) NULL) || (result == (NCGcoordinate_t *) NULL))
		{ fprintf (stderr,"Null Coordinate Pointer in: NCGcoordinateDivide ()\n"); return; }
	result->X = coord0->X / coord1->X;
	result->Y = coord0->Y / coord1->Y;
	}

void NCGcoordinateDivideConst (NCGcoordinate_t *coord0,double constant,NCGcoordinate_t *result)
	{
	if ((coord0 == (NCGcoordinate_t *) NULL) || (result == (NCGcoordinate_t *) NULL))
		{ fprintf (stderr,"Null Coordinate Pointer in: NCGcoordinateDivideConst ()\n"); return; }
	result->X = coord0->X / constant;
	result->Y = coord0->Y / constant;
	}
