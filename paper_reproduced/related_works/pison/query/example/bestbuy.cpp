#include <bits/stdc++.h>
#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"

// {$.products[0].categoryPath[1:3].id}
string query(BitmapIterator* iter) {
    string output = "";
    // auto start_query = std::chrono::high_resolution_clock::now();    
    if (iter->isObject() && iter->moveToKey("products")) {
        if (iter->down() == false) return output;
        while (iter->isArray() && iter->moveNext() == true) {
            // auto start_query = std::chrono::high_resolution_clock::now();    
            if (iter->down() == false) continue;
            if (iter->isObject() && iter->moveToKey("categoryPath")) {
                if (iter->down() == false) continue; 
                for (int idx = 1; idx <= 2; ++idx) {
                    // 2nd and 3rd elements inside "categoryPath" array
                    if (iter->moveToIndex(idx)) {
                        if (iter->down() == false) continue;
                        if (iter->isObject() && iter->moveToKey("id")) {
                            // value of "id"
                            char* value = iter->getValue();
                            output.append(value).append(";");
                            if (value) free(value);
                        }
                        iter->up();
                    }
                }
                // auto end_query = std::chrono::high_resolution_clock::now();
                // auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query);
                // std::cout << "query: " << duration.count() << " nanoseconds" << std::endl;
                return output;
            }
        }
        iter->up();
    }
    return output;
}

// {$.products[0].regularPrice}
string query2(BitmapIterator* iter) {
    string output = "";
    // auto start_query = std::chrono::high_resolution_clock::now();    
    if (iter->isObject() && iter->moveToKey("products")) {
        if (iter->down() == false) return output;
        while (iter->isArray() && iter->moveNext() == true) {
            // auto start_query = std::chrono::high_resolution_clock::now();    
            if (iter->down() == false) continue;
            if (iter->isObject() && iter->moveToKey("regularPrice")) {
                char* value = iter->getValue();
                output.append(value).append(";");
                if (value) free(value);

                // auto end_query = std::chrono::high_resolution_clock::now();
                // auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query);
                // std::cout << "query: " << duration.count() << " nanoseconds" << std::endl;
                return output;
            }
            iter->up();
        }
        iter->up();
    }
    return output;
}


int main() {
    // char* file_path = "../dataset/bestbuy_sample_small_records.json";
    char* file_path = "../../../../../dataset/bestbuy_large_record.json";
    
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

    // auto start = chrono::high_resolution_clock::now();


    /* process the input record in serial order: first build bitmap,
     * then perform the query with a bitmap iterator
     */

    int thread_num = 1;

    /* set the number of levels of bitmaps to create, either based on the
     * query or the JSON records. E.g., query $[*].user.id needs three levels
     * (level 0, 1, 2), but the record may be of more than three levels
     */
    int level_num = 12;

    Bitmap* bm = BitmapConstructor::construct(rec,thread_num,level_num);
    BitmapIterator* iter = BitmapConstructor::getIterator(bm);
   
    auto start_query = std::chrono::high_resolution_clock::now();    
    string output = query(iter);
    // string output = query2(iter);


    auto end_query = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query);
    delete iter;
    delete bm;
    delete rec;

    std::cout << duration.count() << std::endl; // " nanoseconds" << 

    // auto end = chrono::high_resolution_clock::now();
    // double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    // time_taken *= 1e-9;
 
    // double time_taken_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    // cout << time_taken_ms << setprecision(3) << endl;

    //cout<<"matches are: "<<output<<endl;
    return 0;
}
