include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
BIN    = ../bin
CC     = $(UNIXCC) $(UNIXCCOPS) -I$(INC)\
         -I../../CMlib/include\
         -I../../NClib/include
LINK   = $(UNIXCC)
LIBS   = -L../../CMlib/lib\
         -L../../NClib/lib -lNC10 -lCM10 $(UNIXLIBS)

INCLUDES = 

UTILS = $(BIN)/nc2table\
        $(BIN)/ncAttributes\
        $(BIN)/ncAggregate\
        $(BIN)/ncGridResample\
        $(BIN)/ncGridSampling\
        $(BIN)/ncTblCalculate

all: $(UTILS)

install:
	cp $(UTILS) $(INSTALLDIR)/bin/

uninstall:
	rm $(INSTALLDIR)/bin/*

clean:
	rm $(UTILS) $(OBJ)/NC*.o

$(BIN)/%: $(SRC)/NCC%.c $(INCLUDES) $(SRC)/Makefile
	$(CC) -o $(OBJ)/NCC$*.o -c $(SRC)/NCC$*.c
	$(LINK) $(OBJ)/NCC$*.o $(LIBS) -o $(BIN)/$*