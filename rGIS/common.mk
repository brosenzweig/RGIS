UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export MATHLIB=-lm
export NETCDFINC=
export NETCDFLIB=-ludunits -lnetcdf
export UNIXAR=ar -ru
export UNIXCPP=g++
export UNIXCPPOPS=-Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated
export UNIXMAKE=make
export UNIXRL=ranlib
export X11LIBS=-lXm -lXt -lXext -lX11
endif
ifeq ($(UNIX),Darwin)
export MATHLIB=-lm
export NETCDFINC=
export NETCDFLIB=-ludunits -lnetcdf
export UNIXAR=ar -ru
export UNIXCPP=g++
export UNIXCPPOPS=-Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated
export UNIXMAKE=make
export UNIXRL=ranlib
export X11LIBS=-lXm -lXt -lXext -lX11
endif
