# BibTeX Parser - C++ OOP Implementation

## Assignment Information

**Course:** Object-Oriented Programming and Design  
**Assignment:** Assignment 2 - Concepts of Object-Oriented Programming  
**Session:** Monsoon 2025  
**Instructor:** Arani Bhattacharya  

## Project Overview

This project is a complete C++ implementation of a BibTeX parser that converts the provided C code while strictly adhering to Object-Oriented Programming principles. The implementation is built **without using any standard C/C++ libraries** as per assignment requirements.

## Key Features

### 1. Object-Oriented Design
- **Data Abstraction**: Custom classes encapsulate data and behavior
- **Data Hiding**: Private members with controlled public interfaces
- **Encapsulation**: Related data and methods grouped together
- **No global variables**: All functionality within appropriate classes

### 2. Custom Implementation (No Standard Libraries)
- **MyString**: Complete string class replacing `std::string`
- **MyVector**: Dynamic array template replacing `std::vector`
- **Custom memory management**: Manual `malloc`/`free` operations
- **Custom file I/O**: Direct system calls for file operations
- **Custom string functions**: `strlen`, `strcpy`, `strcmp`, etc.

### 3. Core Classes

#### MyString Class (`mystring.h`, `mystring.cpp`)
- Dynamic memory management
- Operator overloading (`+`, `+=`, `==`, `<`, etc.)
- String manipulation methods (find, substr, trim, etc.)
- No fixed-length limitations

#### Author Class (`author.h`, `author.cpp`)
- Represents individual authors with name and affiliation
- Institute affiliation checking
- Parsing of author fields from BibTeX

#### BibEntry Class (`bibentry.h`, `bibentry.cpp`)
- Represents a single bibliography entry
- All standard BibTeX fields supported
- **Additional URL fields**: PDF, source code, presentation URLs
- Sorting support with `<` operator (year descending, title ascending)
- Input validation for years, DOIs, and URLs

#### BibDatabase Class (`bibdatabase.h`, `bibdatabase.cpp`)
- Container for multiple BibEntry objects
- **Operator overloading**: `+` and `+=` for database merging
- File parsing and saving capabilities
- Searching and filtering operations

### 4. Assignment Requirements Fulfilled

✅ **Convert C to C++**: Complete rewrite using OOP principles  
✅ **OOP Principles**: Data abstraction, hiding, and encapsulation implemented  
✅ **URL Fields**: PDF, code, and presentation URLs added and retained  
✅ **No Fixed Strings**: Dynamic MyString class replaces C-style strings  
✅ **Sorting Operator**: `<` operator for `<year descending, title ascending>` sorting  
✅ **Merging Operator**: `+` operator for combining bibliography databases  
✅ **Constructors/Destructors**: Proper memory management implemented  
✅ **Input Validation**: Year, DOI, URL, and other field validation  
✅ **Operator Overloading**: Multiple operators implemented as required  
✅ **No Standard Libraries**: Complete custom implementation  
✅ **Multiple Files**: 4 classes across 8+ files (headers and implementations)  

## File Structure

```
bib-parser/
├── mystring.h          # Custom string class header
├── mystring.cpp        # Custom string class implementation  
├── author.h            # Author class header
├── author.cpp          # Author class implementation
├── bibentry.h          # Bibliography entry class header  
├── bibentry.cpp        # Bibliography entry class implementation
├── bibdatabase.h       # Database container class header
├── bibdatabase.cpp     # Database container class implementation
├── main.cpp            # Main program with demonstrations
├── Makefile            # Build configuration
├── README.md           # This documentation file
└── ref.bib_doi.bib     # Sample BibTeX file for testing
```

## Building and Running

### Prerequisites
- Linux environment with GCC/G++ compiler
- Make utility
- No additional libraries required

### Building
```bash
# Build the project
make

# Clean build files
make clean

# Build with debug symbols
make debug

# Run tests
make test

# Show help
make help
```

### Running the Program
```bash
# Basic usage
./bib-parser <bib_file> <institute_name>

# Example with provided test file
./bib-parser ref.bib_doi.bib "IIITD"

# Example with other institutes
./bib-parser ref.bib_doi.bib "MIT"
./bib-parser ref.bib_doi.bib "University of California"
```

### Expected Output
The program will:
1. Parse the BibTeX file and load entries
2. Display parsing progress and summary
3. Count and display authors from the specified institute
4. Demonstrate sorting by year (descending) and title (ascending)
5. Demonstrate database merging using `+` operator
6. Show validation and error handling

## Technical Implementation Details

### Memory Management
- Custom memory allocation using `malloc`/`free`
- RAII principles with constructors/destructors
- No memory leaks (all allocated memory is properly freed)

### Error Handling
- Input validation for all user inputs
- File operation error checking
- Graceful handling of malformed BibTeX entries
- Comprehensive error messages

### Performance Considerations
- Dynamic memory allocation only when needed
- Efficient string operations
- Simple but effective sorting algorithm (bubble sort for educational purposes)

### Compliance with Assignment Requirements
- **No standard libraries**: Only system calls used
- **OOP principles**: Proper encapsulation, abstraction, and data hiding
- **Operator overloading**: `<`, `+`, `+=`, `==`, `!=` operators implemented
- **Input validation**: Years, DOIs, URLs validated
- **Multiple classes**: 4 distinct classes with clear responsibilities

## Testing

The program has been tested with:
- The provided `ref.bib_doi.bib` file
- Various institute names and search patterns
- Edge cases (empty files, malformed entries, etc.)
- Memory leak testing (valgrind compatible)
- Large BibTeX files for performance testing

## Limitations and Future Improvements

### Current Limitations
- Simple sorting algorithm (O(n²) bubble sort)
- Basic pattern matching for institute affiliation
- Limited BibTeX format variations supported

### Potential Improvements
- More efficient sorting algorithms (quicksort, mergesort)
- Advanced pattern matching for institute names
- Support for more BibTeX entry types and fields
- Better error recovery for malformed entries

## Author Information

This implementation demonstrates advanced C++ programming concepts including:
- Custom container and string classes
- Template programming (MyVector)
- Operator overloading
- Memory management
- File I/O without standard libraries
- Object-oriented design patterns

The code is written to be educational and follows best practices for C++ development while meeting all assignment requirements.
