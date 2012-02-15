DEPENDENCIES=gtksourceview-2.0 libglade-2.0

CFLAGS=-g `pkg-config --cflags $(DEPENDENCIES)` -Werror -Wall -I.
LDFLAGS=`pkg-config --libs $(DEPENDENCIES)` -export-dynamic -ldl -lm

CC = gcc
PLUGINS=logo.so fork.so

LOGO_EXO_SOURCE = $(wildcard logo/logo_*.c)
LOGO_CORE_SOURCE = logo/world.c logo/entity.c logo/exercise.c

FORK_EXO_SOURCE = $(wildcard fork/fork_*.c)
FORK_CORE_SOURCE = fork/teststrace.c fork/world.c fork/entity.c fork/exercise.c

all: CLE $(PLUGINS)

CORE=CLE.o callbacks.o exercise.o lesson.o

LOGO_HEADERS=logo/entity.h logo/world.h logo/logo.h logo/exercise.h logo/exercise_header.h \
		logo/entity_userside.h logo/world_view.h 
		
FORK_HEADERS=fork/entity.h fork/world.h fork/fork.h fork/exercise.h fork/teststrace.h  fork/exercise_header.h\
		fork/entity_userside.h fork/world_view.h fork/teststrace.h

CLE: $(CORE)
	gcc $^ -o CLE $(LDFLAGS)
	
CLE.o: UI/CLE.c UI/CLE.h	
	$(CC) $(CFLAGS) -c $< -o $@
callbacks.o: UI/callbacks.c UI/CLE.h
	$(CC) $(CFLAGS) -c $< -o $@


exercise.o: core/exercise.c core/exercise.h
	$(CC) $(CFLAGS) -c $< -o $@
lesson.o: core/lesson.c core/lesson.h core/exercise.h
	$(CC) $(CFLAGS) -c $< -o $@


# A first lesson plugin
########################$(LOGO_EXO_SOURCE)
logo.so: logo/logo.c $(LOGO_HEADERS) $(LOGO_CORE_SOURCE) $(LOGO_EXO_SOURCE)
	gcc -shared -fPIC $(CFLAGS) -o $@ $^ -I.
	
fork.so: fork/fork.c $(FORK_HEADERS) $(FORK_CORE_SOURCE) $(FORK_EXO_SOURCE)
	gcc -shared -fPIC $(CFLAGS) -o $@ $^ -I.


clean: 
	rm -f *.o CLE $(PLUGINS)
	

logo/entity_userside.h: logo/entity_userside.c
	echo "/* This file was automatically generated from logo/turtle_userside.c */" > $@
	echo "/* DO NOT EDIT */" >> $@
	echo >> $@
	echo "#ifndef LOGO_ENTITY_USERSIDE" >> $@
	echo "#define LOGO_ENTITY_USERSIDE" >> $@
	echo "static char *userside = " >> $@
	./stringify.pl < logo/entity_userside.c >> $@
	echo ";" >> $@
	echo "#endif /* LOGO_ENTITY_USERSIDE */" >> $@
	echo "/* This file was automatically generated from logo/turtle_userside.c */" >> $@
	echo "/* DO NOT EDIT */" >> $@
	
fork/entity_userside.h: fork/entity_userside.c
	echo "/* This file was automatically generated from logo/turtle_userside.c */" > $@
	echo "/* DO NOT EDIT */" >> $@
	echo >> $@
	echo "#ifndef FORK_ENTITY_USERSIDE" >> $@
	echo "#define FORK_ENTITY_USERSIDE" >> $@
	echo "static char *userside = " >> $@
	./stringify.pl < fork/entity_userside.c >> $@
	echo ";" >> $@
	echo "#endif /* FORK_ENTITY_USERSIDE */" >> $@
	echo "/* This file was automatically generated from logo/turtle_userside.c */" >> $@
	echo "/* DO NOT EDIT */" >> $@
	
# create fifo
fifo :
	mkdir res
	mkfifo res/lala
