#include <bits/stdc++.h>
#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"
#include <unistd.h>
#include <fstream>

using namespace std;

// Optional: keep query if you want to reuse
string query(BitmapIterator* iter) {
    string output = "";
    if (iter->isObject() && iter->moveToKey("products")) {
        if (iter->down() == false) return output;
        while (iter->isArray() && iter->moveNext() == true) {
            if (iter->down() == false) continue;
            if (iter->isObject() && iter->moveToKey("categoryPath")) {
                if (iter->down() == false) continue; 
                for (int idx = 1; idx <= 2; ++idx) {
                    if (iter->moveToIndex(idx)) {
                        if (iter->down() == false) continue;
                        if (iter->isObject() && iter->moveToKey("id")) {
                            char* value = iter->getValue();
                            output.append(value).append(";");
                            if (value) free(value);
                        }
                        iter->up();
                    }
                }
                return output;
            }
        }
        iter->up();
    }
    return output;
}

// Function to get file size in MB
// double getFileSizeMB(const string& filename) {
//     struct stat stat_buf;
//     int rc = stat(filename.c_str(), &stat_buf);
//     if (rc != 0) return 0.0;
//     return stat_buf.st_size / (1024.0 * 1024.0);
// }


// Memory printer
void printMemoryUsage(const std::string& label) {
    std::ifstream file("/proc/self/statm");
    long size, resident;
    file >> size >> resident;

    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
    long rss_kb = resident * page_size_kb;
    double rss_mb = rss_kb / 1024.0;

    std::cout << rss_mb << std::endl;
}

int main() {
    char* file_path = "../../../../../dataset/nspl_large_record.json";

    Record* rec = RecordLoader::loadSingleRecord(file_path);
    if (rec == NULL) {
        cout << "record loading fails." << endl;
        return -1;
    }

    int thread_num = 1;
    int level_num = 12;

    Bitmap* bm = BitmapConstructor::construct(rec, thread_num, level_num);
    BitmapIterator* iter = BitmapConstructor::getIterator(bm);

    // Optional: Run query if needed
    // string output = query(iter);

    // Measure memory before freeing
    printMemoryUsage("NSPL");

    delete iter;
    delete bm;
    delete rec;

    return 0;
}
