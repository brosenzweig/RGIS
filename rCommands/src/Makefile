include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
BIN    = ../bin
CPP    = $(UNIXCPP) $(UNIXCPPOPS) -I$(INC)\
         -I../../CMlib/include\
         -I../../MFlib/include\
         -I../../DBlib/include\
         -I../../RGlib/include
LINK   = $(UNIXCPP)
LIBS   = -L../../CMlib/lib\
         -L../../MFlib/lib\
         -L../../DBlib/lib\
         -L../../RGlib/lib -lRG22 -lDB22 -lMF10 -lCM10 $(UNIXLIBS)


INCLUDES = $(INC)/CMDexp.H

UTILS = $(BIN)/dsAggregate\
        $(BIN)/dsDuration\
        $(BIN)/dsSampling\
        $(BIN)/grdDateLayers\
        $(BIN)/grdRenameLayers\
        $(BIN)/grdZoneStats\
        $(BIN)/grdZoneHist\
        $(BIN)/grdNoNeg\
        $(BIN)/grdAppendLayers\
        $(BIN)/grdBoxAggr\
        $(BIN)/grdExtractLayers\
        $(BIN)/grdOperation\
        $(BIN)/grdCalculate\
        $(BIN)/grdCycleMean\
        $(BIN)/grdCellStats\
        $(BIN)/grdCreateNetwork\
        $(BIN)/grdDifference\
        $(BIN)/grdImport\
        $(BIN)/grdMerge\
        $(BIN)/grdMinMax\
        $(BIN)/grdReclassDisc\
        $(BIN)/grdRunningMean\
        $(BIN)/grdSeasonAggr\
        $(BIN)/grdSeasonMean\
        $(BIN)/grdTimeSeries\
        $(BIN)/grdTSAggr\
        $(BIN)/netAccumulate\
        $(BIN)/netUnaccumulate\
        $(BIN)/netInvAccum\
        $(BIN)/netBasinDistrib\
        $(BIN)/netBasinHist\
        $(BIN)/netBasinProf\
        $(BIN)/netBasinStats\
        $(BIN)/netCellSlopes\
        $(BIN)/netUpstreamAvg\
        $(BIN)/netErosion\
        $(BIN)/netImportASCII\
        $(BIN)/netTransfer\
        $(BIN)/netDefragment\
        $(BIN)/netCells2Grid\
        $(BIN)/netConfluence\
        $(BIN)/netCreatePnts\
        $(BIN)/pntGridSampling\
        $(BIN)/pntSTNChar\
        $(BIN)/pntSubbasins\
        $(BIN)/pntSubbasStats\
        $(BIN)/pntSubbasHist\
        $(BIN)/pntSubbasCenter\
        $(BIN)/pntStationDist\
        $(BIN)/pntSymbolField\
        $(BIN)/tblAddField\
        $(BIN)/tblConv2Point\
        $(BIN)/tblRedefField\
        $(BIN)/tblDeleteField\
        $(BIN)/tblJoinTables\
        $(BIN)/tblMJoinTables\
        $(BIN)/tblDeleteRec\
        $(BIN)/tblSelectRec\
        $(BIN)/tblIdleRec\
        $(BIN)/tblCalculate\
        $(BIN)/tblTopoCalc\
        $(BIN)/tblConvCoord\
        $(BIN)/tblMergeCoord\
        $(BIN)/tblDecimalDate\
        $(BIN)/tblMergeDate\
        $(BIN)/tblSplitDate\
        $(BIN)/tblStatistics\
        $(BIN)/getHeader\
        $(BIN)/setHeader\
        $(BIN)/rgis2asciigrid\
        $(BIN)/rgis2domain\
        $(BIN)/rgis2table\
        $(BIN)/rgis2dm\
        $(BIN)/dm2rgis\
        $(BIN)/rgis2ds\
        $(BIN)/ds2rgis\
        $(BIN)/rgisMetadb \
        $(BIN)/rgis2netcdf\
        $(BIN)/netcdf2rgis

all: $(UTILS)

install:
	cp $(UTILS) $(INSTALLDIR)/bin/

uninstall:
	rm $(INSTALLDIR)/bin/*

clean:
	rm $(UTILS) $(OBJ)/CMD*.o

$(BIN)/%: $(SRC)/CMD%.C $(INCLUDES) $(SRC)/Makefile
	$(CPP) -o $(OBJ)/CMD$*.o -c $(SRC)/CMD$*.C
	$(LINK) $(OBJ)/CMD$*.o $(LIBS) -o $(BIN)/$*
