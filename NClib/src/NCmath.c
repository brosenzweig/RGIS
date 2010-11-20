#include<NCmath.h>

static double _NCGRadius = 6371.2213;
static double _NCGEpsilon =   0.000001;

bool NCGmathIsNumber(char *expr)
{
	register int i, strLen = strlen(expr);
	for(i = (expr[0] == '-') ? 1 : 0; i < strLen; i++) if((!isdigit(expr[i])) && (expr[i] != '.')) return false;
	return true;
}

void NCGmathSetSphereRadius (double radius) { _NCGRadius = radius; }
void NCGmathSetEpsilon (double epsilon) { _NCGEpsilon = epsilon; }

bool NCGmathEqualValues (double var0,double var1)
{
	if (fabs (var0) + fabs (var1) == (double) 0.0) return (true);
	return (fabs (var0 - var1) / (fabs (var0) + fabs (var1)) < _NCGEpsilon);
}

double NCGmathCoordinateDistance (NCGprojection projection, const NCGcoordinate_t *coord0, const NCGcoordinate_t *coord1)
{
	double cosC, sinC;
	double lon0, lat0, lon1, lat1;
	double distance;

	switch (projection)
	{
		default:
		case NCGprojCartesian:
			distance = (coord0->X - coord1->X) * (coord0->X - coord1->X) + (coord0->Y - coord1->Y) * (coord0->Y - coord1->Y);
			distance = sqrt (distance);
			break;
		case NCGprojSpherical:
			lon0 = coord0->X * M_PI / 180.0;
			lat0 = coord0->Y * M_PI / 180.0;
			lon1 = coord1->X * M_PI / 180.0;
			lat1 = coord1->Y * M_PI / 180.0;

			cosC = sin (lat0) * sin (lat1) + cos (lat0) * cos (lat1) * cos (lon1 - lon0);

			if (cosC > 0.9999)
			{
				sinC = sqrt (cos (lat1) * cos (lat1) * sin (lon1 - lon0) * sin (lon1 - lon0) +
				            (cos (lat0) * sin (lat1) - sin (lat0) * cos (lat1) * cos (lon1 - lon0)) *
				            (cos (lat0) * sin (lat1) - sin (lat0) * cos (lat1) * cos (lon1 - lon0)));
				distance = _NCGRadius * atan (sinC/cosC);
			}
			else distance = _NCGRadius * acos (cosC);
			break;
	}
	return (distance);
}

double NCGmathRectangleArea (NCGprojection projection, const NCGcoordinate_t *coord0, const NCGcoordinate_t *coord1)
{
	double lat0,lat1,lon0,lon1;
	double area, dc;

	switch (projection)
	{
		default:
		case NCGprojCartesian:
			area = fabs ((coord1->X -coord0->X) * (coord1->Y -coord0->Y)) / 1000000.0;
			break;
		case NCGprojSpherical:
			lon1 = coord1->X * M_PI / 180.0;
			lat1 = coord1->Y * M_PI / 180.0;
			lon0 = coord0->X * M_PI / 180.0;
			lat0 = coord0->Y * M_PI / 180.0;
			dc = fabs (sin (lat1) - sin (lat0));
			area = _NCGRadius * _NCGRadius * dc * fabs (lon1 - lon0);
			break;
	}
	return (area);
}

double NCGmathVectorByVector (const NCGcoordinate_t *commonPoint, const NCGcoordinate_t *pnt0, const NCGcoordinate_t *pnt1)
{
	double x0, y0, x1, y1;

	x0 = pnt0->X - commonPoint->X;
	y0 = pnt0->Y - commonPoint->Y;
	x1 = pnt1->X - commonPoint->X;
	y1 = pnt1->Y - commonPoint->Y;

	return ((x0 * x1) + (y0 * y1));
}

double NCGmathVectorXVector (const NCGcoordinate_t *commonPoint, const NCGcoordinate_t *pnt0, const NCGcoordinate_t *pnt1)
{
	double x0, y0, x1, y1;

	x0 = pnt0->X - commonPoint->X;
	y0 = pnt0->Y - commonPoint->Y;
	x1 = pnt1->X - commonPoint->X;
	y1 = pnt1->Y - commonPoint->Y;

	return ((x0 * y1) - (x1 * y0));
}

void NCGmathGauss (double a [],int n,int m)
{
	int i,j,k;
	double q;
	double *apoint [10], *point;

	for (i = 0; i < n; ++i) apoint [i] = a + i * m;

	for (i = 0;i < n - 1; ++i)
	{
		if (apoint [i][i] == (double) 0.0)
		{
			for (j = i + 1;(apoint [j][i] == 0) && (j < n); ++j);
			point = apoint [i];
			apoint [i] = apoint [j];
			apoint [j] = point;
		}
		for (j = i + 1;j < m; ++j)
		{
			q = apoint [i][j] / apoint [i][i];
			for (k = i + 1; k < n; ++ k) apoint [k][j] -= q * apoint [k][i];
		}
	}
	for (k = n;k < m; ++k)
	{
		apoint [n - 1][k] /= apoint [n - 1][n - 1];
		for (i = n - 2;i >= 0; --i)
		{
			q = 0;
			for (j = i + 1;j < n; ++j) q += apoint [i][j] * apoint [j][k];
			apoint [i][k] = (apoint [i][k] - q) / apoint [i][i];
		}
	}
}

double NCGmathStudentsT(double df)
{
	int i = 0;
	int DFval[37] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,40,60,120,240,520,1000 };
	double Tvalue[37] = {	0,12.706205,4.302653,3.182446,2.776445,2.570582,2.446912,2.364624,
									2.306004,2.262157,2.228139,2.200985,2.178813,2.160369,2.144787,
									2.131450,2.119905,2.109816,2.100922,2.093024,2.085963,2.079614,
									2.073873,2.068658,2.063899,2.059539,2.055529,2.051831,2.048407,
									2.045230,2.042272,2.021075,2.000298,1.979930,1.969898,1.964537, 1.962339 };

	while ((df > (double) DFval[i]) && (i <= 37))	i++;
	return(Tvalue[i]);
}

size_t NCGVariableSize (int type)
{
	switch (type)
	{
		default:
		case NC_BYTE:
		case NC_CHAR:   return (sizeof (char));
		case NC_SHORT:  return (sizeof (short));
		case NC_INT:    return (sizeof (int));
		case NC_FLOAT:  return (sizeof (float));
		case NC_DOUBLE: return (sizeof (double));
	}
}
