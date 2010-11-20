UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-Wall -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-lnetcdf -lm
export UNIXMAKE=make
export UNIXRL=ranlib
endif

ifeq ($(UNIX),Darwin)
export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-Wall -fsigned-char -D_GNU_SOURCE -I/sw/include
export UNIXLIBS=-L/sw/lib -lnetcdf -lm
export UNIXMAKE=make
export UNIXRL=ranlib
endif

ifeq ($(UNIX),SunOS)
export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -I/usr/local/include
export UNIXLIBS=-L/usr/local/lib -lnetcdf -lm
export UNIXMAKE=make
export UNIXRL=ranlib
endif
