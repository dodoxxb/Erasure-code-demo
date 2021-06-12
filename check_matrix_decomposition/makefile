
CC = gcc  
CFLAGS = -g -I$(HOME)/include

ALL =	test_time

all: $(ALL)

clean:
	rm -f core *.o $(ALL) a.out cauchy.h cauchy.c \
              jerasure.c jerasure.h galois.c galois.h

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $*.c

cauchy.h: cauchy.h
	rm -f cauchy.h ; cp cauchy.h . ; chmod 0444 cauchy.h

cauchy.c: cauchy.c
	rm -f cauchy.c ; cp cauchy.c . ; chmod 0444 cauchy.c

jerasure.h: jerasure.h
	rm -f jerasure.h ; cp jerasure.h . ; chmod 0444 jerasure.h

jerasure.c: jerasure.c
	rm -f jerasure.c ; cp jerasure.c . ; chmod 0444 jerasure.c

galois.h: galois.h
	rm -f galois.h ; cp galois.h . ; chmod 0444 galois.h

galois.c: galois.c
	rm -f galois.c ; cp galois.c . ; chmod 0444 galois.c

galois.o: galois.h
jerasure.o: jerasure.h galois.h
cauchy.o: jerasure.h galois.h cauchy.h

test_time.o: galois.h jerasure.h cauchy.h
test_time: test_time.o galois.o jerasure.o cauchy.o
	$(CC) $(CFLAGS) -o test_time test_time.o galois.o jerasure.o cauchy.o

