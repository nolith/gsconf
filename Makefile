CC=gcc
CFLAGS=-I. -Wall
DEPS = 
OBJ = socket.o main.o gs105e.o shell.o shell_ip.o shell_vlan.o shell_sys.o shell_port.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

gsconfig: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

gs105e.o: gs105e.c gs105e.h socket.h

main.o: main.c gs105e.h socket.h shell.h

shell_vlan.o: shell_vlan.c gs105e.h socket.h shell.h

clean:
	rm *.o -rf
	rm gsconfig -rf
