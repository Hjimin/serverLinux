CC = gcc
CFLAGS =  -Wall -g -O2
INCLUDES = -Ilwip/src/include/ipv4 -Ilwip/src/include 
LFLAGS = -Llwip/ports/unix/proj/lib/liblwip.so

all: main
main: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) main.c -o main $(OBJS) $(LFLAGS) -llwip -lpthread 
clean :
	rm -f *.o
