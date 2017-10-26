#compiler
COMPILER=gcc

#Version C
CVERSION=-std=c99

#-Wall flag -> compiler display all warnings
#-Werror -> compiler convert warnings to error
FLAG=-Wall -Werror

#declaring librairies
LIBRARIES=-lpthread

#Version
VERSION=0

#Name of the program
OUT=projet

#how to generate the "main.o" file
main.o: main.c trains/train.h
	$(COMPILER) $(CVERSION) -c main.c -o main.o

#how to generate the "train.o" file
train.o: trains/train.c
	$(COMPILER) $(CVERSION) -c trains/train.c -o train.o

#how to generate the executable
all: main.o train.o
	$(COMPILER) $(CVERSION) $(FLAG) main.o train.o $(LIBRARIES) -o $(OUT)

#Generate directory version
dist: clean main.c trains/train.c trains/train.h
	mkdir $(OUT)_$(VERSION)
	mkdir $(OUT)_$(VERSION)/trains
	cp Makefile $(OUT)_$(VERSION)/Makefile
	cp main.c $(OUT)_$(VERSION)/main.c
	cp -r trains $(OUT)_$(VERSION)
	cp -r documentation $(OUT)_$(VERSION)

#Make the archive of the entire project sources
tgz: dist
	tar -cvzf $(OUT)_$(VERSION).tgz $(OUT)_$(VERSION)
#Generate the documentation of the project
doc: documentation/Doxyfile
	cd documentation && doxygen Doxyfile && cd ..

#Remove not necessary file
clean:
	rm -f *.o
	rm -f $(OUT)
	rm -f -R $(OUT)_*
	rm -rf documentation/latex
	rm -rf documentation/html
