#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/filereadstream.h"
#include "include/rapidjson/error/en.h"

#include <fstream>
#include <iostream>
#include <malloc.h>
#include <unistd.h>
#include <chrono>

using namespace rapidjson;
using namespace std;
using namespace std::chrono;

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

int main() {
    const char* filename = "../../../dataset/merged_output_large.json";

    // Start total timer
    auto total_start = high_resolution_clock::now();

    // Open the JSON file
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        std::cerr << "Error: unable to open file " << filename << std::endl;
        return 1;
    }

    // Start parse timer
    auto parse_start = high_resolution_clock::now();

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document doc;
    doc.ParseStream(is);

    // End parse timer
    auto parse_end = high_resolution_clock::now();

    fclose(fp);

    if (doc.HasParseError()) {
        std::cerr << "Error: failed to parse JSON document\n";
        cerr << "Parse error: " << GetParseError_En(doc.GetParseError()) << std::endl;
        return 1;
    }

    // Start query timer
    auto query_start = high_resolution_clock::now();

    int count = 0;
    if (doc.IsArray()) {
        for (const auto& item : doc.GetArray()) {
            if (item.HasMember("id")) {
                const Value& id = item["id"];
                if (id.IsUint64()) {
                    // std::cout << "ID: " << id.GetUint64() << std::endl;
                } else if (id.IsInt()) {
                    // std::cout << "ID: " << id.GetInt() << std::endl;
                } else if (id.IsString()) {
                    // std::cout << "ID: " << id.GetString() << std::endl;
                } else {
                    // std::cout << "ID: [Unsupported type]" << std::endl;
                }
                ++count;
            } else {
                // std::cout << "ID: [Missing]" << std::endl;
            }
        }
    } else {
        std::cerr << "Error: Expected a JSON array at the top level." << std::endl;
        return 1;
    }

    // End query timer
    auto query_end = high_resolution_clock::now();
    auto total_end = high_resolution_clock::now();

    // Calculate durations
    auto parse_duration = duration_cast<milliseconds>(parse_end - parse_start);
    auto query_duration = duration_cast<milliseconds>(query_end - query_start);
    auto total_duration = duration_cast<milliseconds>(total_end - total_start);

    // Print results
    std::cout << "✅ Extracted " << count << " IDs." << std::endl;
    std::cout << "⏱️ Parse time: " << parse_duration.count() << " ms" << std::endl;
    std::cout << "⏱️ Query time: " << query_duration.count() << " ms" << std::endl;
    std::cout << "⏱️ Total time: " << total_duration.count() << " ms" << std::endl;

    return 0;
}
