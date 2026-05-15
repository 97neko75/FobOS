#include "utils.h"
#include "screen.h"

static unsigned int rng_seed = 1;

int strcmp(const char* a, const char* b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return *a - *b;
}
int strncmp(const char* a, const char* b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return a[i] - b[i];
        if (a[i] == '\0') break;
    }
    return 0;
}
int atoi(const char* s) {
    int res = 0;
    while (*s >= '0' && *s <= '9') {
        res = res * 10 + (*s - '0');
        s++;
    }
    return res;
}
int rand(void) {
    rng_seed = rng_seed * 1103515245 + 12345;
    return (unsigned int)(rng_seed / 65536) % 32768;
}

void print_uint(unsigned int n) {
    char buf[12];
    int i = 0;
    if (n == 0) {
        putchar('0');
        return;
    }
    while (n) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while (i--) putchar(buf[i]);
}