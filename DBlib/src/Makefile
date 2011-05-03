include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
LIB    = ../lib
CPP    = $(UNIXCPP) $(UNIXCPPOPS) -I$(INC) -I../../CMlib/include
TARGET = $(LIB)/libDB22.a

INCLUDES = $(INC)/DB.H $(INC)/DBif.H

OBJS = $(OBJ)/DBDataset.o\
       $(OBJ)/DBObjData.o\
       $(OBJ)/DBObjDataBlck.o\
       $(OBJ)/DBObjTables.o\
       $(OBJ)/DBObjLIST.o\
       $(OBJ)/DBObjReadWrit.o\
       $(OBJ)/DBImpARC.o\
       $(OBJ)/DBImpARCPoint.o\
       $(OBJ)/DBImpARCLine.o\
       $(OBJ)/DBImpARCPoly.o\
       $(OBJ)/DBImpASCIINet.o\
       $(OBJ)/DBImpExpDMGrd.o\
       $(OBJ)/DBImpExpNetCDF.o\
       $(OBJ)/DBInfoDB.o\
       $(OBJ)/DBExpASCII.o\
       $(OBJ)/DBExpARCPnt.o\
       $(OBJ)/DBExpARCGrd.o\
       $(OBJ)/DBExpARCNet.o\
       $(OBJ)/DBVectorMisc.o\
       $(OBJ)/DBVLineMisc.o\
       $(OBJ)/DBVPolyMisc.o\
       $(OBJ)/DBGridMisc.o\
       $(OBJ)/DBNetMisc.o\
       $(OBJ)/DBTableMisc.o\
       $(OBJ)/DBVPoint2Grid.o\
       $(OBJ)/DBGCont2Net.o\
       $(OBJ)/DBGrid2Grid.o\
       $(OBJ)/DBNet2Grid.o\
       $(OBJ)/DBDate.o\
       $(OBJ)/DBFile.o\
       $(OBJ)/DBPause.o\
       $(OBJ)/DBMath.o\
       $(OBJ)/DBMisc.o\
       $(OBJ)/DBByteOrder.o\
       $(OBJ)/DBPredefList.o

all: $(TARGET)

clean:
	rm $(TARGET) $(OBJS)

$(TARGET): $(OBJS) $(SRC)/Makefile
	$(UNIXAR) $(TARGET) $(OBJS)
	$(UNIXRL) $(TARGET)

$(OBJ)/%.o: $(SRC)/%.C $(INCLUDES)
	$(CPP) -o $(OBJ)/$*.o -c $(SRC)/$*.C
