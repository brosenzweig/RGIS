UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXCPP=g++
export UNIXCPPOPS=-Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated
export UNIXMAKE=make
export UNIXLIBS=-ludunits -lnetcdf -lcpgplot -lpgplot -lpng -lm
endif
ifeq ($(UNIX),Darwin)
export UNIXCPP=g++
export UNIXCPPOPS=-Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated -I/opt/local/include
export UNIXMAKE=make
export UNIXLIBS=-L/opt/local/lib -ludunits -lnetcdf -lcpgplot -lpgplot -lpng -L/usr/X11R6/lib -lX11 -lm
endif

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif