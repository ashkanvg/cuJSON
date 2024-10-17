#include <bits/stdc++.h>
#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"

// $.routes[*].legs[*].steps[*].distance.text
string query(BitmapIterator* iter){
    string output = "";
    while(iter->isArray() && iter->moveNext() == true){
        auto start_query = std::chrono::high_resolution_clock::now();    
        if (iter->down() == false) continue;
        if (iter->isObject() && iter->moveToKey("routes")) {
            if (iter->down() == false) continue; 
            while (iter->isArray() && iter->moveNext() == true) {
                if (iter->down() == false) continue; 
                if (iter->isObject() && iter->moveToKey("legs")) {
                    if (iter->down() == false) continue; 
                    while (iter->isArray() && iter->moveNext() == true) {
                        if (iter->down() == false) continue; 
                        if (iter->isObject() && iter->moveToKey("steps")) {     
                            if (iter->down() == false) continue;                    
                            while (iter->isArray() && iter->moveNext() == true) {
                                if (iter->down() == false) continue;
                                if (iter->isObject() && iter->moveToKey("distance")) {
                                    if (iter->down() == false) continue; 
                                    if (iter->isObject() && iter->moveToKey("text")) {
                                        char* value = iter->getValue();
                                        output.append(value).append(";");
                                        if (value) free(value);
                                        // cout << "final";
                                        auto end_query = std::chrono::high_resolution_clock::now();
                                        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query);
                                        std::cout << "query: " << duration.count() << " nanoseconds" << std::endl;
                                        return output;

                                    }else{
                                        cout << "text failed!" <<endl;
                                    }
                                    iter->up();
                                }else{   
                                    // cout << "distance failed!" <<endl;
                                }
                                iter->up();
                            }
                            iter->up();
                        }else{
                            cout << "steps failed!" <<endl;
                        }
                        iter->up();
                    }
                    iter->up();
                }else{
                    cout << "legs failed!" <<endl;
                }
                iter->up();
            }
            iter->up();
        }else{
            cout << "routes failed!" <<endl;
        }
        iter->up();
    }
    return output;
}

int main() {
    char* file_path = "../../../Test-Files/Pison Large Datasets/google_map_large_record.json";
    
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

    /* process the input record in serial order: first build bitmap,
     * then perform the query with a bitmap iterator
     */
    
    int thread_num = 1;

    /* set the number of levels of bitmaps to create, either based on the
     * query or the JSON records. E.g., query $[*].user.id needs three levels
     * (level 0, 1, 2), but the record may be of more than three levels
     */
    int level_num = 10;

    Bitmap* bm = BitmapConstructor::construct(rec,thread_num,level_num);
    BitmapIterator* iter = BitmapConstructor::getIterator(bm);
    
    
    // auto start_query = std::chrono::high_resolution_clock::now();    
    // string output = query(iter);
    // auto end_query = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_query - start_query);
    delete iter;
    delete bm;
    delete rec;  

    // std::cout << "query: " << duration.count() << " nanoseconds" << std::endl;
  

    auto end = chrono::high_resolution_clock::now();

    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;
 
    cout << "Time taken by program is : " << fixed << time_taken << setprecision(9);
    cout << " sec" << endl;

    // delete iter;
    // delete bm;
    // delete rec;

    //cout<<"matches are: "<<output<<endl;    
    return 0;
}