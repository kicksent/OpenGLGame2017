# gameSDL Nick Trierweiler
EXE=gameSDL

# Main target
all: $(EXE)

#  MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall
CLEAN=del *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

LIBS= -lSDL -lSDL_mixer -lGLU -lGL -lm

#Dependencies
gameSDL.o: gameSDL.c CSCIx229.h
fatal.o: fatal.c CSCIx229.h
loadtexbmp.o: loadtexbmp.c CSCIx229.h
printsdl.o: printsdl.c CSCIx229.h
project.o: project.c CSCIx229.h
errcheck.o: errcheck.c CSCIx229.h
object.o: object.c CSCIx229.h
gameSDL.o: gameSDL.c CSCIx229.h

#Create archive(allows for function shadowing)
CSCIx229.a: fatal.o loadtexbmp.o project.o errcheck.o object.o
	ar -rcs $@ $^

# Compile rules
.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

#  Link
gameSDL:gameSDL.o CSCIx229.a printsdl.o
	gcc -O3 -o $@ $^   $(LIBS)

#  Clean
clean:
	$(CLEAN)
