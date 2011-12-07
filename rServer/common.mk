UNIX=$(shell uname)

ifeq ($(UNIX),Darwin)
ifndef ($(CUSTOM_INC))
	CUSTOM_INC=
endif
ifndef ($(CUSTOM_LIB))
	CUSTOM_LIB=
endif
endif

ifeq ($(UNIX),SunOS)
ifndef ($(CUSTOM_INC))
	CUSTOM_INC=
endif
ifndef ($(CUSTOM_LIB))
	CUSTOM_LIB=
endif
endif

export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE $(CUSTOM_INC)
export UNIXLIBS=$(CUSTOM_LIB) -lm
export UNIXMAKE=make

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
