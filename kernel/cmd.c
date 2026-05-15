#include "cmd.h"
#include "screen.h"
#include "utils.h"

// 从 main.c 获取 multiboot 信息地址
extern unsigned int get_multiboot_info(void);
// reboot 函数在 main.c 中实现
extern void reboot(void);

void process_command(const char* cmd) {
    if (cmd[0] == '\0') return;

    if (strcmp(cmd, "help") == 0) {
        print("Available commands:\n");
        print("  help          - Show this help\n");
        print("  hello         - Greeting\n");
        print("  cls / clear   - Clear screen\n");
        print("  reboot        - Restart system\n");
        print("  echo <text>   - Print <text>\n");
        print("  about         - Show about info\n");
        print("  ver           - Show version\n");
        print("  calc <expr>   - Simple addition (e.g., calc 2+3)\n");
        print("  fortune       - Random wisdom\n");
        print("  meminfo       - Show memory layout\n");
        return;
    }
    if (strcmp(cmd, "hello") == 0) {
        print("Hello, user! Welcome to Fob.\n");
        return;
    }
    if (strcmp(cmd, "cls") == 0 || strcmp(cmd, "clear") == 0) {
        clear_screen();
        return;
    }
    if (strcmp(cmd, "reboot") == 0) {
        reboot();
        return;
    }
    if (strcmp(cmd, "about") == 0) {
        print("Fob - A tiny x86 operating system\n");
        print("Built on 2026-05-15\n");
        print("Features: 80x25 text, hardware cursor, Shift/Caps, smooth scroll\n");
        return;
    }
    if (strcmp(cmd, "ver") == 0) {
        print("Fob version 1.2 (2026-05-15)\n");
        return;
    }
    if (strcmp(cmd, "fortune") == 0) {
        const char* fortunes[] = {
            "Fob: Fake, but runs.",
            "Blue screen of death is art; Fob is performance art.",
            "You just typed a command. That's an OS.",
            "Linux has Linus; Fob has 97neko75.",
            "404: OS not found. Oh wait, it's Fob.",
            "This kernel has no bugs, only undocumented features.",
            "Help list too long? Blame me.",
            "Fob 1.0: More stable than Windows 95 (because no drivers).",
            "When you type 'fortune', Fob becomes philosophical.",
            "Coding is fun, debugging is hell.",
            "An OS is like an iceberg; you only see the command line."
        };
        int count = sizeof(fortunes) / sizeof(fortunes[0]);
        int idx = rand() % count;
        print(fortunes[idx]);
        putchar('\n');
        return;
    }
    if (strncmp(cmd, "echo ", 5) == 0) {
        print(cmd + 5);
        putchar('\n');
        return;
    }
    if (strncmp(cmd, "calc ", 5) == 0) {
        const char* expr = cmd + 5;
        while (*expr == ' ') expr++;
        int a = atoi(expr);
        while (*expr && (*expr >= '0' && *expr <= '9')) expr++;
        while (*expr == ' ') expr++;
        if (*expr == '+') {
            expr++;
            while (*expr == ' ') expr++;
            int b = atoi(expr);
            print_int(a + b);
            putchar('\n');
        } else {
            print("Usage: calc number+number\n");
        }
        return;
    }
    if (strcmp(cmd, "meminfo") == 0) {
        unsigned int mb_info = get_multiboot_info();
        if (mb_info == 0) {
            print("No multiboot info available.\n");
            return;
        }
        // Multiboot 信息结构: flags (4B), mem_lower (4B), mem_upper (4B)
        unsigned int* mb = (unsigned int*) mb_info;
        if (mb[0] & 0x1) {  // 检查内存信息标志位
            unsigned int mem_lower = mb[1];   // 偏移 4 字节
            unsigned int mem_upper = mb[2];   // 偏移 8 字节
            print("Low memory: ");
            print_uint(mem_lower);
            print(" KB\n");
            print("High memory: ");
            print_uint(mem_upper);
            print(" KB\n");
            print("Total: ");
            print_uint(mem_lower + mem_upper);
            print(" KB\n");
        } else {
            print("No memory info from GRUB.\n");
        }
        return;
    }

    print("Unknown command: ");
    print(cmd);
    print("\nType 'help' to list commands.\n");
}