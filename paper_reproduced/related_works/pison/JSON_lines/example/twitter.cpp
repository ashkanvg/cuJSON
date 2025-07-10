#include <bits/stdc++.h>
#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"

// {$.user.id, $.retweet_count}
string query(BitmapIterator* iter) {
    string output = "";
    if (iter->isObject()) {
        unordered_set<char*> set;
        set.insert("user");
        set.insert("retweet_count");
        char* key = NULL;
        while ((key = iter->moveToKey(set)) != NULL) {
            if (strcmp(key, "retweet_count") == 0) {
                // value of "retweet_count"
                char* value = iter->getValue();
                output.append(value).append(";");
                if (value) free(value);
            } else {
                if (iter->down() == false) continue;  /* value of "user" */
                if (iter->isObject() && iter->moveToKey("id")) {
                    // value of "id"
                    char* value = iter->getValue();
                    output.append(value).append(";");
                    if (value) free(value);
                }
                iter->up();
            }
        }
    }
    return output;
}

int main() {
    char* file_path = "../../../Test-Files/Pison Large Datasets/twitter_small_records.json";
    

    auto start2 = chrono::high_resolution_clock::now();
    RecordSet* record_set = RecordLoader::loadRecords(file_path);
    if (record_set->size() == 0) {
        cout<<"record loading fails."<<endl;
        return -1;
    }
    auto end2 = chrono::high_resolution_clock::now();
    double time_taken_2 = chrono::duration_cast<chrono::nanoseconds>(end2 - start2).count();
    time_taken_2 *= 1e-9;
    cout << "Time taken by program is (loader): " << fixed << time_taken_2 << setprecision(9);
    cout << " sec" << endl; 

    string output = "";


    auto start = chrono::high_resolution_clock::now();

    
    // set the number of threads for parallel bitmap construction
    int thread_num = 1;  
   
    /* set the number of levels of bitmaps to create, either based on the
     * query or the JSON records. E.g., query $[*].user.id needs three levels
     * (level 0, 1, 2), but the record may be of more than three levels
     */
    int level_num = 2;
 
    /* process the records one by one: for each one, first build bitmap, then perform 
     * the query with a bitmap iterator
     */
    int num_recs = record_set->size();
    Bitmap** bm = (Bitmap**)malloc(num_recs * sizeof(Bitmap*));  // Allocate memory using malloc

    if (bm == NULL) {
        // Handle memory allocation failure
        std::cerr << "Memory allocation failed for bm array" << std::endl;
        return 0;  // or exit the function
    }

    for (int i = 0; i < num_recs; i++) {
        bm[i] = BitmapConstructor::construct((*record_set)[i], thread_num, level_num);
        // BitmapIterator* iter = BitmapConstructor::getIterator(bm[i]);
        // output.append(query(iter));
        // delete iter;
    }

    for (int i = 0; i < num_recs; i++) {
        // BitmapIterator* iter = BitmapConstructor::getIterator(bm[i]);
        // output.append(query(iter));
        // delete iter;
        delete bm[i];  // Delete each Bitmap object
    }

    free(bm);  // Free the allocated memory
    delete record_set;

    auto end = chrono::high_resolution_clock::now();

    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;
 
    cout << "Time taken by program is : " << fixed << time_taken << setprecision(9);
    cout << " sec" << endl;
    
    //cout<<"matches are: "<<output<<endl;
    return 0;
}
