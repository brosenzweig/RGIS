include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
BIN    = ../bin
CC     = $(UNIXCC) $(UNIXCCOPS) -I$(INC)\
         -I../../CMlib/include\
         -I../../DBlib/include\
         -I../../Flib/include
LINKCC = $(UNIXCC)
CPP    = $(UNIXCPP) $(UNIXCPPOPS) -I$(INC)\
         -I../../CMlib/include\
         -I../../DBlib/include\
         -I../../Flib/include
LINKCPP = $(UNIXCPP)
LIBS   = -L../../CMlib/lib\
         -L../../Flib/lib\
         -L../../DBlib/lib -lDB22 -lF10 -lCM10 $(UNIXLIBS)

INCLUDES = 

UTILS = $(BIN)/FAccumulate\
        $(BIN)/FCut\
        $(BIN)/FSort\
        $(BIN)/FStat\
        $(BIN)/Ftbl2rgis\
        $(BIN)/FPrintDataTypes

all: $(UTILS)

install:
	cp $(UTILS) $(INSTALLDIR)/bin/

uninstall:
	rm $(INSTALLDIR)/bin/F*

clean:
	rm $(UTILS) $(OBJ)/*.o

$(BIN)/%: $(SRC)/%.c $(INCLUDES) $(SRC)/makefile
	$(CC) -o $(OBJ)/$*.o -c $(SRC)/$*.c
	$(LINKCC) $(OBJ)/$*.o $(LIBS) -o $(BIN)/$*
$(BIN)/%: $(SRC)/%.C $(INCLUDES) $(SRC)/makefile
	$(CPP) -o $(OBJ)/$*.o -c $(SRC)/$*.C
	$(LINKCPP) $(OBJ)/$*.o $(LIBS) -o $(BIN)/$*
