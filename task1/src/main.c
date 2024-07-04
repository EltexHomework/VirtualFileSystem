#include <stdio.h>

#define BUFFER_SIZE 256

int write_to_file(char* path, char* string);

void read_reverse(char* path, char buffer[BUFFER_SIZE]);

int get_file_size(FILE* file);

int main(void) {
  char* path = "./bin/output.txt";
  char* string = "String from file";
  char buffer[BUFFER_SIZE];

  if (!write_to_file(path, string)) {
    printf("Error!");
    return 0;
  }

  read_reverse(path, buffer);
  printf("%s\n", buffer);
  return 0;
}

int write_to_file(char* path, char* string) {
  FILE* file = fopen(path, "w");
  
  if (fputs(string, file) == EOF) {
    return 0;
  }
  fclose(file);
  
  return 1;
}

void read_reverse(char* path, char buffer[BUFFER_SIZE]) {
  FILE* file = fopen(path, "r");
  int size = get_file_size(file);
  int i;
  
  for (i = 0; i < size; i++) {
    fseek(file, -i - 1, SEEK_END); 
    buffer[i] = fgetc(file);   
  }

  buffer[i] = '\0';
  fclose(file);
}

int get_file_size(FILE* file) {
  fseek(file, 0, SEEK_END);
  return ftell(file); 
}
