UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-ludunits2 -lnetcdf -lXm -lXt -lXext -lX11 -lm
export UNIXMAKE=make
endif
ifeq ($(UNIX),Darwin)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -I/sw/include
export UNIXLIBS=-L/usr/X11R6/lib -L/sw/lib -lXm -lXt -lXext -lX11 -lnetcdf -ludunits2
export UNIXMAKE=make
endif
ifeq ($(UNIX),SunOS)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-L/usr/local/netcdf/lib -L/usr/local/udunits/lib -lXm -lXt -lXext -lX11 -ludunits2 -lnetcdf
export UNIXMAKE=make
endif

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
