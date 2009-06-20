/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2008, University of New Hampshire

DBPredefList.C

balazs.fekete@unh.edu

*******************************************************************************/

#include <DB.H>
char *_DBPredefinedSubjetList [] = {
	GHAASSubjNetwork,
	GHAASSubjRunoff,
	GHAASSubjDischarge,
	GHAASSubjRiverStorage,
	GHAASSubjFlowVelocity,
	GHAASSubjPrecip,
	GHAASSubjAirTemp,
	GHAASSubjHumidity,
	GHAASSubjVaporPres,
	GHAASSubjWindSpeed,
	GHAASSubjSolarRad,
	GHAASSubjStations,
	GHAASSubjReservoirs,
	GHAASSubjElevation,
	(char *) NULL};

char *_DBPredefinedGeoDomainList [] = {
		(char *) "Africa",
	(char *) "Asia",
	(char *) "Australia",
	(char *) "Europe",
	(char *) "North America",
	(char *) "South America",
	(char *) "Central America",
	(char *) "Arctic Region",
	(char *) "Canada",
	(char *) "Hungary",
	(char *) "Russia",
	(char *) "United States",
	(char *) "Amazonas",
	(char *) "Danube",
	(char *) "Lena",
	(char *) "Mississippi",
	(char *) "Gulf of Maine",
	(char *) NULL};

