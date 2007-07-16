/******************************************************************************

GHAAS Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2004, University of New Hampshire

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
	GHAASSubjReservoirs
	GHAASSubjElevation,
	(char *) NULL};

char *_DBPredefinedGeoDomainList [] = {
	"Africa",
	"Asia",
	"Australia",
	"Europe",
	"North America",
	"South America",
	"Central America",
	"Arctic Region",
	"Canada",
	"Hungary",
	"Russia",
	"United States",
	"Amazonas",
	"Danube",
	"Lena",
	"Mississippi",
	"Gulf of Maine",
	(char *) NULL};

