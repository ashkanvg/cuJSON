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

using namespace std;
using namespace std::chrono;
// using namespace simdjson::haswell::stage1;


using namespace simdjson;
// using namespace simdjson::haswell::utf8_validation;

double calcTimeSplit(string fileNameChunk);
double calcTime(string fileNam, int which);

// void printMemoryUsage() {
//     struct mallinfo mi = mallinfo();
//     std::cout << "Total non-mmapped bytes allocated (arena): " << mi.arena / 1024 << " KB\n";
//     std::cout << "Number of free chunks (ordblks): " << mi.ordblks << "\n";
//     std::cout << "Number of fastbin blocks (smblks): " << mi.smblks << "\n";
//     std::cout << "Number of mapped regions (hblks): " << mi.hblks << "\n";
//     std::cout << "Total allocated space (uordblks): " << mi.uordblks / 1024 << " KB\n";
//     std::cout << "Total free space (fordblks): " << mi.fordblks / 1024 << " KB\n";
//     std::cout << "Top-most, releasable (via malloc_trim) space (keepcost): " << mi.keepcost / 1024 << " KB\n";
// }

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
    string fileName = "../Test-Files/Pison Large Datasets/nspl_large_record.json";
    calcTime(fileName, 0);
    // calcTime(fileName, 1);
    
    fileName = "../Test-Files/Pison Large Datasets/twitter_large_record.json";
    calcTime(fileName, 1);
    // calcTime(fileName, 2);
    // calcTime(fileName, 3);
    // calcTime(fileName, 4);

    fileName = "../Test-Files/Pison Large Datasets/walmart_large_record.json";
    calcTime(fileName, 5);

    fileName = "../Test-Files/Pison Large Datasets/wiki_large_record.json";
    calcTime(fileName, 6);
    // calcTime(fileName, 7);

    fileName = "../Test-Files/Pison Large Datasets/google_map_large_record.json";
    calcTime(fileName, 8);    
    // calcTime(fileName, 9);    
    
    fileName = "../Test-Files/Pison Large Datasets/bestbuy_large_record.json";
    calcTime(fileName, 10);
    // calcTime(fileName, 11);

    return 0;
    
}



double calcTime(string fileName, int which){
    cout << "FILE NAME:" << fileName << endl;
    time_t start, end;
    time_t start_load, end_load;

    ondemand::parser parser;

    start_load = clock();
    padded_string json = padded_string::load(fileName);
    end_load = clock();


    start = clock();
    ondemand::document tweets = parser.iterate(json);
    end = clock();

    simdjson::simdjson_result<simdjson::dom::element> query_res;
    

    std::cout << "load: " << ((double)(end_load-start_load)/CLOCKS_PER_SEC) << std::endl;
    std::cout << "total: " << ((double)(end-start)/CLOCKS_PER_SEC) << std::endl;

    return 1;
}
