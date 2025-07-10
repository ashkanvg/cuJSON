#include "ParallelBitmap.h"

ParallelBitmap::ParallelBitmap(char* record, int thread_num, int depth) {
    mRecord = record;
    mDepth = depth;
    mThreadNum = thread_num;
    char* start_chunk = record;
    int rec_len = strlen(record);
    mRecordLength = rec_len;
    int chunk_len = rec_len / thread_num;
    if (chunk_len % 64 > 0) {
        chunk_len = chunk_len + 64 - chunk_len % 64;
    }
    int cur_len = 0;
    mParallelMode = NONSPECULATIVE;
    for (int i = 0; i < thread_num; ++i) {
        mBitmaps[i] = new LocalBitmap(start_chunk, depth);
        mBitmaps[i]->setThreadId(i);
        if (i < thread_num - 1) {
            int pad_len = 0;
            // escaped backslashes are not separated into different chunks
            while (start_chunk[chunk_len + pad_len - 1] == '\\') {
                pad_len += 64;
            }
            mBitmaps[i]->setRecordLength(chunk_len + pad_len);
            start_chunk = start_chunk + chunk_len + pad_len;
            cur_len += (chunk_len + pad_len);
        } else {
            int last_chunk_len = rec_len - cur_len;
            mBitmaps[i]->setRecordLength(last_chunk_len);
        }
        // perform context inference and decide whether the program runs in speculative mode
        if (mBitmaps[i]->contextInference() == UNKNOWN) {
            mParallelMode = SPECULATIVE;
        }
    }
}

ParallelBitmap::ParallelBitmap(char* record, long rec_len, int thread_num, int depth) {
    mRecord = record;
    mDepth = depth;
    mThreadNum = thread_num;
    char* start_chunk = record;
    mRecordLength = rec_len;
    int chunk_len = rec_len / thread_num;
    if (chunk_len % 64 > 0) {
        chunk_len = chunk_len + 64 - chunk_len % 64;
    }
    int cur_len = 0;
    mParallelMode = NONSPECULATIVE;
    for (int i = 0; i < thread_num; ++i) {
        mBitmaps[i] = new LocalBitmap(start_chunk, depth);
        if (i < thread_num - 1) {
            int pad_len = 0;
            // escaped backslashes are not separated into different chunks
            while (start_chunk[chunk_len + pad_len - 1] == '\\') {
                pad_len += 64;
            }
            mBitmaps[i]->setRecordLength(chunk_len + pad_len);
            start_chunk = start_chunk + chunk_len + pad_len;
            cur_len += (chunk_len + pad_len);
        } else {
            int last_chunk_len = rec_len - cur_len;
            mBitmaps[i]->setRecordLength(last_chunk_len);
        }
        // perform context inference and decide whether the program runs in speculative mode
        if (mBitmaps[i]->contextInference() == UNKNOWN) {
            mParallelMode = SPECULATIVE;
        }
    }
}

ParallelBitmap::~ParallelBitmap() {
    for (int i = 0; i < mThreadNum; ++i) {
        delete mBitmaps[i];
    }
}

int ParallelBitmap::parallelMode() {
    return mParallelMode;
}

void ParallelBitmap::setRecordLength(long length) {
    mRecordLength = length;
}

void ParallelBitmap::rectifyStringMaskBitmaps() {
    //cout<<"start verification"<<endl;
    unsigned long prev_iter_inside_quote = mBitmaps[0]->mEndInStrBitmap;
    for (int i = 1; i < mThreadNum; ++i) {
       if (prev_iter_inside_quote != mBitmaps[i]->mStartInStrBitmap) {
           mBitmaps[i]->mStartInStrBitmap = prev_iter_inside_quote;
           // flip string mask bitmaps
           //cout<<"flip for "<<i<<"th thread "<<endl;
           for (int j = 0; j < mBitmaps[i]->mNumWords; ++j) {
               mBitmaps[i]->mStrBitmap[j] = ~mBitmaps[i]->mStrBitmap[j];
           }
           if (mBitmaps[i]->mEndInStrBitmap == 0) {
               mBitmaps[i]->mEndInStrBitmap = 0xffffffffffffffffULL;
           } else {
               mBitmaps[i]->mEndInStrBitmap = 0ULL;
           }
       }
       prev_iter_inside_quote = mBitmaps[i]->mEndInStrBitmap;
    }
    //cout<<"end verification"<<endl;
}

void ParallelBitmap::mergeBitmaps() {
    //cout<<"start merge"<<endl;
    int cur_level = mBitmaps[0]->mEndLevel;
    long offset = 0;
    for (int i = 0; i <= mBitmaps[0]->mMaxPositiveLevel; ++i) {
        mBitmaps[0]->mFinalLevColonBitmap[i] = mBitmaps[0]->mLevColonBitmap[i];
        mBitmaps[0]->mFinalLevCommaBitmap[i] = mBitmaps[0]->mLevCommaBitmap[i];
    }
    offset += mBitmaps[0]->mNumWords;
    mBitmaps[0]->mStartWordId = 0;
    mBitmaps[0]->mEndWordId = offset;
    // link leveled colon and comma bitmaps generated from different threads
    for (int i = 1; i < mThreadNum; ++i) {
        mBitmaps[i]->mStartWordId = offset;
        mBitmaps[i]->mEndWordId = offset + mBitmaps[i]->mNumWords;
        for(int j = 1; j <= -mBitmaps[i]->mMinNegativeLevel && (cur_level - j + 1) >= 0; ++j) {
            mBitmaps[i]->mFinalLevColonBitmap[cur_level - j + 1] = mBitmaps[i]->mNegLevColonBitmap[j];
            mBitmaps[i]->mFinalLevCommaBitmap[cur_level - j + 1] = mBitmaps[i]->mNegLevCommaBitmap[j]; 
        }
        for(int j = 0; j <= mBitmaps[i]->mMaxPositiveLevel && (cur_level + j + 1) >= 0; ++j) {
            mBitmaps[i]->mFinalLevColonBitmap[cur_level + j + 1] = mBitmaps[i]->mLevColonBitmap[j];
            mBitmaps[i]->mFinalLevCommaBitmap[cur_level + j + 1] = mBitmaps[i]->mLevCommaBitmap[j];
        }
        cur_level += (mBitmaps[i]->mEndLevel + 1);
        offset += mBitmaps[i]->mNumWords;
    }
    //cout<<"final level after merge "<<cur_level<<" "<<endl;
}
