OBJECTS = functions.o server.o host.o

all: $(OBJECTS)
	cc $(OBJECTS) -o serve

server.o: server.c
	cc -g -c server.c

functions.o: functions.c
	cc -g -c functions.c

host.o: host.c
	cc -g -c host.c