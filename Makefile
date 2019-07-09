CFLAGS=-Wall -std=c11 -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

comcom: $(OBJS)
	cc -o $@ $(OBJS) $(LDFLAGS)

test:
	cc -o $@ $(OBJS) $(LDFLAGS)
	./test.sh

$(OBJS): comcom.h

clean:
	rm -f comcom *.o *~ tmp* a.out test/*~ *.s

format:
	clang-format -i *.c *.h

.PHONY: test clean format
