OBJS 	= test.o  read_functions.o list.o
# OUT  	= dropbox_server
OUT  	= test
CC		= gcc
FLAGS   = -Wall -g -c 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

test.o: test.c
	$(CC) $(FLAGS) test.c

# dropbox_server.o: src/dropbox_server.c
# 	$(CC) $(FLAGS) src/dropbox_server.c

read_functions.o: src/read_functions.c
	$(CC) $(FLAGS) src/read_functions.c

list.o: src/list.c
	$(CC) $(FLAGS) src/list.c

clean:
	rm -f $(OBJS) $(OUT)