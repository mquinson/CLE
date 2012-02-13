DEPENDENCIES=gtksourceview-2.0 libglade-2.0

CFLAGS=-g `pkg-config --cflags $(DEPENDENCIES)` -Werror -Wall -I.
LDFLAGS=`pkg-config --libs $(DEPENDENCIES)` -export-dynamic -ldl -lm

PLUGINS=logo.so

all: CLE $(PLUGINS)

CORE=CLE.o callbacks.o exercise.o lesson.o

LOGO_EXO_HEADERS=logo/entity.h logo/world.h logo/logo.h logo/exercise.h logo/exercise_header.h \
		logo/entity_userside.h logo/world_view.h

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
#######################
logo.so: logo/logo.c $(LOGO_EXO_HEADERS) \
         logo/logo_threesquare.c logo/logo_forward.c logo/world.c logo/entity.c logo/exercise.c logo/logo_square.c
	gcc -shared -fPIC $(CFLAGS) -o $@ $^ -I.


clean: 
	rm -f *.o CLE $(PLUGINS)
	

logo/turtle_userside.h: logo/turtle_userside.c
	echo "/* This file was automatically generated from logo/turtle_userside.c */" > $@
	echo "/* DO NOT EDIT */" >> $@
	echo >> $@
	echo "#ifndef USERSIDE" >> $@
	echo "#define USERSIDE" >> $@
	echo "static char *userside = " >> $@
	./stringify.pl < logo/turtle_userside.c >> $@
	echo ";" >> $@
	echo "#endif /* USERSIDE */" >> $@
	echo "/* This file was automatically generated from logo/turtle_userside.c */" >> $@
	echo "/* DO NOT EDIT */" >> $@
	
template: CLE-0.0.tar.gz
CLE-0.0.tar.gz: CLE projet-1011-CLE.pdf Makefile CLE.glade $(shell find -name '*.[ch]')
	rm -rf CLE-0.0
	mkdir CLE-0.0
#	cp projet-1011-CLE.pdf CLE-0.0
	cp CLE CLE-0.0/CLE-demo
	strip -s CLE-0.0/CLE-demo
	cp CLE.glade CLE-0.0
	for file in Makefile `find -name '*.[ch]' |sed 's/.\///'` ; do \
	  echo "Filtering $$file"; \
	  mkdir -p CLE-0.0/`dirname $$file`; \
	  ./killparts < $$file > CLE-0.0/$$file;\
	done
	tar cfz CLE-0.0.tar.gz CLE-0.0
	