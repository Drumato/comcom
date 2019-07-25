CFLAGS=-Wall -std=c11 -g
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)

comcom: $(OBJS)
	gcc -o $@ $(OBJS) $(LDFLAGS)

test:
	gcc -o $@ $(OBJS) $(LDFLAGS)
	./test.sh

$(OBJS): src/comcom.h

clean:
	rm -f core comcom *.o *.txt *~ tmp* a.out test/*~ *.s

format:
	clang-format -i *.c *.h

.PHONY: test clean format
