#include "SerialBitmapIterator.h"

void SerialBitmapIterator::generateColonPositions(long start_pos, long end_pos, int level, long* colon_positions, long& top_colon_positions) {
    unsigned long colonbit;
    long st = start_pos / 64;
    long ed = ceil(double(end_pos) / 64);
    for (long i = st; i < ed; ++i) {
        colonbit = mSerialBitmap->mLevColonBitmap[level][i];
        int cnt = __builtin_popcountl(colonbit);
        while (colonbit) {
            long offset = i * 64 + __builtin_ctzll(colonbit);
            if (start_pos <= offset && offset <= end_pos) {
                colon_positions[++top_colon_positions] = offset;
            }
            colonbit = colonbit & (colonbit - 1);
        }  
    }
}

void SerialBitmapIterator::generateCommaPositions(long start_pos, long end_pos, int level, long* comma_positions, long& top_comma_positions) {
    unsigned long commabit;
    long st = start_pos / 64;
    long ed = ceil(double(end_pos) / 64);
    for (long i = st; i < ed; ++i) {
        commabit = mSerialBitmap->mLevCommaBitmap[level][i];
        int cnt = __builtin_popcountl(commabit);
        while (commabit) {
            long offset = i * 64 + __builtin_ctzll(commabit);
            if (start_pos <= offset && offset <= end_pos) {
                comma_positions[++top_comma_positions] = offset;
            }
            commabit = commabit & (commabit - 1);
        }
    }
}

bool SerialBitmapIterator::findFieldQuotePos(long colon_pos, long& start_pos, long& end_pos) {
    long w_id = colon_pos/64;
    long offset = colon_pos%64;
    long start_quote = 0;
    long end_quote = 0;
    start_pos = 0; end_pos = 0;
    while (w_id >= 0)
    {
        unsigned long quotebit = mSerialBitmap->mQuoteBitmap[w_id];
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

SerialBitmapIterator* SerialBitmapIterator::getCopy() {
    SerialBitmapIterator* sbi = new SerialBitmapIterator();
    sbi->mSerialBitmap = mSerialBitmap;
    sbi->mCurLevel = mCurLevel;
    sbi->mTopLevel = mCurLevel;
    if (sbi->mTopLevel >= 0) {
        sbi->mCtxInfo[mCurLevel].type = mCtxInfo[mCurLevel].type;
        sbi->mCtxInfo[mCurLevel].positions = mCtxInfo[mCurLevel].positions;
        sbi->mCtxInfo[mCurLevel].start_idx = mCtxInfo[mCurLevel].start_idx;
        sbi->mCtxInfo[mCurLevel].end_idx = mCtxInfo[mCurLevel].end_idx;
        sbi->mCtxInfo[mCurLevel].cur_idx = -1;
        sbi->mCtxInfo[mCurLevel + 1].positions = NULL;
    }
    return sbi;
}

bool SerialBitmapIterator::up() {
    if (mCurLevel == mTopLevel) return false;
    --mCurLevel; 
    return true;
}

bool SerialBitmapIterator::down() {
    if (mCurLevel < mTopLevel || mCurLevel > mSerialBitmap->mDepth) return false;
    ++mCurLevel;
    //cout<<"down function cur level "<<mCurLevel<<" "<<mTopLevel<<" "<<mSerialBitmap->mDepth<<endl;
    long start_pos = -1;
    long end_pos = -1;
    if (mCurLevel == mTopLevel + 1) {
        if (mTopLevel == -1) {
            long record_length = mSerialBitmap->mRecordLength;
            start_pos = 0;
            end_pos = record_length;
            mCtxInfo[mCurLevel].positions = new long[record_length / 8 + 1]; 
            mPosArrAlloc[mCurLevel] = true;
        } else {
            long cur_idx = mCtxInfo[mCurLevel - 1].cur_idx;
            start_pos = mCtxInfo[mCurLevel - 1].positions[cur_idx];
            end_pos = mCtxInfo[mCurLevel - 1].positions[cur_idx + 1];
            if (mCtxInfo[mCurLevel].positions == NULL || mPosArrAlloc[mCurLevel] == false) {
                mCtxInfo[mCurLevel].positions = new long[MAX_NUM_ELE / 8 + 1];
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
  //          cout<<"exception "<<cur_idx<<" "<<mCtxInfo[mCurLevel - 1].end_idx<<endl;
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
    char ch = mSerialBitmap->mRecord[i];
    while (i < end_pos && (ch == ' ' || ch == '\n')) {
        ch = mSerialBitmap->mRecord[++i];
    }
    if (mSerialBitmap->mRecord[i] == '{') {
        mCtxInfo[mCurLevel].type = OBJECT; 
        generateColonPositions(i, end_pos, mCurLevel, mCtxInfo[mCurLevel].positions, mCtxInfo[mCurLevel].end_idx); 
        return true;
    } else if (mSerialBitmap->mRecord[i] == '[') {
        mCtxInfo[mCurLevel].type = ARRAY;
        generateCommaPositions(i, end_pos, mCurLevel, mCtxInfo[mCurLevel].positions, mCtxInfo[mCurLevel].end_idx);
        return true;
    }
    --mCurLevel;
    return false;
}

bool SerialBitmapIterator::isObject() {
    if (mCurLevel >= 0 && mCurLevel <= mSerialBitmap->mDepth && mCtxInfo[mCurLevel].type == OBJECT) {
        return true;
    }
    return false;
}

bool SerialBitmapIterator::isArray() {
    if (mCurLevel >= 0 && mCurLevel <= mSerialBitmap->mDepth && mCtxInfo[mCurLevel].type == ARRAY) {
        return true;
    }
    return false;
}

bool SerialBitmapIterator::moveNext() {
    if (mCurLevel < 0 || mCurLevel > mSerialBitmap->mDepth || mCtxInfo[mCurLevel].type != ARRAY) return false;
    long next_idx = mCtxInfo[mCurLevel].cur_idx + 1;
    if (next_idx >= mCtxInfo[mCurLevel].end_idx) return false;
    mCtxInfo[mCurLevel].cur_idx = next_idx;
    return true;
}

bool SerialBitmapIterator::moveToKey(char* key) {
    if (mCurLevel < 0 || mCurLevel > mSerialBitmap->mDepth || mCtxInfo[mCurLevel].type != OBJECT) return false;
    long cur_idx = mCtxInfo[mCurLevel].cur_idx + 1;
    long end_idx = mCtxInfo[mCurLevel].end_idx;
    while (cur_idx < end_idx) {
        long colon_pos = mCtxInfo[mCurLevel].positions[cur_idx];
        long start_pos = 0, end_pos = 0;
        if (!findFieldQuotePos(colon_pos, start_pos, end_pos)) {
            return false;
        }
        ++mVisitedFields;
        int key_size = end_pos - start_pos - 1;
        if (key_size == strlen(key)) {
            memcpy(mKey, mSerialBitmap->mRecord + start_pos + 1, key_size);
            mKey[end_pos - start_pos - 1] = '\0';
            if (memcmp(mKey, key, key_size) == 0) {
                mCtxInfo[mCurLevel].cur_idx = cur_idx;
                return true;
            }
        }
        ++cur_idx;
    }
    mCtxInfo[mCurLevel].cur_idx = cur_idx;
    return false;
}

char* SerialBitmapIterator::moveToKey(unordered_set<char*>& key_set) {
    if (key_set.empty() == true|| mCurLevel < 0 || mCurLevel > mSerialBitmap->mDepth || mCtxInfo[mCurLevel].type != OBJECT) return NULL;
    long cur_idx = mCtxInfo[mCurLevel].cur_idx + 1;
    long end_idx = mCtxInfo[mCurLevel].end_idx;
    while (cur_idx < end_idx) {
        long colon_pos = mCtxInfo[mCurLevel].positions[cur_idx];
        long start_pos = 0, end_pos = 0;
        if (!findFieldQuotePos(colon_pos, start_pos, end_pos)) {
            return NULL;
        }
        ++mVisitedFields;
        bool has_m_key = false;
        unordered_set<char*>::iterator iter;
        for (iter = key_set.begin(); iter != key_set.end(); ++iter) {
            char* key = (*iter);
            int key_size = end_pos - start_pos - 1;
            if (key_size == strlen(key)) {
                if (has_m_key == false) {
                    memcpy(mKey, mSerialBitmap->mRecord + start_pos + 1, key_size);
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

int SerialBitmapIterator::numArrayElements() {
    if (mCurLevel >= 0 && mCurLevel <= mSerialBitmap->mDepth && mCtxInfo[mCurLevel].type == ARRAY) {
        return mCtxInfo[mCurLevel].end_idx - mCtxInfo[mCurLevel].start_idx;
    }
    return 0;
}

bool SerialBitmapIterator::moveToIndex(int index) {
    if (mCurLevel < 0 || mCurLevel > mSerialBitmap->mDepth || mCtxInfo[mCurLevel].type != ARRAY) return false;
    long next_idx = mCtxInfo[mCurLevel].start_idx + index;
    if (next_idx > mCtxInfo[mCurLevel].end_idx) return false;
    mCtxInfo[mCurLevel].cur_idx = next_idx;
    return true;
}

char* SerialBitmapIterator::getValue() {
    if (mCurLevel < 0 || mCurLevel > mSerialBitmap->mDepth) return NULL;
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
///    cout<<"cur pos "<<(cur_pos + 1)<<" length "<<text_length<<endl;
    memcpy(ret, mSerialBitmap->mRecord + cur_pos + 1, text_length);
    ret[text_length] = '\0';
    return ret;
}
