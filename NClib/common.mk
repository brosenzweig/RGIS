UNIX=$(shell uname)

ifeq ($(UNIX),Darwin)
ifndef ($(CUSTOM_INC))
	CUSTOM_INC=/sw/include
endif
ifndef ($(CUSTOM_LIB))
	CUSTOM_LIB=/sw/lib
endif
endif

ifeq ($(UNIX),SunOS)
ifndef ($(CUSTOM_INC))
	CUSTOM_INC=/usr/local/include
endif
ifndef ($(CUSTOM_LIB))
	CUSTOM_LIB=/usr/local/lib
endif
endif

export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-Wall -fsigned-char -D_GNU_SOURCE $(CUSTOM_INC)
export UNIXLIBS=$(CUSTOM_LIB) -lnetcdf -lm
export UNIXMAKE=make
export UNIXRL=ranlib
