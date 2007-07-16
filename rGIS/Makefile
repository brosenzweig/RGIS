include ./common.mk
OBJ = ./obj
BIN = ./bin

all: $(OBJ) $(BIN) source

clean:
	$(UNIXMAKE) -C src clean

source:
	$(UNIXMAKE) -C src all

$(OBJ):
	mkdir -p $(OBJ)
$(BIN):
	mkdir -p $(BIN)
