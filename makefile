OBJS 	= read_functions.o list.o signal_handlers.o connection_handlers.o session_functions.o
OUT  	= dropbox_server dropbox_client test
CC		= gcc
FLAGS   = -Wall -g -c 

all: $(OBJS) dropbox_client.o dropbox_server.o test.o
	$(CC) -g $(OBJS) dropbox_server.o -o dropbox_server
	$(CC) -g $(OBJS) dropbox_client.o -o dropbox_client
	$(CC) -g $(OBJS) test.o -o test

test.o: test.c
	$(CC) $(FLAGS) test.c

dropbox_server.o: src/dropbox_server.c
	$(CC) $(FLAGS) src/dropbox_server.c

dropbox_client.o: src/dropbox_client.c
	$(CC) $(FLAGS) src/dropbox_client.c

read_functions.o: src/read_functions.c
	$(CC) $(FLAGS) src/read_functions.c

list.o: src/list.c
	$(CC) $(FLAGS) src/list.c

signal_handlers.o: src/signal_handlers.c
	$(CC) $(FLAGS) src/signal_handlers.c

connection_handlers.o: src/connection_handlers.c
	$(CC) $(FLAGS) src/connection_handlers.c

session_functions.o: src/session_functions.c
	$(CC) $(FLAGS) src/session_functions.c

clean:
	rm -f $(OBJS) $(OUT) dropbox_client.o dropbox_server.o test.o