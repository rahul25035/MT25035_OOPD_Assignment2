# Makefile for BibTeX Parser (C++ version without standard libraries)
# Object-Oriented Programming and Design Assignment 2

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -O2 -fno-exceptions -fno-rtti
LDFLAGS = -nostdlib -lc

# Target executable
TARGET = bib-parser

# Source files
SOURCES = main.cpp mystring.cpp author.cpp bibentry.cpp bibdatabase.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Header files (for dependencies)
HEADERS = mystring.h author.h bibentry.h bibdatabase.h

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build successful!"

# Compile source files to object files
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Dependencies
main.o: main.cpp bibdatabase.h
mystring.o: mystring.cpp mystring.h
author.o: author.cpp author.h mystring.h
bibentry.o: bibentry.cpp bibentry.h mystring.h author.h
bibdatabase.o: bibdatabase.cpp bibdatabase.h bibentry.h mystring.h author.h

# Clean target
clean:
	@echo "Cleaning up..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete!"

# Install target (optional)
install: $(TARGET)
	@echo "Installing $(TARGET)..."
	cp $(TARGET) /usr/local/bin/
	@echo "Installation complete!"

# Test target
test: $(TARGET)
	@echo "Running test..."
	./$(TARGET) ref.bib_doi.bib "IIITD"
	@echo "Test complete!"

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Help target
help:
	@echo "Available targets:"
	@echo "  all     - Build the project (default)"
	@echo "  clean   - Remove object files and executable"
	@echo "  test    - Build and run a simple test"
	@echo "  debug   - Build with debug symbols"
	@echo "  install - Install the executable to /usr/local/bin"
	@echo "  help    - Show this help message"

# Phony targets
.PHONY: all clean test debug install help

# Additional information
info:
	@echo "=== Project Information ==="
	@echo "Project: BibTeX Parser (C++ OOP Implementation)"
	@echo "Assignment: OOPD Assignment 2 - Monsoon 2025"
	@echo "Features:"
	@echo "  - No standard C/C++ libraries used"
	@echo "  - Custom string and container classes"
	@echo "  - OOP principles (encapsulation, inheritance, polymorphism)"
	@echo "  - Operator overloading for sorting and merging"
	@echo "  - Memory management with constructors/destructors"
	@echo "  - Input validation and error handling"
	@echo ""
	@echo "Usage: ./$(TARGET) <bib_file> <institute_name>"
	@echo "Example: ./$(TARGET) papers.bib "IIIT Delhi""
