all : testheapmgrgnu testheapmgrkr testheapmgrbase testheapmgr1 testheapmgr2
clobber: clean
	rm -f *~ \#*\# core
clean:
	rm -f testheapmgrgnu testheapmgrkr testheapmgrbase testheapmgr1 testheapmgr2 *.o

testheapmgrgnu: testheapmgr.o heapmgrgnu.o
	gcc209 -O3 -D NDEBUG -std=gnu99 testheapmgr.o heapmgrgnu.o -o testheapmgrgnu

testheapmgrkr: testheapmgr.o heapmgrkr.o
	gcc209 -O3 -D NDEBUG -std=gnu99 testheapmgr.o heapmgrkr.o -o testheapmgrkr

testheapmgrbase: testheapmgr.o heapmgrbase.o chunkbase.o
	gcc209 -O3 -D NDEBUG -std=gnu99 testheapmgr.o heapmgrbase.o chunkbase.o -o testheapmgrbase

testheapmgr1: testheapmgr.o heapmgr1.o chunk.o
	gcc209 -O3 -D NDEBUG -std=gnu99 testheapmgr.o heapmgr1.o chunk.o -o testheapmgr1

testheapmgr2: testheapmgr.o heapmgr2.o chunk.o
	gcc209 -O3 -D NDEBUG -std=gnu99 testheapmgr.o heapmgr2.o chunk.o -o testheapmgr2

testheapmgr.o: testheapmgr.c heapmgr.h
	gcc209 -O3 -D NDEBUG -std=gnu99 -c testheapmgr.c

heapmgrgnu.o: heapmgrgnu.c heapmgr.h
	gcc209 -O3 -D NDEBUG -std=gnu99 -c heapmgrgnu.c

heapmgrkr.o: heapmgrkr.c heapmgr.h
	gcc209 -O3 -D NDEBUG -std=gnu99 -c heapmgrkr.c

heapmgrbase.o: heapmgrbase.c heapmgr.h
	gcc209 -O3 -D NDEBUG -std=gnu99 -c heapmgrbase.c

chunkbase.o: chunkbase.c chunkbase.h
	gcc209 -O3 -D NDEBUG -std=gnu99 -c chunkbase.c

heapmgr1.o: heapmgr1.c heapmgr.h
	gcc209 -O3 -D NDEBUG -std=gnu99 -c heapmgr1.c

heapmgr2.o: heapmgr2.c heapmgr.h
	gcc209 -O3 -D NDEBUG -std=gnu99 -c heapmgr2.c

chunk.o: chunk.c chunk.h
	gcc209 -O3 -D NDEBUG -std=gnu99 -c chunk.c

