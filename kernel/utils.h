#ifndef UTILS_H
#define UTILS_H

int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, int n);
int atoi(const char* s);
int rand(void);
void print_uint(unsigned int n);   // 新增：打印无符号整数

#endif