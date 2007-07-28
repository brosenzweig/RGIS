UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXAR=ar -ru
export UNIXCPP=g++
export UNIXCPPOPS=-Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecateds
export UNIXMAKE=make
export UNIXRL=ranlib
endif
ifeq ($(UNIX),Darwin)
export UNIXAR=ar -ru
export UNIXCPP=g++
export UNIXCPPOPS=-Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated -I/opt/local/include
export UNIXMAKE=make
export UNIXRL=ranlib
endif
