UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXMAKE=make
export UNIXLIBS=-lcpgplot -lpgplot -lpng -lm
endif
ifeq ($(UNIX),Darwin)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -I/sw/include -I/sw/lib/pgplot
export UNIXMAKE=make
export UNIXLIBS=-L/sw/lib -L/sw/lib/pgplot -L/sw/lib/gcc4.3/lib -L/usr/X11R6/lib -Wl,-framework -Wl,Foundation -lcpgplot -lpgplot -lgfortran -lz -laquaterm -lpng -lX11 -lm
endif
ifeq ($(UNIX),SunOS)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXMAKE=make
export UNIXLIBS=-lcpgplot -lpgplot -lgfortran -lX11 -lm
endif

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
