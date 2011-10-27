all:spectranet
spectranet: main.c spectranet.o tcp.o
	cc main.c spectranet.o tcp.o -o spectranet
spectranet.o: spectranet.h spectranet.c
	cc -c spectranet.c -o spectranet.o
tcp.o: tcp.c tcp.h
	cc -c tcp.c -o tcp.o
clean:
	rm -f *.o spectranet
