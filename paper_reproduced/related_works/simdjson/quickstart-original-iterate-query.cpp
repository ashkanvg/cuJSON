#include "simdjson.h"
#include <iostream>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace simdjson;

double calcTime(string fileName, int which);

int main(void) {
    vector<pair<string, vector<int>>> files_and_queries = {
        {"../../dataset/nspl_large_record.json", {0}},
        {"../../dataset/twitter_large_record.json", {1, 2, 3, 4}},
        {"../../dataset/walmart_large_record.json", {5}},
        {"../../dataset/wiki_large_record.json", {6, 7}},
        {"../../dataset/google_map_large_record.json", {8, 9}},
        {"../../dataset/bestbuy_large_record.json", {10, 11}},
    };

    double sum = 0.0;
    int count = 0;
    for (const auto& entry : files_and_queries) {
        const string& fileName = entry.first;
        for (int query_id : entry.second) {
            count++;
            sum += calcTime(fileName, query_id);
        }
    }
    cout << "AVERAGE," << (sum / count) << endl;

    return 0;
}


double calcTime(string fileName, int which) {
    // cout << "FILE NAME: " << fileName << ", Query: " << which << endl;

    ondemand::parser parser;
    padded_string json = padded_string::load(fileName);

    double total_time_ns = 0.0;

    ondemand::document tweets = parser.iterate(json);

    auto startQ = high_resolution_clock::now();

    switch (which) {
        case 0:
            tweets["meta"]["view"]["columns"].at(0)["name"];
            break;
        case 1:
            tweets["user"]["lang"];
            tweets["lang"];
            break;
        case 2:
            tweets["user"]["id"];
            tweets["user"]["lang"];
            break;
        case 3:
            tweets["user"]["id"];
            break;
        case 4:
            tweets["entities"]["urls"].at(0)["indices"].at(0);
            break;
        case 5:
            tweets["bestMarketplacePrice"]["price"];
            tweets["items"]["name"];
            break;
        case 6:
            tweets["descriptions"];
            break;
        case 7:
            tweets["claims"]["P1245"]["mainsnak"]["property"];
            break;
        case 8:
            tweets["routes"];
            break;
        case 9:
            tweets["routes"].at(0)["legs"].at(0)["steps"].at(0)["distance"]["text"];
            break;
        case 10:
            tweets["products"].at(0)["regularPrice"];
            break;
        case 11:
            tweets["products"].at(0)["categoryPath"].at(1)["id"];
            tweets["products"].at(0)["categoryPath"].at(2)["id"];
            tweets["products"].at(0)["categoryPath"].at(3)["id"];
            break;
    }

    auto stopQ = high_resolution_clock::now();
    auto duration_ns = duration_cast<duration<double, std::nano>>(stopQ - startQ);
    total_time_ns += duration_ns.count();
    

    // double avg_ms = total_time_ms / 10.0;
    // cout << "Average query time: " << avg_ms << " ms" << endl;
    return total_time_ns;
}
