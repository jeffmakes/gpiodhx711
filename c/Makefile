PROJ=gpiodhx711
CC=cc
AR=ar
SRC=src/main.c src/gpiodhx711.c
LIBS=-lgpiod
CFLAGS=-D CONSUMER=\"$(PROJ)\" 
CFLAGS+=-g3
all:
	$(CC) -c $(SRC) $(LIBS) $(CFLAGS) 
	$(AR) cr gpiodhx711.a gpiodhx711.o
	$(CC) $(LIBS) main.o gpiodhx711.a -o $(PROJ)
	rm -f *.o
clean:
	rm -f $(PROJ) *.o *.a
