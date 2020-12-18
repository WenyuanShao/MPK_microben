CC=gcc
CFLAGS=-Wall -g -fno_pie -O0
INCLUDE=-I .
LIBS=
CFILES=$(wildcard ./*.c)
OBJS=$(patsubst %.c,%.o,$(CFILES))
TARGETS=$(CFILES:%.c=%)

all: $(TARGETS)

$(TARGETS): %: %.o
	$(CC) $(INCLUDE) $(LIBS) -no-pie -o $@ $<

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	@rm $(TARGETS) $(OBJS)

.PHONY: all clean
