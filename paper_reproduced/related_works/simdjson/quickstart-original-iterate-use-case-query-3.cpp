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
        {"../../dataset/github_archive_large_record.json", {0}},
    };

    double sum = 0.0;
    sum = calcTime("../../dataset/github_archive_large_record.json", 0);
    cout << "Total (end-to-end)," << sum << endl;

    return 0;
}

double calcTime(string fileName, int which) {
    auto start = high_resolution_clock::now();
    int count = 0;

    ondemand::parser parser;
    
    padded_string json = padded_string::load(fileName);
    auto end = high_resolution_clock::now();
    std::chrono::duration<double, std::milli> parse_duration = end - start;
    double parse_time = parse_duration.count();

    // Measure parse time
    ondemand::document doc = parser.iterate(json);

    double query_time = 0.0;

    auto startQ = high_resolution_clock::now();

    ondemand::array tweets_array = doc.get_array();
    for (ondemand::value tweet : tweets_array) {
        // Check if type == "PushEvent"
        std::string_view type;
        if (tweet["type"].get(type) == simdjson::SUCCESS && type == "PushEvent") {
            // Try to access repo.name
            simdjson::ondemand::object repo;
            if (tweet["repo"].get_object().get(repo) == simdjson::SUCCESS) {
                std::string_view name;
                if (repo["name"].get(name) == simdjson::SUCCESS) {
                    count++;
                }
            }
        }
    }

    auto stopQ = high_resolution_clock::now();
    auto duration_ns = duration_cast<duration<double, std::nano>>(stopQ - startQ);
    query_time = duration_ns.count();

    cout << "Matched PushEvent repo.name count: " << count << endl;
    cout << "Load time: " << parse_time << " ms" << endl;
    cout << "Query time: " << query_time / 1e6 << " ms" << endl;

    return parse_time + query_time;
}
