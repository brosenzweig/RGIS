#include<NC.h>
#include<NCnames.h>

char *NCGnameTypeString (NCGdataType dataType)
{
	switch (dataType)
	{
		default:	break;
		case NCGtypeGCont:   return (NCGnameTypeGCont);
//		case NCGtypeGDisc:   return (NCGnameTypeGDisc); 
		case NCGtypeNetwork: return (NCGnameTypeNetwork);
		case NCGtypePoint:   return (NCGnameTypePoint);
//		case NCGtypeLine:    return (NCGnameTypeLine);
//		case NCGtypePolygon: return (NCGnameTypePolygon);
	}
	return ((char *) NULL);
}

char *NCGnameProjString (NCGprojection proj)
{
	switch (proj)
	{
		default: break;
		case NCGprojCartesian: return (NCGnameGAProjCartesian);
		case NCGprojSpherical: return (NCGnameGAProjSpherical);
	}
	return ((char *) NULL);
}
