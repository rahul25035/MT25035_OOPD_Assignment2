# BibTeX Parser - NO STANDARD LIBRARIES VERSION

## Object-Oriented Programming and Design - Assignment 2
### Monsoon 2025

This project is a complete C++ implementation of a BibTeX parser built **entirely from scratch** without using ANY standard libraries. It demonstrates advanced OOP principles while using only direct system calls for all functionality.

## 🚫 ZERO LIBRARIES USED

This implementation uses:
- ❌ NO stdio.h (no printf, scanf, fopen, etc.)
- ❌ NO stdlib.h (no malloc, free, atoi, etc.)
- ❌ NO string.h (no strlen, strcmp, strcpy, etc.)
- ❌ NO iostream (no cout, cin, etc.)
- ❌ NO std::string, std::vector, or any STL
- ❌ NO ctype.h (no isspace, tolower, etc.)

## ✅ WHAT WE BUILT FROM SCRATCH

Instead, everything is implemented using:
- **Direct system calls** (read, write, open, close, brk)
- **Inline assembly** for system call interface
- **Custom memory management** using brk system call
- **Manual string operations** (length, comparison, concatenation)
- **Custom I/O functions** (print, read line)
- **Pure OOP design** with proper encapsulation

## 📁 FILE STRUCTURE

### Core System Interface
- **SystemInterface.h** - Direct system call interface using inline assembly

### OOP Classes (Following Assignment Requirements)
- **String.h** - Custom string class with dynamic memory management
- **Author.h** - Author information with institute checking
- **Publication.h** - Publication data with URL fields and sorting
- **Bibliography.h** - Collection management with + operator and file parsing

### Main Program
- **main.cpp** - Main program following original C logic exactly
- **Makefile** - Build system with comprehensive testing
- **README.md** - This documentation

## 🎯 ASSIGNMENT COMPLIANCE

### ✅ Core Requirements Met:
1. **Converted C to C++** - Full OOP implementation following original logic
2. **Added URL fields** - PDF, source code, and presentation URLs
3. **Removed fixed strings** - Dynamic String class, no char arrays
4. **Implemented < operator** - Year descending, title lexicographic ascending
5. **Implemented + operator** - Bibliography combination functionality
6. **At least 3 class files** - 4 classes: String, Author, Publication, Bibliography
7. **Constructors/Destructors** - Proper memory management throughout
8. **NO standard libraries** - Taken to the extreme - zero libraries used
9. **Data abstraction** - Private members, public interfaces
10. **Error checking** - Input validation and bounds checking
11. **Operator overloading** - Comparison, assignment, arithmetic operators

### 🔥 Beyond Requirements:
- **System-level programming** - Direct OS interface
- **Custom memory allocator** - Using brk system call
- **Assembly integration** - Inline assembly for system calls
- **Advanced OOP** - Multiple inheritance-ready design

## 🛠️ BUILDING AND RUNNING

### Prerequisites
- Linux x86_64 system (uses Linux system calls)
- g++ compiler with C++11 support
- Make utility

### Build Commands
```bash
# Build the program
make

# Create sample test files
make sample_files

# Run with MIT institute
./bib_parser sample.bib "MIT"

# Run with Stanford institute  
./bib_parser sample.bib "Stanford"

# Verify no libraries used
make check_libs

# Show assignment compliance
make compliance
```

### Usage
```bash
./bib_parser <bib_file> <institute_name>

Examples:
./bib_parser papers.bib "MIT"
./bib_parser papers.bib "University of California"
```

## 📊 ORIGINAL C LOGIC PRESERVATION

The implementation follows the original C code logic exactly:

### BibTeX Parsing:
- Same entry detection (`@` character)
- Same field extraction (title, year, author, journal)
- Same author splitting (" and " delimiter)
- Same institute checking (substring search in author names)
- Same output formatting and statistics

### Enhanced Features:
- **URL field support** - PDF, source code, presentation URLs
- **Dynamic memory** - No fixed-size arrays
- **Proper sorting** - Year descending, title ascending
- **Bibliography merging** - + operator functionality
- **OOP design** - Encapsulation and data hiding

## 🧪 TESTING

### Sample Data
The Makefile creates comprehensive test files:
- **sample.bib** - Multiple publications with various institutes
- **sample2.bib** - Additional data for testing + operator

### Test Cases
```bash
# Basic institute searching
make test_mit      # Find MIT authors
make test_stanford # Find Stanford authors

# Interactive features
./bib_parser sample.bib "MIT"
# Choose 'y' for OOP demonstration to see:
# - Sorting functionality
# - + operator
# - URL field display
```

## 🔧 TECHNICAL IMPLEMENTATION

### System Calls Used:
- **SYS_read (0)** - File and keyboard input
- **SYS_write (1)** - Console output
- **SYS_open (2)** - File opening
- **SYS_close (3)** - File closing
- **SYS_brk (12)** - Memory allocation
- **SYS_exit (60)** - Program termination

### Memory Management:
- **Custom allocator** using brk system call
- **Placement new** for object construction
- **No free()** - simplified allocator model
- **Automatic cleanup** through destructors

### String Operations:
All implemented manually:
- Length calculation
- Comparison (lexicographic)
- Concatenation
- Substring extraction
- Trimming whitespace
- Case conversion
- Search functionality

## 🎓 EDUCATIONAL VALUE

This implementation demonstrates:
- **Systems Programming** - Direct OS interface
- **Advanced OOP** - Proper encapsulation and abstraction
- **Memory Management** - Custom allocation strategies
- **Algorithm Implementation** - Sorting, parsing, searching
- **Assembly Integration** - System call interface
- **Error Handling** - Robust input validation

## 🐛 TROUBLESHOOTING

### Build Issues:
```bash
# If compilation fails
make compile_test  # Test compilation only
make check_libs    # Verify no forbidden includes
```

### Runtime Issues:
- Ensure Linux x86_64 system (uses Linux syscalls)
- Check file permissions for BibTeX files
- Verify input files are in valid BibTeX format

### Memory Issues:
- Program uses brk() for memory allocation
- Large files may require system memory limits adjustment
- No memory leaks as allocator doesn't free individual blocks

## 📈 PERFORMANCE CHARACTERISTICS

- **Memory**: O(n) where n is total content size
- **Parsing**: O(n) linear scan of file
- **Sorting**: O(n log n) quicksort implementation
- **Search**: O(n*m) substring search where m is institute name length

## 🚀 ADVANCED FEATURES

### Demonstrated OOP Concepts:
- **Encapsulation** - Private data members
- **Data Hiding** - Public interfaces only
- **Abstraction** - High-level operations on complex data
- **Operator Overloading** - Natural syntax for operations
- **Resource Management** - RAII principles
- **Composition** - Classes containing other classes

### System Integration:
- **No Library Dependencies** - Completely self-contained
- **Direct Hardware Interface** - Via system calls
- **Efficient I/O** - Minimal system call overhead
- **Portable Assembly** - Standard x86_64 Linux interface

## 📝 ASSIGNMENT COMPLETION

This implementation represents the **ultimate interpretation** of "no libraries allowed" - it proves that sophisticated OOP programs can be built using only the most basic system primitives. Every function, every operation, every I/O action is implemented from scratch while maintaining clean, readable, and maintainable code structure.

The result is a fully functional BibTeX parser that meets all assignment requirements while demonstrating advanced systems programming concepts and pure OOP design principles.
