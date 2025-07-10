#ifndef LOCALBITMAP_H
#define LOCALBITMAP_H
#include <string>
#include <iostream>
#include <vector>
#include <bitset>
#include <cassert>
#include <stack>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <math.h>
#include <immintrin.h>
#include "Bitmap.h"
#include "Tokenizer.h"
using namespace std;

class LocalBitmap : public Bitmap {
    friend class ParallelBitmap;
    friend class ParallelBitmapIterator;
  private:
    int mNumTknErr;
    int mNumTrial;
    int mThreadId;
    char* mRecord;
    // for a single large record, stream length equals to record length
    long mRecordLength;
    // each temp word has 32 bytes
    long mNumTmpWords;
    // each word has 64 bytes
    long mNumWords;
    // the deepest level of leveled bitmap indexes (starting from 0)
    int mDepth;
    // structural character bitmaps
    unsigned long *mEscapeBitmap, *mStrBitmap, *mColonBitmap, *mCommaBitmap, *mLbracketBitmap, *mRbracketBitmap, *mLbraceBitmap, *mRbraceBitmap;

    // following two variables are used for validating inference results of Step 3 (build string mask bitmap)
    // marks whether current chunk starts inside string or not
    unsigned long mStartInStrBitmap;
    // marks whether current chunk ends inside string or not
    unsigned long mEndInStrBitmap;

    // following variables are used for merging phase (after Step 5, merge leveled bitmap)
    // each thread starts with level 0, following two arrays save bitmaps for levels higher than 0 (temporary result)
    unsigned long *mLevColonBitmap[MAX_LEVEL];
    unsigned long *mLevCommaBitmap[MAX_LEVEL];
    // each thread starts with level 0, following two arrays save bitmaps for levels less than 0 (temporary result)
    unsigned long *mNegLevColonBitmap[MAX_LEVEL];
    unsigned long *mNegLevCommaBitmap[MAX_LEVEL];
    // each thread starts with level 0
    // mMaxPositiveLevel saves the maximum positive level in current thread
    int mMaxPositiveLevel;
    // mMinNegativeLevel saves the minimum negative level in current thread
    int mMinNegativeLevel;
    // saves the level after processing the whole chunk, used for parallel index construction
    int mEndLevel;

    // following variables are used by ParallelBitmapIterator
    // temporary leveled colon bitmap is mapped to the correct level, which happens during the merging phase
    unsigned long *mFinalLevColonBitmap[MAX_LEVEL];
    unsigned long *mFinalLevCommaBitmap[MAX_LEVEL];
    // structural quote bitmap, used for getting the key field when iterating bitmaps
    unsigned long *mQuoteBitmap; 
    // word ids for the first and last words, often used when iterating leveled bitmap to get some information like colon, comma and key field positions
    long mStartWordId;
    long mEndWordId;
  
  public:
    LocalBitmap();	
    LocalBitmap(char* record, int level_num);
    ~LocalBitmap();
    // context inference for parallel index construction (step 3).
    // if it context information couldn't be inferred, return SPECULATIVE; else return NOSPECULATIVE
    int contextInference();
    // function for non-speculative parallel index construction
    void nonSpecIndexConstruction();
    // following two functions are used for speculative parallel index construction 
    void buildStringMaskBitmap();
    void buildLeveledBitmap();
    void setRecordLength(long length);
    void setThreadId(int thread_id) {mThreadId = thread_id;}
 
  private:
    void freeMemory();
};
#endif
