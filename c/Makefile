PROJ=gpiodhx711
CC=cc
AR=ar
SRC=src/main.c src/gpiodhx711.c
LIBS=-lgpiod
CFLAGS=-D CONSUMER=\"$(PROJ)\" 
CFLAGS+=-g3 -fPIC
all:
	$(CC) -c $(SRC) $(LIBS) $(CFLAGS) 
	$(AR) cr libgpiodhx711.a gpiodhx711.o
	$(CC) main.o libgpiodhx711.a $(LIBS) -o $(PROJ)
	mkdir -p build
	mv $(PROJ) *.a *.o build
clean:
	rm -rf build
