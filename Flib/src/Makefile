include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
LIB    = ../lib
CC     = $(UNIXCC) $(UNIXCCOPS) -I$(INC) -I../../CMlib/include
LINK   = $(UNIXCC)
TARGET = $(LIB)/libF10.a

INCLUDES = $(INC)/Flib.h

OBJS = $(OBJ)/FData.o\
       $(OBJ)/FTableRead.o\
       $(OBJ)/Flib.o

all: $(TARGET)

clean:
	rm $(TARGET) $(OBJS)

$(TARGET): $(OBJS) $(SRC)/Makefile
	$(UNIXAR) $(TARGET) $(OBJS)
	$(UNIXRL) $(TARGET)

$(OBJ)/%.o: $(SRC)/%.c $(INCLUDES)
	$(CC) -o $(OBJ)/$*.o -c $(SRC)/$*.c
