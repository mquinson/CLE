CC = gcc
CFLAGS = -Wall -g
LDFLAGS =
EXEC=parser

all: $(EXEC)

parser: parser.o baliseProcess.o generate.o
	$(CC) $^ -o parser $(LDFLAGS)

parser.o: parser.c parser.h baliseProcess.h generate.h
	$(CC) -o $@ -c $< $(CFLAGS)

baliseProcess.o: baliseProcess.c baliseProcess.h parser.h
	$(CC) -o $@ -c $< $(CFLAGS)

generate.o: generate.c generate.h parser.h
	$(CC) -o $@ -c $< $(CFLAGS)

clean: 
	rm -f *.o parser