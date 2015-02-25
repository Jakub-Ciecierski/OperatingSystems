#---Makefile--
all: server1 client1 client2 client3 server2 server3
# stage 1
server1: server1.o
	gcc -o server1 server1.c
# stage 2
client1: client1.o io.o
	gcc -o client1 client1.o io.o
client1.o: client1.c io.h
	gcc -Wall -c -o client1.o client1.c
io.o: io.c io.h
	gcc -Wall -c -o io.o io.c
# stage 3
client2: client2.o io.o
	gcc -o client2 client2.o io.o
client2.o: client2.c io.h packet.h
	gcc -Wall -c -o client2.o client2.c
server2: server2.o io.o
	gcc -o server2 server2.o io.o
server2.o: server2.c io.h packet.h
	gcc -Wall -c -o server2.o server2.c
# stage4
client3: client3.o io.o
	gcc -o client3 client3.o io.o
client3.o: client3.c io.h packet.h
	gcc -Wall -c -o client3.o client3.c
server3: server3.o io.o	
	gcc -Wall -o server3 server3.c io.o
server3.o: server3.c io.h packet.h
	gcc -c -o server3.o server3.c

.PHONY: clean
clean:
	rm server1 server2 server3 client1 client2 client3 *.o
