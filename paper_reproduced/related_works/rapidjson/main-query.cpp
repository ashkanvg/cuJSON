#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/filereadstream.h"
#include "include/rapidjson/error/en.h"

#include <iostream>
#include <chrono>
#include <string>
#include <cstdio>
#include <vector>
#include <map>

using namespace rapidjson;
using namespace std;
using namespace std::chrono;

double calcFunction(const char* fileName, int which);

int main() {
    map<const char*, vector<int>> files_and_queries = {
        // {"../../../dataset/nspl_large_record.json", {0}},
        {"../../dataset/twitter_large_record.json", {1, 2, 3, 4}},
        {"../../dataset/walmart_large_record.json", {5}},
        {"../../dataset/wiki_large_record.json", {6, 7}},
        {"../../dataset/google_map_large_record.json", {8, 9}},
        {"../../dataset/bestbuy_large_record.json", {10}}
    };



    int count = 0;
    double sum = 0.0;
    for (const auto& entry : files_and_queries) {
        const char* fileName = entry.first;
        for (int query_id : entry.second) {
            // cout << "Processing file: " << fileName << ", Query ID: " << query_id << endl;
            count++;
            sum += calcFunction(fileName, query_id);
        }
    }
    cout << "AVERAGE," << (sum / count) << endl;

    return 0;
}

double calcFunction(const char* fileName, int which) {

    double total_time_ns = 0.0;

    FILE* fp = fopen(fileName, "r");
    if (!fp) {
        cerr << "Error: unable to open file" << endl;
        return 0.0;
    }

    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    Document doc;
    doc.ParseStream(is);

    if (doc.HasParseError()) {
        cerr << "Error parsing JSON: " << GetParseError_En(doc.GetParseError()) << endl;
        fclose(fp);
        return 0.0;
    }

    auto startQ = high_resolution_clock::now();

    switch (which) {
        // case 0: {
        //     Value& s0 = doc["meta"]["view"]["columns"][0]["name"];
        //     break;
        // }
        case 1: {
            Value& s1 = doc[0]["user"]["lang"];
            Value& s11 = doc[0]["lang"];
            break;
        }
        case 2: {
            Value& s2 = doc[0]["user"]["id"];
            Value& s22 = doc[0]["user"]["lang"];
            break;
        }
        case 3: {
            Value& s3 = doc[0]["user"]["id"];
            break;
        }
        case 4: {
            Value& s4 = doc[0]["entities"]["urls"][0]["indices"][0];
            break;
        }
        case 5: {
            Value& s5 = doc["items"][0];
            break;
        }
        case 6: {
            Value& s6 = doc[0]["descriptions"];
            break;
        }
        case 7: {
            Value& s7 = doc[0]["claims"]["P1245"][0]["mainsnak"]["property"];
            break;
        }
        case 8: {
            Value& s8 = doc[0]["routes"];
            break;
        }
        case 9: {
            Value& s9 = doc[0]["routes"][0]["legs"][0]["steps"][0]["distance"]["text"];
            break;
        }
        case 10: {
            Value& s10 = doc["products"][0]["regularPrice"];
            break;
        }
        // case 11: {
        //     Value& s111 = doc["products"][0]["categoryPath"][1]["id"];
        //     // Value& s112 = doc["products"][0]["categoryPath"][2]["id"];
        //     // Value& s113 = doc["products"][0]["categoryPath"][3]["id"];
        //     break;
        // }
    }

    auto stopQ = high_resolution_clock::now();
    auto elapsed = duration_cast<duration<double, std::nano>>(stopQ - startQ);
    total_time_ns += elapsed.count();

    fclose(fp);
    
    // std::cout << "Query " << which << " on file " << fileName << ": " 
    //           << (total_time_ns / 1e6) << " ms" << std::endl; // Convert to milliseconds
    return total_time_ns; // Return the average time for the query
}
