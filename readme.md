# Share folders between different machines through TCP ports (like dropbox) #
## Server ##
Server is listening for requests from clients and also keeps a list with the tuples (ip address, listening port) of the connected to the app clients. When a new client wants to connect, it sends a LOG_ON request along with its information (ip address, listening port) to the server. Server has to send a USER_ON request to every client that is already connected to the app along with the information of the new client. Then new clients send a GET_CLIENTS request to the server and server returns a CLIENT_LIST request along with a list with the information of the already connected clients. If a client wants to exit it sends a LOG_OFF request to the server and server remove this client from the list of tuples and send USER_OFF message to all the other clients. 
<br><br>
Run server as:
```
./dropbox_server -p portNum
```
where **portNum** is the listening port of the server

## Client ##
Client sends LOG_ON, GET_CLIENTS and LOG_OFF requests to server and keeps a list with all the other connected clients (after getting response from GET_CLIENTS request to the server). For each client in the list the client sends a GET_FILE_LIST to get all files in the sharing folders of the other clients. Clients response with FILE_LIST and a list of pathnames. For every pathname the clients sends a GET_FILE request to the client that has the file and a FILE_SIZE response is returned with the size and the content of the file. Client also creates a circular buffer with structs {pathname(128bytes), version, IPaddress, portNum} to store requests. Threads read from the circular buffer if it not empty and execute the requests. <br>
Requests that can be stored in circular buffer are: <br>
* CLIENT_LIST : list of clients already connected (ip adress, listening port)
* FILE_LIST : list of files the client has in the directory it is sharing
* FILE_SIZE : the path of the file followed by its size in bytes and its version 
* FILE_UP_TO_DATE : if file is already up to date
* FILE_NOT_FOUND : if no such file in the client's directory
 <br><br>
Run clients as (order of arguments doens't matter as long as flags are correct):
```
./dropbox_client –d dirName –p portNum –w workerThreads –b bufferSize –sp serverPort –sip serverIP
```
where <br>
* **dirName** is the folder that the client will share with the other clients
* **portNum** is the listening port of the client to accept requests from server or other clients
* **workerThreads** is the number of threads the client will create to process requests
* **bufferSize** is the size (the number of items) of the circular buffer 
* **serverPort** is the port number where dropbox_server, that client will be connected to, listens
* **serverPort** is ip address of the dropbox_server that client will be connected to

## Notes ##
### You can run the server and the clients from the same or different machines. ###
### When server and client are on the same machine give argument as -sip localhost else give ip address with dot form e.g. (124.56.210.3) ###
### Every client creates a directory with name ipaddress_portnum and inside there are the directories (with same name formating) of the clients that are sharing their directories. ###
For example if there are three clients communicating with ip addresses and ports as (124.56.210.3, 8888), (103.4.125.4, 6002), (220.10.100.4, 5000)  
a client with ip address 124.56.210.3 and listening port 8888 will create a directory with name 124.56.210.3_8888 and inside will have two directories  103.4.125.4_6002, 220.10.100.4_5000 which will contain the files and subdirectories of the other two clients.
