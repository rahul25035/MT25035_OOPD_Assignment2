// SystemInterface.h - Direct system calls, NO standard libraries
#ifndef SYSTEM_INTERFACE_H
#define SYSTEM_INTERFACE_H

// Basic type definitions (no stddef.h)
typedef long size_t;
typedef long ssize_t;
typedef int bool;
#define true 1
#define false 0
#define NULL ((void*)0)

// System call numbers for Linux x86_64
#define SYS_read    0
#define SYS_write   1
#define SYS_open    2
#define SYS_close   3
#define SYS_exit    60
#define SYS_brk     12

// File descriptors
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

// File access modes
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2

// System call interface using inline assembly
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
            : "rax", "rdi", "rcx", "r11", "memory"
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
            : "rax", "rdi", "rsi", "rcx", "r11", "memory"
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
            : "rax", "rdi", "rsi", "rdx", "rcx", "r11", "memory"
        );
        return result;
    }

    // Basic I/O operations
    static ssize_t read(int fd, void* buf, size_t count) {
        return syscall3(SYS_read, fd, (long)buf, count);
    }

    static ssize_t write(int fd, const void* buf, size_t count) {
        return syscall3(SYS_write, fd, (long)buf, count);
    }

    static int open(const char* pathname, int flags) {
        return syscall2(SYS_open, (long)pathname, flags);
    }

    static int close(int fd) {
        return syscall1(SYS_close, fd);
    }

    static void exit(int status) {
        syscall1(SYS_exit, status);
    }

    // Memory management using brk system call
    static void* sbrk(long increment) {
        static char* current_brk = NULL;
        if (current_brk == NULL) {
            current_brk = (char*)syscall1(SYS_brk, 0);
        }
        char* old_brk = current_brk;
        char* new_brk = (char*)syscall1(SYS_brk, (long)(current_brk + increment));
        if (new_brk == current_brk) return (void*)-1; // Error
        current_brk = new_brk;
        return old_brk;
    }
};

#endif
