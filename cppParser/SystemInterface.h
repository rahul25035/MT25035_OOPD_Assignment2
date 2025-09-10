// SystemInterface.h - Direct system calls, NO high-level stdio
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
#define O_RDWR 2

// System call interface using inline assembly (x86_64)
// These wrappers return long (raw syscall return). Caller converts as needed.
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
    // Use (long)(intptr_t) for pointer arguments to avoid warnings
    static ssize_t read(int fd, void* buf, size_t count) {
        return (ssize_t)syscall3(SYS_read, (long)fd, (long)(intptr_t)buf, (long)count);
    }

    static ssize_t write(int fd, const void* buf, size_t count) {
        return (ssize_t)syscall3(SYS_write, (long)fd, (long)(intptr_t)buf, (long)count);
    }

    // open: use the 3-arg syscall. Passing 0 for mode is safe for read-only.
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
    // Behavior:
    // - syscall1(SYS_brk, 0) returns current program break (or -1 on error).
    // - syscall1(SYS_brk, addr) returns the new break address on success, or a value != addr on failure.
    static void* sbrk(long increment) {
        static char* current_brk = (char*)0;

        // Initialize current break
        if (current_brk == (char*)0) {
            long initial = syscall1(SYS_brk, 0);
            if (initial == -1) {
                return (void*)-1;
            }
            current_brk = (char*)(intptr_t)initial;
        }

        char* old_brk = current_brk;
        char* desired_brk = current_brk + increment;

        long brk_ret = syscall1(SYS_brk, (long)(intptr_t)desired_brk);

        // On success the raw syscall returns the new break (desired_brk).
        if (brk_ret == (long)(intptr_t)desired_brk) {
            current_brk = desired_brk;
            return (void*)old_brk;
        } else {
            return (void*)-1;
        }
    }
};

#endif // SYSTEM_INTERFACE_H