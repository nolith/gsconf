CC=gcc
CFLAGS=-I.
DEPS = 
OBJ = socket.o main.o gs105e.o shell.o shell_ip.o shell_vlan.o shell_sys.o shell_port.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

gsconfig: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm *.o -rf
	rm gsconfig -rf
