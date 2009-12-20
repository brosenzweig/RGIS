/******************************************************************************

GHAAS Command Line Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

cmMath.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <math.h>
#include <cm.h>

bool CMmathEqualValues (double var0,double var1) {
	if (fabs (var0) + fabs (var1) == (double) 0.0) return (true);
	return (fabs (var0 - var1) / (fabs (var0) + fabs (var1)) < CMmathEpsilon);
}
