#include "../headers/file_explorer.h"

struct file_explorer* create_explorer() {
  struct file_explorer* explorer = (struct file_explorer*) malloc(sizeof(struct file_explorer));
  
  init_curses();
  getmaxyx(stdscr, explorer->height, explorer->width);
  
  int menu_height = 20;
  int menu_width = explorer->width / 2 - 2;

  explorer->left_menu = create_file_menu(menu_width, menu_height, 1, 1);
  explorer->right_menu = create_file_menu(menu_width, menu_height, menu_width + 2, 1);
  explorer->current_menu = explorer->left_menu;
  explorer->active_menu = 0;

  return explorer;
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

void run_explorer(struct file_explorer* explorer, char* dir) {
  update_menu(explorer->left_menu, dir);
  update_menu(explorer->right_menu, dir);
  print_reference();
  
  process_input(explorer);

  dispose(explorer);
  endwin();
}

void process_input(struct file_explorer* explorer) {
  int c;

  WINDOW *current_window = explorer->current_menu->menu_win;
  MENU *current_menu_ptr = explorer->current_menu->menu;

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
        switch_windows(explorer);
        current_window = explorer->current_menu->menu_win;
        current_menu_ptr = explorer->current_menu->menu;
        break;
      // Enter
      case 10:  
      {
        char path[BUFFER_SIZE];
        get_new_path(explorer, path); 
        update_menu(explorer->current_menu, path);
      }
      break;
    }
    wrefresh(current_window);
  }
}

void switch_windows(struct file_explorer* explorer) {
  int* active_menu = &explorer->active_menu; 

  if (*active_menu == 0) {
    *active_menu = 1;
    explorer->current_menu = explorer->right_menu;
  } else {
    *active_menu = 0;
    explorer->current_menu = explorer->left_menu;
  }
}

void get_new_path(struct file_explorer* explorer, char buffer[BUFFER_SIZE]) {
  struct file_menu* menu = explorer->current_menu;
  MENU* menu_ptr = menu->menu;
  
  ITEM *cur = current_item(menu_ptr);
  char *name = (char *)item_name(cur);

  if (strcmp(name, "..") == 0) {
    int index = strlen(menu->dir_path) - 1;
    do {
        menu->dir_path[index] = '\0';
      index--;
    } while (menu->dir_path[index] != '/');
    snprintf(buffer, BUFFER_SIZE, "%s", menu->dir_path);
  } 
  else {
    snprintf(buffer, BUFFER_SIZE, "%s%s/", menu->dir_path, name);
  }
}

void dispose(struct file_explorer* explorer) {
  dispose_menu(explorer->left_menu);
  dispose_menu(explorer->right_menu);
  free(explorer);
}

void print_reference() {
  attron(COLOR_PAIR(2));
  mvprintw(LINES - 1, 0, "Arrow Keys to navigate (F1 to Exit, Tab to switch between menus)");
  attroff(COLOR_PAIR(2));
  refresh();
}
