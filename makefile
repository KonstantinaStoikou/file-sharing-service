OBJS 	= dropbox_server.o read_functions.o
OUT  	= dropbox_server
CC		= gcc
FLAGS   = -Wall -g -c 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

dropbox_server.o: src/dropbox_server.c
	$(CC) $(FLAGS) src/dropbox_server.c

read_functions.o: src/read_functions.c
	$(CC) $(FLAGS) src/read_functions.c

clean:
	rm -f $(OBJS) $(OUT)