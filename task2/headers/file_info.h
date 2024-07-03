#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE 256

struct file_info {
  char *name;
  off_t size;
  time_t mtime;
};

struct file_info* read_dir_data(const char* dir, int* size);

#endif // !FILE_INFO_H
