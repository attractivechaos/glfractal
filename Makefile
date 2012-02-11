CC=			gcc
CFLAGS=		-g -Wall -O2
DFLAGS=
PROG=		glfractal glslfractal glslfractale
LIBS=		-framework GLUT -framework OpenGL  # for Mac OS X

.SUFFIXES:.c .o

.c.o:
		$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) $< -o $@

all:$(PROG)

glfractal:glfractal.c
		gcc -Wall -O2 $< -o $@ $(LIBS)

glslfractal:glslfractal.c
		gcc -Wall -O2 $< -o $@ $(LIBS)

glslfractale:glslfractale.c
		gcc -Wall -O2 $< -o $@ $(LIBS)

clean:
		rm -fr gmon.out *.o a.out $(PROG) *~ *.a *.dSYM
