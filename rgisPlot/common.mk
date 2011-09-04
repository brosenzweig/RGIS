UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXLIBS=-lcpgplot -lpgplot -lpng -lm
endif

ifeq ($(UNIX),Darwin)
ifndef ($(CUSTOM_INC)
	CUSTOM_INC=-I/sw/include -I/sw/lib/pgplot
endif
ifndef ($(CUSTOM_LIB)
	CUSTOM_LIB=-L/sw/lib -L/sw/lib/pgplot -L/sw/lib/gcc4.6/lib -L/usr/X11R6/lib
endif
export UNIXLIBS=$(CUSTOM_LIB) -Wl,-framework -Wl,Foundation -lcpgplot -lpgplot -lgfortran -lz -laquaterm -lpng -lX11 -lm
endif

ifeq ($(UNIX),SunOS)
export UNIXLIBS=-lcpgplot -lpgplot -lgfortran -lX11 -lm
endif

export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE $(CUSTOM_INC)
export UNIXMAKE=make

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
