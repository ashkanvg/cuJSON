// rapidjson/example/simpledom/simpledom.cpp
#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/filereadstream.h"
#include "include/rapidjson/error/en.h"

#include <fstream>
#include <iostream>
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>  // for stat()
#include <cstring>     // for strerror()

using namespace rapidjson;
using namespace std;

void printMemoryUsage(const string& filePath) {
    std::ifstream file("/proc/self/statm");
    long rss;
    file >> rss;

    // Convert pages to MB
    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    long resident_set = rss * page_size_kb;
    double resident_set_mb = resident_set / 1024.0;

    // Compute file size in MB
    struct stat st;
    // double file_size_mb = 0.0;
    if (stat(filePath.c_str(), &st) == 0) {
        resident_set_mb += st.st_size / (1024.0 * 1024.0);
    } else {
        cerr << "Warning: Failed to get file size for " << filePath << ": " << strerror(errno) << endl;
    }

    std::cout << "Memory Usage: " << resident_set_mb << " MB\n";
    // std::cout << "InputSize_MB=" << file_size_mb
    //           << ", RSS_MB=" << resident_set_mb << std::endl;
}

int main() {
    const string filePath = "../../../dataset/wiki_large_record.json";

    // Open the file
    FILE* fp = fopen(filePath.c_str(), "r");
    if (!fp) {
        std::cerr << "Error: unable to open file " << filePath << std::endl;
        return 1;
    }

    time_t start, end;
    start = clock();

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document doc;
    doc.ParseStream(is);

    if (doc.HasParseError()) {
        std::cerr << "Error: failed to parse JSON document" << std::endl;
        std::cerr << "Error code: " << GetParseError_En(doc.GetParseError()) << std::endl;
        fclose(fp);
        return 1;
    }

    printMemoryUsage(filePath);

    end = clock();
    // std::cout << "Parse time (ms): " << ((double)(end - start) / CLOCKS_PER_SEC) * 1000 << std::endl;

    fclose(fp);
    return 0;
}