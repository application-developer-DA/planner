.PHONY: all clean

CC = i586-mingw32msvc-gcc

CFLAGS += -g -O2 -std=c99 -pedantic -pedantic-errors

OBJS = main.o planner.o scheduler.o sysdep.o

all: main.exe
clean:
	-rm $(OBJS)

main.exe : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
