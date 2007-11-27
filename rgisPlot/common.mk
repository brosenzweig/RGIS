UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated
export UNIXMAKE=make
export UNIXLIBS=-ludunits -lnetcdf -lcpgplot -lpgplot -lpng -lm
endif
ifeq ($(UNIX),Darwin)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated -I/opt/local/include -I/usr/local/pgplot
export UNIXMAKE=make
export UNIXLIBS=-L/opt/local/lib -L/usr/local/pgplot -L/opt/local/lib/gcc42 -ludunits -lnetcdf -lcpgplot -lpgplot -lgfortran -L/usr/X11R6/lib -lX11 -lm
endif
ifeq ($(UNIX),SunOS)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated
export UNIXMAKE=make
export UNIXLIBS=-lcpgplot -lpgplot -lgfortran -lX11 -lm
endif

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
