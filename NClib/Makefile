include ./common.mk
OBJ = ./obj
LIB = ./lib

all: $(OBJ) $(LIB) source

clean:
	$(UNIXMAKE) -C src clean

source:
	$(UNIXMAKE) -C src all

$(OBJ):
	mkdir -p $(OBJ)
$(LIB):
	mkdir -p $(LIB)
