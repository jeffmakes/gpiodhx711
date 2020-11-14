PROJ=libgpiod-hx711
CC=cc
SRC=main.c hx711.c
LIBS=-lgpiod
CFLAGS=-D CONSUMER=\"$(PROJ)\" 
CFLAGS+=-g3
all:
	$(CC) $(SRC) $(LIBS) $(CFLAGS) -o $(PROJ)

clean:
	rm $(PROJ)
