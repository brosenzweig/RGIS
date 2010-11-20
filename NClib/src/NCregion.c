#include<NCmath.h>

void NCGregionInitialize (NCGregion_t *region)
	{
	if (region == (NCGregion_t *) NULL)
		{ fprintf (stderr,"Null Region Pointer in: NCGregionInitialize()\n"); return; }
	region->LowerLeft.X = HUGE_VAL;
	region->LowerLeft.Y = HUGE_VAL;
	region->UpperRight.X = - HUGE_VAL;
	region->UpperRight.Y = - HUGE_VAL;
	}

void NCGregionExpand (NCGregion_t *region0,NCGregion_t *region1,NCGregion_t *result)
	{
	if ((region0 == (NCGregion_t *) NULL) || (region1 == (NCGregion_t *) NULL) || (result == (NCGregion_t *) NULL))
		{ fprintf (stderr,"Null Region Pointer in: NCGregionExpand ()\n"); return; }
	result->LowerLeft.X = region0->LowerLeft.X < region1->LowerLeft.X ?  region0->LowerLeft.X : region1->LowerLeft.X;
	result->LowerLeft.Y = region0->LowerLeft.Y < region1->LowerLeft.Y ?  region0->LowerLeft.Y : region1->LowerLeft.Y;
	result->UpperRight.X = region0->UpperRight.X > region0->UpperRight.X ? region0->UpperRight.X : region0->UpperRight.X;
	result->UpperRight.Y = region0->UpperRight.Y > region0->UpperRight.Y ? region0->UpperRight.Y : region0->UpperRight.Y;
	}

void NCGregionExpandCoord (NCGregion_t *region,NCGcoordinate_t *coord,NCGregion_t *result)
	{
	if ((region == (NCGregion_t *) NULL) || (coord == (NCGcoordinate_t *) NULL) || (result == (NCGregion_t *) NULL))
		{ fprintf (stderr,"Null Region Pointer in: NCGregionExpandCoord ()\n"); return; }
	result->LowerLeft.X = region->LowerLeft.X < coord->X ?  region->LowerLeft.X : coord->X;
	result->LowerLeft.Y = region->LowerLeft.Y < coord->Y ?  region->LowerLeft.Y : coord->Y;
	result->UpperRight.X = region->UpperRight.X > coord->X ? region->UpperRight.X : coord->X;
	result->UpperRight.Y = region->UpperRight.Y > coord->Y ? region->UpperRight.Y : coord->Y;
	}

void NCGregionUnion (NCGregion_t *region0,NCGregion_t *region1,NCGregion_t *result)
	{
	if ((region0 == (NCGregion_t *) NULL) || (region1 == (NCGregion_t *) NULL) || (result == (NCGregion_t *) NULL))
		{ fprintf (stderr,"Null Region Pointer in: NCGregionUnion ()\n"); return; }
	result->LowerLeft.X = region0->LowerLeft.X > region1->LowerLeft.X ?  region0->LowerLeft.X : region1->LowerLeft.X;
	result->LowerLeft.Y = region0->LowerLeft.Y > region1->LowerLeft.Y ?  region0->LowerLeft.Y : region1->LowerLeft.Y;
	result->UpperRight.X = region0->UpperRight.X < region0->UpperRight.X ? region0->UpperRight.X : region0->UpperRight.X;
	result->UpperRight.Y = region0->UpperRight.Y < region0->UpperRight.Y ? region0->UpperRight.Y : region0->UpperRight.Y;
	}

bool NCGregionTestCoord (NCGregion_t *region,NCGcoordinate_t *coord)
	{
	if ((region == (NCGregion_t *) NULL) || (coord == (NCGcoordinate_t *) NULL))
		{ fprintf (stderr,"Null Region Pointer in: NCGregionTestCoord ()\n"); return (false); }
	if (coord->X < region->LowerLeft.X)  return (false);
	if (coord->Y < region->LowerLeft.Y)  return (false);
	if (coord->X > region->UpperRight.X) return (false);
	if (coord->Y > region->UpperRight.Y) return (false);
	return (true);
	}

bool NCGregionTestRegion (NCGregion_t *testRegion,NCGregion_t *region)
	{
	if ((testRegion == (NCGregion_t *) NULL) || (region == (NCGregion_t *) NULL))
		{ fprintf (stderr,"Null Region Pointer in: NCGregionTestRegion()\n"); return (false); }
	if (region->LowerLeft.X   < testRegion->LowerLeft.X)  return (false);
	if (region->LowerLeft.Y   < testRegion->LowerLeft.Y)  return (false);
	if (region->UpperRight.X > testRegion->UpperRight.X) return (false);
	if (region->UpperRight.X > testRegion->UpperRight.X) return (false);
	return (true);
	}
