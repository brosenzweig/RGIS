/******************************************************************************

GHAAS RiverGIS Library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2010, UNH - CCNY/CUNY

RG.H

balazs.fekete@unh.edu

*******************************************************************************/

#define RGlibCycleMonthly 12
#define RGlibCycleWeekly  52
#define RGlibCycleDaily  365

#define RGlibAggrMinimum 0
#define RGlibAggrMaximum 1
#define RGlibAggrAverage 2
#define RGlibAggrSum     3

#define RGlibCycleIncr(cycle,tStep) (tStep + 1 < cycle ? tStep + 1 : 0)
#define RGlibCycleDecr(cycle,tStep) ((tStep > 0 ? tStep : cycle) - 1)
#define RGlibCycleStep(cycle,tStep) ((tStep) % cycle)

#define RGlibCellID         ((char *) "CellID")
#define RGlibBasinName      ((char *) "BasinName")
#define RGlibStreamOrder    ((char *) "Stream Order")
#define RGlibColor          ((char *) "Color")
#define RGlibCellNum        ((char *)  "NumberOfCells")
#define RGlibLength         ((char *) "STNMainstemLength")
#define RGlibArea           ((char *) "STNCatchmentArea")
#define RGlibInterStation   ((char *) "STNInterStationArea")
#define RGlibNextStation    ((char *) "NextStation")

#define RGlibMinSLOPE 0.01

DBInt RGlibGenFuncFieldCompare   (DBObjTable *,char *,char *,char *,DBInt);
DBInt RGlibGenFuncFieldCalculate (DBObjTable *,char *,char *,char *,DBInt);
DBInt RGlibGenFuncTopoAccum      (DBObjTable *,char *,char *,char *);
DBInt RGlibGenFuncTopoSubtract   (DBObjTable *,char *,char *,char *);
DBInt RGlibGenFuncSymbolField    (DBObjData*,const char *);

DBInt RGlibPointSTNCoordinates (DBObjData *,DBObjTableField *);
DBInt RGlibPointSTNCharacteristics (DBObjData *);

#define RGlibMassCoordX	"MassCoordX"
#define RGlibMassCoordY	"MassCoordY"

DBInt RGlibPointSubbasinCenter (DBObjData *, DBObjData *);
DBInt RGlibPointInterStationTS (DBObjData *,DBObjData *, char *,char *);
DBInt RGlibPointSubbasinStats (DBObjData *, DBObjData *, DBObjData *, DBObjData *);
DBInt RGlibPointSubbasinHist (DBObjData *, DBObjData *, DBObjData *, DBObjData *);

DBInt RGlibGridRemovePits (DBObjData *,DBObjData *);
DBInt RGlibGridResampling (DBObjData *, DBObjRecord *, DBObjData *, DBObjData *);
DBInt RGlibGridUniformRunoff  (DBObjData *, DBObjData *, char *, char *, char *, char *,DBObjData *);
DBInt RGlibGridUniformGrid  (DBObjData *, DBObjData *,char *,char *,char *,char *,DBObjData *);
DBInt RGlibGridReclassDiscrete (DBObjData *,char *,DBObjData *);
DBInt RGlibGridReclassContinuous (DBObjData *,char *,DBObjData *);
DBInt RGlibGridMakeDiscrete (DBObjData *, DBObjData *, float [], int);
DBInt RGlibGridZoneHistogram (DBObjData *,DBObjData *, DBObjData *);
DBInt RGlibGridZoneStatistics (DBObjData *, DBObjData *, DBObjData *);
DBInt RGlibGridNoNegatives (DBObjData *,DBInt);
DBInt RGlibGridCellStats (DBObjData *,DBObjData *);
DBInt RGlibGridBivarCellStats (DBObjData *,DBObjData *, DBObjData *);
DBInt RGlibGridSampling (DBObjData *,DBObjData *,DBObjData *);
DBInt RGlibCycleMean (DBObjData *,DBObjData *,DBInt,DBInt);
DBInt RGlibLayerAggregate (DBObjData *,DBObjData *,DBInt,DBInt,DBInt);
DBInt RGlibTSAggregate (DBObjData *,DBObjData *,DBInt, DBInt);
DBInt RGlibSeasonAggregate (DBObjData *,DBObjData *,DBInt,DBInt,DBInt);
DBInt RGlibSeasonMean (DBObjData *,DBObjData *,DBInt,DBInt,DBInt);
DBInt RGlibMinMax     (DBObjData *,DBObjData *,bool);

DBInt RGlibNetworkToGrid (DBObjData *,DBObjTableField *,DBObjData *);
DBInt RGlibNetworkBasinGrid (DBObjData *,DBObjData *);
DBInt RGlibNetworkStations (DBObjData *,DBFloat,DBFloat,DBObjData *);
DBInt RGlibNetworkAccumulate (DBObjData *,DBObjData *, DBObjData *,DBObjData *,char **,DBFloat,bool,bool,bool,DBObjData *);
DBInt RGlibNetworkAccumulate (DBObjData *,DBObjData *, DBObjData *);
DBInt RGlibNetworkUnaccumulate (DBObjData *,DBObjData *,DBFloat, bool, DBObjData *);
DBInt RGlibNetworkInvAccumulate (DBObjData *,DBObjData *,DBObjData *,DBFloat);
DBInt RGlibNetworkBasinDistrib (DBObjData *,DBObjData *,DBObjData *);
DBInt RGlibNetworkBasinProf (DBObjData *,DBObjData *,DBObjData *);
DBInt RGlibNetworkBasinStats (DBObjData *,DBObjData *,DBObjData *);
DBInt RGlibNetworkCellSlopes (DBObjData *,DBObjData *,DBObjData *);
DBInt RGlibNetworkUpStreamAvg (DBObjData *,DBObjData *,DBObjData *);
DBInt RGlibNetworkHeadStats (DBObjData *,DBObjData *,DBObjData *,DBInt);
DBInt RGlibNetworkHistogram (DBObjData *,DBObjData *,DBObjData *);
DBInt RGlibNetworkPsmRouting (DBObjData *,DBObjData *, DBObjData *, DBInt, DBFloat, DBObjData *);
DBInt RGlibNetworkDefragment (DBObjData *, DBObjData *,DBFloat, DBFloat, bool);
void RGlibGridSampling (DBObjData *,DBObjData *);

DBInt RGLibTableJoin (DBObjTable *,DBObjTableField *,DBObjTable *,DBObjTableField *);

DBInt RGlibRGIS2DataStream (DBObjData *,DBObjData *,char *,FILE *);
DBInt RGlibDataStream2RGIS (DBObjData *,DBObjData *,FILE *);
void  RGlibPauseOpen (const char *);
void  RGlibPauseClose ();
