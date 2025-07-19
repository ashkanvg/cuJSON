// #include <iostream>
#include "simdjson.h"
#include <time.h>
#include <string>
#include <chrono>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#define N 4

using namespace std;

using namespace simdjson;

double calcTimeSplit(string fileNameChunk);
double calcTime(string fileNam);


int main(void) {
    // #ifdef SIMDJSON_THREADS_ENABLED
    //     cout << "enable";
    // ALL TOGETHER
    string fileName = "../../../dataset/bestbuy_small_records_remove.json";
    calcTime(fileName);
    fileName = "../../../dataset/nspl_small_records_remove.json";
    calcTime(fileName);
    fileName = "../../../dataset/twitter_small_records_remove.json";
    calcTime(fileName);
    fileName = "../../../dataset/google_map_small_records_remove.json";
    calcTime(fileName);    
    fileName = "../../../dataset/wiki_small_records_remove.json";
    calcTime(fileName);
    fileName = "../../../dataset/walmart_small_records_remove.json";
    calcTime(fileName);

    return 0;
    
}



double calcTime(string fileName){
    cout << "FILE NAME:" << fileName << endl;

    time_t start, end;
    time_t start_load, end_load;

    start_load = clock();
    padded_string json = padded_string::load(fileName);
    end_load = clock();

    start = clock();
    simdjson::simdjson_result<simdjson::dom::element> query_res;
    simdjson::dom::parser parser;
    simdjson::dom::document_stream stream;
    auto error = parser.parse_many(json,json.size()).get(stream);
    if (error) { std::cerr << error << std::endl; return 0; }

    for( auto i = stream.begin() ; i != stream.end(); ++i) {
        auto doc = *i;
        if (!doc.error()) {
            continue;
        } else {
            cout << "got broken document at " << i.current_index() << endl;
            return 0;
        }
    }

    end = clock();

    std::cout << "load: " << ((double)(end_load-start_load)/CLOCKS_PER_SEC) << std::endl;
    std::cout << "total: " << ((double)(end-start)/CLOCKS_PER_SEC) << std::endl;

    return 1;
}
