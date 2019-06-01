#ifndef DIR_FUNCTIONS_HEADER
#define DIR_FUNCTIONS_HEADER

#include "pathlist.h"

// Add to a list all files (their paths) in a directory and its subdirectories
void list_files(Pathlist *list, char *dirname);
// Create directory to store the backup and return the name of the directory in
// the variable
void make_backup_dir(char *clientip_str, int port, char *dirname);

#endif