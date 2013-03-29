CC=gcc
CFLAGS=-I. -Wall
DEPS = 
OBJ = socket.o main.o gs105e.o shell.o shell_ip.o shell_vlan.o shell_sys.o shell_port.o

ifeq ($(OS),Windows_NT)
	CFLAGS += -D WIN32
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		CFLAGS += -D AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		CFLAGS += -D IA32
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CFLAGS += -D LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		CFLAGS += -D OSX
	endif
	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		CFLAGS += -D AMD64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		CFLAGS += -D IA32
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		CFLAGS += -D ARM
	endif
endif



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
