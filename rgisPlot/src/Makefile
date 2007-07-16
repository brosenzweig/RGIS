include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
BIN    = ../bin
CPP    = $(UNIXCPP) $(UNIXCPPOPS) -I$(INC)\
         -I../../CMlib/include\
         -I../../DBlib/include $(PGINC)
LINK   = $(UNIXCPP)
LIBS   = -L../../CMlib/lib\
         -L../../DBlib/lib -lDB21 -lCM10 $(PGLIB) $(X11LIBS) $(MATHLIB)
TARGET = $(BIN)/rgisPlot

INCLUDES = $(INC)/rgisPlot.H

OBJS = $(OBJ)/RGPMain.o\
       $(OBJ)/RGPDrawVecPoint.o\
       $(OBJ)/RGPDrawVecLine.o\
       $(OBJ)/RGPDrawGridCont.o\
       $(OBJ)/RGPDrawNetwork.o\
       $(OBJ)/RGPColorMaps.o

all: $(TARGET)

clean:
	rm $(TARGET) $(OBJS)

$(TARGET): $(OBJS) $(SRC)/Makefile
	$(LINK) $(OBJS) $(LIBS) -o $(TARGET)

$(OBJ)/%.o: $(SRC)/%.C $(INCLUDES)
	$(CPP) -o $(OBJ)/$*.o -c $(SRC)/$*.C
