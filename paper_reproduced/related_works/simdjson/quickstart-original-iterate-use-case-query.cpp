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
        {"../../dataset/merged_output_large.json", {0}},
    };

    double sum = 0.0;
    sum = calcTime("../../dataset/merged_output_large.json", 0);
    cout << "Total (end-to-end)," << sum << endl;

    return 0;
}


// double calcTime(string fileName, int which) {
//     // cout << "FILE NAME: " << fileName << ", Query: " << which << endl;

//     ondemand::parser parser;
//     padded_string json = padded_string::load(fileName);

//     double query_time = 0.0, parse_time = 0.0   ;

//     // Measure parse time
//     auto start = high_resolution_clock::now();
//     simdjson::ondemand::document tweets = parser.iterate(json);
//     auto end = high_resolution_clock::now();

//     simdjson::simdjson_result<simdjson::dom::element> query_res;

//     std::chrono::duration<double, std::milli> parse_duration = end - start;
//     double parse_time = parse_duration.count();
    
    
//     auto startQ = high_resolution_clock::now();

//     switch (which) {
//         case 0:
//             for(int i = 0; i < tweets.get_array().size(); i++) {
//                 auto tweet = tweets.at(i)["id"];
//                 // cout << "Tweet ID: " << tweet["id"].get_string() << endl;
//             }
//             // tweets["id"];
//             break;
//     }

//     auto stopQ = high_resolution_clock::now();
//     auto duration_ns = duration_cast<duration<double, std::nano>>(stopQ - startQ);
//     query_time += duration_ns.count();
//     cout << "Parse time: " << parse_time << " ms" << endl;
//     cout << "Query time: " << query_time / 1e6 << " ms" << endl;
    

//     // double avg_ms = total_time_ms / 10.0;
//     // cout << "Average query time: " << avg_ms << " ms" << endl;
//     return parse_time + query_time;
// }

double calcTime(string fileName, int which) {
    auto start = high_resolution_clock::now();
    int count = 0;

    ondemand::parser parser;
    padded_string json = padded_string::load(fileName);

    // Measure parse time
    ondemand::document doc = parser.iterate(json);
    auto end = high_resolution_clock::now();

    std::chrono::duration<double, std::milli> parse_duration = end - start;
    double parse_time = parse_duration.count();

    double query_time = 0.0;

    auto startQ = high_resolution_clock::now();

    ondemand::array tweets_array = doc.get_array();
    for (ondemand::value tweet : tweets_array) {
        simdjson::ondemand::value id_value;
        simdjson::error_code err = tweet["id"].get(id_value);
        if (err) {
            cerr << "Warning: could not access 'id' in one of the entries: " << simdjson::error_message(err) << endl;
            continue;
        }

        // Now try to interpret id_value as a number or string depending on your data
        uint64_t id_num;
        if (id_value.get(id_num) == simdjson::SUCCESS) {
            count++;
            // cout << "ID (uint64): " << id_num << endl;
        } else {
            std::string_view id_str;
            if (id_value.get(id_str) == simdjson::SUCCESS) {
                count++;
                // cout << "ID (string): " << id_str << endl;
            } else {
                cerr << "Warning: 'id' exists but could not be interpreted as string or uint64." << endl;
            }
        }
    }


    auto stopQ = high_resolution_clock::now();
    auto duration_ns = duration_cast<duration<double, std::nano>>(stopQ - startQ);
    query_time = duration_ns.count();

    cout << "Processed " << count << " tweets." << endl;
    cout << "Parse time: " << parse_time << " ms" << endl;
    cout << "Query time: " << query_time / 1e6 << " ms" << endl;

    return parse_time + query_time;
}
