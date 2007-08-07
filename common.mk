UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXMAKE=make
endif
ifeq ($(UNIX),Darwin)
export UNIXMAKE=make
endif

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif