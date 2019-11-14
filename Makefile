CC = gcc
CFLAGS = -g -Wall -O0 -std=c99
LDFLAGS = -lm
SOURCES = shell.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = baltzSHELL

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	@rm -f $(tARGET) $(OBJECTS) core
