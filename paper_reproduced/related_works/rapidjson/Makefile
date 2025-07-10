# Compiler and flags
CXX = g++
CXXFLAGS = -O3

# Source files
SOURCES = \
    ./related_works/rapidjson/main-bestbuy.cpp \
    ./related_works/rapidjson/main-nspl.cpp \
    ./related_works/rapidjson/main-wiki.cpp \
    ./related_works/rapidjson/main-walmart.cpp \
    ./related_works/rapidjson/main-google.cpp \
    ./related_works/rapidjson/main-twitter.cpp

# Executable files (outputs)
EXECUTABLES = \
    ./related_works/rapidjson/output-bestbuy.exe \
    ./related_works/rapidjson/output-nspl.exe \
    ./related_works/rapidjson/output-wiki.exe \
    ./related_works/rapidjson/output-walmart.exe \
    ./related_works/rapidjson/output-google.exe \
    ./related_works/rapidjson/output-twitter.exe

# Compile each source file to corresponding executable
all: $(EXECUTABLES)

./related_works/rapidjson/output-bestbuy.exe: ./related_works/rapidjson/main-bestbuy.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

./related_works/rapidjson/output-nspl.exe: ./related_works/rapidjson/main-nspl.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

./related_works/rapidjson/output-wiki.exe: ./related_works/rapidjson/main-wiki.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

./related_works/rapidjson/output-walmart.exe: ./related_works/rapidjson/main-walmart.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

./related_works/rapidjson/output-google.exe: ./related_works/rapidjson/main-google.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

./related_works/rapidjson/output-twitter.exe: ./related_works/rapidjson/main-twitter.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# Run all executables with a message before each run
run:
	@echo "Running BestBuy:"; ./related_works/rapidjson/output-bestbuy.exe
	@echo "Running NSPL:"; ./related_works/rapidjson/output-nspl.exe
	@echo "Running Wiki:"; ./related_works/rapidjson/output-wiki.exe
	@echo "Running Walmart:"; ./related_works/rapidjson/output-walmart.exe
	@echo "Running Google:"; ./related_works/rapidjson/output-google.exe
	@echo "Running Twitter:"; ./related_works/rapidjson/output-twitter.exe

# Clean all executables
clean:
	rm -f $(EXECUTABLES)
