// main.cpp - Main program following original C logic exactly
#include "SystemInterface.h"
#include "String.h"
#include "Author.h"
#include "Publication.h"
#include "Bibliography.h"

// Input/Output helpers (no scanf/printf)
class IOHelper {
public:
    static String readLine() {
        char buffer[1024];
        int pos = 0;
        char c;

        while (SystemInterface::read(STDIN_FILENO, &c, 1) > 0 && c != '\n' && pos < 1023) {
            buffer[pos++] = c;
        }
        buffer[pos] = '\0';

        return String(buffer);
    }

    static void printUsage(const String& programName) {
        SystemInterface::write(STDOUT_FILENO, "Usage: ", 7);
        programName.print();
        SystemInterface::write(STDOUT_FILENO, " <bib_file> <institute_name>\n", 30);
        SystemInterface::write(STDOUT_FILENO, "Example: ", 9);
        programName.print();
        SystemInterface::write(STDOUT_FILENO, " papers.bib \"MIT\"\n", 19);
        SystemInterface::write(STDOUT_FILENO, "Example: ", 9);
        programName.print();
        SystemInterface::write(STDOUT_FILENO, " papers.bib \"University of California\"\n", 41);
    }

    static void printNumber(int n) {
        if (n == 0) {
            SystemInterface::write(STDOUT_FILENO, "0", 1);
            return;
        }

        char buffer[32];
        int i = 0;
        bool negative = false;

        if (n < 0) {
            negative = true;
            n = -n;
        }

        while (n > 0) {
            buffer[i++] = '0' + (n % 10);
            n /= 10;
        }

        if (negative) {
            buffer[i++] = '-';
        }

        // Reverse and print
        for (int j = i - 1; j >= 0; j--) {
            SystemInterface::write(STDOUT_FILENO, &buffer[j], 1);
        }
    }
};

// Function to parse command line arguments (no standard argc/argv parsing)
struct CommandLineArgs {
    String program_name;
    String bib_file;
    String institute_name;
    bool valid;

    CommandLineArgs() : valid(false) {}
};

CommandLineArgs parseCommandLine(int argc, char* argv[]) {
    CommandLineArgs args;

    if (argc != 3) {
        args.valid = false;
        if (argc > 0) {
            args.program_name = String(argv[0]);
        }
        return args;
    }

    args.program_name = String(argv[0]);
    args.bib_file = String(argv[1]);
    args.institute_name = String(argv[2]);
    args.valid = true;

    return args;
}

// Main parsing function following original C logic exactly
int parseBibFile(const String& filename, const String& instituteName) {
    Bibliography bib;

    if (!bib.loadFromBibFile(filename)) {
        return -1;
    }

    // Count and display institute authors (following original C format)
    int result = bib.countAuthorsFromInstitute(instituteName);

    // Print summary following original C format
    bib.printSummary(instituteName);

    return result;
}

// Demo functions to show additional features
void demonstrateOOPFeatures() {
    SystemInterface::write(STDOUT_FILENO, "\n=== Demonstrating OOP Features ===\n", 37);

    // Test sorting (year desc, title asc)
    SystemInterface::write(STDOUT_FILENO, "\nTesting sorting (year desc, title asc):\n", 42);

    Bibliography bib;

    Publication pub1("Z Paper", "2020");
    pub1.addAuthor(Author("John Doe"));
    bib.addPublication(pub1);

    Publication pub2("A Paper", "2023");
    pub2.addAuthor(Author("Jane Smith"));
    bib.addPublication(pub2);

    Publication pub3("M Paper", "2023");
    pub3.addAuthor(Author("Bob Johnson"));
    bib.addPublication(pub3);

    SystemInterface::write(STDOUT_FILENO, "Before sorting:\n", 16);
    bib.print();

    bib.sort();

    SystemInterface::write(STDOUT_FILENO, "\nAfter sorting:\n", 16);
    bib.print();

    // Test + operator
    SystemInterface::write(STDOUT_FILENO, "\nTesting + operator:\n", 21);

    Bibliography bib2;
    Publication pub4("Additional Paper", "2024");
    pub4.addAuthor(Author("Alice Brown"));
    bib2.addPublication(pub4);

    Bibliography combined = bib + bib2;
    SystemInterface::write(STDOUT_FILENO, "Combined bibliography:\n", 23);
    combined.print();

    // Test URL fields
    SystemInterface::write(STDOUT_FILENO, "\nTesting URL fields:\n", 21);
    Publication pubWithUrls("Paper with URLs", "2025");
    pubWithUrls.setPdfUrl("http://example.com/paper.pdf");
    pubWithUrls.setSourceCodeUrl("http://github.com/user/code");
    pubWithUrls.setPresentationUrl("http://example.com/slides.pdf");
    pubWithUrls.printDetailed();
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    CommandLineArgs args = parseCommandLine(argc, argv);

    if (!args.valid) {
        IOHelper::printUsage(args.program_name);
        return 1;
    }

    // Main parsing logic following original C exactly
    int result = parseBibFile(args.bib_file, args.institute_name);

    if (result >= 0) {
        SystemInterface::write(STDOUT_FILENO, "\nProgram completed successfully.\n", 33);

        // Show additional OOP features
        SystemInterface::write(STDOUT_FILENO, "\nWould you like to see OOP demonstration? (y/n): ", 50);
        String response = IOHelper::readLine();
        if (!response.empty() && (response[0] == 'y' || response[0] == 'Y')) {
            demonstrateOOPFeatures();
        }

        return 0;
    } else {
        SystemInterface::write(STDOUT_FILENO, "\nProgram failed with errors.\n", 30);
        return 1;
    }
}
