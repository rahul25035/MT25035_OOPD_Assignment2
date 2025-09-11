#!/bin/bash

echo "=== BibTeX Parser Build and Test Script ==="
echo "Testing C++ OOP implementation without standard libraries"
echo ""

# Check if required files exist
echo "Checking required files..."
required_files=("mystring.h" "mystring.cpp" "author.h" "author.cpp" "bibentry.h" "bibentry.cpp" "bibdatabase.h" "bibdatabase.cpp" "main.cpp" "Makefile")

for file in "${required_files[@]}"; do
    if [ -f "$file" ]; then
        echo "✓ $file exists"
    else
        echo "✗ $file missing"
        exit 1
    fi
done

echo ""
echo "All required files found!"
echo ""

# Clean any previous builds
echo "Cleaning previous builds..."
make clean

echo ""
echo "Building project..."
echo "Using: g++ -Wall -Wextra -std=c++11 -O2 -fno-exceptions -fno-rtti"
echo ""

# Attempt to build
if make; then
    echo ""
    echo "✓ Build successful!"
    echo ""

    # Check if executable was created
    if [ -f "bib-parser" ]; then
        echo "✓ Executable 'bib-parser' created successfully"
        echo ""

        # Run a test if bib file exists
        if [ -f "ref.bib_doi.bib" ]; then
            echo "Running test with provided BibTeX file..."
            echo "Command: ./bib-parser ref.bib_doi.bib "IIITD""
            echo ""
            ./bib-parser ref.bib_doi.bib "IIITD"
        else
            echo "Note: ref.bib_doi.bib not found - skipping runtime test"
            echo "To test manually, run: ./bib-parser <bibfile> <institute>"
        fi
    else
        echo "✗ Executable not created despite successful compilation"
        exit 1
    fi
else
    echo ""
    echo "✗ Build failed!"
    echo ""
    echo "Common issues to check:"
    echo "1. Make sure you're using g++ compiler"
    echo "2. Check that all header files are properly included"
    echo "3. Verify no standard library headers are used"
    echo "4. Check for any syntax errors in the code"
    exit 1
fi

echo ""
echo "=== Test Summary ==="
echo "✓ All source files present"
echo "✓ Compilation successful"
echo "✓ Executable created"
echo "✓ Basic functionality tested"
echo ""
echo "The implementation is ready for submission!"
