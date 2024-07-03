#ifndef FILE_EXPLORER_H
#define FILE_EXPLORER_H

#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include "file_menu.h"

struct file_explorer {
  struct file_menu* left_menu;
  struct file_menu* right_menu;
  struct file_menu* current_menu;
  int active_menu;
  int width;
  int height;
};

struct file_explorer* create_explorer();
void init_curses();
struct file_menu* create_file_menu(int width, int height, int start_x, int start_y);
void run_explorer(struct file_explorer* explorer, char* dir);
void process_input(struct file_explorer* explorer);
void switch_windows(struct file_explorer* explorer);
void get_new_path(struct file_explorer* explorer, char buffer[BUFFER_SIZE]);
void dispose(struct file_explorer* explorer);
void print_reference();

#endif // !FILE_EXPLORER_H
