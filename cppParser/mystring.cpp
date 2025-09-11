// mystring.cpp - Implementation of custom string class
#include "mystring.h"

// Constructor implementations
MyString::MyString() : data(nullptr), len(0), capacity(0) {
    allocate(1);
    data[0] = '\0';
}

MyString::MyString(const char* str) : data(nullptr), len(0), capacity(0) {
    if (str) {
        len = strlen(str);
        allocate(len + 1);
        strcpy(data, str);
    } else {
        allocate(1);
        data[0] = '\0';
    }
}

MyString::MyString(const MyString& other) : data(nullptr), len(0), capacity(0) {
    len = other.len;
    allocate(len + 1);
    strcpy(data, other.data);
}

// Destructor
MyString::~MyString() {
    deallocate();
}

// Assignment operators
MyString& MyString::operator=(const MyString& other) {
    if (this != &other) {
        deallocate();
        len = other.len;
        allocate(len + 1);
        strcpy(data, other.data);
    }
    return *this;
}

MyString& MyString::operator=(const char* str) {
    deallocate();
    if (str) {
        len = strlen(str);
        allocate(len + 1);
        strcpy(data, str);
    } else {
        len = 0;
        allocate(1);
        data[0] = '\0';
    }
    return *this;
}

// Comparison operators
bool MyString::operator==(const MyString& other) const {
    return strcmp(data, other.data) == 0;
}

bool MyString::operator!=(const MyString& other) const {
    return !(*this == other);
}

bool MyString::operator<(const MyString& other) const {
    return strcmp(data, other.data) < 0;
}

bool MyString::operator>(const MyString& other) const {
    return strcmp(data, other.data) > 0;
}

// Concatenation operators
MyString MyString::operator+(const MyString& other) const {
    MyString result;
    result.len = len + other.len;
    result.allocate(result.len + 1);
    strcpy(result.data, data);
    strcpy(result.data + len, other.data);
    return result;
}

MyString& MyString::operator+=(const MyString& other) {
    unsigned long new_len = len + other.len;
    if (new_len + 1 > capacity) {
        resize(new_len + 1);
    }
    strcpy(data + len, other.data);
    len = new_len;
    return *this;
}

MyString& MyString::operator+=(const char* str) {
    if (str) {
        unsigned long str_len = strlen(str);
        unsigned long new_len = len + str_len;
        if (new_len + 1 > capacity) {
            resize(new_len + 1);
        }
        strcpy(data + len, str);
        len = new_len;
    }
    return *this;
}

// Access operators
char& MyString::operator[](unsigned long index) {
    return data[index];
}

const char& MyString::operator[](unsigned long index) const {
    return data[index];
}

// Utility methods
unsigned long MyString::length() const {
    return len;
}

const char* MyString::c_str() const {
    return data;
}

bool MyString::empty() const {
    return len == 0;
}

void MyString::clear() {
    deallocate();
    len = 0;
    allocate(1);
    data[0] = '\0';
}

// String manipulation methods
MyString MyString::substr(unsigned long pos, unsigned long length) const {
    if (pos >= len) return MyString();

    unsigned long actual_len = (length == 0 || pos + length > len) ? len - pos : length;
    MyString result;
    result.len = actual_len;
    result.allocate(actual_len + 1);
    strncpy(result.data, data + pos, actual_len);
    result.data[actual_len] = '\0';
    return result;
}

unsigned long MyString::find(const MyString& str, unsigned long pos) const {
    return find(str.data, pos);
}

unsigned long MyString::find(const char* str, unsigned long pos) const {
    if (!str || pos >= len) return len; // Return length as "not found"

    char* found = strstr(data + pos, str);
    return found ? (found - data) : len;
}

unsigned long MyString::find_first_of(const char* chars, unsigned long pos) const {
    if (!chars || pos >= len) return len;

    for (unsigned long i = pos; i < len; ++i) {
        for (const char* c = chars; *c; ++c) {
            if (data[i] == *c) return i;
        }
    }
    return len;
}

MyString& MyString::trim() {
    // Trim leading spaces
    unsigned long start = 0;
    while (start < len && isspace(data[start])) start++;

    // Trim trailing spaces
    unsigned long end = len;
    while (end > start && isspace(data[end - 1])) end--;

    if (start > 0 || end < len) {
        unsigned long new_len = end - start;
        memmove(data, data + start, new_len);
        data[new_len] = '\0';
        len = new_len;
    }

    return *this;
}

MyString& MyString::to_lower() {
    for (unsigned long i = 0; i < len; ++i) {
        data[i] = tolower(data[i]);
    }
    return *this;
}

// Private helper methods
void MyString::allocate(unsigned long size) {
    capacity = size;
    data = (char*)malloc(capacity);
    if (!data) {
        // Handle allocation failure - for simplicity, we'll just set to null
        capacity = 0;
        len = 0;
    }
}

void MyString::deallocate() {
    if (data) {
        free(data);
        data = nullptr;
        len = 0;
        capacity = 0;
    }
}

void MyString::resize(unsigned long new_size) {
    if (new_size <= capacity) return;

    char* new_data = (char*)malloc(new_size);
    if (!new_data) return; // Handle allocation failure

    if (data) {
        memcpy(new_data, data, len + 1);
        free(data);
    }

    data = new_data;
    capacity = new_size;
}

// Static utility functions
unsigned long MyString::strlen(const char* str) {
    if (!str) return 0;
    unsigned long len = 0;
    while (str[len]) len++;
    return len;
}

char* MyString::strcpy(char* dest, const char* src) {
    if (!dest || !src) return dest;
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

char* MyString::strncpy(char* dest, const char* src, unsigned long n) {
    if (!dest || !src) return dest;
    char* d = dest;
    while (n-- && (*d++ = *src++));
    while (n-- > 0) *d++ = '\0';
    return dest;
}

int MyString::strcmp(const char* s1, const char* s2) {
    if (!s1 || !s2) return (!s1 && !s2) ? 0 : (!s1 ? -1 : 1);
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int MyString::strncmp(const char* s1, const char* s2, unsigned long n) {
    if (!s1 || !s2 || n == 0) return 0;
    while (--n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char* MyString::strstr(const char* haystack, const char* needle) {
    if (!haystack || !needle) return nullptr;
    if (!*needle) return (char*)haystack;

    for (; *haystack; haystack++) {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && (*h == *n)) {
            h++;
            n++;
        }
        if (!*n) return (char*)haystack;
    }
    return nullptr;
}

char MyString::tolower(char c) {
    return (c >= 'A' && c <= 'Z') ? (c + 32) : c;
}

bool MyString::isspace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}
