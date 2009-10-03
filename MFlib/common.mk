UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -pthread -fsigned-char -D_GNU_SOURCE
export UNIXMAKE=make
export UNIXRL=ranlib
endif
ifeq ($(UNIX),Darwin)
export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -pthread -fsigned-char -D_GNU_SOURCE
export UNIXMAKE=make
export UNIXRL=ranlib
endif
ifeq ($(UNIX),SunOS)
export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -pthread -fsigned-char -D_GNU_SOURCE
export UNIXMAKE=gmake
export UNIXRL=ranlib
endif
