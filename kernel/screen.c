#include "screen.h"

#define VIDEO_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define COLOR 0x0F

static char* video = (char*) VIDEO_MEMORY;
int cursor_row = 0, cursor_col = 0;   // 改为全局

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "d"(port));
}

void set_cursor(int row, int col) {
    if (row < 0) row = 0;
    if (row >= VGA_HEIGHT) row = VGA_HEIGHT - 1;
    if (col < 0) col = 0;
    if (col >= VGA_WIDTH) col = VGA_WIDTH - 1;
    int pos = row * VGA_WIDTH + col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void get_cursor(int* row, int* col) {
    *row = cursor_row;
    *col = cursor_col;
}

void clear_screen(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        video[i] = ' ';
        video[i+1] = COLOR;
    }
    cursor_row = 0;
    cursor_col = 0;
    set_cursor(cursor_row, cursor_col);
}

void scroll_lines(int lines) {
    if (lines <= 0) return;
    if (lines >= VGA_HEIGHT) {
        clear_screen();
        return;
    }
    for (int row = lines; row < VGA_HEIGHT; row++) {
        int src = row * VGA_WIDTH * 2;
        int dst = (row - lines) * VGA_WIDTH * 2;
        for (int i = 0; i < VGA_WIDTH * 2; i++)
            video[dst + i] = video[src + i];
    }
    for (int row = VGA_HEIGHT - lines; row < VGA_HEIGHT; row++) {
        int off = row * VGA_WIDTH * 2;
        for (int i = 0; i < VGA_WIDTH * 2; i += 2) {
            video[off + i] = ' ';
            video[off + i + 1] = COLOR;
        }
    }
    cursor_row -= lines;
    if (cursor_row < 0) cursor_row = 0;
    set_cursor(cursor_row, cursor_col);
}

void putchar(char c) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
        if (cursor_row >= VGA_HEIGHT) {
            int overflow = cursor_row - VGA_HEIGHT + 1;
            scroll_lines(overflow);
            cursor_row = VGA_HEIGHT - 1;
        }
        set_cursor(cursor_row, cursor_col);
        return;
    }
    if (c == '\t') {
        for (int i = 0; i < 4; i++) putchar(' ');
        return;
    }
    if (c == '\b') {
        if (cursor_col > 0) {
            cursor_col--;
            int off = (cursor_row * VGA_WIDTH + cursor_col) * 2;
            video[off] = ' ';
            video[off+1] = COLOR;
            set_cursor(cursor_row, cursor_col);
        } else if (cursor_row > 0) {
            cursor_row--;
            cursor_col = VGA_WIDTH - 1;
            int off = (cursor_row * VGA_WIDTH + cursor_col) * 2;
            video[off] = ' ';
            video[off+1] = COLOR;
            set_cursor(cursor_row, cursor_col);
        }
        return;
    }
    int off = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video[off] = c;
    video[off+1] = COLOR;
    cursor_col++;
    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
        if (cursor_row >= VGA_HEIGHT) {
            int overflow = cursor_row - VGA_HEIGHT + 1;
            scroll_lines(overflow);
            cursor_row = VGA_HEIGHT - 1;
        }
    }
    set_cursor(cursor_row, cursor_col);
}

void print(const char* str) {
    while (*str) putchar(*str++);
}

void print_int(int n) {
    char buf[12];
    int i = 0;
    if (n == 0) { putchar('0'); return; }
    if (n < 0) { putchar('-'); n = -n; }
    while (n) { buf[i++] = '0' + (n % 10); n /= 10; }
    while (i--) putchar(buf[i]);
}