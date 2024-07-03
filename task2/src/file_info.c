#include "../headers/file_info.h"

struct file_info* read_dir_data(const char* dir, int* size) {
  struct dirent **namelist;
  struct stat stats;

  int n = scandir(dir, &namelist, 0, alphasort);
  if (n < 0) {
    return NULL;
  }

  struct file_info* files_data = (struct file_info *) malloc(n * sizeof(struct file_info));
  *size = n;

  for (int i = 0; i < n; i++) {
    char file_path[BUFFER_SIZE];
    snprintf(file_path, sizeof(file_path), "%s/%s", dir, namelist[i]->d_name);

    if (stat(file_path, &stats) == 0) {
      files_data[i].name = strdup(namelist[i]->d_name);
      files_data[i].size = stats.st_size;
      files_data[i].mtime = stats.st_mtim.tv_sec;
    } else {
      files_data[i].name = strdup("Unknown");
      files_data[i].size = 0;
      files_data[i].mtime = 0;
    }
    free(namelist[i]);
  }
  free(namelist);

  return files_data;
}
