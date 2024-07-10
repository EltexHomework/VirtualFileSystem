#ifndef FILE_MENU_H
#define FILE_MENU_H

#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include "file_info.h"

struct file_menu {
  ITEM **items;
  MENU *menu;
  WINDOW *menu_win;
  char *dir_header;
  char *dir_path;
  int size;
};

struct file_menu* create_file_menu(int width, int height, int start_x, int start_y);
void update_menu(struct file_menu *menu, const char* selected_dir);
ITEM** update_items(struct file_info* files, int menu_width, int size);
int get_max_length(struct file_info* files, int size);
void dispose_menu(struct file_menu* menu);
int check_for_dir(const char* path);
void print_header(WINDOW *win, int width, chtype color);
void print_dir_name(WINDOW *win, char* string, int width, chtype color);

#endif // !FILE_MENU_H
