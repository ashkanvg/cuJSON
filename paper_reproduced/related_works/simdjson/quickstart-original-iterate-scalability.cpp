// #include <iostream>
#include "simdjson.h"
#include <time.h>
#include <string>
#include <chrono>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#define N 4

#include <iostream>
#include <malloc.h>
#include <fstream>
#include <unistd.h>
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace simdjson;

double calcTime(string fileNam);

void printMemoryUsage(const std::string& message) {
    std::ifstream file("/proc/self/statm");
    long rss;
    file >> rss;

    // Convert pages to MB
    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    long resident_set = rss * page_size_kb;
    double resident_set_mb = resident_set / 1024.0;
    std::cout << message << " - Memory Usage: " << resident_set_mb << " MB\n";
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Please provide size in MB (e.g., 2, 4, 8...)\n";
        return 1;
    }

    string size_str = argv[1];
    string sizeMB = size_str + "MB";
    vector<string> datasets = {"tt", "bb", "gmp", "nspl", "wm", "wp"};

    for (const string& key : datasets) {
        string fileName = "/rhome/aveda002/bigdata/Test-Files/scalability/" + key + "/output_" + sizeMB + "_large.json";

        double totalTime = 0.0;
        for (int i = 0; i < 10; i++) {
            totalTime += calcTime(fileName);
        }
        cout << key << "," << (totalTime / 10.0) << endl;
    }

    return 0;
}



double calcTime(string fileName){
    // cout << "FILE NAME:" << fileName << endl;
    ondemand::parser parser;


    // start_load = clock();
    padded_string json = padded_string::load(fileName);
    // end_load = clock();

    // Measure parse time
    auto start = high_resolution_clock::now();
    
    simdjson::ondemand::document tweets = parser.iterate(json);
    auto end = high_resolution_clock::now();

    simdjson::simdjson_result<simdjson::dom::element> query_res;

    std::chrono::duration<double, std::milli> parse_duration = end - start;
    return parse_duration.count();
}

