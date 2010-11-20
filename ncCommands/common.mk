UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-ludunits2 -lnetcdf -lm
export UNIXMAKE=make
endif
ifeq ($(UNIX),Darwin)
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -I/sw/include
export UNIXLIBS=-L/sw/lib -ludunits2 -lnetcdf -lm
export UNIXMAKE=make
endif
ifeq ($(UNIX),SunOS)
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-L/usr/local/netcdf/lib -L/usr/local/udunits/lib -ludunits2 -lnetcdf -lm
export UNIXMAKE=make
endif

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
