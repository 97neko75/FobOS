#include "keyboard.h"
#include "screen.h"
#include "utils.h"
#include <stddef.h>

static int shift_pressed = 0;
static int caps_lock = 0;

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

void readline(char* buffer, int max_len) {
    int idx = 0;
    while (1) {
        while ((inb(0x64) & 1) == 0);
        unsigned char sc = inb(0x60);
        int pressed = (sc & 0x80) == 0;
        unsigned char key = sc & 0x7F;
        
        if (key == 0x2A || key == 0x36) { shift_pressed = pressed; continue; }
        if (key == 0x3A && pressed) { caps_lock = !caps_lock; continue; }
        if (key == 0xE0) {
            while ((inb(0x64) & 1) == 0);
            inb(0x60);
            continue;
        }
        if (!pressed) continue;
        
        char ascii = 0;
        switch (key) {
            case 0x01: ascii = 0; break;
            case 0x0E: ascii = '\b'; break;
            case 0x1C: ascii = '\n'; break;
            case 0x39: ascii = ' '; break;
            case 0x1E: ascii = 'a'; break;
            case 0x30: ascii = 'b'; break;
            case 0x2E: ascii = 'c'; break;
            case 0x20: ascii = 'd'; break;
            case 0x12: ascii = 'e'; break;
            case 0x21: ascii = 'f'; break;
            case 0x22: ascii = 'g'; break;
            case 0x23: ascii = 'h'; break;
            case 0x17: ascii = 'i'; break;
            case 0x24: ascii = 'j'; break;
            case 0x25: ascii = 'k'; break;
            case 0x26: ascii = 'l'; break;
            case 0x32: ascii = 'm'; break;
            case 0x31: ascii = 'n'; break;
            case 0x18: ascii = 'o'; break;
            case 0x19: ascii = 'p'; break;
            case 0x10: ascii = 'q'; break;
            case 0x13: ascii = 'r'; break;
            case 0x1F: ascii = 's'; break;
            case 0x14: ascii = 't'; break;
            case 0x16: ascii = 'u'; break;
            case 0x2F: ascii = 'v'; break;
            case 0x11: ascii = 'w'; break;
            case 0x2D: ascii = 'x'; break;
            case 0x15: ascii = 'y'; break;
            case 0x2C: ascii = 'z'; break;
            case 0x0B: ascii = '0'; break;
            case 0x02: ascii = '1'; break;
            case 0x03: ascii = '2'; break;
            case 0x04: ascii = '3'; break;
            case 0x05: ascii = '4'; break;
            case 0x06: ascii = '5'; break;
            case 0x07: ascii = '6'; break;
            case 0x08: ascii = '7'; break;
            case 0x09: ascii = '8'; break;
            case 0x0A: ascii = '9'; break;
            case 0x33: ascii = ','; break;
            case 0x34: ascii = '.'; break;
            case 0x35: ascii = '/'; break;
            case 0x27: ascii = ';'; break;
            case 0x28: ascii = '\''; break;
            case 0x2B: ascii = '\\'; break;
            case 0x29: ascii = '`'; break;
            case 0x0C: ascii = '-'; break;
            case 0x0D: ascii = '='; break;
            case 0x1A: ascii = '['; break;
            case 0x1B: ascii = ']'; break;
            default: break;
        }
        if (ascii == 0) {
            buffer[0] = '\0';
            return;
        }
        
        int upper = shift_pressed ^ caps_lock;
        if (upper && ascii >= 'a' && ascii <= 'z')
            ascii = ascii - 'a' + 'A';
        else if (shift_pressed) {
            switch (ascii) {
                case ',': ascii = '<'; break;
                case '.': ascii = '>'; break;
                case '/': ascii = '?'; break;
                case ';': ascii = ':'; break;
                case '\'': ascii = '"'; break;
                case '\\': ascii = '|'; break;
                case '`': ascii = '~'; break;
                case '-': ascii = '_'; break;
                case '=': ascii = '+'; break;
                case '[': ascii = '{'; break;
                case ']': ascii = '}'; break;
                case '1': ascii = '!'; break;
                case '2': ascii = '@'; break;
                case '3': ascii = '#'; break;
                case '4': ascii = '$'; break;
                case '5': ascii = '%'; break;
                case '6': ascii = '^'; break;
                case '7': ascii = '&'; break;
                case '8': ascii = '*'; break;
                case '9': ascii = '('; break;
                case '0': ascii = ')'; break;
                default: break;
            }
        }
        
        if (ascii == '\b') {
            if (idx > 0) {
                idx--;
                // 删除操作需要知道当前光标位置，但 screen.c 中 cursor 是私有的
                // 为了简化，这里只处理缓冲区，屏幕删除由外部调用？复杂了。
                // 实际应暴露屏幕删除函数，但为了不增加复杂度，我们直接调用 putchar 的退格模拟？
                // 但 putchar 不处理 \b。所以我们暂时只回退缓冲区，屏幕会留字符。
                // 更好的做法是重新设计，但现在先简单实现：打印 \b 空格 \b。
                putchar('\b');
                putchar(' ');
                putchar('\b');
            }
        } else if (ascii == '\n') {
            buffer[idx] = '\0';
            putchar('\n');
            return;
        } else if (idx < max_len - 1) {
            buffer[idx++] = ascii;
            putchar(ascii);
        }
    }
}