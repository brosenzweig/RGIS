include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
BIN    = ../bin
CPP    = $(UNIXCPP) $(UNIXCPPOPS) -I$(INC)\
         -I../../CMlib/include\
         -I../../DBlib/include
LINK   = $(UNIXCPP)
LIBS   = -L../../CMlib/lib\
         -L../../DBlib/lib -lDB21 -lCM10 $(UNIXLIBS)
TARGET = rgisPlot

INCLUDES = $(INC)/rgisPlot.H

OBJS = $(OBJ)/RGPMain.o\
       $(OBJ)/RGPDrawVecPoint.o\
       $(OBJ)/RGPDrawVecLine.o\
       $(OBJ)/RGPDrawGridCont.o\
       $(OBJ)/RGPDrawNetwork.o\
       $(OBJ)/RGPColorMaps.o

all: $(BIN)/$(TARGET)

install:
	cp $(BIN)/$(TARGET) $(INSTALLDIR)/bin/

uninstall:
	rm $(INSTALLDIR)/bin/$(TARGET)

clean:
	rm $(TARGET) $(OBJS)

$(BIN)/$(TARGET): $(OBJS) $(SRC)/Makefile
	$(LINK) $(OBJS) $(LIBS) -o $(BIN)/$(TARGET)

$(OBJ)/%.o: $(SRC)/%.C $(INCLUDES)
	$(CPP) -o $(OBJ)/$*.o -c $(SRC)/$*.C
