#include "ParallelBitmapIterator.h"
#include <sys/time.h>
#include <pthread.h>

struct ParallelBitmapMetadata {
    int start_word_id;
    int end_word_id;
    unsigned long* quote_bitmap;
    unsigned long* lev_colon_bitmap[MAX_LEVEL + 1];
    unsigned long* lev_comma_bitmap[MAX_LEVEL + 1];
};

ParallelBitmapMetadata pb_metadata[MAX_THREAD];

typedef struct CommaPosInfo {
    int thread_id;
    int level;
    long start_pos;
    long end_pos;
    long* comma_positions;
    long top_comma_positions;
}CommaPosInfo;

CommaPosInfo comma_pos_info[MAX_THREAD];

int num_of_threads = 1;

void* generateCommaPositionsInThread(void* arg) {
    int thread_id = (int)(*((int*)arg));;
    int level = comma_pos_info[thread_id].level;
    long start_pos = comma_pos_info[thread_id].start_pos;
    long end_pos = comma_pos_info[thread_id].end_pos;
    comma_pos_info[thread_id].comma_positions = new long[MAX_NUM_ELE /num_of_threads + 1];
    comma_pos_info[thread_id].top_comma_positions = -1;
    // cout<<"thread "<<thread_id<<" start generating comma positions."<<endl;
    // bind CPU
    cpu_set_t mask;
    cpu_set_t get;
    CPU_ZERO(&mask);
    CPU_SET(thread_id, &mask);
    if(pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
        cout<<"CPU binding failed for thread "<<thread_id<<endl;
    unsigned long* levels = pb_metadata[thread_id].lev_comma_bitmap[level];
    if (levels == NULL) {
        return NULL;
    }
    unsigned long commabit;
    long cur_start_pos = pb_metadata[thread_id].start_word_id;
    long cur_end_pos = pb_metadata[thread_id].end_word_id;
    long st = cur_start_pos > (start_pos / 64) ? cur_start_pos : (start_pos / 64);
    long ed = cur_end_pos < (ceil(double(end_pos) / 64)) ? cur_end_pos : (ceil(double(end_pos) / 64));
    for (long i = st; i < ed; ++i) {
        unsigned long idx = 0;
        if (thread_id >= 1) idx = i - cur_start_pos;
        else idx = i;
        commabit = levels[idx];
        int cnt = __builtin_popcountl(commabit);
        while (commabit) {
            long offset = i * 64 + __builtin_ctzll(commabit);
            if (start_pos <= offset && offset <= end_pos) {
                comma_pos_info[thread_id].comma_positions[++comma_pos_info[thread_id].top_comma_positions] = offset;
            }
            commabit = commabit & (commabit - 1);
        }
    }
    return NULL;    
}

void ParallelBitmapIterator::generateCommaPositionsParallel(long start_pos, long end_pos, int level, long* comma_positions, long& top_comma_positions) {
    // find starting and ending chunks in linked leveled comma bitmaps
    int start_chunk = -1;
    int end_chunk = -1;
    int chunk_num = mParallelBitmap->mThreadNum;
    for (int i = mCurChunkId; i < chunk_num; ++i) {
        if (pb_metadata[i].start_word_id <= (start_pos / 64)) {
            start_chunk = i;
        }
        if (pb_metadata[i].end_word_id >= (ceil(double(end_pos) / 64)) && end_chunk == -1) {
            end_chunk = i;
        }
        if (start_chunk > -1 && end_chunk > -1) break;
    }
    if(start_chunk == 0 && end_chunk == -1) end_chunk = 0;
    mCurChunkId = start_chunk;
    pthread_t thread[MAX_THREAD];
    int thread_args[MAX_THREAD];
    // iterate through corresponding linked leveled comma bitmaps
    for (int i = start_chunk; i <= end_chunk; ++i) {
        thread_args[i] = i;
        comma_pos_info[i].thread_id = i;
        comma_pos_info[i].level = level;
        comma_pos_info[i].start_pos = start_pos;
        comma_pos_info[i].end_pos = end_pos;
        int rc = pthread_create(&thread[i], NULL, generateCommaPositionsInThread, &thread_args[i]);
        if (rc)
        {
            cout<<"Thread Error; return code is "<<rc<<endl;
            return;
        }
    }
    for (int i = start_chunk; i <= end_chunk; ++i) {
        int rc = pthread_join(thread[i], NULL);
        if (rc)
        {
            cout<<"Thread Error; return code is "<<rc<<endl;
            return;
        }
    }
    for (int i = start_chunk; i <= end_chunk; ++i) {
        for (int j = 0; j <= comma_pos_info[i].top_comma_positions; ++j) {
            comma_positions[++top_comma_positions] = comma_pos_info[i].comma_positions[j];
        }
        free(comma_pos_info[i].comma_positions);
    }
}

// Saving metadata of linked leveled bitmap in consecutive order can further improve the performance.
void ParallelBitmapIterator::gatherParallelBitmapInfo() {
    int chunk_num = mParallelBitmap->mThreadNum;
    int depth = mParallelBitmap->mDepth;
    for (int chunk_id = 0; chunk_id < chunk_num; ++chunk_id) {
        pb_metadata[chunk_id].start_word_id = mParallelBitmap->mBitmaps[chunk_id]->mStartWordId;
        pb_metadata[chunk_id].end_word_id = mParallelBitmap->mBitmaps[chunk_id]->mEndWordId;
        pb_metadata[chunk_id].quote_bitmap = mParallelBitmap->mBitmaps[chunk_id]->mQuoteBitmap;
        for (int l = 0; l <= depth; ++l) {
            pb_metadata[chunk_id].lev_colon_bitmap[l] = mParallelBitmap->mBitmaps[chunk_id]->mFinalLevColonBitmap[l];
            pb_metadata[chunk_id].lev_comma_bitmap[l] = mParallelBitmap->mBitmaps[chunk_id]->mFinalLevCommaBitmap[l];
        }
    }
}

void ParallelBitmapIterator::generateColonPositions(long start_pos, long end_pos, int level, long* colon_positions, long& top_colon_positions) {
    // find starting and ending chunks in linked leveled colon bitmaps
    int start_chunk = -1;
    int end_chunk = -1;
    int thread_num = mParallelBitmap->mThreadNum;
    for (int i = mCurChunkId; i < thread_num; ++i) {
        if (pb_metadata[i].start_word_id <= (start_pos / 64)) {
            start_chunk = i;
        }
        if (pb_metadata[i].end_word_id >= (ceil(double(end_pos) / 64)) && end_chunk == -1) {
            end_chunk = i;
        }
        if (start_chunk > -1 && end_chunk > -1) break;
    }
    if(start_chunk == 0 && end_chunk == -1) end_chunk = 0;
    mCurChunkId = start_chunk;
    // iterate through the corresponding linked leveled colon bitmaps
    int cur_chunk = start_chunk;
    while (cur_chunk <= end_chunk) {
        unsigned long* levels = pb_metadata[cur_chunk].lev_colon_bitmap[level];
        if (levels == NULL) {
            ++cur_chunk;
            continue;
        } 
        unsigned long colonbit;
        long cur_start_pos = pb_metadata[cur_chunk].start_word_id;
        long cur_end_pos = pb_metadata[cur_chunk].end_word_id;
        long st = cur_start_pos > (start_pos / 64) ? cur_start_pos : (start_pos / 64);
        long ed = cur_end_pos < (ceil(double(end_pos) / 64)) ? cur_end_pos : (ceil(double(end_pos) / 64));
        for (long i = st; i < ed; ++i) {
            unsigned long idx = 0;
            if (cur_chunk >= 1) idx = i - cur_start_pos;
            else idx = i;
            colonbit = levels[idx];
            int cnt = __builtin_popcountl(colonbit);
            while (colonbit) {
                long offset = i * 64 + __builtin_ctzll(colonbit);
                if (start_pos <= offset && offset <= end_pos) {
                    colon_positions[++top_colon_positions] = offset;
                }
                colonbit = colonbit & (colonbit - 1);
            }
        }
        ++cur_chunk;
    }
}

void ParallelBitmapIterator::generateCommaPositions(long start_pos, long end_pos, int level, long* comma_positions, long& top_comma_positions) {
    // find starting and ending chunks in linked leveled comma bitmaps
    int start_chunk = -1;
    int end_chunk = -1;
    int chunk_num = mParallelBitmap->mThreadNum;
    for (int i = mCurChunkId; i < chunk_num; ++i) {
        if (pb_metadata[i].start_word_id <= (start_pos / 64)) {
            start_chunk = i;
        }
        if (pb_metadata[i].end_word_id >= (ceil(double(end_pos) / 64)) && end_chunk == -1) {
            end_chunk = i;
        }
        if (start_chunk > -1 && end_chunk > -1) break;
    }
    if(start_chunk == 0 && end_chunk == -1) end_chunk = 0;
    mCurChunkId = start_chunk;
    // iterate through the corresponding linked leveled comma bitmaps
    int cur_chunk = start_chunk; 
    while (cur_chunk <= end_chunk) {
        unsigned long* levels = pb_metadata[cur_chunk].lev_comma_bitmap[level];
        if (levels == NULL) {
            ++cur_chunk;
            continue;
        }
        unsigned long commabit;
        long cur_start_pos = pb_metadata[cur_chunk].start_word_id;
        long cur_end_pos = pb_metadata[cur_chunk].end_word_id;
        long st = cur_start_pos > (start_pos / 64) ? cur_start_pos : (start_pos / 64);
        long ed = cur_end_pos < (ceil(double(end_pos) / 64)) ? cur_end_pos : (ceil(double(end_pos) / 64));
        for (long i = st; i < ed; ++i) {
            unsigned long idx = 0;
            if (cur_chunk >= 1) idx = i - cur_start_pos;
            else idx = i;
            commabit = levels[idx];
            int cnt = __builtin_popcountl(commabit);
            while (commabit) {
                long offset = i * 64 + __builtin_ctzll(commabit);
                if (start_pos <= offset && offset <= end_pos) {
                    comma_positions[++top_comma_positions] = offset;
                }
                commabit = commabit & (commabit - 1);
            }
        }
        ++cur_chunk;
    }
}

bool ParallelBitmapIterator::findFieldQuotePos(long colon_pos, long& start_pos, long& end_pos) {
    long w_id = colon_pos/64;
    long offset = colon_pos%64;
    long start_quote = 0;
    long end_quote = 0;
    start_pos = 0; end_pos = 0;
    int cur_chunk = -1;
    int chunk_num = mParallelBitmap->mThreadNum;
    // find the chunk where the current colon is in
    for (int i = mCurChunkId; i < chunk_num; ++i) {
        if (w_id >= pb_metadata[i].start_word_id && w_id < pb_metadata[i].end_word_id) {
            cur_chunk = i;
            break;
        }
    }
    if (cur_chunk == -1) {
        return false;
    }
    while (w_id >= 0)
    {
        // check whether the current chunk needs to be updated  
        if (w_id < pb_metadata[cur_chunk].start_word_id) {
            //cout<<"update chunk id "<<cur_chunk<<endl;
            if ((--cur_chunk) == -1) {
                return false;
            }
        }
        long quote_id = w_id - pb_metadata[cur_chunk].start_word_id;
        unsigned long quotebit = pb_metadata[cur_chunk].quote_bitmap[quote_id];
        unsigned long offset = w_id * 64 + __builtin_ctzll(quotebit);
        while (quotebit && offset < colon_pos)
        {
            if (end_pos != 0)
            {
                start_quote = offset;
            }
            else if(start_quote == 0)
            {
                start_quote = offset;
            }
            else if(end_quote == 0)
            { 
                end_quote = offset;
            }
            else
            {
                start_quote = end_quote;
                end_quote = offset;
            }
            quotebit = quotebit & (quotebit - 1);
            offset = w_id * 64 + __builtin_ctzll(quotebit); 
        }
        if(start_quote != 0 && end_quote == 0)
        {
            end_quote = start_quote;
            start_quote = 0;
            end_pos = end_quote;
        }
        else if(start_quote != 0 && end_quote != 0)
        {
            start_pos = start_quote;
            end_pos = end_quote;
            return true;
        }
        --w_id;
    }
    return false;
}

ParallelBitmapIterator* ParallelBitmapIterator::getCopy() {
    ParallelBitmapIterator* pbi = new ParallelBitmapIterator();
    pbi->mParallelBitmap = mParallelBitmap;
    pbi->mCurLevel = mCurLevel;
    pbi->mTopLevel = mCurLevel;
    pbi->mCurChunkId = mCurChunkId;
    pbi->mFindDomArray = mFindDomArray;
    if (pbi->mTopLevel >= 0) {
        pbi->mCtxInfo[mCurLevel].type = mCtxInfo[mCurLevel].type;
        pbi->mCtxInfo[mCurLevel].positions = mCtxInfo[mCurLevel].positions;
        pbi->mCtxInfo[mCurLevel].start_idx = mCtxInfo[mCurLevel].start_idx;
        pbi->mCtxInfo[mCurLevel].end_idx = mCtxInfo[mCurLevel].end_idx;
        pbi->mCtxInfo[mCurLevel].cur_idx = -1;
        pbi->mPosArrAlloc[mCurLevel] = mPosArrAlloc[mCurLevel];
        pbi->mCtxInfo[mCurLevel + 1].positions = NULL;
        for (int i = mCurLevel + 1; i < MAX_LEVEL; ++i) {
            pbi->mPosArrAlloc[i] = false;
            pbi->mPosArrAlloc[i] = NULL;
        }
    }
    pbi->mCopiedIterator = true;
    return pbi;
}

bool ParallelBitmapIterator::up() {
    if (mCurLevel == mTopLevel) return false;
    --mCurLevel;
    return true;
}

bool ParallelBitmapIterator::down() {
    if (mCurLevel < mTopLevel || mCurLevel > mParallelBitmap->mDepth) return false;
    ++mCurLevel;
    long  start_pos = -1;
    long end_pos = -1;
    int thread_num = mParallelBitmap->mThreadNum;
    if (mCurLevel == mTopLevel + 1) {
        if (mTopLevel == -1) {
            long text_length = mParallelBitmap->mRecordLength;
            start_pos = 0;
            end_pos = text_length;
            mCtxInfo[mCurLevel].positions = (long*)malloc((text_length / thread_num + 1) * sizeof (long));
            mPosArrAlloc[mCurLevel] = true;
        } else {
            long cur_idx = mCtxInfo[mCurLevel - 1].cur_idx;
            start_pos = mCtxInfo[mCurLevel - 1].positions[cur_idx];
            end_pos = mCtxInfo[mCurLevel - 1].positions[cur_idx + 1];
            if (mCtxInfo[mCurLevel].positions == NULL || mPosArrAlloc[mCurLevel] == false) {
                mCtxInfo[mCurLevel].positions = (long*)malloc((MAX_NUM_ELE / thread_num + 1) * sizeof (long));
                mPosArrAlloc[mCurLevel] = true;
            }
        }
        mCtxInfo[mCurLevel].start_idx = 0;
        mCtxInfo[mCurLevel].cur_idx = -1;
        mCtxInfo[mCurLevel].end_idx = -1; 
    } else {
        long cur_idx = mCtxInfo[mCurLevel - 1].cur_idx;
        if (cur_idx > mCtxInfo[mCurLevel - 1].end_idx) {
            --mCurLevel;
            return false;
        }
        start_pos = mCtxInfo[mCurLevel - 1].positions[cur_idx];
        end_pos = mCtxInfo[mCurLevel - 1].positions[cur_idx + 1];
        mCtxInfo[mCurLevel].positions = mCtxInfo[mCurLevel - 1].positions;
        mCtxInfo[mCurLevel].start_idx = mCtxInfo[mCurLevel - 1].end_idx + 1;
        mCtxInfo[mCurLevel].cur_idx = mCtxInfo[mCurLevel - 1].end_idx;
        mCtxInfo[mCurLevel].end_idx = mCtxInfo[mCurLevel - 1].end_idx;
    }
    long i = start_pos;
    if (start_pos > 0 || mCurLevel > 0) ++i;
    char ch = mParallelBitmap->mRecord[i];
    while (i < end_pos && (ch == ' ' || ch == '\n')) {
        ch = mParallelBitmap->mRecord[++i];
    }
    if (mParallelBitmap->mRecord[i] == '{') {
        mCtxInfo[mCurLevel].type = OBJECT;
        generateColonPositions(i, end_pos, mCurLevel, mCtxInfo[mCurLevel].positions, mCtxInfo[mCurLevel].end_idx);
        return true;
    } else if (mParallelBitmap->mRecord[i] == '[') {
        mCtxInfo[mCurLevel].type = ARRAY;
        if (mFindDomArray == false && (end_pos - i + 1) > SINGLE_THREAD_MAX_ARRAY_SIZE) {
            generateCommaPositionsParallel(i, end_pos, mCurLevel, mCtxInfo[mCurLevel].positions, mCtxInfo[mCurLevel].end_idx);
            mFindDomArray = true;  
        } else {
            generateCommaPositions(i, end_pos, mCurLevel, mCtxInfo[mCurLevel].positions, mCtxInfo[mCurLevel].end_idx);
        }
        return true;
    }
    --mCurLevel;
    return false;
}

bool ParallelBitmapIterator::isObject() {
    if (mCurLevel >= 0 && mCurLevel <= mParallelBitmap->mDepth && mCtxInfo[mCurLevel].type == OBJECT) {
        return true;
    }
    return false;
}

bool ParallelBitmapIterator::isArray() {
    if (mCurLevel >= 0 && mCurLevel <= mParallelBitmap->mDepth && mCtxInfo[mCurLevel].type == ARRAY) {
        return true;
    }
    return false;
}

bool ParallelBitmapIterator::moveNext() {
    if (mCurLevel < 0 || mCurLevel > mParallelBitmap->mDepth || mCtxInfo[mCurLevel].type != ARRAY) return false;
    long next_idx = mCtxInfo[mCurLevel].cur_idx + 1;
    if (next_idx >= mCtxInfo[mCurLevel].end_idx) return false;
    mCtxInfo[mCurLevel].cur_idx = next_idx;
    return true;
}

bool ParallelBitmapIterator::moveToKey(char* key) {
    if (mCurLevel < 0 || mCurLevel > mParallelBitmap->mDepth || mCtxInfo[mCurLevel].type != OBJECT) return false;
    long cur_idx = mCtxInfo[mCurLevel].cur_idx + 1;
    long end_idx = mCtxInfo[mCurLevel].end_idx;
    while (cur_idx < end_idx) {
        long colon_pos = mCtxInfo[mCurLevel].positions[cur_idx];
        long start_pos = 0, end_pos = 0;
        if (!findFieldQuotePos(colon_pos, start_pos, end_pos)) {
            return false;
        }
        int key_size = end_pos - start_pos - 1;
        if (key_size == strlen(key)) {
            memcpy(mKey, mParallelBitmap->mRecord + start_pos + 1, key_size);
            mKey[end_pos - start_pos - 1] = '\0';
            if (memcmp(mKey, key, key_size) == 0) {
                mCtxInfo[mCurLevel].cur_idx = cur_idx;
                return true;
            }
        }
        ++cur_idx;
    }
    return false;
}

char* ParallelBitmapIterator::moveToKey(unordered_set<char*>& key_set) {
    if (key_set.empty() == true || mCurLevel < 0 || mCurLevel > mParallelBitmap->mDepth || mCtxInfo[mCurLevel].type != OBJECT) return NULL;
    long cur_idx = mCtxInfo[mCurLevel].cur_idx + 1;
    long end_idx = mCtxInfo[mCurLevel].end_idx;
    while (cur_idx < end_idx) {
        long colon_pos = mCtxInfo[mCurLevel].positions[cur_idx];
        long start_pos = 0, end_pos = 0;
        if (!findFieldQuotePos(colon_pos, start_pos, end_pos)) {
            return NULL;
        }
        bool has_m_key = false;
        unordered_set<char*>::iterator iter;
        for (iter = key_set.begin(); iter != key_set.end(); ++iter) {
            char* key = (*iter);
            int key_size = end_pos - start_pos - 1;
            if (key_size == strlen(key)) {
                if (has_m_key == false) {
                    memcpy(mKey, mParallelBitmap->mRecord + start_pos + 1, key_size);
                    mKey[end_pos - start_pos - 1] = '\0';
                    has_m_key = true;
                }
                if (memcmp(mKey, key, key_size) == 0) {
                    mCtxInfo[mCurLevel].cur_idx = cur_idx;
                    key_set.erase(iter);
                    return key;
                }
            }
        }
        ++cur_idx;
    }
    mCtxInfo[mCurLevel].cur_idx = cur_idx;
    return NULL;
}

int ParallelBitmapIterator::numArrayElements() {
    if (mCurLevel >= 0 && mCurLevel <= mParallelBitmap->mDepth && mCtxInfo[mCurLevel].type == ARRAY) {
        return mCtxInfo[mCurLevel].end_idx - mCtxInfo[mCurLevel].start_idx;
    }
    return 0;
}

bool ParallelBitmapIterator::moveToIndex(int index) {
    if (mCurLevel < 0 || mCurLevel > mParallelBitmap->mDepth || mCtxInfo[mCurLevel].type != ARRAY) return false;
    long next_idx = mCtxInfo[mCurLevel].start_idx + index;
    if (next_idx > mCtxInfo[mCurLevel].end_idx) return false;
    mCtxInfo[mCurLevel].cur_idx = next_idx;
    return true;
}

char* ParallelBitmapIterator::getValue() {
    if (mCurLevel < 0 || mCurLevel > mParallelBitmap->mDepth) return NULL;
    long cur_idx = mCtxInfo[mCurLevel].cur_idx;
    long next_idx = cur_idx + 1;
    if (next_idx > mCtxInfo[mCurLevel].end_idx) return NULL;
    // current ':' or ','
    long cur_pos = mCtxInfo[mCurLevel].positions[cur_idx];
    // next ':' or ','
    long next_pos = mCtxInfo[mCurLevel].positions[next_idx];
    int type = mCtxInfo[mCurLevel].type;
    if (type == OBJECT && next_idx < mCtxInfo[mCurLevel].end_idx) {
        long start_pos = 0, end_pos = 0;
        if (findFieldQuotePos(next_pos, start_pos, end_pos) == false) {
            return "";
        }
        // next quote
        next_pos = start_pos;
    }
    long text_length = next_pos - cur_pos - 1;
    if (text_length <= 0) return "";
    char* ret = (char*)malloc(text_length + 1);
    memcpy(ret, mParallelBitmap->mRecord + cur_pos + 1, text_length);
    ret[text_length] = '\0';
    return ret;
}
