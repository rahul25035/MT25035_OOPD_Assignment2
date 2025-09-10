#ifndef SYSTEM_INTERFACE_H
#define SYSTEM_INTERFACE_H

// Define our own types instead of including standard headers
typedef unsigned long size_t;
typedef long ssize_t;
typedef long intptr_t;

/*
 * System call numbers for Linux x86_64
 */
#define SYS_read 0
#define SYS_write 1
#define SYS_open 2
#define SYS_close 3
#define SYS_exit 60
#define SYS_brk 12

// File descriptors
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

// File access modes (simple definitions kept from original)
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2

// System call interface using inline assembly (x86_64)
class SystemInterface {
public:
    static long syscall1(long number, long arg1) {
        long result;
        asm volatile(
            "movq %1, %%rax\n\t"
            "movq %2, %%rdi\n\t"
            "syscall\n\t"
            "movq %%rax, %0"
            : "=r"(result)
            : "r"(number), "r"(arg1)
            : "rcx", "r11", "memory"
        );
        return result;
    }

    static long syscall2(long number, long arg1, long arg2) {
        long result;
        asm volatile(
            "movq %1, %%rax\n\t"
            "movq %2, %%rdi\n\t"
            "movq %3, %%rsi\n\t"
            "syscall\n\t"
            "movq %%rax, %0"
            : "=r"(result)
            : "r"(number), "r"(arg1), "r"(arg2)
            : "rcx", "r11", "memory"
        );
        return result;
    }

    static long syscall3(long number, long arg1, long arg2, long arg3) {
        long result;
        asm volatile(
            "movq %1, %%rax\n\t"
            "movq %2, %%rdi\n\t"
            "movq %3, %%rsi\n\t"
            "movq %4, %%rdx\n\t"
            "syscall\n\t"
            "movq %%rax, %0"
            : "=r"(result)
            : "r"(number), "r"(arg1), "r"(arg2), "r"(arg3)
            : "rcx", "r11", "memory"
        );
        return result;
    }

    // Basic I/O operations
    static ssize_t read(int fd, void* buf, size_t count) {
        return (ssize_t)syscall3(SYS_read, (long)fd, (long)(intptr_t)buf, (long)count);
    }

    static ssize_t write(int fd, const void* buf, size_t count) {
        return (ssize_t)syscall3(SYS_write, (long)fd, (long)(intptr_t)buf, (long)count);
    }

    static int open(const char* pathname, int flags) {
        long ret = syscall3(SYS_open, (long)(intptr_t)pathname, (long)flags, (long)0);
        return (int)ret;
    }

    static int close(int fd) {
        return (int)syscall1(SYS_close, (long)fd);
    }

    static void exit(int status) {
        syscall1(SYS_exit, (long)status);
        // unreachable; infinite loop to satisfy compiler
        for (;;) {}
    }

    // sbrk-like allocator wrapper using brk syscall.
    static void* sbrk(long increment) {
        // Query current break
        long current = syscall1(SYS_brk, 0);
        if (current == -1) {
            return (void*)-1;
        }

        long desired = current + increment;
        // Try to set new break
        long result = syscall1(SYS_brk, desired);
        if (result != desired) {
            return (void*)-1;
        }

        // Return the start of the newly allocated block (old break)
        return (void*)(intptr_t)current;
    }
};

#endif // SYSTEM_INTERFACE_H
