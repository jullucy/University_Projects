CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=gnu17
LDFLAGS =

TARGETS = peer-time-sync

.PHONY: all clean

all: $(TARGETS)

peer-time-sync: peer-time-sync.o err.o common.o messages.o

peer-time-sync.o: peer-time-sync.c err.h common.h messages.h
messages.o: messages.c messages.h err.h common.h
common.o: common.c err.h common.h
err.o: err.c err.h

clean:
	rm -f $(TARGETS) *.o