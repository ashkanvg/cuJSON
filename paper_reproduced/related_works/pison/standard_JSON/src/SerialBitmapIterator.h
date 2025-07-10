#ifndef SERIALBITMAPITERATOR_H
#define SERIALBITMAPITERATOR_H

#include "SerialBitmap.h"
#include "BitmapIterator.h"
#include <string.h>
#include <unordered_set>
using namespace std;

#define MAX_NUM_ELE 1000000

class SerialBitmap;

class SerialBitmapIterator : public BitmapIterator {
  private:
    SerialBitmap* mSerialBitmap;
    IterCtxInfo mCtxInfo[MAX_LEVEL];
    bool mPosArrAlloc[MAX_LEVEL];
    int mCurLevel;
    // the top level of the input record, or each sub-record (for parallel querying)
    int mTopLevel;
    // more efficient than allocating memory during runtime
    char mKey[MAX_FIELD_SIZE];
    
  public:
    SerialBitmapIterator() {}
    SerialBitmapIterator(SerialBitmap* sbm) {
        mSerialBitmap = sbm;
        mCurLevel = -1;
        mTopLevel = -1;
        mVisitedFields = 0;
        for (int i = 0; i < MAX_LEVEL; ++i) {
            mPosArrAlloc[i] = false;
            mCtxInfo[i].positions = NULL;
        }
        // initially, iterator points to the first level of the record
        down();
    }

    ~SerialBitmapIterator() {
        for (int i = mTopLevel + 1; i < MAX_LEVEL; ++i) {
            if (mPosArrAlloc[i] == true) {
                free(mCtxInfo[i].positions);
                mCtxInfo[i].positions = NULL;
            } else {
                break;
            }
        }
    }

    // Creates a copy of iterator. Often used for parallel querying.
    SerialBitmapIterator* getCopy();
    // Moves back to the object or array which contains the current nested record.
    // Often used when the current nested record has been processed.
    // Valid except for the first level of the record.
    bool up();
    // Moves to the start of the nested object or array.
    // Gets all colon or comma positions from leveled bitmap indexes for current nested record.
    // Valid if we are at { or [.
    bool down();
    // Whether the iterator points to an object.
    bool isObject();
    // Whether the iterator points to an array.
    bool isArray();
    // Moves iterator to the next array item.
    bool moveNext();
    // Moves to the corresponding key field inside the current object.
    bool moveToKey(char* key);
    // Moves to the corresponding key fields inside the current object, returns the current key name.
    // After this operation, the current key field will be removed from key_set.
    char* moveToKey(unordered_set<char*>& key_set);
    // Returns the number of elements inside current array.
    int numArrayElements();
    // If the current record is an array, moves to an item based on index.
    // Returns false if the index is out of the boundary.
    bool moveToIndex(int index);
    // Gets the content of the current value inside an object or array.
    char* getValue();
   
  private:
    // get positions of all colons between start_idx and end_idx from input stream
    void generateColonPositions(long start_pos, long end_pos, int level, long* colon_positions, long& top_colon_positions);
    // get positions of all commas between start_idx and end_idx from input stream
    void generateCommaPositions(long start_pos, long end_pos, int level, long* comma_positions, long& top_comma_positions);
    // get starting and ending positions for a string
    bool findFieldQuotePos(long colon_pos, long& start_pos, long& end_pos);
};
#endif
