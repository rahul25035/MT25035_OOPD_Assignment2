// mystring.h - Custom string class without standard libraries
#ifndef MYSTRING_H
#define MYSTRING_H

// Basic system calls for memory allocation
extern "C" {
    void* malloc(unsigned long size);
    void free(void* ptr);
    void* memcpy(void* dest, const void* src, unsigned long n);
    void* memmove(void* dest, const void* src, unsigned long n);
    int memcmp(const void* s1, const void* s2, unsigned long n);
    void* memset(void* s, int c, unsigned long n);
}

class MyString {
private:
    char* data;
    unsigned long len;
    unsigned long capacity;

    void allocate(unsigned long size);
    void deallocate();
    void resize(unsigned long new_size);

public:
    // Constructors
    MyString();
    MyString(const char* str);
    MyString(const MyString& other);

    // Destructor
    ~MyString();

    // Assignment operator
    MyString& operator=(const MyString& other);
    MyString& operator=(const char* str);

    // Comparison operators
    bool operator==(const MyString& other) const;
    bool operator!=(const MyString& other) const;
    bool operator<(const MyString& other) const;
    bool operator>(const MyString& other) const;

    // Concatenation operators
    MyString operator+(const MyString& other) const;
    MyString& operator+=(const MyString& other);
    MyString& operator+=(const char* str);

    // Access operators
    char& operator[](unsigned long index);
    const char& operator[](unsigned long index) const;

    // Utility methods
    unsigned long length() const;
    const char* c_str() const;
    bool empty() const;
    void clear();

    // String manipulation
    MyString substr(unsigned long pos, unsigned long len = 0) const;
    unsigned long find(const MyString& str, unsigned long pos = 0) const;
    unsigned long find(const char* str, unsigned long pos = 0) const;
    unsigned long find_first_of(const char* chars, unsigned long pos = 0) const;

    // Trimming functions
    MyString& trim();
    MyString& to_lower();

    // Static utility functions
    static unsigned long strlen(const char* str);
    static char* strcpy(char* dest, const char* src);
    static char* strncpy(char* dest, const char* src, unsigned long n);
    static int strcmp(const char* s1, const char* s2);
    static int strncmp(const char* s1, const char* s2, unsigned long n);
    static char* strstr(const char* haystack, const char* needle);
    static char tolower(char c);
    static bool isspace(char c);
};

#endif // MYSTRING_H
