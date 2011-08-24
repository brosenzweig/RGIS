include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
LIB    = ../lib
CPP    = $(UNIXCPP) $(UNIXCPPOPS) -I$(INC) -I../../DBlib/include -I../../CMlib/include -I../../MFlib/include
TARGET = $(LIB)/libRG22.a

INCLUDES = $(INC)/RG.H

OBJS = $(OBJ)/RGlibPoint.o\
       $(OBJ)/RGlibGrid.o\
       $(OBJ)/RGlibNetwork.o\
       $(OBJ)/RGlibTable.o\
       $(OBJ)/RGlibGenFuncs.o\
       $(OBJ)/RGlibDataStream.o\
       $(OBJ)/RGlibPause.o

all: $(TARGET)

clean:
	rm $(TARGET) $(OBJS)

$(TARGET): $(OBJS) $(SRC)/Makefile
	$(UNIXAR) $(TARGET) $(OBJS)
	$(UNIXRL) $(TARGET)

$(OBJ)/%.o: $(SRC)/%.C $(INCLUDES)
	$(CPP) -o $(OBJ)/$*.o -c $(SRC)/$*.C
