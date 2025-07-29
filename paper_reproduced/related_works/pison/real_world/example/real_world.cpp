#include <bits/stdc++.h>
#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"

using namespace std;
using namespace chrono;

// Query: extract `.id` for all top-level array entries
string query_id(BitmapIterator* iter) {
    string output = "";
    int match_count = 0;

    auto start_query = high_resolution_clock::now();

    while (iter->isArray() && iter->moveNext()) {
        if (!iter->down()) continue;

        if (iter->isObject() && iter->moveToKey("id")) {
            char* value = iter->getValue();
            if (value) {
                output.append(value).append(";");
                free(value);
                match_count++;
            }
        }

        iter->up();
    }

    auto end_query = high_resolution_clock::now();
    double query_time = duration_cast<duration<double, std::milli>>(end_query - start_query).count();
    cout << "Query time: " << fixed << setprecision(3) << query_time << " ms" << endl;
    return output;
}

int main(int argc, char* argv[]) {
    // const char* file_path = "../../../../../dataset/merged_output_large.json";
    char file_path[] = "../../../../../dataset/merged_output_large.json";

    int thread_num = 4; // default
    if (argc >= 2) {
        thread_num = atoi(argv[1]);
    }

    int level_num = 8;

    auto total_start = high_resolution_clock::now();

    // ----------------- Load JSON -----------------
    auto parse_start = high_resolution_clock::now();
    Record* rec = RecordLoader::loadSingleRecord(file_path);
    auto parse_end = high_resolution_clock::now();

    if (!rec) {
        cerr << "❌ Record loading failed." << endl;
        return -1;
    }

    double parse_time = duration_cast<duration<double, std::milli>>(parse_end - parse_start).count();
    cout << "Parse time: " << fixed << setprecision(3) << parse_time << " ms" << endl;

    // ----------------- Build Bitmap & Query -----------------
    Bitmap* bm = BitmapConstructor::construct(rec, thread_num, level_num);
    BitmapIterator* iter = BitmapConstructor::getIterator(bm);

    string output = query_id(iter);  // runs and times the query inside

    delete iter;
    delete bm;
    delete rec;

    auto total_end = high_resolution_clock::now();
    double total_time = duration_cast<duration<double, std::milli>>(total_end - total_start).count();
    cout << "Total time: " << fixed << setprecision(3) << total_time << " ms" << endl;

    return 0;
}
