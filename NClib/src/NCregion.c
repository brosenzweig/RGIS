#include<NCmath.h>

void NCregionInitialize (NCregion_t *region)
	{
	if (region == (NCregion_t *) NULL)
		{ fprintf (stderr,"Null Region Pointer in: NCregionInitialize()\n"); return; }
	region->LowerLeft.X = HUGE_VAL;
	region->LowerLeft.Y = HUGE_VAL;
	region->UpperRight.X = - HUGE_VAL;
	region->UpperRight.Y = - HUGE_VAL;
	}

void NCregionExpand (NCregion_t *region0,NCregion_t *region1,NCregion_t *result)
	{
	if ((region0 == (NCregion_t *) NULL) || (region1 == (NCregion_t *) NULL) || (result == (NCregion_t *) NULL))
		{ fprintf (stderr,"Null Region Pointer in: NCregionExpand ()\n"); return; }
	result->LowerLeft.X = region0->LowerLeft.X < region1->LowerLeft.X ?  region0->LowerLeft.X : region1->LowerLeft.X;
	result->LowerLeft.Y = region0->LowerLeft.Y < region1->LowerLeft.Y ?  region0->LowerLeft.Y : region1->LowerLeft.Y;
	result->UpperRight.X = region0->UpperRight.X > region0->UpperRight.X ? region0->UpperRight.X : region0->UpperRight.X;
	result->UpperRight.Y = region0->UpperRight.Y > region0->UpperRight.Y ? region0->UpperRight.Y : region0->UpperRight.Y;
	}

void NCregionExpandCoord (NCregion_t *region,NCcoordinate_t *coord,NCregion_t *result)
	{
	if ((region == (NCregion_t *) NULL) || (coord == (NCcoordinate_t *) NULL) || (result == (NCregion_t *) NULL))
		{ fprintf (stderr,"Null Region Pointer in: NCregionExpandCoord ()\n"); return; }
	result->LowerLeft.X = region->LowerLeft.X < coord->X ?  region->LowerLeft.X : coord->X;
	result->LowerLeft.Y = region->LowerLeft.Y < coord->Y ?  region->LowerLeft.Y : coord->Y;
	result->UpperRight.X = region->UpperRight.X > coord->X ? region->UpperRight.X : coord->X;
	result->UpperRight.Y = region->UpperRight.Y > coord->Y ? region->UpperRight.Y : coord->Y;
	}

void NCregionUnion (NCregion_t *region0,NCregion_t *region1,NCregion_t *result)
	{
	if ((region0 == (NCregion_t *) NULL) || (region1 == (NCregion_t *) NULL) || (result == (NCregion_t *) NULL))
		{ fprintf (stderr,"Null Region Pointer in: NCregionUnion ()\n"); return; }
	result->LowerLeft.X = region0->LowerLeft.X > region1->LowerLeft.X ?  region0->LowerLeft.X : region1->LowerLeft.X;
	result->LowerLeft.Y = region0->LowerLeft.Y > region1->LowerLeft.Y ?  region0->LowerLeft.Y : region1->LowerLeft.Y;
	result->UpperRight.X = region0->UpperRight.X < region0->UpperRight.X ? region0->UpperRight.X : region0->UpperRight.X;
	result->UpperRight.Y = region0->UpperRight.Y < region0->UpperRight.Y ? region0->UpperRight.Y : region0->UpperRight.Y;
	}

bool NCregionTestCoord (NCregion_t *region,NCcoordinate_t *coord)
	{
	if ((region == (NCregion_t *) NULL) || (coord == (NCcoordinate_t *) NULL))
		{ fprintf (stderr,"Null Region Pointer in: NCregionTestCoord ()\n"); return (false); }
	if (coord->X < region->LowerLeft.X)  return (false);
	if (coord->Y < region->LowerLeft.Y)  return (false);
	if (coord->X > region->UpperRight.X) return (false);
	if (coord->Y > region->UpperRight.Y) return (false);
	return (true);
	}

bool NCregionTestRegion (NCregion_t *testRegion,NCregion_t *region)
	{
	if ((testRegion == (NCregion_t *) NULL) || (region == (NCregion_t *) NULL))
		{ fprintf (stderr,"Null Region Pointer in: NCregionTestRegion()\n"); return (false); }
	if (region->LowerLeft.X   < testRegion->LowerLeft.X)  return (false);
	if (region->LowerLeft.Y   < testRegion->LowerLeft.Y)  return (false);
	if (region->UpperRight.X > testRegion->UpperRight.X) return (false);
	if (region->UpperRight.X > testRegion->UpperRight.X) return (false);
	return (true);
	}
