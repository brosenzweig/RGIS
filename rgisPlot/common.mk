UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-lcpgplot -lpgplot -lpng -lm
export UNIXMAKE=make
endif
ifeq ($(UNIX),Darwin)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -I/sw/include -I/sw/lib/pgplot
export UNIXLIBS=-L/sw/lib -L/sw/lib/pgplot -L/sw/lib/gcc4.4/lib -L/usr/X11R6/lib -Wl,-framework -Wl,Foundation -lcpgplot -lpgplot -lgfortran -lz -laquaterm -lpng -lX11 -lm
export UNIXMAKE=make
endif
ifeq ($(UNIX),SunOS)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-lcpgplot -lpgplot -lgfortran -lX11 -lm
export UNIXMAKE=make
endif

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
