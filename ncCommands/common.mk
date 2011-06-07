UNIX=$(shell uname)

ifeq ($(UNIX),Darwin)
ifndef ($(CUSTOM_INC))
	CUSTOM_INC=-I/sw/include
endif
ifndef ($(CUSTOM_LIB))
	CUSTOM_LIB=-L/sw/lib
endif
endif

ifeq ($(UNIX),SunOS)
ifndef ($(CUSTOM_LIB))
	CUSTOM_LIB=-L/usr/local/netcdf/lib -L/usr/local/udunits/lib
endif
endif

export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -fsigned-char -D_GNU_SOURCE $(CUSTOM_INC)
export UNIXLIBS=$(CUSTOM_LIB) -ludunits2 -lnetcdf -lm
export UNIXMAKE=make

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
