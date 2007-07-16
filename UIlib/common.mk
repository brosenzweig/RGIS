UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXAR=ar -ru
export UNIXCPP=g++
export UNIXCPPOPS=-Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated
export UNIXMAKE=make
export UNIXRL=ranlib
export X11INC=/usr/X11R6/include
endif
ifeq ($(UNIX),Darwin)
export UNIXAR=ar -ru
export UNIXCPP=g++
export UNIXCPPOPS=-Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated
export UNIXMAKE=make
export UNIXRL=ranlib
export X11INC=/usr/X11R6/include
endif
