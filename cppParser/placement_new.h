// placement_new.h - Placement new operator declaration
#ifndef PLACEMENT_NEW_H
#define PLACEMENT_NEW_H

// Declaration for placement new operator
inline void* operator new(unsigned long, void* ptr) {
    return ptr;
}

// Declaration for placement new[] operator
inline void* operator new[](unsigned long, void* ptr) {
    return ptr;
}

// These don't need to do anything for placement new
inline void operator delete(void*, void*) {}
inline void operator delete[](void*, void*) {}

#endif // PLACEMENT_NEW_H