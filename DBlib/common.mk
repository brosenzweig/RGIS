UNIX=$(shell uname)

ifeq ($(UNIX),Darwin)
ifndef ($(CUSTOM_INC))
	CUSTOM_INC=-I/sw/include
endif
endif

ifeq ($(UNIX),SunOS)
ifndef ($(CUSTOM_INC))
	CUSTOM_INC=-I/usr/local/netcdf/include -I/usr/local/udunits/include 
endif
endif

export UNIXAR=ar -ru
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE $(CUSTOM_INC)
export UNIXMAKE=make
export UNIXRL=ranlib
