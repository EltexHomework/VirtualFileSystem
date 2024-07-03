#include "../headers/file_explorer.h"

int main() {
  char* start_dir = "/";
  struct file_explorer* explorer = create_explorer();
  run_explorer(explorer, start_dir);
  return 0;
}
