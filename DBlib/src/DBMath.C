/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBMath.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>

char *DBMathIntAutoFormat (DBInt maxVal)
	{
	static char format [12];
	DBInt length = 1;
	for(int i = 9; abs(maxVal) > i; i = i * 10 + 9, length++);
	if(maxVal < 0) length++;
	format [0] = '%';
	sprintf (format + 1,"%dd",length);
	return (format);
	}

char *DBMathFloatAutoFormat (DBFloat maxVal)
	{
	static char format [12];
	DBInt length, decimals, log10Val;

	maxVal = fabs (maxVal);
	log10Val = (DBInt) (rint (log10 (maxVal)));
	if (log10Val >= 0)
		{
		length = 8 > log10Val + 2 ? 8 : log10Val + 2;
		decimals = length - log10Val - 2;
		}
	else
		{
		length = abs (log10Val) + 6;
		decimals = length - 3;
		}
	format [0] = '%';
	sprintf (format + 1,"%d.%df",length,decimals);
	return (format);
	}

DBInt DBMathGuessProjection (DBRegion extent)
   {
   if (extent.LowerLeft.X < -180.0) return (DBProjectionCartesian);
   if (extent.LowerLeft.Y < - 90.0) return (DBProjectionCartesian);
   if (extent.UpperRight.X > 180.0) return (DBProjectionCartesian);
   if (extent.UpperRight.Y >  90.0) return (DBProjectionCartesian);
   return (DBProjectionSpherical);
   }

DBInt DBMathGuessPrecision (DBRegion extent)
   {
   DBFloat mapSize;

   mapSize = extent.UpperRight.X - extent.LowerLeft.X < extent.UpperRight.Y - extent.LowerLeft.Y ?
             extent.UpperRight.X - extent.LowerLeft.X : extent.UpperRight.Y - extent.LowerLeft.Y;
   return ((int) (floor (log10 (mapSize) - 5.0)));
   }

static DBFloat _DBGlobeRadius	= 6371.2213;
void DBMathSetGlobeRadius (DBFloat radius) { _DBGlobeRadius = radius; printf ("Radius = %f\n",_DBGlobeRadius); }

DBFloat DBMathCartesianDistance (DBCoordinate coord0,DBCoordinate coord1)

	{ return (sqrt ((coord0.X - coord1.X) * (coord0.X - coord1.X) + (coord0.Y - coord1.Y) * (coord0.Y - coord1.Y))); }

DBFloat DBMathSphericalDistance (DBCoordinate coord0,DBCoordinate coord1)

	{
	DBFloat cosC, sinC;
	DBFloat lon0, lat0, lon1, lat1;

	lon0 = coord0.X * M_PI / 180.0;
	lat0 = coord0.Y * M_PI / 180.0;
	lon1 = coord1.X * M_PI / 180.0;
	lat1 = coord1.Y * M_PI / 180.0;

	cosC = sin (lat0) * sin (lat1) + cos (lat0) * cos (lat1) * cos (lon1 - lon0);

	if (cosC > 0.9999)
		{
		sinC = sqrt (cos (lat1) * cos (lat1) * sin (lon1 - lon0) * sin (lon1 - lon0) +
						(cos (lat0) * sin (lat1) - sin (lat0) * cos (lat1) * cos (lon1 - lon0)) *
						(cos (lat0) * sin (lat1) - sin (lat0) * cos (lat1) * cos (lon1 - lon0)));
		return (_DBGlobeRadius * atan (sinC/cosC));
		}
	return (_DBGlobeRadius * acos (cosC));
	}


DBMathDistanceFunction DBMathGetDistanceFunction (DBInt projection)

	{ return (projection == DBProjectionSpherical ? DBMathSphericalDistance : DBMathCartesianDistance); }

DBMathDistanceFunction DBMathGetDistanceFunction (DBObjData *data)

	{ return (DBMathGetDistanceFunction (data->Projection ())); }

DBFloat DBMathCoordinateDistance (DBMathDistanceFunction func,DBCoordinate coord0,DBCoordinate coord1)

	{ return ((*func) (coord0,coord1)); }

DBFloat DBMathCoordinateDistance (DBInt projection,DBCoordinate coord0,DBCoordinate coord1)

	{ return ((*DBMathGetDistanceFunction (projection)) (coord0,coord1)); }

DBFloat DBMathCoordinateDistance (DBObjData *data,DBCoordinate coord0,DBCoordinate coord1)

	{ return ((*DBMathGetDistanceFunction (data)) (coord0,coord1)); }

DBFloat DBMathRectangleArea (DBInt projection,DBCoordinate coord0,DBCoordinate coord1)

	{
	DBFloat lat0,lat1,lon0,lon1;
	DBFloat area, dc;

	if (projection == DBProjectionSpherical)
		{
		lon1 = coord1.X * M_PI / 180.0;
		lat1 = coord1.Y * M_PI / 180.0;
		lon0 = coord0.X  * M_PI / 180.0;
		lat0 = coord0.Y  * M_PI / 180.0;
		dc = fabs (sin (lat1) - sin (lat0));
		area = _DBGlobeRadius * _DBGlobeRadius * dc * fabs (lon1 - lon0);
		}
	else area = fabs ((coord1.X -coord0.X) * (coord1.Y -coord0.Y)) / 1000000.0;
	return (area);
	}

DBFloat DBMathRectangleArea (DBObjData *data,DBCoordinate coord0,DBCoordinate coord1)

	{ return (DBMathRectangleArea (data->Projection (),coord0,coord1)); }

DBFloat DBMathRectangleArea (DBInt projection,DBRegion region)

	{ return (DBMathRectangleArea (projection,region.UpperRight, region.LowerLeft)); }

DBFloat DBMathRectangleArea (DBObjData *data,DBRegion region)

	{ return (DBMathRectangleArea (data->Projection (),region)); }

DBInt DBMathCoordinateXCompare (const DBCoordinate *coord0,const DBCoordinate *coord1)

	{
	if (coord0->X < coord1->X) return (-1);
	if (coord0->X > coord1->X) return (1);
	return (0);
	}

DBInt DBMathCoordinateYCompare (const DBCoordinate *coord0,const DBCoordinate *coord1)

	{
	if (coord0->Y < coord1->Y) return (-1);
	if (coord0->Y > coord1->Y) return (1);
	return (0);
	}

DBInt DBMathObservationTimeCompare (const DBObservation *obs0,const DBObservation *obs1)

	{
	if (obs0->Date < obs1->Date) return (-1);
	if (obs0->Date > obs1->Date) return (1);
	return (0);
	}

DBInt DBMathObservationValueCompare (const DBObservation *obs0,const DBObservation *obs1)

	{
	if (obs0->Value < obs1->Value) return (-1);
	if (obs0->Value > obs1->Value) return (1);
	return (0);
	}

DBFloat DBMathVectorByVector (DBCoordinate *commonPoint,DBCoordinate *pnt0,DBCoordinate *pnt1)

	{
	DBFloat x0, y0, x1, y1;

	x0 = pnt0->X - commonPoint->X;
	y0 = pnt0->Y - commonPoint->Y;
	x1 = pnt1->X - commonPoint->X;
	y1 = pnt1->Y - commonPoint->Y;

	return ((x0 * x1) + (y0 * y1));
	}

DBFloat DBMathVectorXVector (DBCoordinate *commonPoint,DBCoordinate *pnt0,DBCoordinate *pnt1)

	{
	DBFloat x0, y0, x1, y1;

	x0 = pnt0->X - commonPoint->X;
	y0 = pnt0->Y - commonPoint->Y;
	x1 = pnt1->X - commonPoint->X;
	y1 = pnt1->Y - commonPoint->Y;

	return ((x0 * y1) - (x1 * y0));
	}

void DBMathGauss (DBFloat a [],int n,int m)

	{
	int i,j,k;
	DBFloat q;
	DBFloat *apoint [10], *point;

	for (i = 0; i < n; ++i) apoint [i] = a + i * m;

	for (i = 0;i < n - 1; ++i)
		{
		if (apoint [i][i] == (DBFloat) 0)
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

DBFloat DBMathStudentsT(DBFloat df)

	{
	int i = 0;
	int DFval[37] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,40,60,120,240,520,1000 };
	DBFloat Tvalue[37] = {	0,12.706205,4.302653,3.182446,2.776445,2.570582,2.446912,2.364624,
									2.306004,2.262157,2.228139,2.200985,2.178813,2.160369,2.144787,
									2.131450,2.119905,2.109816,2.100922,2.093024,2.085963,2.079614,
									2.073873,2.068658,2.063899,2.059539,2.055529,2.051831,2.048407,
									2.045230,2.042272,2.021075,2.000298,1.979930,1.969898,1.964537, 1.962339 };

	while ((df > (float) DFval[i]) && (i <= 37))	i++;
	return(Tvalue[i]);
	}

static DBInt _DBMathOperator (char *str,DBInt *len)

	{
	const char *operStr [] = { " + "," - ", " * ", " % "," / ", " ^ ", " ** ",
								" && "," || "," < "," <= "," == "," != "," <> "," >= "," > ", (char *) NULL };
	DBInt operCodes [] = { 	DBMathOperatorAdd,
									DBMathOperatorSub,
									DBMathOperatorMul,
									DBMathOperatorMod,
									DBMathOperatorDiv,
									DBMathOperatorExp,
									DBMathOperatorExp,
									DBMathOperatorAnd,
									DBMathOperatorOr,
									DBMathOperatorSml,
									DBMathOperatorSml | DBMathOperatorEqu,
									DBMathOperatorEqu,
									DBMathOperatorSml | DBMathOperatorGrt,
									DBMathOperatorSml | DBMathOperatorGrt,
									DBMathOperatorGrt | DBMathOperatorEqu,
									DBMathOperatorGrt }, oper;
	DBInt operLen [] = { 3,3,3,3,3,3,4,4,4,3,4,4,4,4,4,3 };

	if ((oper = CMoptLookup (operStr,str,false)) == DBFault) return (DBFault);

	*len = operLen [oper];
	return (operCodes [oper]);
	}

static DBInt _DBMathOperator (char *str) { DBInt len; return (_DBMathOperator (str,&len)); }


static void *_DBMathFunction (char *funcName,int *nameLen)

	{
	int i;
	const char *funcNames [] = { "abs", "sin", "cos", "tan", "log2", "log", "ln", "floor", "ceil", (char *) NULL };
	double (*functions [])(double) = { fabs, sin, cos, tan, log2, log10, log, floor, ceil };

	if ((i = CMoptLookup (funcNames, funcName, false)) == DBFault) return ((void *) NULL);
	*nameLen = strlen (funcNames [i]);
	return ((void *) functions [i]);
	}

DBMathExpression::DBMathExpression (char *leftOper, char *rightOper, DBInt oper)

	{
	OperVAR = oper;
	TypeVAR = DBFault;

	LeftPTR  = new DBMathOperand (leftOper);
	RightPTR = new DBMathOperand (rightOper);
	}

DBMathExpression::DBMathExpression (char *leftOper, char *rightOper)

	{
	OperVAR = DBMathOperatorArgs;
	TypeVAR = DBVariableInt;

	LeftPTR  = new DBMathOperand (leftOper);
	RightPTR = new DBMathOperand (rightOper);
	}

DBMathExpression::DBMathExpression (char *cond, DBMathExpression *expr)

	{
	OperVAR = DBMathOperatorCond;
	TypeVAR = DBVariableInt;

	LeftPTR  = new DBMathOperand (cond);
	RightPTR = (DBMathOperand *) expr;
	}

DBMathExpression::DBMathExpression (char *oper, DBInt type, void *function)

	{
	OperVAR = DBMathOperatorFunc;
	TypeVAR = type;

	LeftPTR	= new DBMathOperand (oper);
	RightPTR = (DBMathOperand *) function;
	}

DBInt DBMathExpression::Expand (DBObjectLIST<DBObject> *variables)

	{
	if (LeftPTR->Expand (variables) == DBFault) return (DBFault);
	if ((OperVAR != DBMathOperatorFunc) && (OperVAR != DBMathOperatorCond))
		return (RightPTR->Expand (variables));
	return (DBSuccess);
	}

DBInt DBMathExpression::Configure (DBObjectLIST<DBObjTableField> *fieldList)

	{
	DBInt lType, rType;

	if ((lType = LeftPTR->Configure (fieldList)) == DBFault) return (DBFault);

	if (OperVAR == DBMathOperatorFunc) { TypeVAR = lType; return (TypeVAR); }
	if (OperVAR == DBMathOperatorCond)
		{
		TypeVAR = ((DBMathExpression *) RightPTR)->Configure (fieldList);
		return (TypeVAR);
		}
	if ((rType = RightPTR->Configure (fieldList)) == DBFault) return (DBFault);
	if ((OperVAR == DBMathOperatorOr) || (OperVAR == DBMathOperatorAnd)) TypeVAR = DBVariableInt;
	else if ((lType == DBVariableString) || (rType == DBVariableString)) TypeVAR = DBVariableString;
	else if ((lType == DBVariableFloat)  || (rType == DBVariableFloat))	TypeVAR = DBVariableFloat;
	else TypeVAR = DBVariableInt;
	return (TypeVAR);
	}

char *DBMathExpression::String (DBObjRecord *record,DBInt operand)

	{ return (operand ? LeftPTR->String (record) : RightPTR->String (record));	}

DBInt DBMathExpression::Int (DBObjRecord *record,DBInt operand)

	{ return (operand ? LeftPTR->Int (record) : RightPTR->Int (record));	}

DBFloat DBMathExpression::Float (DBObjRecord *record,DBInt operand)

	{ return (operand ? LeftPTR->Float (record) : RightPTR->Float (record));	}

char *DBMathExpression::String (DBObjRecord *record)

	{
	if (OperVAR == DBMathOperatorCond)
		{ return (((DBMathExpression *) RightPTR)->String (record,LeftPTR->Int (record))); }
	return ((char *) NULL);
	}

DBInt DBMathExpression::Int (DBObjRecord *record)

	{
	if (OperVAR == DBMathOperatorArgs) return (DBDefaultMissingIntVal);
	if (OperVAR == DBMathOperatorFunc)
		{
		DBInt var;
		int (*func) (int) = (int (*) (int)) RightPTR;
		return ((var = LeftPTR->Int (record)) == DBDefaultMissingIntVal ?
				  DBDefaultMissingIntVal : ((DBInt) ((*func) (var))));
		}
	if (OperVAR == DBMathOperatorCond)
		{ return (((DBMathExpression *) RightPTR)->Int (record,LeftPTR->Int (record))); }

	switch (TypeVAR)
		{
		case DBVariableString:
			{
			char *lVar, *rVar;
			DBInt comp;

			lVar = LeftPTR->String (record);
			rVar = RightPTR->String (record);
			if ((lVar == (char *) NULL) || (rVar == (char *) NULL)) return (DBDefaultMissingIntVal);
			comp = strcmp (lVar,rVar);
			switch (OperVAR)
				{
				case (DBMathOperatorGrt):								return (comp >  0 ? true : false);
				case (DBMathOperatorGrt | DBMathOperatorEqu):	return (comp >= 0 ? true : false);
				case (DBMathOperatorEqu):								return (comp == 0 ? true : false);
				case (DBMathOperatorSml | DBMathOperatorGrt):	return (comp != 0 ? true : false);
				case (DBMathOperatorSml | DBMathOperatorEqu):	return (comp <= 0 ? true : false);
				case (DBMathOperatorSml):								return (comp <  0 ? true : false);
				default:	break;
				}
			} break;
		case DBVariableInt:
			{
			DBInt lVar, rVar;

			lVar = LeftPTR->Int (record);
			rVar = RightPTR->Int (record);
			if ((OperVAR != DBMathOperatorEqu) && (OperVAR != (DBMathOperatorSml | DBMathOperatorGrt)) &&
				 ((lVar == DBDefaultMissingIntVal) || (rVar == DBDefaultMissingIntVal)))
				return (DBDefaultMissingIntVal);
			switch (OperVAR)
				{
				case (DBMathOperatorOr):								return (lVar || rVar);
				case (DBMathOperatorAnd): 								return (lVar && rVar);
				case (DBMathOperatorGrt):								return (lVar >  rVar ? true : false);
				case (DBMathOperatorGrt | DBMathOperatorEqu):	return (lVar >= rVar ? true : false);
				case (DBMathOperatorEqu):								return (lVar == rVar ? true : false);
				case (DBMathOperatorSml | DBMathOperatorGrt):	return (lVar != rVar ? true : false);
				case (DBMathOperatorSml | DBMathOperatorEqu):	return (lVar <= rVar ? true : false);
				case (DBMathOperatorSml):								return (lVar <  rVar ? true : false);
				case (DBMathOperatorAdd):								return (lVar + rVar);
				case (DBMathOperatorSub):								return (lVar - rVar);
				case (DBMathOperatorMul):								return (lVar * rVar);
				case (DBMathOperatorMod):								return (lVar % rVar);
				case (DBMathOperatorDiv):								return (lVar / rVar);
				case (DBMathOperatorExp):	return ((DBInt) (pow ((double) lVar,(double) rVar)));
				default:	break;
				}
			} break;
		case DBVariableFloat:
			{
			DBFloat lVar, rVar;
			lVar = LeftPTR->Float (record);
			rVar = RightPTR->Float (record);
			if ((OperVAR != DBMathOperatorEqu) && (OperVAR != (DBMathOperatorSml | DBMathOperatorGrt)) &&
			    ((CMmathEqualValues (lVar,DBDefaultMissingFloatVal) ||
				 	CMmathEqualValues (rVar,DBDefaultMissingFloatVal))))
				return (DBDefaultMissingIntVal);
			switch (OperVAR)
				{
				case (DBMathOperatorGrt): 								return (lVar >  rVar ? true : false);
				case (DBMathOperatorGrt | DBMathOperatorEqu):	return (lVar >= rVar ? true : false);
				case (DBMathOperatorEqu): 								return ( CMmathEqualValues (lVar,rVar));
				case (DBMathOperatorSml | DBMathOperatorGrt):	return (!CMmathEqualValues (lVar,rVar));
				case (DBMathOperatorSml | DBMathOperatorEqu):	return (lVar <= rVar ? true : false);
				case (DBMathOperatorSml):								return (lVar <  rVar ? true : false);
				case (DBMathOperatorAdd):								return ((DBInt) (lVar + rVar));
				case (DBMathOperatorSub):								return ((DBInt) (lVar - rVar));
				case (DBMathOperatorMul):								return ((DBInt) (lVar * rVar));
				case (DBMathOperatorMod):								return ((DBInt) (fmod (lVar,rVar)));
				case (DBMathOperatorDiv):								return ((DBInt) (lVar / rVar));
				case (DBMathOperatorExp):								return ((DBInt) (pow (lVar,rVar)));
				default:	break;
				}
			} break;
		}
	return (DBDefaultMissingIntVal);
	}

DBFloat DBMathExpression::Float (DBObjRecord *record)

	{
	if (OperVAR == DBMathOperatorArgs) return (DBDefaultMissingFloatVal);
	if (OperVAR == DBMathOperatorFunc)
		{
		DBFloat var;
		double (*func) (double) = (double (*) (double)) RightPTR;
		var = LeftPTR->Float (record);
		return (CMmathEqualValues (var, DBDefaultMissingFloatVal) ?
				  DBDefaultMissingFloatVal : ((DBFloat) ((*func) ((double) var))));
		}
	if (OperVAR == DBMathOperatorCond)
		{ return (((DBMathExpression *) RightPTR)->Float (record,LeftPTR->Int (record))); }

	switch (TypeVAR)
		{
		case DBVariableInt:
			{
			DBInt lVar, rVar;
			lVar = LeftPTR->Int (record);
			rVar = RightPTR->Int (record);
			if ((OperVAR != DBMathOperatorEqu) && (OperVAR != (DBMathOperatorSml | DBMathOperatorGrt)) &&
			    ((lVar == DBDefaultMissingIntVal) || (rVar == DBDefaultMissingIntVal)))
				return (DBDefaultMissingFloatVal);
			switch (OperVAR)
				{
				case (DBMathOperatorAdd):	return ((DBFloat) lVar + (DBFloat) rVar);
				case (DBMathOperatorSub):	return ((DBFloat) lVar - (DBFloat) rVar);
				case (DBMathOperatorMul):	return ((DBFloat) lVar * (DBFloat) rVar);
				case (DBMathOperatorMod):	return ((DBFloat) (lVar % rVar));
				case (DBMathOperatorDiv):	return ((DBFloat) lVar / (DBFloat) rVar);
				case (DBMathOperatorExp):	return ((DBFloat) pow ((double) lVar,(double) rVar));
				default:	break;
				}
			} break;
		case DBVariableFloat:
			{
			DBFloat lVar, rVar;
			lVar = LeftPTR->Float (record);
			rVar = RightPTR->Float (record);
			if ((OperVAR != DBMathOperatorEqu) && (OperVAR != (DBMathOperatorSml | DBMathOperatorGrt)) &&
			    (CMmathEqualValues (lVar,DBDefaultMissingFloatVal) ||
			    CMmathEqualValues (rVar,DBDefaultMissingFloatVal)))
				return (DBDefaultMissingFloatVal);
			switch (OperVAR)
				{
				case (DBMathOperatorAdd):	return (lVar + rVar);
				case (DBMathOperatorSub):	return (lVar - rVar);
				case (DBMathOperatorMul):	return (lVar * rVar);
				case (DBMathOperatorMod):	return ((DBFloat) (fmod (lVar,rVar)));
				case (DBMathOperatorDiv):	return (lVar / rVar);
				case (DBMathOperatorExp):	return (pow (lVar,rVar));
				default:	break;
				}
			} break;
		}
	return (DBDefaultMissingFloatVal);
	}

DBMathOperand::DBMathOperand (char *expression)

	{
	if ((Var.String = (char *) malloc (strlen (expression) + 1)) == (char *) NULL)
		{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); }
	else
		{
		strcpy (Var.String, expression);
		DBMiscTrimWhiteSpaces (Var.String);
		DBMiscTrimBrackets (Var.String);
		if ((int) strlen (Var.String) < 1) { free (Var.String); Var.String = (char *) NULL; }
		}
	}

DBInt DBMathOperand::Expand (DBObjectLIST<DBObject> *variables)

	{
	char *expression = Var.String, *exprPtr;
	DBInt i, strLen = expression != (char *) NULL ? (int) strlen (expression) : 0, bracketNum = 0;
	DBInt operPos = DBFault, oper, operLen, nonWhite;
	long intVal;
	double floatVal;
	void *function;

	if (expression == (char *) NULL) return (DBFault);

	for (i = 0;i < strLen;++i)
		{
		if (expression [i] == '(')
			{
			bracketNum = 0;
			for ( ;i < strLen;++i)
				{
				if (expression [i] == '(')   bracketNum++;
				if (expression [i] == ')') { bracketNum--;	if (bracketNum == 0) break; }
				}
			if (i == strLen) return (DBFault);
			}
		if (strncmp (expression + i," ? ",3) == 0)
			{
			DBMathExpression *expr;
			DBInt condPos = i + 1, condNum;

			condNum = 0;
			for ( ;i < strLen;++i)
				{
				if (strncmp (expression + i," ? ",3) == 0) { condNum++; i += 2; }
				if (strncmp (expression + i," : ",3) == 0) { condNum--; if (condNum == 0) break; }
				}
			if (i == strLen) return (DBFault);

			OprTypeVAR = DBMathOperandCond;
			expression [condPos] = '\0';
			expression [i + 1] = '\0';
			i += 2;
//			CMmsgPrint (CMmsgDebug,"if %s then %s else %s\n",expression,expression + condPos + 1,expression + i + 1);
			expr = new DBMathExpression (expression + condPos + 1,expression + i + 1);
			if (expr->Expand (variables) == DBFault) return (DBFault);
			Var.ExpPTR = new DBMathExpression (expression, expr);
			return (Var.ExpPTR->Expand (variables));
			}
		}

	nonWhite = false;
	for (i = 0;i < strLen;++i)
		{
		if (expression [i] == '(')
			{
			bracketNum = 0;
			for ( ;i < strLen;++i)
				{
				if (expression [i] == '(')   bracketNum++;
				if (expression [i] == ')') { bracketNum--;	if (bracketNum == 0) break; }
				}
			if (i == strLen) return (DBFault);
			}
		if (((oper = _DBMathOperator (expression + i,&operLen)) != DBFault) && nonWhite)
			{
			if (operPos == DBFault) operPos = i;
			else { if (_DBMathOperator (expression + operPos) <= oper) operPos = i; }
			for ( ;operLen > 1;--operLen) ++i;
			nonWhite = false;
			}
		else if (DBMiscTestWhiteChar (expression [i]) != true) nonWhite = true;
		}

	if (operPos != DBFault)
		{
		OprTypeVAR = DBMathOperandExpr;
		VarTypeVAR = DBFault;

		oper = _DBMathOperator (expression + operPos,&operLen);
		expression [operPos] = '\0';
		for ( ;operLen > 1;--operLen) ++operPos;
//		CMmsgPrint (CMmsgDebug,"%s and %s\n", expression, expression + operPos + 1);
		Var.ExpPTR = new DBMathExpression (expression,expression + operPos + 1,oper);
		free (expression);
		return (Var.ExpPTR->Expand (variables));
		}

	if ((function = _DBMathFunction (expression,&operPos)) != (void *) NULL)
		{
		OprTypeVAR = DBMathOperandExpr;
		VarTypeVAR = DBFault;

		expression [operPos] = '\0';
//		CMmsgPrint (CMmsgDebug,"%s of %s\n", expression, expression + operPos + 1);
		Var.ExpPTR = new DBMathExpression (expression + operPos + 1,DBVariableFloat,function);
		free (expression);
		return (Var.ExpPTR->Expand (variables));
		}

	intVal = strtol (expression,&exprPtr,10);
	if (expression + strlen (expression) == exprPtr)
		{
		OprTypeVAR = DBMathOperandConst;
		VarTypeVAR = DBVariableInt;

		Var.Int = intVal;
//		CMmsgPrint (CMmsgDebug,"Constant integer: %d\n",(int) intVal);
		free (expression);
		return (DBSuccess);
		}

	floatVal = strtod (expression,&exprPtr);
	if (expression + strlen (expression) == exprPtr)
		{
		OprTypeVAR = DBMathOperandConst;
		VarTypeVAR = DBVariableFloat;

		Var.Float = floatVal;
//		CMmsgPrint (CMmsgDebug,"Constant float: %f\n",floatVal);
		free (expression);
		return (DBSuccess);
		}

	if (DBMiscTrimQuotes (expression) > 0)
		{
		floatVal = strtod (expression,&exprPtr);
		if (expression + strlen (expression) != exprPtr)
			{
			OprTypeVAR = DBMathOperandConst;
			VarTypeVAR = DBVariableString;
//			CMmsgPrint (CMmsgDebug,"Constant string: %s\n",expression);
			if ((Var.String = (char *) malloc (strlen (expression) + 1)) == (char *) NULL)
				{ CMmsgPrint (CMmsgSysError, "Memory allocation error in: %s %d",__FILE__,__LINE__); return (DBFault); }
			strcpy (Var.String,expression);
			free (expression);
			return (DBSuccess);
			}
		}

	if (strcmp (expression,"M_PI") == 0)
		{
		OprTypeVAR = DBMathOperandConst;
		VarTypeVAR = DBVariableFloat;

		Var.Float = M_PI;
//		CMmsgPrint (CMmsgDebug,"Constant float: %f\n",floatVal);
		free (expression);
		return (DBSuccess);
		}

	if (strcmp (expression,"M_E") == 0)
		{
		OprTypeVAR = DBMathOperandConst;
		VarTypeVAR = DBVariableFloat;

		Var.Float = M_E;
//		CMmsgPrint (CMmsgDebug,"Constant float: %f\n",floatVal);
		free (expression);
		return (DBSuccess);
		}

	if (strcmp (expression,"nodata") == 0)
		{
		OprTypeVAR = DBMathOperandConst;
		VarTypeVAR = DBVariableInt;

		Var.Int = DBDefaultMissingIntVal;
//		CMmsgPrint (CMmsgDebug,"Nodata\n");
		free (expression);
		return (DBSuccess);
		}

	if (strcmp (expression,"true") == 0)
		{
		OprTypeVAR = DBMathOperandConst;
		VarTypeVAR = DBVariableInt;

		Var.Int = true;
//		CMmsgPrint (CMmsgDebug,"True\n");
		free (expression);
		return (DBSuccess);
		}

	if (strcmp (expression,"false") == 0)
		{
		OprTypeVAR = DBMathOperandConst;
		VarTypeVAR = DBVariableInt;

		Var.Int = false;
//		CMmsgPrint (CMmsgDebug,"False\n");
		free (expression);
		return (DBSuccess);
		}

	OprTypeVAR = DBMathOperandVar;
	VarTypeVAR = DBFault;
	if ((Var.ObjPTR = variables->Item (expression)) == (DBObject *) NULL)
		{ Var.ObjPTR = new DBObject (expression); variables->Add (Var.ObjPTR); }

//	CMmsgPrint (CMmsgDebug,"Variable: %s(%d)\n",expression,Var.ObjPTR->RowID ());
	free (expression);
	return (DBSuccess);
	}

DBInt DBMathOperand::Configure (DBObjectLIST<DBObjTableField> *fieldList)

	{
	switch (OprTypeVAR)
		{
		default:
		case DBMathOperandConst:	break;
		case DBMathOperandVar:
			{
			DBObjTableField *fieldPTR;
			if ((fieldPTR = fieldList->Item (Var.ObjPTR->Name ())) == (DBObject *) NULL) return (DBFault);
			Var.FldPTR = fieldPTR;
			VarTypeVAR = Var.FldPTR->Type ();
			break;
			}
		case DBMathOperandExpr:
		case DBMathOperandCond:
			VarTypeVAR = Var.ExpPTR->Configure (fieldList);
			break;
		}
	return (VarTypeVAR);
	}

char *DBMathOperand::String (DBObjRecord *record)

	{
	switch (OprTypeVAR)
		{
		case DBMathOperandConst:	return (VarTypeVAR == DBVariableString ? Var.String : (char *) NULL);
		case DBMathOperandVar:		return (Var.FldPTR->String (record));
		case DBMathOperandCond:		return (Var.ExpPTR->String (record));
		default:	break;
		}
	return ((char *) NULL);
	}

DBInt DBMathOperand::Int (DBObjRecord *record)

	{
	switch (OprTypeVAR)
		{
		case DBMathOperandConst:
			switch (VarTypeVAR)
				{
				case DBVariableString:	return (DBDefaultMissingIntVal);
				case DBVariableInt:		return (Var.Int);
				case DBVariableFloat:
					return (CMmathEqualValues (Var.Float,DBDefaultMissingFloatVal) ?
							  DBDefaultMissingIntVal : (DBInt) Var.Float);
				}
		case DBMathOperandVar:			return (Var.FldPTR->Int (record));
		case DBMathOperandExpr:
		case DBMathOperandCond:			return (Var.ExpPTR->Int (record));
		default:	break;
		}
	return (DBDefaultMissingIntVal);
	}

DBFloat DBMathOperand::Float (DBObjRecord *record)

	{
	switch (OprTypeVAR)
		{
		case DBMathOperandConst:
			switch (VarTypeVAR)
				{
				case DBVariableString:	return (DBDefaultMissingFloatVal);
				case DBVariableInt:
					return (Var.Int == DBDefaultMissingIntVal ? DBDefaultMissingFloatVal : (DBFloat) Var.Int);
				case DBVariableFloat:	return (Var.Float);
				} break;
		case DBMathOperandVar:			return (Var.FldPTR->Float (record));
		case DBMathOperandExpr:
		case DBMathOperandCond:			return (Var.ExpPTR->Float (record));
		default:	break;
		}
	return (DBDefaultMissingFloatVal);
	}
