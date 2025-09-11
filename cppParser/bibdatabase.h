// bibdatabase.h - Bibliography database class definition (FIXED VERSION)
#ifndef BIBDATABASE_H
#define BIBDATABASE_H

#include "bibentry.h"
#include "mystring.h"
#include "Author.h"
#include "placement_new.h"


// Simple vector-like container since we can't use std::vector - COMPLETELY FIXED
template<typename T>
class MyVector {
private:
    T* data;
    unsigned long size;
    unsigned long capacity;

    void resize();
    void deallocate();

public:
    MyVector();
    MyVector(const MyVector& other);
    ~MyVector();

    MyVector& operator=(const MyVector& other);

    void push_back(const T& item);
    void clear();
    unsigned long get_size() const;
    bool empty() const;

    T& operator[](unsigned long index);
    const T& operator[](unsigned long index) const;

    // Sorting method (simple bubble sort)
    void sort();
};

class BibDatabase {
private:
    MyVector<BibEntry> entries;
    MyString database_name;

    // File I/O helper methods
    static const int MAX_LINE_LENGTH = 8192;
    bool read_line(int fd, char* buffer, int max_len);
    bool is_whitespace_line(const MyString& line);

    // Parsing helper methods
    bool parse_bib_entry(int fd, MyString& current_line);

public:
    // Constructors
    BibDatabase();
    BibDatabase(const MyString& name);
    BibDatabase(const BibDatabase& other);

    // Destructor
    ~BibDatabase();

    // Assignment operator
    BibDatabase& operator=(const BibDatabase& other);

    // Addition operator for merging databases
    BibDatabase operator+(const BibDatabase& other) const;
    BibDatabase& operator+=(const BibDatabase& other);

    // File operations
    bool load_from_file(const MyString& filename);
    bool save_to_file(const MyString& filename) const;

    // Entry management
    void add_entry(const BibEntry& entry);
    bool remove_entry(const MyString& entry_key);
    BibEntry* find_entry(const MyString& entry_key);
    const BibEntry* find_entry(const MyString& entry_key) const;

    // Database operations
    void sort_entries(); // Sort by <year descending, title ascending>
    void clear();
    bool empty() const;
    unsigned long size() const;

    // Search and filter operations
    int count_institute_authors(const MyString& institute_name) const;

    // Accessors
    const MyString& get_name() const;
    void set_name(const MyString& name);

    BibEntry& get_entry(unsigned long index);
    const BibEntry& get_entry(unsigned long index) const;

    // Display methods
    MyString to_string() const;
    void print_summary() const;
    void print_entries() const;
    void print_institute_authors(const MyString& institute_name) const;

    // Validation
    bool validate() const;
};

// FIXED Template implementation - Using only malloc/free for consistency
template<typename T>
MyVector<T>::MyVector() : data(nullptr), size(0), capacity(0) {}

template<typename T>
MyVector<T>::MyVector(const MyVector& other) : data(nullptr), size(0), capacity(0) {
    if (other.size > 0) {
        capacity = other.capacity;
        data = (T*)malloc(sizeof(T) * capacity);
        if (data) {
            // Use placement new to properly construct objects
            for (unsigned long i = 0; i < other.size; i++) {
                new (&data[i]) T(other.data[i]); // Copy constructor
            }
            size = other.size;
        }
    }
}

template<typename T>
MyVector<T>::~MyVector() {
    deallocate();
}

template<typename T>
MyVector<T>& MyVector<T>::operator=(const MyVector& other) {
    if (this != &other) {
        deallocate();
        if (other.size > 0) {
            capacity = other.capacity;
            data = (T*)malloc(sizeof(T) * capacity);
            if (data) {
                // Use placement new to properly construct objects
                for (unsigned long i = 0; i < other.size; i++) {
                    new (&data[i]) T(other.data[i]); // Copy constructor
                }
                size = other.size;
            }
        }
    }
    return *this;
}

template<typename T>
void MyVector<T>::push_back(const T& item) {
    if (size >= capacity) {
        resize();
    }
    if (data && size < capacity) {
        // Use placement new to construct the object
        new (&data[size]) T(item); // Copy constructor
        size++;
    }
}

template<typename T>
void MyVector<T>::clear() {
    deallocate();
}

template<typename T>
void MyVector<T>::deallocate() {
    if (data) {
        // Call destructors for all constructed objects
        for (unsigned long i = 0; i < size; i++) {
            data[i].~T();
        }
        free(data);
        data = nullptr;
    }
    size = 0;
    capacity = 0;
}

template<typename T>
void MyVector<T>::resize() {
    unsigned long new_capacity = capacity == 0 ? 1 : capacity * 2;
    T* new_data = (T*)malloc(sizeof(T) * new_capacity);

    if (new_data) {
        // Move-construct elements to new location
        for (unsigned long i = 0; i < size; i++) {
            new (&new_data[i]) T(data[i]); // Copy construct
            data[i].~T(); // Destroy old object
        }

        if (data) free(data);
        data = new_data;
        capacity = new_capacity;
    }
}

template<typename T>
unsigned long MyVector<T>::get_size() const {
    return size;
}

template<typename T>
bool MyVector<T>::empty() const {
    return size == 0;
}

template<typename T>
T& MyVector<T>::operator[](unsigned long index) {
    return data[index];
}

template<typename T>
const T& MyVector<T>::operator[](unsigned long index) const {
    return data[index];
}

template<typename T>
void MyVector<T>::sort() {
    // Simple bubble sort (for educational purposes)
    for (unsigned long i = 0; i < size && size > 0; i++) {
        for (unsigned long j = 0; j < size - i - 1; j++) {
            if (data[j + 1] < data[j]) {
                // Swap elements using temporary
                T temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
}

#endif // BIBDATABASE_H