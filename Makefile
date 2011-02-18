DEPENDENCIES=gtksourceview-2.0 libglade-2.0

CFLAGS=-g `pkg-config --cflags $(DEPENDENCIES)` -Werror -Wall -I.
LDFLAGS=`pkg-config --libs $(DEPENDENCIES)` -export-dynamic

PLUGINS=logo.so recursion.so

all: CLE $(PLUGINS)

CORE=CLE.o callbacks.o exercise.o lesson.o
LOGO=turtle.o world.o
EXOS=logo_threesquare.o logo_circle.o logo_star.o logo.o

EXO_HEADERS=logo/turtle.h logo/world.h lessons/logo.h

CLE: $(CORE) $(LOGO) $(EXOS)
	gcc $^ -o CLE $(LDFLAGS)
	
CLE.o: UI/CLE.c UI/CLE.h	
	$(CC) $(CFLAGS) -c $< -o $@
callbacks.o: UI/callbacks.c UI/CLE.h
	$(CC) $(CFLAGS) -c $< -o $@


turtle.o: logo/turtle.c logo/turtle.h
	$(CC) $(CFLAGS) -c $< -o $@
world.o: logo/world.c logo/world.h logo/turtle.h
	$(CC) $(CFLAGS) -c $< -o $@



exercise.o: core/exercise.c core/exercise.h
	$(CC) $(CFLAGS) -c $< -o $@
lesson.o: core/lesson.c core/lesson.h core/exercise.h
	$(CC) $(CFLAGS) -c $< -o $@

# The embeeded lesson
#####################

logo_threesquare.o: lessons/logo_threesquare.c $(EXO_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
logo_circle.o: lessons/logo_circle.c $(EXO_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
logo_star.o: lessons/logo_star.c $(EXO_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
logo.o: lessons/logo.c $(EXO_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# A first lesson plugin
#######################
logo.so: lessons/logo.c $(EXO_HEADERS) \
         lessons/logo_threesquare.c lessons/logo_circle.c lessons/logo_star.c
	gcc -shared -fPIC -o $@ $^ -I.
	 
recursion.so: lessons/recursion.c $(EXO_HEADERS) \
              lessons/rec_spiral.c lessons/rec_tree.c 
	gcc -shared -fPIC -o $@ $^ -I.


clean: 
	rm -f *.o CLE $(PLUGINS)
	
# BEGINKILL
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
# ENDKILL	