OBJS 	= read_functions.o list.o connection_handlers.o session_functions.o circular_buffer.o signal_handlers.o pathlist.o send_functions.o thread_functions.o parsing_functions.o dir_functions.o
OUT  	= dropbox_server dropbox_client
CC		= gcc
FLAGS   = -Wall -g -c 

all: $(OBJS) dropbox_client.o dropbox_server.o
	$(CC) -g $(OBJS) -pthread dropbox_server.o -o dropbox_server
	$(CC) -g $(OBJS) -pthread dropbox_client.o -o dropbox_client

dropbox_server.o: src/dropbox_server.c
	$(CC) $(FLAGS) src/dropbox_server.c

dropbox_client.o: src/dropbox_client.c
	$(CC) $(FLAGS) src/dropbox_client.c

read_functions.o: src/read_functions.c
	$(CC) $(FLAGS) src/read_functions.c

list.o: src/list.c
	$(CC) $(FLAGS) src/list.c

pathlist.o: src/pathlist.c
	$(CC) $(FLAGS) src/pathlist.c

connection_handlers.o: src/connection_handlers.c
	$(CC) $(FLAGS) src/connection_handlers.c

session_functions.o: src/session_functions.c
	$(CC) $(FLAGS) src/session_functions.c

circular_buffer.o: src/circular_buffer.c
	$(CC) $(FLAGS) src/circular_buffer.c

signal_handlers.o: src/signal_handlers.c
	$(CC) $(FLAGS) src/signal_handlers.c

send_functions.o: src/send_functions.c
	$(CC) $(FLAGS) src/send_functions.c

thread_functions.o: src/thread_functions.c
	$(CC) -pthread $(FLAGS) src/thread_functions.c

parsing_functions.o: src/parsing_functions.c
	$(CC) $(FLAGS) src/parsing_functions.c

dir_functions.o: src/dir_functions.c
	$(CC) $(FLAGS) src/dir_functions.c

clean:
	rm -f $(OBJS) $(OUT) dropbox_client.o dropbox_server.o