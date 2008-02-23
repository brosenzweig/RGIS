UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXAR=ar -ru
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXMAKE=make
export UNIXRL=ranlib
endif
ifeq ($(UNIX),Darwin)
export UNIXAR=ar -ru
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXMAKE=make
export UNIXRL=ranlib
endif
ifeq ($(UNIX),SunOS)
export UNIXAR=ar -ru
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXMAKE=make
export UNIXRL=ranlib
endif
