#include "../headers/file_menu.h"
#include <menu.h>

struct file_menu* create_file_menu(int width, int height, int start_x, int start_y) {
  struct file_menu* file_menu = (struct file_menu*) malloc(sizeof(struct file_menu));
  WINDOW* window;
  MENU* menu;
  ITEM** items = NULL;

  window = newwin(height, width, start_y, start_x);
  keypad(window, TRUE);
  
  set_menu_format(NULL, height - 4, 1);
  menu = new_menu((ITEM **)items);
  set_menu_mark(menu, NULL);

  set_menu_win(menu, window);
  set_menu_sub(menu, derwin(window, height - 4, width - 2, 3, 1));

  box(window, 0, 0);
  print_header(window, width, COLOR_PAIR(1));

  file_menu->menu = menu;
  file_menu->menu_win = window;
  return file_menu;
}

void update_menu(struct file_menu *menu, const char* selected_dir) {
  if (!check_for_dir(selected_dir)) {
    return;
  }

  int size = 0;
  struct file_info* files = read_dir_data(selected_dir, &size);
  
  if (menu->items != NULL) { 
    for (int i = 0; i < menu->size; ++i) {
      free_item(menu->items[i]);
    }
  }

  menu->items = update_items(files, getmaxx(menu->menu_win), size);
  
  for (int i = 0; i < size; i++) {
    free(files[i].name);
  }
  free(files);

  unpost_menu(menu->menu);
  
  set_menu_items(menu->menu, menu->items); 
  menu->size = size;

  free(menu->dir_path);
  menu->dir_path = strdup(selected_dir);
  print_dir_name(menu->menu_win, menu->dir_path, getmaxx(menu->menu_win), COLOR_PAIR(1));

  post_menu(menu->menu);
  wrefresh(menu->menu_win);
  refresh();
}

ITEM** update_items(struct file_info* files, int menu_width, int size) {
  ITEM** items = (ITEM **)calloc(size, sizeof(ITEM *));
  int size_width = 8;
  int index = 0;
  int max_name_length = get_max_length(files, size);

  for (int i = 0; i < size; ++i) {
    if (strcmp(files[i].name, ".") == 0)
      continue;

    char *buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
    char *name = strdup(files[i].name);

    struct tm *tm_info = localtime(&files[i].mtime);
    char mtime_str[20];

    strftime(mtime_str, sizeof(mtime_str), "%Y-%m-%d %H:%M:%S", tm_info);
    int time_length = strlen(mtime_str);

    snprintf(buffer, 256, "%-*s | %*ld | %-*s",
             menu_width - (size_width + time_length + 9 + max_name_length), "", 
             size_width, files[i].size,
             time_length, mtime_str);

    items[index] = new_item(name, buffer);
    index++;
  }
  items[size - 1] = NULL;
  return items;
}

int get_max_length(struct file_info* files, int size) {
  int max_name_length = 0;
  for (int i = 0; i < size; i++) {
    int len = strlen(files[i].name);
    if (len > max_name_length)
      max_name_length = len;
  }
  return max_name_length;
}

void dispose_menu(struct file_menu* menu) {
  unpost_menu(menu->menu);
  free_menu(menu->menu);
  for (int i = 0; i < menu->size; ++i)
    free_item(menu->items[i]);
  free(menu->items);
  free(menu->dir_path);
  free(menu->dir_header);
  free(menu);
}

int check_for_dir(const char* path) {
  struct stat path_stat;
  if (stat(path, &path_stat) != 0 || !S_ISDIR(path_stat.st_mode)) {
    return 0;
  }
  return 1;
}

void print_header(WINDOW *win, int width, chtype color) {
  int x, y;
  getyx(win, y, x);
  wattron(win, color);
  mvwprintw(win, y + 1, x + 1, "%-*s | %*s | %-*s", width - (19 + 8 + 8), "Name", 8, "Size", 19, "Modify time");
  wattroff(win, color);
  refresh();
}

void print_dir_name(WINDOW *win, char* string, int width, chtype color) {
  int x, y;
  
  box(win, 0, 0);
  getyx(win, y, x);

  wattron(win, color);
  wattron(win, A_STANDOUT);
  mvwprintw(win, 0, 1, " %s ", string);
  wattroff(win, A_STANDOUT);
  wattroff(win, color);

  refresh();
}
