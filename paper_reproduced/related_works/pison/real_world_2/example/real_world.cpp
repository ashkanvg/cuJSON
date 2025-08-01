#include <bits/stdc++.h>
#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"

using namespace std;
using namespace chrono;

// Query: extract `.id` for all top-level array entries
// Query: extract `.repo.name` if `.type == "PushEvent"`
// string query_push_event_repo_name(BitmapIterator* iter) {
//     string output = "";
//     int match_count = 0;

//     auto start_query = high_resolution_clock::now();

//     while (iter->isArray() && iter->moveNext()) {
//         if (!iter->down()) continue;

//         bool is_push_event = false;
//         std::string type_value;

//         if (iter->isObject() && iter->moveToKey("type")) {
//             char* type = iter->getValue();
//             if (type && string(type) == "PushEvent") {
//                 is_push_event = true;
//             }
//             if (type) free(type);
//         }

//         if (is_push_event && iter->moveToKey("repo")) {
//             if (iter->down()) {
//                 if (iter->isObject() && iter->moveToKey("name")) {
//                     char* name_value = iter->getValue();
//                     if (name_value) {
//                         output.append(name_value).append(";");
//                         free(name_value);
//                         match_count++;
//                     }
//                 }
//                 iter->up(); // back from repo
//             }
//         }

//         iter->up(); // back from this object
//     }

//     auto end_query = high_resolution_clock::now();
//     double query_time = duration_cast<duration<double, std::milli>>(end_query - start_query).count();
//     cout << "Query time: " << fixed << setprecision(3) << query_time << " ms" << endl;
//     cout << "Matched PushEvent repo.name count: " << match_count << endl;

//     return output;
// }

// string query_push_event_repo_name(BitmapIterator* iter) {
//     string output = "";
//     int match_count = 0;

//     auto start_query = high_resolution_clock::now();

//     while (iter->isArray() && iter->moveNext()) {
//         if (!iter->down()) continue;  // go inside object

//         bool is_push_event = false;

//         // First, find type field
//         if (iter->isObject()) {
//             if (iter->moveToKey("type")) {
//                 char* type = iter->getValue();
//                 if (type && string(type) == "PushEvent") {
//                     is_push_event = true;
//                 }
//                 if (type) free(type);
//             }
//         }

//         iter->up();               // exit current object
//         iter->down();             // re-enter same object (reset position)

//         // Now look for repo.name if it's a PushEvent
//         if (is_push_event && iter->isObject() && iter->moveToKey("repo")) {
//             if (iter->down()) {
//                 if (iter->isObject() && iter->moveToKey("name")) {
//                     char* name_value = iter->getValue();
//                     if (name_value) {
//                         output.append(name_value).append(";");
//                         free(name_value);
//                         match_count++;
//                     }
//                 }
//                 iter->up();  // exit repo
//             }
//         }

//         iter->up();  // exit object
//     }

//     auto end_query = high_resolution_clock::now();
//     double query_time = duration_cast<duration<double, std::milli>>(end_query - start_query).count();
//     cout << "Query time: " << fixed << setprecision(3) << query_time << " ms" << endl;
//     cout << "Matched PushEvent repo.name count: " << match_count << endl;

//     return output;
// }

// string query_push_event_repo_name(BitmapIterator* iter) {
//     string output = "";
//     int match_count = 0;

//     auto start_query = high_resolution_clock::now();

//     // Traverse top-level array
//     while (iter->isArray() && iter->moveNext()) {
//         if (!iter->down()) continue;  // enter current object

//         bool is_push_event = false;

//         // Pass 1: check type
//         if (iter->isObject() && iter->moveToKey("type")) {
//             char* type_val = iter->getValue();
//             if (type_val && string(type_val) == "PushEvent") {
//                 is_push_event = true;
//             }
//             if (type_val) free(type_val);
//         }

//         iter->up(); // go back out of object
//         if (!iter->down()) continue; // re-enter same object to scan again

//         // Pass 2: if type is PushEvent, get repo.name
//         if (is_push_event && iter->isObject() && iter->moveToKey("repo")) {
//             if (iter->down()) {
//                 if (iter->isObject() && iter->moveToKey("name")) {
//                     char* name_val = iter->getValue();
//                     if (name_val) {
//                         output.append(name_val).append(";");
//                         free(name_val);
//                         match_count++;
//                     }
//                 }
//                 iter->up(); // exit repo object
//             }
//         }

//         iter->up(); // exit current object
//     }

//     auto end_query = high_resolution_clock::now();
//     double query_time = duration_cast<duration<double, std::milli>>(end_query - start_query).count();
//     cout << "Query time: " << fixed << setprecision(3) << query_time << " ms" << endl;
//     cout << "Matched PushEvent repo.name count: " << match_count << endl;

//     return output;
// }

// string query_push_event_repo_name(BitmapIterator* iter) {
//     string output = "";
//     int match_count = 0;

//     auto start_query = high_resolution_clock::now();

//     while (iter->isArray() && iter->moveNext()) {
//         if (!iter->down()) continue;  // enter object
//         bool is_push_event = false;

//         // DEBUG
//         // cout << "[DEBUG] Top-level object\n";

//         // Pass 1 — get type
//         if (iter->isObject() && iter->moveToKey("type")) {
//             char* type_val = iter->getValue();
//             if (type_val) {
//                 // cout << "[DEBUG] type = " << type_val << endl;
//                 if (string(type_val) == "PushEvent") {
//                     is_push_event = true;
//                 }
//                 free(type_val);
//             }
//         }

//         iter->up();               // reset iterator
//         if (!iter->down()) continue;  // re-enter same object

//         // Pass 2 — get repo.name if it's PushEvent
//         if (is_push_event && iter->isObject() && iter->moveToKey("repo")) {
//             if (iter->down()) {
//                 if (iter->isObject() && iter->moveToKey("name")) {
//                     char* name_val = iter->getValue();
//                     if (name_val) {
//                         cout << "[DEBUG] repo.name = " << name_val << endl;
//                         output.append(name_val).append(";");
//                         free(name_val);
//                         match_count++;
//                     }
//                 }
//                 iter->up(); // back from repo
//             }
//         }

//         iter->up(); // back from object
//     }

//     auto end_query = high_resolution_clock::now();
//     double query_time = duration_cast<duration<double, std::milli>>(end_query - start_query).count();
//     cout << "Query time: " << fixed << setprecision(3) << query_time << " ms" << endl;
//     cout << "Matched PushEvent repo.name count: " << match_count << endl;

//     return output;
// }

string query_push_event_repo_name(BitmapIterator* iter) {
    string output = "";
    int match_count = 0;

    auto start_query = high_resolution_clock::now();

    // Traverse top-level array
    while (iter->isArray() && iter->moveNext()) {
        if (!iter->down()) continue;  // enter top-level object

        // STEP 1: check if "type" == "PushEvent"
        bool is_push_event = false;
        if (iter->isObject() && iter->moveToKey((char*)"type")) {
            char* type_val = iter->getValue();
            if (type_val) {
                // cout << "[DEBUG] type = " << type_val << endl;
                if (strcmp(type_val, "\"PushEvent\",") == 0) {
                    is_push_event = true;
                    // cout << "[DEBUG] Found PushEvent type" << endl;
                    // match_count++;
                }
                free(type_val);
            }
        }

        // STEP 2: reset iterator to beginning of object
        iter->up();              // exit current object
        if (!iter->down()) {    // re-enter the same object
            iter->up();         // ensure we clean up before continuing
            continue;
        }

        // STEP 3: if it's a PushEvent, go inside repo and get name
        if (is_push_event && iter->isObject() && iter->moveToKey((char*)"repo")) {
            if (iter->down()) {
                if (iter->isObject() && iter->moveToKey((char*)"name")) {
                    char* name_val = iter->getValue();
                    if (name_val) {
                        output.append(name_val).append(";");
                        free(name_val);
                        match_count++;
                    }
                }
                iter->up(); // exit from repo object
            }
        }

        iter->up(); // exit from top-level object
    }

    auto end_query = high_resolution_clock::now();
    double query_time = duration_cast<duration<double, std::milli>>(end_query - start_query).count();
    cout << "Query time: " << fixed << setprecision(3) << query_time << " ms" << endl;
    cout << "Matched PushEvent repo.name count: " << match_count << endl;

    return output;
}


int main(int argc, char* argv[]) {
    // const char* file_path = "../../../../../dataset/merged_output_large.json";
    char file_path[] = "../../../../../dataset/github_archive_large_record.json";

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

    string output = query_push_event_repo_name(iter);  // runs and times the query inside

    delete iter;
    delete bm;
    delete rec;

    auto total_end = high_resolution_clock::now();
    double total_time = duration_cast<duration<double, std::milli>>(total_end - total_start).count();
    cout << "Total time: " << fixed << setprecision(3) << total_time << " ms" << endl;

    return 0;
}
