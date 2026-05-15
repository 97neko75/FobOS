#include "screen.h"
#include "keyboard.h"
#include "cmd.h"

#define MAX_CMD_LEN 64

// 声明 boot.s 中的 multiboot_info 变量
extern unsigned int multiboot_info;

// 提供给命令模块使用的函数
unsigned int get_multiboot_info(void) {
    return multiboot_info;
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}
static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "d"(port));
}

void reboot(void) {
    print("Rebooting...\n");
    for (int i = 0; i < 100000; i++) asm volatile ("pause");
    while ((inb(0x64) & 2) != 0);
    outb(0x64, 0xFE);
    while (1);
}

void kernel_main(void) {
    clear_screen();
    print("Fob 1.2 (with meminfo)\n");
    print("Type 'help' for commands.\n");
    char cmdline[MAX_CMD_LEN];
    while (1) {
        print("> ");
        readline(cmdline, MAX_CMD_LEN);
        process_command(cmdline);
    }
}