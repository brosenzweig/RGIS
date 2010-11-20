#include<NC.h>
#include<NCnames.h>

char *NCnameTypeString (NCdataType dataType)
{
	switch (dataType)
	{
		default:	break;
		case NCtypeGCont:   return (NCnameTypeGCont);
//		case NCtypeGDisc:   return (NCnameTypeGDisc); 
		case NCtypeNetwork: return (NCnameTypeNetwork);
		case NCtypePoint:   return (NCnameTypePoint);
//		case NCtypeLine:    return (NCnameTypeLine);
//		case NCtypePolygon: return (NCnameTypePolygon);
	}
	return ((char *) NULL);
}

char *NCnameProjString (NCprojection proj)
{
	switch (proj)
	{
		default: break;
		case NCprojCartesian: return (NCnameGAProjCartesian);
		case NCprojSpherical: return (NCnameGAProjSpherical);
	}
	return ((char *) NULL);
}
