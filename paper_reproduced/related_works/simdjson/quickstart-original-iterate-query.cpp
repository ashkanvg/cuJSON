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

using namespace simdjson;

double calcTimeSplit(string fileNameChunk);
double calcTime(string fileNam, int which);


int main(void) {

    // ALL TOGETHER
    string fileName = "../Test-Files/Pison Large Datasets/nspl_large_record.json";
    calcTime(fileName, 0);
    calcTime(fileName, 1);
    
    fileName = "../Test-Files/Pison Large Datasets/twitter_large_record.json";
    calcTime(fileName, 1);
    calcTime(fileName, 2);
    calcTime(fileName, 3);
    calcTime(fileName, 4);

    fileName = "../Test-Files/Pison Large Datasets/walmart_large_record.json";
    calcTime(fileName, 5);

    fileName = "../Test-Files/Pison Large Datasets/wiki_large_record.json";
    calcTime(fileName, 6);
    calcTime(fileName, 7);

    fileName = "../Test-Files/Pison Large Datasets/google_map_large_record.json";
    calcTime(fileName, 8);    
    calcTime(fileName, 9);    
    
    fileName = "../Test-Files/Pison Large Datasets/bestbuy_large_record.json";
    calcTime(fileName, 10);
    calcTime(fileName, 11);

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
    high_resolution_clock::time_point startQ, stopQ;

    cout << "Query\n";
    startQ = high_resolution_clock::now();
            
    switch(which){
        case 0:
            // nspl
            tweets["meta"]["view"]["columns"].at(0)["name"];
            // cout << name << endl;
            break;
        case 1:
            // tt1
            tweets["user"]["lang"];
            tweets["lang"];
            break;
        case 2:
            // tt2:
            tweets["user"]["id"];
            tweets["user"]["lang"];
            break;
        case 3:
            // tt3
            tweets["user"]["id"];
            break;
        case 4:
            // tt4
            tweets["entities"]["urls"].at(0)["indices"].at(0);
            break;
        case 5:
            // walmart
            tweets["bestMarketplacePrice"]["price"];
            tweets["items"]["name"];
            break;
        case 6:
            // wiki:
            tweets["descriptions"];
            break;
        case 7:
            // wiki2
            tweets["claims"]["P1245"]["mainsnak"]["property"];
            break;
        case 8:
            // gg
            tweets["routes"];
            break;
        case 9:
            // gg
            tweets["routes"].at(0)["legs"].at(0)["steps"].at(0)["distance"]["text"];
            break;
        case 10:
            // bb1
            tweets["products"].at(0)["regularPrice"];
            break;
        case 11:
            // bb2
            tweets["products"].at(0)["categoryPath"].at(1)["id"];
            tweets["products"].at(0)["categoryPath"].at(2)["id"];
            tweets["products"].at(0)["categoryPath"].at(3)["id"];            
            break;
    }

    stopQ = high_resolution_clock::now();
    auto elapsed = duration_cast<nanoseconds>(stopQ - startQ);
    cout << "Total processing time: " << elapsed.count() << " nanoseconds." << endl;

    std::cout << "load: " << ((double)(end_load-start_load)/CLOCKS_PER_SEC) << std::endl;
    std::cout << "total: " << ((double)(end-start)/CLOCKS_PER_SEC) << std::endl;

    return 1;
}
