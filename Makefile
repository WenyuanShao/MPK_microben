CC=gcc
CFLAGS=-Wall -g
INCLUDE=-I .
LIBS=
CFILES=$(wildcard ./*.c)
OBJS=$(patsubst %.c,%.o,$(CFILES))
TARGETS=$(CFILES:%.c=%)

all: $(TARGETS)

$(TARGETs): %: %.o
	$(CC) $(INCLUDE) $(LIBS) -o $@ $<

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	@rm $(TARGETS) $(OBJS)

.PHONY: all clean
