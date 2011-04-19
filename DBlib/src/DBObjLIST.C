/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

DBObjLIST.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <string.h>
#include <DB.H>

int _DBObjectLISTNameSort (const DBObject **obj0,const DBObject **obj1)

	{ return (strcmp ((*obj0)->Name (),(*obj1)->Name ())); }

int _DBObjectLISTNameReversedSort (const DBObject **obj0,const DBObject **obj1)

	{ return (strcmp ((*obj1)->Name (),(*obj0)->Name ())); }
