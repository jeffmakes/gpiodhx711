PROJ=libhx711
CC=cc
AR=ar
SRC=src/main.c src/libhx711.c
LIBS=-lgpiod
CFLAGS=-D CONSUMER=\"$(PROJ)\" 
CFLAGS+=-g3
all:
	$(CC) -c $(SRC) $(LIBS) $(CFLAGS) 
	$(AR) cr libhx711.a libhx711.o
	$(CC) $(LIBS) main.o libhx711.a -o $(PROJ)
	rm -f *.o
clean:
	rm -f $(PROJ) *.o *.a
