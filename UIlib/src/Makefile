include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
LIB    = ../lib
CPP    = $(UNIXCPP) $(UNIXCPPOPS) -I$(X11INC) -I$(INC) -I../../DBlib/include
TARGET = $(LIB)/libUI21.a

INCLUDES = $(INC)/UI.H

OBJS = $(OBJ)/UIInit.o\
       $(OBJ)/UIColorSets.o\
       $(OBJ)/UIGlobe.o\
       $(OBJ)/UILoop.o\
       $(OBJ)/UIMenu.o\
       $(OBJ)/UIDialogForm.o\
       $(OBJ)/UIDialog.o\
       $(OBJ)/UISelect.o\
       $(OBJ)/UIFileSel.o\
       $(OBJ)/UIDataset.o\
       $(OBJ)/UIPause.o\
       $(OBJ)/UISymbols.o\
       $(OBJ)/UIPatterns.o\
       $(OBJ)/UIMarkers.o\
       $(OBJ)/UITable.o\
       $(OBJ)/UITableFields.o\
       $(OBJ)/UIFieldEdit.o\
       $(OBJ)/UI2DView.o\
       $(OBJ)/UI2DViewDisp.o\
       $(OBJ)/UI2DViewAux.o\
       $(OBJ)/UIAttribView.o\
       $(OBJ)/UIInqRegion.o\
       $(OBJ)/UIInqVector.o\
       $(OBJ)/UIXYGraph.o\
       $(OBJ)/UIXYGraphSh.o\
       $(OBJ)/UIGetLine.o\
       $(OBJ)/UIGetString.o\
       $(OBJ)/UINumberInput.o\
       $(OBJ)/UIAux.o\
       $(OBJ)/UIDataHeaderF.o\
       $(OBJ)/UIDataPropF.o\
       $(OBJ)/UIRelate.o

all: $(TARGET)

clean:
	rm $(TARGET) $(OBJS)

$(TARGET): $(OBJS) $(SRC)/Makefile
	$(UNIXAR) $(TARGET) $(OBJS)
	$(UNIXRL) $(TARGET)

$(OBJ)/%.o: $(SRC)/%.C $(INCLUDES)
	$(CPP) -o $(OBJ)/$*.o -c $(SRC)/$*.C
