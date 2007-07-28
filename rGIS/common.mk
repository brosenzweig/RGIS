UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXCPP=g++
export UNIXCPPOPS=-Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated
export UNIXMAKE=make
export UNIXLIBS=-ludunits -lnetcdf -lXm -lXt -lXext -lX11 -lm
endif
ifeq ($(UNIX),Darwin)
export UNIXCPP=g++
export UNIXCPPOPS=-Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated -I/opt/local/include
export UNIXMAKE=make
export UNIXLIBS=-L/usr/X11R6/lib -L/opt/local/lib -lXm -lXt -lXext -lX11 -ludunits -lnetcdf
endif
