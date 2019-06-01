#ifndef DEFINES_HEADER
#define DEFINES_HEADER

// Define color codes
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define RESET "\033[0m"

// the size of the socket buffer
#define BUF_SIZE 256
// the size of the buffer for the message CLIENT_LIST
#define CLIENT_LIST_SIZE 1000
// the size of the buffer for the message FILE_LIST
#define FILE_LIST_SIZE 1000
// the size of the buffer for a file-version tuple
#define FILE_BUF_SIZE 200
// the size of the buffer for a simple tuple
#define TUPLE_BUF_SIZE 100
// the size of the pathnames of files
#define PATH_SIZE 128
#define DIRNAME_SIZE 30

#endif