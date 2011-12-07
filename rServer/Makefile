include ./common.mk
OBJ = ./obj
BIN = ./bin

all: $(OBJ) $(BIN) source

install:
	$(UNIXMAKE) -C src install

uninstall:
	$(UNIXMAKE) -C src uninstall

clean:
	$(UNIXMAKE) -C src clean

source:
	$(UNIXMAKE) -C src all

$(OBJ):
	mkdir -p $(OBJ)
$(BIN):
	mkdir -p $(BIN)
