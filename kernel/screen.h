#ifndef SCREEN_H
#define SCREEN_H

void clear_screen(void);
void putchar(char c);
void print(const char* str);
void print_int(int n);
void set_cursor(int row, int col);
void scroll_lines(int lines);

// 新增：获取光标位置
void get_cursor(int* row, int* col);

#endif