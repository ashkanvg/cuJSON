#include "simdjson.h"
#include <string>
#include <chrono>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

using namespace std;
using namespace std::chrono;
using namespace simdjson;

void calcMemoryUsage(const string& fileName, const string& short_file_name);
void printMemoryUsage(const std::string& message, const std::string& fileName);

int main() {
    calcMemoryUsage("../../dataset/twitter_large_record.json", "TT");
    calcMemoryUsage("../../dataset/bestbuy_large_record.json", "BB");
    calcMemoryUsage("../../dataset/google_map_large_record.json", "GMD");
    calcMemoryUsage("../../dataset/nspl_large_record.json", "NSPL");
    calcMemoryUsage("../../dataset/walmart_large_record.json", "WM");
    calcMemoryUsage("../../dataset/wiki_large_record.json", "WP");
    return 0;
}

void calcMemoryUsage(const string& fileName, const string& short_file_name) {
    simdjson::ondemand::parser parser;

    // Load file into padded_string
    padded_string json = padded_string::load(fileName);

    // Run parsing once to trigger any allocations
    auto start = high_resolution_clock::now();
    simdjson::ondemand::document doc = parser.iterate(json);
    auto end = high_resolution_clock::now();

    // Output memory usage with input file size
    printMemoryUsage(short_file_name, fileName);
}

void printMemoryUsage(const std::string& message, const std::string& fileName) {
    std::ifstream file("/proc/self/statm");
    long size, resident;
    file >> size >> resident;

    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    long rss_kb = resident * page_size_kb;
    double rss_mb = rss_kb / 1024.0;

    // Compute file size in MB
    struct stat st;
    // double file_size_mb = 0.0;
    if (stat(fileName.c_str(), &st) == 0) {
        rss_mb += st.st_size / (1024.0 * 1024.0);
    }

    std::cout << message << "," << rss_mb << std::endl;
}


// #include "simdjson.h"
// #include <string>
// #include <chrono>
// #include <unistd.h>
// #include <iostream>
// #include <fstream>

// using namespace std;
// using namespace std::chrono;
// using namespace simdjson;

// void calcMemoryUsage(string fileName, string short_file_name);

// void printMemoryUsage(const std::string& message) {
//     std::ifstream file("/proc/self/statm");
//     long size, resident;
//     file >> size >> resident;

//     long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
//     long rss_kb = resident * page_size_kb;
//     double rss_mb = rss_kb / 1024.0;

//     std::cout << message << "," << rss_mb << std::endl;
// }

// int main() {
//     calcMemoryUsage("../../../dataset/twitter_large_record.json", "TT");
//     calcMemoryUsage("../../../dataset/bestbuy_large_record.json", "BB");
//     calcMemoryUsage("../../../dataset/google_map_large_record.json", "GMD");
//     calcMemoryUsage("../../../dataset/nspl_large_record.json", "NSPL");
//     calcMemoryUsage("../../../dataset/walmart_large_record.json", "WM");
//     calcMemoryUsage("../../../dataset/wiki_large_record.json", "WP");
//     return 0;
// }

// void calcMemoryUsage(string fileName, string short_file_name) {
//     simdjson::ondemand::parser parser;

//     // Load file into padded_string
//     padded_string json = padded_string::load(fileName);

//     // Run parsing once to trigger any allocations
//     auto start = high_resolution_clock::now();
//     simdjson::ondemand::document doc = parser.iterate(json);
//     auto end = high_resolution_clock::now();

//     // Output memory usage
//     printMemoryUsage(short_file_name);
// }


