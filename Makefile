buildir:
	mkdir -p build/

library: buildir
	gcc -c -Wall -Werror -fpic lib/serial.c -o build/serial.o
	gcc -shared -o libmc.so build/serial.o

server: library
	gcc server.c -L. -llo -lmc -o osc-server

client:
	gcc test_client.c -llo -o test_client
