UNIX=$(shell uname)

export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXCPP=g++
export UNIXCPPOPS=-Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated
export UNIXLIBS=-lm
export UNIXMAKE=make

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
