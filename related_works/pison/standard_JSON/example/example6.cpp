#include <bits/stdc++.h>
#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"

// $.routes[*].legs[*].steps[*].distance.tex
string query(BitmapIterator* iter) {
    string output = "";
    if (iter->isObject() && iter->moveToKey("routes")) {
        if (iter->down() == false) return output; 
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
                                if (iter->isObject() && iter->moveToKey("tex")) {
                                    char* value = iter->getValue();
                                    output.append(value).append(";");
                                    if (value) free(value);
                                }
                                iter->up();
                            }
                            iter->up();
                        }
                        iter->up();
                    }
                    iter->up();
                }
                iter->up();
            }
            iter->up();
        }
        iter->up();
    }
    return output;
}

int main() {
    char* file_path = "../../../Test-Files/Pison Large Datasets/google_map_large_record.json";
    
    auto start2 = chrono::high_resolution_clock::now();
    Record* rec = RecordLoader::loadSingleRecord(file_path);
    if (rec == NULL) {
        cout<<"record loading fails."<<endl;
        return -1;
    }
    auto end2 = chrono::high_resolution_clock::now();
    double time_taken_2 = chrono::duration_cast<chrono::nanoseconds>(end2 - start2).count();
    time_taken_2 *= 1e-9;
    cout << "Time taken by program is (loader): " << fixed << time_taken_2 << setprecision(9);
    cout << " sec" << endl;
    
    auto start = chrono::high_resolution_clock::now();

    /* process the input record in serial order: first build bitmap,
     * then perform the query with a bitmap iterator
     */
    
    int thread_num = 4;

    /* set the number of levels of bitmaps to create, either based on the
     * query or the JSON records. E.g., query $[*].user.id needs three levels
     * (level 0, 1, 2), but the record may be of more than three levels
     */
    int level_num = 6;

    Bitmap* bm = BitmapConstructor::construct(rec,thread_num,level_num);
    BitmapIterator* iter = BitmapConstructor::getIterator(bm);
    string output = query(iter);
    

    auto end = chrono::high_resolution_clock::now();

    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;
 
    cout << "Time taken by program is : " << fixed << time_taken << setprecision(9);
    cout << " sec" << endl;

    delete iter;
    delete bm;
    delete rec;

    //cout<<"matches are: "<<output<<endl;    
    return 0;
}
