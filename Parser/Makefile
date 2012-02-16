CC = gcc
CFLAGS = -Wall -g
LDFLAGS =
EXEC=parser

all: $(EXEC)

parser: parser.o baliseProcess.o generate.o parserUtils.o structUtility.o structUtility.o
	$(CC) $^ -o parser $(LDFLAGS)

parser.o: parser.c parser.h baliseProcess.h generate.h parserUtils.h structUtility.h
	$(CC) -o $@ -c $< $(CFLAGS)

baliseProcess.o: baliseProcess.c baliseProcess.h parser.h structUtility.h
	$(CC) -o $@ -c $< $(CFLAGS)

generate.o: generate.c generate.h parser.h structUtility.h
	$(CC) -o $@ -c $< $(CFLAGS)

parserUtils.o: parserUtils.c parserUtils.h parser.h structUtility.h
	$(CC) -o $@ -c $< $(CFLAGS)

structUtility.o: structUtility.c structUtility.h parser.h
	$(CC) -o $@ -c $< $(CFLAGS)

clean: 
	rm -f *.o parser