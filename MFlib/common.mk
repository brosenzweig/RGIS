UNIX=$(shell uname)

export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -pthread -fsigned-char -D_GNU_SOURCE
export UNIXMAKE=make
export UNIXRL=ranlib
