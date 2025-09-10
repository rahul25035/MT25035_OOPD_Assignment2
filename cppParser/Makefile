# Makefile for BibTeX Parser - NO STANDARD LIBRARIES VERSION
# Object-Oriented Programming Assignment 2

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -g -O2
TARGET = bib_parser
SOURCES = main.cpp
HEADERS = SystemInterface.h String.h Author.h Publication.h Bibliography.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)
	@echo "========================================="
	@echo "Build completed"
	@echo "========================================="

compile_test: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o
	@echo "Compilation successful!"
	@rm -f main.o

check_libs:
	@echo "Checking for standard library usage..."
	@echo "Searching for forbidden includes:"
	@grep -n "#include <" *.h *.cpp || echo "✅ No standard library includes found!"
	@echo "Checking object file dependencies:"
	@if [ -f $(TARGET) ]; then \
		ldd $(TARGET) 2>/dev/null || echo "✅ Statically linked - no shared library dependencies"; \
	else \
		echo "Build the program first with 'make'"; \
	fi

sample_files:
	@echo "Creating sample.bib for testing..."
	@cat > sample.bib << 'EOF'
@article{doe2023ai,
  title={Artificial Intelligence and Machine Learning},
  author={John Doe and Jane MIT Smith and Alice Stanford Brown},
  journal={IEEE Transactions on AI},
  year={2023},
  volume={10},
  pages={123-145},
  pdf={http://example.com/ai_paper.pdf},
  code={http://github.com/johndoe/ai-ml}
}

@inproceedings{smith2024deep,
  title={Deep Learning Applications},
  author={Jane MIT Smith and Bob Stanford Wilson},
  booktitle={Conference on Neural Networks},
  year={2024},
  pages={67-89},
  slides={http://example.com/deep_learning_slides.pdf}
}

@article{brown2022data,
  title={Data Science Methods},
  author={Alice Stanford Brown and Charlie MIT Davis},
  journal={Data Science Journal},
  year={2022},
  volume={15},
  pages={234-267},
  pdf={http://example.com/data_science.pdf}
}

@phdthesis{wilson2025quantum,
  title={Quantum Computing Research},
  author={Bob Stanford Wilson},
  year={2025},
  school={Stanford University},
  pdf={http://example.com/quantum_thesis.pdf},
  code={http://github.com/bobwilson/quantum}
}
EOF
	@echo "Created sample.bib"
	@echo ""
	@cat > sample2.bib << 'EOF'
@article{johnson2021ml,
  title={Machine Learning Fundamentals},
  author={David MIT Johnson},
  journal={ML Review},
  year={2021},
  volume={8},
  pages={45-67},
  pdf={http://example.com/ml_fundamentals.pdf}
}

@inproceedings{davis2024ai,
  title={AI Ethics and Society},
  author={Emma Stanford Davis and Frank MIT Lee},
  booktitle={AI Ethics Conference},
  year={2024},
  pages={12-34},
  slides={http://example.com/ai_ethics.pdf}
}
EOF
	@echo "Created sample2.bib"

test_mit: $(TARGET) sample_files
	@echo "=== Testing with MIT institute ==="
	./$(TARGET) sample.bib "MIT"

test_stanford: $(TARGET) sample_files
	@echo "=== Testing with Stanford institute ==="
	./$(TARGET) sample.bib "Stanford"

clean:
	rm -f $(TARGET) *.o sample.bib sample2.bib

rebuild: clean all

.PHONY: all compile_test check_libs sample_files test_mit test_stanford clean rebuild
