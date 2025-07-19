#include <bits/stdc++.h>
#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"

// {$.user.id, $.retweet_count}
string query(BitmapIterator* iter){
    string output = "";
    while(iter->isArray() && iter->moveNext() == true){
        auto start_query = std::chrono::high_resolution_clock::now();    
        if (iter->down() == false) continue;
        if (iter->isObject() && iter->moveToKey("user")) {
            if (iter->down() == false) continue; 
            if (iter->isObject() && iter->moveToKey("id")) {
                char* value = iter->getValue();
                output.append(value).append(";");
                if (value) free(value);
                // cout << "final";
                auto end_query = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query);
                std::cout << "query: " << duration.count() << " nanoseconds" << std::endl;
                return output;
            }else{
                cout << "id failed!" <<endl;
            }
            iter->up();
        }else{
            cout << "user failed!" <<endl;
        }
        iter->up();
    }
    return output;
}

// {$.entities.urls[0].indices[0]}
string query2(BitmapIterator* iter){
    string output = "";
    while(iter->isArray() && iter->moveNext() == true){
        auto start_query = std::chrono::high_resolution_clock::now();    
        if (iter->down() == false) continue;
        if (iter->isObject() && iter->moveToKey("entities")) {
            if (iter->down() == false) continue; 
            if (iter->isObject() && iter->moveToKey("urls")) {
                if (iter->down() == false) continue; 
                while(iter->isArray() && iter->moveNext() == true){
                    if (iter->down() == false) continue;
                    if (iter->isObject() && iter->moveToKey("indices")) {
                        if (iter->down() == false) continue; 
                        // char* value = iter->getValue();
                        // output.append(value).append(";");
                        // if (value) free(value);
                        // cout << "final";
                        auto end_query = std::chrono::high_resolution_clock::now();
                        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query);
                        std::cout << "query: " << duration.count() << " nanoseconds" << std::endl;
                        return output;
                    }
                }
            }else{
                cout << "id failed!" <<endl;
            }
            iter->up();
        }else{
            cout << "user failed!" <<endl;
        }
        iter->up();
    }
    return output;
}

// string query(BitmapIterator* iter) {
//     string output = "";


//     if (iter->isObject()) {
//         auto start_query = std::chrono::high_resolution_clock::now();    
//         unordered_set<char*> set;
//         set.insert("user");
//         set.insert("retweet_count");
//         char* key = NULL;
//         while ((key = iter->moveToKey(set)) != NULL) {
//             if (strcmp(key, "retweet_count") == 0) {
//                 // value of "retweet_count"
//                 char* value = iter->getValue();
//                 output.append(value).append(";");
//                 if (value) free(value);
//             } else {
//                 if (iter->down() == false) continue;  /* value of "user" */
//                 if (iter->isObject() && iter->moveToKey("id")) {
//                     // value of "id"
//                     char* value = iter->getValue();
//                     output.append(value).append(";");
//                     if (value) free(value);
//                 }
//                 iter->up();
//             }
//         }
//         auto end_query = std::chrono::high_resolution_clock::now();
//         auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query);
//         std::cout << "query: " << duration.count() << " nanoseconds" << std::endl;
//     }
//     return output;
// }

int main() {
    char* file_path = "../../../../../dataset/twitter_large_record.json";
    

    // auto start2 = chrono::high_resolution_clock::now();
    Record* rec = RecordLoader::loadSingleRecord(file_path);
    if (rec == NULL) {
        cout<<"record loading fails."<<endl;
        return -1;
    }
    // auto end2 = chrono::high_resolution_clock::now();
    // double time_taken_2 = chrono::duration_cast<chrono::nanoseconds>(end2 - start2).count();
    // time_taken_2 *= 1e-9;
    // cout << "Time taken by program is (loader): " << fixed << time_taken_2 << setprecision(9);
    // cout << " sec" << endl; 

    auto start = chrono::high_resolution_clock::now();

    int thread_num = 1;
    int level_num = 8;

    Bitmap* bm = BitmapConstructor::construct(rec, thread_num,level_num);
    BitmapIterator* iter = BitmapConstructor::getIterator(bm);
 
    // auto start_query = std::chrono::high_resolution_clock::now();    
    // string output = query(iter);
    // string output = query2(iter);
    // auto end_query = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query);
    delete iter;
    delete bm;
    delete rec;
    
    // std::cout << "query: " << duration.count() << " nanoseconds" << std::endl;
    

    auto end = chrono::high_resolution_clock::now();


    double time_taken_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << time_taken_ms << setprecision(3) << endl;
    
    // delete iter;
    // delete bm;
    // delete rec;
    //cout<<"matches are: "<<output<<endl;
    return 0;
}
