UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXCPP=g++
export UNIXCPPOPS=-Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated
export UNIXMAKE=make
export UNIXLIBS=-ludunits -lnetcdf -lcpgplot -lpgplot -lpng -lm
endif
ifeq ($(UNIX),Darwin)
export UNIXCPP=g++
export UNIXCPPOPS=-Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated -I/sw/lib/pgplot
export UNIXMAKE=make
export UNIXLIBS=-L/sw/lib -ludunits -lnetcdf -L/sw/lib/pgplot -lcpgplot -lpgplot -lpng -L/usr/X11R6/lib -lX11 -lg95 -lm
endif
