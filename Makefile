
SHELL=/bin/sh
CC = gcc
LIBS = -lm -lgcc
OBJECTS = main.o astro.o sol.o

solunar : $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o solunar

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean :
	rm *.o solunar
	

