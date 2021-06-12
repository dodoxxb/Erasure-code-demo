
CC = gcc  
CFLAGS = -g -I$(HOME)/include

ALL =	decoder \
		encoder \
		pupdater \
		updater \
		test_decoder


all: $(ALL)

clean:
	rm -f core *.o $(ALL) a.out cauchy.h cauchy.c liberation.h liberation.c reed_sol.c reed_sol.h\
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

decoder.o: galois.h jerasure.h cauchy.h
decoder: decoder.o galois.o jerasure.o cauchy.o
	$(CC) $(CFLAGS) -o decoder decoder.o jerasure.o galois.o cauchy.o

encoder.o: galois.h jerasure.h cauchy.h
encoder: encoder.o galois.o jerasure.o cauchy.o
	$(CC) $(CFLAGS) -o encoder encoder.o jerasure.o galois.o cauchy.o

pupdater.o: galois.h jerasure.h cauchy.h
pupdater: pupdater.o galois.o jerasure.o cauchy.o
	$(CC) $(CFLAGS) -o pupdater pupdater.o jerasure.o galois.o cauchy.o

updater.o: galois.h jerasure.h cauchy.h
updater: updater.o galois.o jerasure.o cauchy.o
	$(CC) $(CFLAGS) -o updater updater.o jerasure.o galois.o cauchy.o

test_decoder.o: galois.h jerasure.h cauchy.h
test_decoder: test_decoder.o galois.o jerasure.o cauchy.o
	$(CC) $(CFLAGS) -o test_decoder test_decoder.o jerasure.o galois.o cauchy.o