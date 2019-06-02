#include "../include/dir_functions.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../include/defines.h"

void list_files(Pathlist *list, char *dirname) {
    struct dirent *dp;
    DIR *dir = opendir(dirname);

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }
        char path[PATH_SIZE];
        sprintf(path, "%s/%s", dirname, dp->d_name);
        if (dp->d_type == DT_DIR) {
            list_files(list, path);
        } else {
            // find md5 hash of file content
            char cmd[BUF_SIZE];
            sprintf(cmd, "md5sum %s", path);
            FILE *fp = popen(cmd, "r");
            if (fp == NULL) {
                perror(RED "Error while hashing" RESET);
                exit(EXIT_FAILURE);
            }
            char str[BUF_SIZE];
            fgets(str, 100, fp);
            char *md5 = strtok(str, " ");
            pclose(fp);

            add_pathlist_node(list, path, md5);
        }
    }
    closedir(dir);
}

void make_backup_dir(char *clientip_str, int port, char *dirname) {
    // name of the folder will be ip + port
    sprintf(dirname, "%s_%d", clientip_str, port);
    // create backup directory if it doesn't exist
    struct stat st = {0};
    if (stat(dirname, &st) == -1) {
        mkdir(dirname, 0700);
    }
}
void get_md5_hash() {}
