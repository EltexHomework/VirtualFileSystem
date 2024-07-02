#include <curses.h>
#include <menu.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 4

struct file_info {
  char *name;
  off_t size;
  time_t mtime;
};

struct file_menu {
  ITEM **items;
  MENU *menu;
  WINDOW *menu_win;
  char *dir_header;
  char *dir_path;
  int size;
};

void process_input(struct file_menu *left_menu, struct file_menu *right_menu);
void dispose(MENU* menu, ITEM** items, int size);
struct file_menu* create_file_menu(int width, int height, int start_x, int start_y);
void update_items(struct file_info* files, ITEM*** items, int menu_width, int size);
struct file_info* read_dir_data(const char* dir, int* size);
void update_menu(struct file_menu *menu, const char* selected_dir);
void print_reference();
void print_dir_name(WINDOW *win, char* string, int width, chtype color);
void print_header(WINDOW *win, int width, chtype color);
int check_for_dir(const char* path);
int get_max_length(struct file_info* files, int size);
void init_curses();

int main() {
  int width, height;
  int size;

  char* dir = "/";

  init_curses();  
  getmaxyx(stdscr, height, width);

  int menu_height = 20;
  int menu_width = width / 2 - 2;

  struct file_menu* left_menu = create_file_menu(menu_width, menu_height, 1, 1);
  struct file_menu* right_menu = create_file_menu(menu_width, menu_height, menu_width + 2, 1);
  update_menu(left_menu, dir);
  update_menu(right_menu, dir);
  
  print_reference();
  process_input(left_menu, right_menu);

  dispose(left_menu->menu, left_menu->items, left_menu->size);
  dispose(right_menu->menu, right_menu->items, right_menu->size);
  endwin();
}

void process_input(struct file_menu *left_menu, struct file_menu *right_menu) {
  int c;
  int active_menu = 0;

  struct file_menu *current_menu = left_menu;
  WINDOW *current_window = left_menu->menu_win;
  MENU *current_menu_ptr = left_menu->menu;

  while ((c = wgetch(current_window)) != KEY_F(1)) {
    switch (c) {
      case KEY_DOWN:
        menu_driver(current_menu_ptr, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver(current_menu_ptr, REQ_UP_ITEM);
        break;
      case KEY_NPAGE:
				menu_driver(current_menu_ptr, REQ_SCR_DPAGE);
				break;
			case KEY_PPAGE:
				menu_driver(current_menu_ptr, REQ_SCR_UPAGE);
				break;
      // Tab
      case 9:  
        if (active_menu == 0) {
          active_menu = 1;
          current_menu = right_menu;
        } else {
          active_menu = 0;
          current_menu = left_menu;
        }
        current_window = current_menu->menu_win;
        current_menu_ptr = current_menu->menu;
        break;
      // Enter
      case 10:  
      {
        ITEM *cur = current_item(current_menu_ptr);
        char *name = (char *)item_name(cur);

        char new_dir[256];

        if (strcmp(name, "..") == 0) {
          int index = strlen(current_menu->dir_path) - 1;
          do {
              current_menu->dir_path[index] = '\0';
            index--;
          } while (current_menu->dir_path[index] != '/');
          snprintf(new_dir, sizeof(new_dir), "%s", current_menu->dir_path);
        } 
        else {
          snprintf(new_dir, sizeof(new_dir), "%s%s/", current_menu->dir_path, name);
        }
        update_menu(current_menu, new_dir);
      }
      break;
    }
    wrefresh(current_window);
  }
}

struct file_menu* create_file_menu(int width, int height, int start_x, int start_y) {
  struct file_menu* file_menu = (struct file_menu*) malloc(sizeof(struct file_menu));
  WINDOW* window;
  MENU* menu;
  ITEM** items = NULL;

  window = newwin(height, width, start_y, start_x);
  keypad(window, TRUE);

  menu = new_menu((ITEM **)items);
  set_menu_mark(menu, NULL);

  set_menu_win(menu, window);
  set_menu_sub(menu, derwin(window, height - 4, width - 2, 3, 1));
  set_menu_format(menu, 1, 1);

  box(window, 0, 0);
  print_header(window, width, COLOR_PAIR(1));

  file_menu->menu = menu;
  file_menu->menu_win = window;
  return file_menu;
}

void update_items(struct file_info* files, ITEM*** items, int menu_width, int size) {
  *items = (ITEM **)calloc(size, sizeof(ITEM *));
  int size_width = 8;
  int index = 0;
  int max_name_length = get_max_length(files, size);

  for (int i = 0; i < size; ++i) {
    char *buffer = (char *) malloc(256 * sizeof(char));
    
    if (strcmp(files[i].name, ".") == 0)
      continue;
    
    char *name = strdup(files[i].name); 
    struct tm *tm_info = localtime(&files[i].mtime);
    char mtime_str[20];

    strftime(mtime_str, sizeof(mtime_str), "%Y-%m-%d %H:%M:%S", tm_info);
    int time_length = strlen(mtime_str);

    snprintf(buffer, 256, "%-*s | %*ld | %-*s",
             menu_width - (size_width + time_length + 9 + max_name_length), "", 
             size_width, files[i].size,
             time_length, mtime_str);

    (*items)[index] = new_item(name, buffer);
    index++;
  }
  (*items)[size - 1] = NULL;
}

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
    char file_path[256];
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

void update_menu(struct file_menu *menu, const char* selected_dir) {
  if (!check_for_dir(selected_dir)) {
    return;
  }

  int size = 0;
  struct file_info* files = read_dir_data(selected_dir, &size);
  
  if (menu->items != NULL) { 
    for (int i = 0; i < menu->size; ++i)
      free_item(menu->items[i]);
  }

  update_items(files, &(menu->items), getmaxx(menu->menu_win), size);
  unpost_menu(menu->menu);

  int result = set_menu_items(menu->menu, menu->items); 
  if (result != E_POSTED) {
    printf("%d", result);
  }
  menu->size = size;

  free(menu->dir_path);
  menu->dir_path = strdup(selected_dir);
  print_dir_name(menu->menu_win, menu->dir_path, getmaxx(menu->menu_win), COLOR_PAIR(1));

  post_menu(menu->menu);
  wrefresh(menu->menu_win);
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

void print_header(WINDOW *win, int width, chtype color) {
  int x, y;
  getyx(win, y, x);
  wattron(win, color);
  mvwprintw(win, y + 1, x + 1, "%-*s | %*s | %-*s", width - (19 + 8 + 8), "Name", 8, "Size", 19, "Modify time");
  wattroff(win, color);
  refresh();
}

void print_reference() {
  attron(COLOR_PAIR(2));
  mvprintw(LINES - 1, 0, "Arrow Keys to navigate (F1 to Exit, Tab to switch between menus)");
  attroff(COLOR_PAIR(2));
  refresh();
}

int check_for_dir(const char* path) {
  struct stat path_stat;
  if (stat(path, &path_stat) != 0 || !S_ISDIR(path_stat.st_mode)) {
    return 0;
  }
  return 1;
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

void init_curses() {
  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);
  init_pair(2, COLOR_CYAN, COLOR_BLACK);
}

void dispose(MENU* menu, ITEM** items, int size) {
  unpost_menu(menu);
  free_menu(menu);
  for (int i = 0; i < size; ++i)
    free_item(items[i]);
}
