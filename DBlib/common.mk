UNIX=$(shell uname)


ifeq ($(UNIX),Linux)
export UNIXAR=ar -ru
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -I/usr/local/include
export UNIXMAKE=make
export UNIXRL=ranlib
endif
ifeq ($(UNIX),Darwin)
export UNIXAR=ar -ru
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -I/sw/include
export UNIXMAKE=make
export UNIXRL=ranlib
endif
ifeq ($(UNIX),SunOS)
export UNIXAR=ar -ru
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -I/usr/local/netcdf/include -I/usr/local/udunits/include 
export UNIXMAKE=make
export UNIXRL=ranlib
endif