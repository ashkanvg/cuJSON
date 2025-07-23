#include <bits/stdc++.h>
#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"
#include <chrono>
using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./bestbuy SIZE_MB" << endl;
        return 1;
    }

    string size_mb = argv[1];
    string file_path = "/rhome/aveda002/bigdata/Test-Files/scalability/nspl/output_" + size_mb + "MB_large.json";

    auto start = high_resolution_clock::now();
    
    // 👇 CAST const char* to char* to match legacy API
    Record* rec = RecordLoader::loadSingleRecord(const_cast<char*>(file_path.c_str()));
    if (!rec) {
        cerr << "❌ Failed to load record: " << file_path << endl;
        return 1;
    }

    int thread_num = 1;
    int level_num = 12;
    Bitmap* bm = BitmapConstructor::construct(rec, thread_num, level_num);
    BitmapIterator* iter = BitmapConstructor::getIterator(bm);

    delete iter;
    delete bm;
    delete rec;

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<chrono::duration<double, milli>>(end - start);
    cout << duration.count() << endl;

    return 0;
}
