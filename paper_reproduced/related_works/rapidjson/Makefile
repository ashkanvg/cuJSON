# Compiler and flags
CXX = g++
CXXFLAGS = -O3

# Source files
SOURCES = \
    ./main-bestbuy.cpp \
    ./main-nspl.cpp \
    ./main-wiki.cpp \
    ./main-walmart.cpp \
    ./main-google.cpp \
    ./main-twitter.cpp

# Executable files (outputs)
EXECUTABLES = \
    ./output-bestbuy.exe \
    ./output-nspl.exe \
    ./output-wiki.exe \
    ./output-walmart.exe \
    ./output-google.exe \
    ./output-twitter.exe

# Compile each source file to corresponding executable
all: $(EXECUTABLES)

./output-bestbuy.exe: ./main-bestbuy.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

./output-nspl.exe: ./main-nspl.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

./output-wiki.exe: ./main-wiki.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

./output-walmart.exe: ./main-walmart.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

./output-google.exe: ./main-google.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

./output-twitter.exe: ./main-twitter.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# Run all executables with a message before each run
run:
	@echo "Running Twitter:"; ./output-twitter.exe
	@echo "Running BestBuy:"; ./output-bestbuy.exe
	@echo "Running Google:"; ./output-google.exe
	@echo "Running NSPL:"; ./output-nspl.exe
	@echo "Running Walmart:"; ./output-walmart.exe
	@echo "Running Wiki:"; ./output-wiki.exe

# Clean all executables
clean:
	rm -f $(EXECUTABLES)
