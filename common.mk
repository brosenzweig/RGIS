UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXMAKE=make
endif
ifeq ($(UNIX),Darwin)
export UNIXMAKE=make
endif
