OBJS 	= dropbox_server.o read_functions.o list.o signal_handlers.o
# OBJS 	= test.o read_functions.o list.o
OUT  	= dropbox_server
# OUT  	= test
CC		= gcc
FLAGS   = -Wall -g -c 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

# test.o: test.c
# 	$(CC) $(FLAGS) test.c

dropbox_server.o: src/dropbox_server.c
	$(CC) $(FLAGS) src/dropbox_server.c

read_functions.o: src/read_functions.c
	$(CC) $(FLAGS) src/read_functions.c

list.o: src/list.c
	$(CC) $(FLAGS) src/list.c

signal_handlers.o: src/signal_handlers.c
	$(CC) $(FLAGS) src/signal_handlers.c

clean:
	rm -f $(OBJS) $(OUT)