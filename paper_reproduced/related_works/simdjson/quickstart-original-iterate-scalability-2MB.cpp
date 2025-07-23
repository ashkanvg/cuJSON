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

double calcTime(string fileNam, int which);

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


int main(void) {

    // ALL TOGETHER
    string fileName;
    double totalTime = 0;
    double totalTimeTogether = 0;

    totalTime = 0;
    for(int i = 0; i < 10; i++){
        fileName = "/rhome/aveda002/bigdata/Test-Files/scalability/tt/output_2MB_large.json";
        // fileName = "/rhome/aveda002/bigdata/Test-Files/twitter_large_record.json";
        totalTime += calcTime(fileName, 0);
    }
    // cout << "TT, " << totalTime / 10.0 << endl;
    totalTimeTogether+= totalTime / 10.0;
   
    totalTime = 0;
    for(int i = 0; i < 10; i++){
        fileName = "/rhome/aveda002/bigdata/Test-Files/scalability/bb/output_2MB_large.json";
        // fileName = "/rhome/aveda002/bigdata/Test-Files/bestbuy_large_record.json";
        totalTime += calcTime(fileName, 0);
    }
    // cout << "BB, " << totalTime / 10.0 << endl;
    totalTimeTogether+= totalTime / 10.0;


    totalTime = 0;
    for(int i = 0; i < 10; i++){
        fileName = "/rhome/aveda002/bigdata/Test-Files/scalability/gmp/output_2MB_large.json";
        // fileName = "/rhome/aveda002/bigdata/Test-Files/google_map_large_record.json";
        totalTime += calcTime(fileName, 0);
    }
    // cout << "GMD, " << totalTime / 10.0 << endl;
    totalTimeTogether+= totalTime / 10.0;
  
    
    totalTime = 0;
    for(int i = 0; i < 10; i++){
        fileName = "/rhome/aveda002/bigdata/Test-Files/scalability/nspl/output_2MB_large.json";
        // fileName = "/rhome/aveda002/bigdata/Test-Files/nspl_large_record.json";
        totalTime += calcTime(fileName, 0);
    }
    // cout << "NSPL, " << totalTime / 10.0 << endl;
    totalTimeTogether+= totalTime / 10.0;

        
    totalTime = 0;
    for(int i = 0; i < 10; i++){
        fileName = "/rhome/aveda002/bigdata/Test-Files/scalability/wm/output_2MB_large.json";
        totalTime += calcTime(fileName, 0);
    }
    // cout << "WM, " << totalTime / 10.0 << endl;
    totalTimeTogether+= totalTime / 10.0;
   

    totalTime = 0;
    for(int i = 0; i < 10; i++){
        fileName = "/rhome/aveda002/bigdata/Test-Files/scalability/wp/output_2MB_large.json";
        totalTime += calcTime(fileName, 0);
    }
    // cout << "WP, " << totalTime / 10.0 << endl;
    totalTimeTogether+= totalTime / 10.0;
    cout << "2MB, " << totalTimeTogether / 6.0 << endl;
    return 0;
}



double calcTime(string fileName, int which){
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
