all: ish
clobber: clean
	rm -f *~ \#*\# core
clean:
	rm -f ish *.o

ish: ish.o dynarray.o
	gcc209 ish.o dynarray.o -o ish

dynarray.o: dynarray.c dynarray.h
	gcc209 -D_BSD_SOURCE -D_GNU_SOURCE -c dynarray.c

ish.o: ish.c dynarray.h
	gcc209 -D_BSD_SOURCE -D_GNU_SOURCE -c ish.c

