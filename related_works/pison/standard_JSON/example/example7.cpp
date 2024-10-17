#include <bits/stdc++.h>
#include "../src/RecordLoader.h"
#include "../src/BitmapIterator.h"
#include "../src/BitmapConstructor.h"


// {$.meta.view.columns[*].name}
string query(BitmapIterator* iter) {
    string output = "";
    if (iter->isObject() && iter->moveToKey("meta")) {
        if (iter->down() == false) return output;
        if(iter->isObject() && iter->moveToKey("P150")){
            if (iter->down() == false) return output;
            if(iter->isObject() && iter->moveToKey("columns")){
                if (iter->down() == false) return output;
                while (iter->isArray() && iter->moveNext() == true){
                    if (iter->down() == false) continue;
                    if (iter->isObject() && iter->moveToKey("name")) {
                        // if (iter->down() == false) continue;
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
    return output;
}

int main() {
    char* file_path = "../../../Test-Files/Pison Large Datasets/nspl_large_record.json";
    

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


    // set the number of threads for parallel bitmap construction
    int thread_num = 4;

    /* set the number of levels of bitmaps to create, either based on the
     * query or the JSON records. E.g., query $[*].user.id needs three levels 
     * (level 0, 1, 2), but the record may be of more than three levels
     */
    int level_num = 5;

    /* process the input record: first build bitmap, then perform 
     * the query with a bitmap iterator
     */
    // Bitmap* bm = BitmapConstructor::construct(rec, thread_num, level_num);
    Bitmap* bm = BitmapConstructor::construct(rec,thread_num,level_num);
    BitmapIterator* iter = BitmapConstructor::getIterator(bm);
    string output = query(iter);
    delete iter;
    delete bm;
    delete rec;

    auto end = chrono::high_resolution_clock::now();

    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;

    cout << "Time taken by program is : " << fixed << time_taken << setprecision(9);
    cout << " sec" << endl;


    // cout<<"matches are: "<<output<<endl;
    return 0;
}
