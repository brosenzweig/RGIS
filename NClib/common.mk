UNIX=$(shell uname)

ifeq ($(UNIX),Darwin)
ifndef ($(CUSTOM_INC))
	CUSTOM_INC=-I/sw/include
endif
ifndef ($(CUSTOM_LIB))
	CUSTOM_LIB=-L/sw/lib
endif
endif

ifeq ($(UNIX),SunOS)
ifndef ($(CUSTOM_INC))
	CUSTOM_INC=-I/usr/local/include
endif
ifndef ($(CUSTOM_LIB))
	CUSTOM_LIB=-L/usr/local/lib
endif
endif

export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-Wall -fsigned-char -D_GNU_SOURCE $(CUSTOM_INC)
export UNIXLIBS=$(CUSTOM_LIB) -lnetcdf -lm
export UNIXMAKE=make
export UNIXRL=ranlib
