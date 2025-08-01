#include "LocalBitmap.h"
#include <immintrin.h>
#include <emmintrin.h>
#include <string.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/file.h>
#include <unistd.h>
#include <sched.h>
#include <unordered_map>
using namespace std;

LocalBitmap::LocalBitmap() {

}

LocalBitmap::LocalBitmap(char* record, int level_num) {
    this->mThreadId = 0;
    this->mRecord = record;
    this->mDepth = level_num - 1;
    this->mStartWordId = 0;
    this->mEndWordId = 0;
    this->mQuoteBitmap = NULL;
    this->mEscapeBitmap = NULL;
    this->mColonBitmap = NULL;
    this->mCommaBitmap = NULL;
    this->mStrBitmap = NULL;
    this->mLbraceBitmap = NULL;
    this->mRbraceBitmap = NULL;
    this->mLbracketBitmap = NULL;
    this->mRbracketBitmap = NULL;
    for (int i = 0; i < MAX_LEVEL; ++i) {
        this->mLevColonBitmap[i] = NULL;
        this->mLevCommaBitmap[i] = NULL;
        this->mNegLevColonBitmap[i] = NULL;
        this->mNegLevCommaBitmap[i] = NULL;
        this->mFinalLevColonBitmap[i] = NULL;
        this->mFinalLevCommaBitmap[i] = NULL;
    }
    this->mStartInStrBitmap = 0ULL;
    this->mEndInStrBitmap = 0ULL;
    this->mMaxPositiveLevel = 0;
    this->mMinNegativeLevel = -1;

    this->mNumTknErr = 0;
    this->mNumTrial = 0;
}

void LocalBitmap::freeMemory()
{
    for(int m = 0; m < MAX_LEVEL; ++m){
        if (mLevColonBitmap[m]) {
            free(mLevColonBitmap[m]);
            mLevColonBitmap[m] = NULL;
        }
        if (mLevCommaBitmap[m]) {
            free(mLevCommaBitmap[m]);
            mLevCommaBitmap[m] = NULL;
        }
        if (mNegLevColonBitmap[m]) {
            free(mNegLevColonBitmap[m]);
            mNegLevColonBitmap[m] = NULL;
        }
        if (mNegLevCommaBitmap[m]) {
            free(mNegLevCommaBitmap[m]);
            mNegLevCommaBitmap[m] = NULL;
        }
    }
    if (mQuoteBitmap) {
        free(mQuoteBitmap);
        mQuoteBitmap = NULL;
    }
    if (mEscapeBitmap) {
        free(mEscapeBitmap);
        mEscapeBitmap = NULL;
    }
    if (mStrBitmap) {
        free(mStrBitmap);
        mStrBitmap = NULL;
    }
    if (mColonBitmap) {
        free(mColonBitmap);
        mColonBitmap = NULL;
    }
    if (mCommaBitmap) {
        free(mCommaBitmap);
        mCommaBitmap = NULL;
    }
    if (mLbraceBitmap) {
        free(mLbraceBitmap);
        mLbraceBitmap = NULL;
    }
    if (mRbraceBitmap) {
        free(mRbraceBitmap);
        mRbraceBitmap = NULL;
    }
    if (mLbracketBitmap) {
        free(mLbracketBitmap);
        mLbracketBitmap = NULL;
    }
    if (mRbracketBitmap) {
        free(mRbracketBitmap);
        mRbracketBitmap = NULL;
    }
}

LocalBitmap::~LocalBitmap()
{
    freeMemory();
}

void LocalBitmap::setRecordLength(long length) {
    this->mRecordLength = length;
    this->mNumTmpWords = length / 32;
    this->mNumWords = length / 64;
    this->mQuoteBitmap = (unsigned long*)malloc((mNumWords) * sizeof(unsigned long));
}

int LocalBitmap::contextInference() {
    Tokenizer tkn;
    int start_states[2] = {OUT, IN};
    bool getStartState = false;
    int start_state = OUT;
    for (int j = 0; j < 2; ++j) {
        ++mNumTrial;
        int state = start_states[j];
        tkn.createIterator(mRecord, state);
        while (true) {
            int tkn_status = tkn.hasNextToken();
            if (tkn_status == END)
                break;
            if (tkn_status == ERROR) {
                ++mNumTknErr;
                start_state = tkn.oppositeState(state);
                getStartState = true;
                break;
            }
            tkn.nextToken();
        }
        if (getStartState == true) break;
    }
    if (start_state == IN) {
        mStartInStrBitmap = 0xffffffffffffffffULL;
    } else {
        mStartInStrBitmap = 0ULL;
    }
    //cout<<"inference result num of trails: "<<mNumTrial<<" num of token error "<<mNumTknErr<<endl;
    //cout<<"inference result "<<start_state<<" "<<getStartState<<endl;
    if (getStartState == true) return start_state;
    return UNKNOWN;
}

void LocalBitmap::nonSpecIndexConstruction() {
    // vectors for structural characters
    __m256i v_quote = _mm256_set1_epi8(0x22);
    __m256i v_colon = _mm256_set1_epi8(0x3a);
    __m256i v_escape = _mm256_set1_epi8(0x5c);
    __m256i v_lbrace = _mm256_set1_epi8(0x7b);
    __m256i v_rbrace = _mm256_set1_epi8(0x7d);
    __m256i v_comma = _mm256_set1_epi8(0x2c); 
    __m256i v_lbracket = _mm256_set1_epi8(0x5b);
    __m256i v_rbracket = _mm256_set1_epi8(0x5d);
	
    // variables for saving temporary results in the first four steps
    unsigned long colonbit0, quotebit0, escapebit0, stringbit0, lbracebit0, rbracebit0, commabit0, lbracketbit0, rbracketbit0;
    unsigned long colonbit, quotebit, escapebit, stringbit, lbracebit, rbracebit, commabit, lbracketbit, rbracketbit;
    unsigned long str_mask;
	
    // variables for saving temporary results in the last step
    unsigned long lb_mask, rb_mask, cb_mask;
    unsigned long lb_bit, rb_bit, cb_bit;
    unsigned long first, second;
    int cur_level = -1;
	
    // variables for saving context information among different words
    int top_word = -1;
    uint64_t prev_iter_ends_odd_backslash = 0ULL;
    uint64_t prev_iter_inside_quote = mStartInStrBitmap;
    const uint64_t even_bits = 0x5555555555555555ULL;
    const uint64_t odd_bits = ~even_bits; 

    for (int j = 0; j < mNumTmpWords; ++j) {
        colonbit = 0, quotebit = 0, escapebit = 0, stringbit = 0, lbracebit = 0, rbracebit = 0, commabit = 0, lbracketbit = 0, rbracketbit = 0;
        unsigned long i = j * 32; 
        // step 1: build structural character bitmaps
        __m256i v_text = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(mRecord + i));
        colonbit = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_colon));
        quotebit = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_quote)); 
        escapebit = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_escape)); 
        lbracebit  = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_lbrace));
        rbracebit  = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_rbrace));
        commabit = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_comma));
	lbracketbit = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_lbracket));
	rbracketbit = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_rbracket));
        // first half of the word (lowest 32 bits)
        if(j % 2 == 0) {
            colonbit0 = colonbit;
            quotebit0 = quotebit;
            escapebit0 = escapebit;
            lbracebit0 = lbracebit;
            rbracebit0 = rbracebit;
            commabit0 = commabit;
            lbracketbit0 = lbracketbit;
            rbracketbit0 = rbracketbit;
            continue;
        } else {
            // highest 32 bits inside a word
            colonbit = (colonbit << 32) | colonbit0;
            quotebit = (quotebit << 32) | quotebit0;
            escapebit = (escapebit << 32) | escapebit0;
            lbracebit = (lbracebit << 32) | lbracebit0;
            rbracebit = (rbracebit << 32) | rbracebit0;
            commabit = (commabit << 32) | commabit0;
            lbracketbit = (lbracketbit << 32) | lbracketbit0;
            rbracketbit = (rbracketbit << 32) | rbracketbit0;

            // step 2: update structural quote bitmaps
            uint64_t bs_bits = escapebit;
            uint64_t start_edges = bs_bits & ~(bs_bits << 1);
            int64_t even_start_mask = even_bits ^ prev_iter_ends_odd_backslash;
            uint64_t even_starts = start_edges & even_start_mask;
            uint64_t odd_starts = start_edges & ~even_start_mask;
            uint64_t even_carries = bs_bits + even_starts;
            int64_t odd_carries;
            bool iter_ends_odd_backslash = __builtin_uaddll_overflow(bs_bits, odd_starts,
                (unsigned long long *)(&odd_carries));
            odd_carries |= prev_iter_ends_odd_backslash;
            prev_iter_ends_odd_backslash = iter_ends_odd_backslash ? 0x1ULL : 0x0ULL;
            uint64_t even_carry_ends = even_carries & ~bs_bits;
            uint64_t odd_carry_ends = odd_carries & ~bs_bits;
            uint64_t even_start_odd_end = even_carry_ends & odd_bits;
            uint64_t odd_start_even_end = odd_carry_ends & even_bits;
            uint64_t odd_ends = even_start_odd_end | odd_start_even_end;
            int64_t quote_bits = quotebit & ~odd_ends;
            mQuoteBitmap[++top_word] = quote_bits;
        
            str_mask = _mm_cvtsi128_si64(_mm_clmulepi64_si128(
                _mm_set_epi64x(0ULL, quote_bits), _mm_set1_epi8(0xFFu), 0));
            str_mask ^= prev_iter_inside_quote;
            prev_iter_inside_quote = static_cast<uint64_t>(static_cast<int64_t>(str_mask) >> 63);
	
            // step 4: update structural character bitmaps
            unsigned long tmp = (~str_mask);
            colonbit = colonbit & tmp;
            lbracebit = lbracebit & tmp;
            rbracebit = rbracebit & tmp;
            commabit = commabit & tmp;
            lbracketbit = lbracketbit & tmp;
            rbracketbit = rbracketbit & tmp;
	
            // step 5: generate leveled bitmaps
            lb_mask = lbracebit | lbracketbit;
            rb_mask = rbracebit | rbracketbit;
            cb_mask = lb_mask | rb_mask;
            lb_bit = lb_mask & (-lb_mask);
            rb_bit = rb_mask & (-rb_mask);
            if (!cb_mask) {
                if (cur_level >= 0 && cur_level <= mDepth) {
                    if (!mLevColonBitmap[cur_level]) {
                        mLevColonBitmap[cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                    }
                    if (!mLevCommaBitmap[cur_level]) {
                        mLevCommaBitmap[cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                    }
                    if (colonbit) {
                        mLevColonBitmap[cur_level][top_word] = colonbit;
                    } else {
                        mLevCommaBitmap[cur_level][top_word] = commabit;
	            }
	        } else if (cur_level < 0) {
                    if (!mNegLevColonBitmap[-cur_level]) {
                        mNegLevColonBitmap[-cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                        // before finding the first bracket, update minimum negative level
                        if (cur_level < mMinNegativeLevel) {
                            mMinNegativeLevel = cur_level;
                        }
                    }
                    if (!mNegLevCommaBitmap[-cur_level]) {
                        mNegLevCommaBitmap[-cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                    }
                    if (colonbit) {
                        mNegLevColonBitmap[-cur_level][top_word] = colonbit;
                    } else {
                        mNegLevCommaBitmap[-cur_level][top_word] = commabit;
                    }
                }
            } else {
                first = 1;
                while (cb_mask || first) {
                    if (!cb_mask) {
                        second = 1UL<<63;
                    } else {
                        cb_bit = cb_mask & (-cb_mask);
                        second = cb_bit;
                    }
                    if (cur_level >= 0 && cur_level <= mDepth) {
                        if (!mLevColonBitmap[cur_level]) {
                            mLevColonBitmap[cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                        }
                        if (!mLevCommaBitmap[cur_level]) {
                            mLevCommaBitmap[cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                        }
                        unsigned long mask = second - first;
                        if (!cb_mask) mask = mask | second;
                        unsigned long colon_mask = mask & colonbit;
                        if (colon_mask) {
                            mLevColonBitmap[cur_level][top_word] |= colon_mask;
                        } else {
                            mLevCommaBitmap[cur_level][top_word] |= (commabit & mask);
                        }
                        if (cb_mask) {
                            if (cb_bit == rb_bit) {
                                mLevColonBitmap[cur_level][top_word] |= cb_bit;
                                mLevCommaBitmap[cur_level][top_word] |= cb_bit;
                            }
                            else if (cb_bit == lb_bit && cur_level + 1 <= mDepth) {
                                if (!mLevCommaBitmap[cur_level + 1]) {
                                     mLevCommaBitmap[cur_level + 1] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                                }
                                mLevCommaBitmap[cur_level + 1][top_word] |= cb_bit;
                            }
                        }
                    } else if (cur_level < 0) {
                        if (!mNegLevColonBitmap[-cur_level]) { 
                            mNegLevColonBitmap[-cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                        }
                        if (!mNegLevCommaBitmap[-cur_level]) {
                            mNegLevCommaBitmap[-cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                        }
                        unsigned long mask = second - first;
                        if (!cb_mask) mask = mask | second;
                        unsigned long colon_mask = mask & colonbit;
                        if (colon_mask) {
                            mNegLevColonBitmap[-cur_level][top_word] |= colon_mask;
                        } else {
                            mNegLevCommaBitmap[-cur_level][top_word] |= (commabit & mask);
                        }
                        if (cb_mask) {
                            if (cb_bit == rb_bit) {
                                mNegLevColonBitmap[-cur_level][top_word] |= cb_bit;
                                mNegLevCommaBitmap[-cur_level][top_word] |= cb_bit;
                            }
                            else if (cb_bit == lb_bit) {
                                if (cur_level + 1 == 0) {
                                    if (!mLevCommaBitmap[0]) {
                                         mLevCommaBitmap[0] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                                    }
                                    mLevCommaBitmap[0][top_word] |= cb_bit;
                                } else {
                                    if (!mNegLevCommaBitmap[-(cur_level + 1)]) {
                                        mNegLevCommaBitmap[-(cur_level + 1)] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                                    }
                                    mNegLevCommaBitmap[-(cur_level + 1)][top_word] |= cb_bit;
                                }
                            }
                        } 
                    }
                    if (cb_mask) {
                        if (cb_bit == lb_bit) {
                            lb_mask = lb_mask & (lb_mask - 1);
                            lb_bit = lb_mask & (-lb_mask);
                            ++cur_level;
                            if (mThreadId == 0 && cur_level == 0) {
                                // JSON record at the top level could be an array
                                if (!mLevCommaBitmap[cur_level]) {
                                     mLevCommaBitmap[cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                                }
                                mLevCommaBitmap[cur_level][top_word] |= cb_bit;
                            }
                        } else if (cb_bit == rb_bit) {
                            rb_mask = rb_mask & (rb_mask - 1);
                            rb_bit = rb_mask & (-rb_mask);
                            --cur_level;
                        }
                        first = second;
                        cb_mask = cb_mask & (cb_mask - 1);
                        if (cur_level > mMaxPositiveLevel) {
                            mMaxPositiveLevel = cur_level;
                        } else if (cur_level < mMinNegativeLevel) {
                            mMinNegativeLevel = cur_level;
                        }
                    } else {
                        first = 0;
                    }
                }
	    }
        }
    }
    if (mDepth == MAX_LEVEL - 1) mDepth = mMaxPositiveLevel;
    mEndLevel = cur_level;
}

void LocalBitmap::buildStringMaskBitmap() {
    // allocate memory space for saving results
    if (!mQuoteBitmap) {
        mQuoteBitmap = (unsigned long*)malloc((mNumWords) * sizeof(unsigned long));
    }
    if (!mColonBitmap) {
        mColonBitmap = (unsigned long*)malloc((mNumWords) * sizeof(unsigned long));
    }
    if (!mCommaBitmap) {
        mCommaBitmap = (unsigned long*)malloc((mNumWords) * sizeof(unsigned long));
    }
    if (!mStrBitmap) {
        mStrBitmap = (unsigned long*)malloc((mNumWords) * sizeof(unsigned long));
    }
    if (!mLbraceBitmap) {
        mLbraceBitmap = (unsigned long*)malloc((mNumWords) * sizeof(unsigned long));
    }
    if (!mRbraceBitmap) {
        mRbraceBitmap = (unsigned long*)malloc((mNumWords) * sizeof(unsigned long));
    }
    if (!mLbracketBitmap) {
        mLbracketBitmap = (unsigned long*)malloc((mNumWords) * sizeof(unsigned long));
    }
    if (!mRbracketBitmap) {
        mRbracketBitmap = (unsigned long*)malloc((mNumWords) * sizeof(unsigned long));
    }
 
    // vectors for structural characters
    __m256i v_quote = _mm256_set1_epi8(0x22);
    __m256i v_colon = _mm256_set1_epi8(0x3a);
    __m256i v_escape = _mm256_set1_epi8(0x5c);
    __m256i v_lbrace = _mm256_set1_epi8(0x7b);
    __m256i v_rbrace = _mm256_set1_epi8(0x7d);
    __m256i v_comma = _mm256_set1_epi8(0x2c); 
    __m256i v_lbracket = _mm256_set1_epi8(0x5b);
    __m256i v_rbracket = _mm256_set1_epi8(0x5d);
	
    // variables for saving temporary results
    unsigned long colonbit0, quotebit0, escapebit0, stringbit0, lbracebit0, rbracebit0, commabit0, lbracketbit0, rbracketbit0;
    unsigned long colonbit, quotebit, escapebit, stringbit, lbracebit, rbracebit, commabit, lbracketbit, rbracketbit;
    unsigned long str_mask;

    // variables for saving context information among different words
    int top_word = -1;
    uint64_t prev_iter_ends_odd_backslash = 0ULL;
    uint64_t prev_iter_inside_quote = mStartInStrBitmap;
    const uint64_t even_bits = 0x5555555555555555ULL;
    const uint64_t odd_bits = ~even_bits; 

    for (int j = 0; j < mNumTmpWords; ++j) {
        colonbit = 0, quotebit = 0, escapebit = 0, stringbit = 0, lbracebit = 0, rbracebit = 0, commabit = 0, lbracketbit = 0, rbracketbit = 0;
        unsigned long i = j * 32; 
        // step 1: build structural character bitmaps
        __m256i v_text = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(mRecord + i));
        colonbit = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_colon));
        quotebit = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_quote)); 
        escapebit = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_escape)); 
        lbracebit  = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_lbrace));
        rbracebit  = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_rbrace));
        commabit = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_comma));
	lbracketbit = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_lbracket));
	rbracketbit = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(v_text, v_rbracket));
        // first half of the word (lowest 32 bits)
        if(j % 2 == 0) {
            colonbit0 = colonbit;
            quotebit0 = quotebit;
            escapebit0 = escapebit;
            lbracebit0 = lbracebit;
            rbracebit0 = rbracebit;
            commabit0 = commabit;
            lbracketbit0 = lbracketbit;
            rbracketbit0 = rbracketbit;
            continue;
        } else {
            // highest 32 bits inside a word
            colonbit = (colonbit << 32) | colonbit0;
            quotebit = (quotebit << 32) | quotebit0;
            escapebit = (escapebit << 32) | escapebit0;
            lbracebit = (lbracebit << 32) | lbracebit0;
            rbracebit = (rbracebit << 32) | rbracebit0;
            commabit = (commabit << 32) | commabit0;
            lbracketbit = (lbracketbit << 32) | lbracketbit0;
            rbracketbit = (rbracketbit << 32) | rbracketbit0;
            mColonBitmap[++top_word] = colonbit;
            mCommaBitmap[top_word] = commabit;
            mLbraceBitmap[top_word] = lbracebit;
            mRbraceBitmap[top_word] = rbracebit;
            mLbracketBitmap[top_word] = lbracketbit;
            mRbracketBitmap[top_word] = rbracketbit;

            // step 2: update structural quote bitmaps
            uint64_t bs_bits = escapebit;
            uint64_t start_edges = bs_bits & ~(bs_bits << 1);
            int64_t even_start_mask = even_bits ^ prev_iter_ends_odd_backslash;
            uint64_t even_starts = start_edges & even_start_mask;
            uint64_t odd_starts = start_edges & ~even_start_mask;
            uint64_t even_carries = bs_bits + even_starts;
            int64_t odd_carries;
            bool iter_ends_odd_backslash = __builtin_uaddll_overflow(bs_bits, odd_starts,
                (unsigned long long *)(&odd_carries));
            odd_carries |= prev_iter_ends_odd_backslash;
            prev_iter_ends_odd_backslash = iter_ends_odd_backslash ? 0x1ULL : 0x0ULL;
            uint64_t even_carry_ends = even_carries & ~bs_bits;
            uint64_t odd_carry_ends = odd_carries & ~bs_bits;
            uint64_t even_start_odd_end = even_carry_ends & odd_bits;
            uint64_t odd_start_even_end = odd_carry_ends & even_bits;
            uint64_t odd_ends = even_start_odd_end | odd_start_even_end;
            int64_t quote_bits = quotebit & ~odd_ends;
            mQuoteBitmap[top_word] = quote_bits;
        
            // step 3: build string mask bitmaps
            str_mask = _mm_cvtsi128_si64(_mm_clmulepi64_si128(
                _mm_set_epi64x(0ULL, quote_bits), _mm_set1_epi8(0xFFu), 0));
            str_mask ^= prev_iter_inside_quote;
            mStrBitmap[top_word] = str_mask;
            prev_iter_inside_quote = static_cast<uint64_t>(static_cast<int64_t>(str_mask) >> 63);
        }
    }
    mEndInStrBitmap = prev_iter_inside_quote;
}

void LocalBitmap::buildLeveledBitmap() {
    // variables for saving temporary results in the first four steps
    unsigned long colonbit, quotebit, escapebit, stringbit, lbracebit, rbracebit, commabit, lbracketbit, rbracketbit;
    unsigned long str_mask;

     // variables for saving temporary results in the last step
    unsigned long lb_mask, rb_mask, cb_mask;
    unsigned long lb_bit, rb_bit, cb_bit;
    unsigned long first, second;
    int cur_level = -1;

    for (int j = 0; j < mNumWords; ++j) {
        // get input info
        colonbit = mColonBitmap[j];
        commabit = mCommaBitmap[j];
        lbracebit = mLbraceBitmap[j];
        rbracebit = mRbraceBitmap[j];
        lbracketbit = mLbracketBitmap[j];
        rbracketbit = mRbracketBitmap[j];
        str_mask = mStrBitmap[j];
        
        // step 4: update structural character bitmaps
        unsigned long tmp = (~str_mask);
        colonbit = colonbit & tmp;
        lbracebit = lbracebit & tmp;
        rbracebit = rbracebit & tmp;
        commabit = commabit & tmp;
        lbracketbit = lbracketbit & tmp;
        rbracketbit = rbracketbit & tmp;
        
        // step 5: generate leveled bitmaps
        lb_mask = lbracebit | lbracketbit;
        rb_mask = rbracebit | rbracketbit;
        cb_mask = lb_mask | rb_mask;
        lb_bit = lb_mask & (-lb_mask);
        rb_bit = rb_mask & (-rb_mask);
        int top_word = j;
        if (!cb_mask) {
            if (cur_level >= 0 && cur_level <= mDepth) {
                if (!mLevColonBitmap[cur_level]) {
                    mLevColonBitmap[cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                }
                if (!mLevCommaBitmap[cur_level]) {
                    mLevCommaBitmap[cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                }
                if (colonbit) {
                    mLevColonBitmap[cur_level][top_word] = colonbit;
                } else {
                    mLevCommaBitmap[cur_level][top_word] = commabit;
                }
            } else if (cur_level < 0) {
                if (!mNegLevColonBitmap[-cur_level]) {
                    mNegLevColonBitmap[-cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                }
                if (!mNegLevCommaBitmap[-cur_level]) {
                    mNegLevCommaBitmap[-cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                }
                if (colonbit) {
                    mNegLevColonBitmap[-cur_level][top_word] = colonbit;
                } else {
                    mNegLevCommaBitmap[-cur_level][top_word] = commabit;
                }
            }
        } else {
            first = 1;
            while (cb_mask || first) {
                if (!cb_mask) {
                    second = 1UL<<63;
                } else {
                    cb_bit = cb_mask & (-cb_mask);
                    second = cb_bit;
                }
                if (cur_level >= 0 && cur_level <= mDepth) {
                    if (!mLevColonBitmap[cur_level]) {
                        mLevColonBitmap[cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                    }
                    if (!mLevCommaBitmap[cur_level]) {
                        mLevCommaBitmap[cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                    }
                    unsigned long mask = second - first;
                    if (!cb_mask) mask = mask | second;
                    unsigned long colon_mask = mask & colonbit;
                    if (colon_mask) {
                        mLevColonBitmap[cur_level][top_word] |= colon_mask;
                    } else {
                        mLevCommaBitmap[cur_level][top_word] |= (commabit & mask);
                    }
                    if (cb_mask) {
                        if (cb_bit == rb_bit) {
                            mLevColonBitmap[cur_level][top_word] |= cb_bit;
                            mLevCommaBitmap[cur_level][top_word] |= cb_bit;
                        }
                        else if (cb_bit == lb_bit && cur_level + 1 <= mDepth) {
                            if (!mLevCommaBitmap[cur_level + 1]) {
                                mLevCommaBitmap[cur_level + 1] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                            }
                            mLevCommaBitmap[cur_level + 1][top_word] |= cb_bit;
                        }
                    }
                } else if (cur_level < 0) {
                    if (!mNegLevColonBitmap[-cur_level]) {
                        mNegLevColonBitmap[-cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                    }
                    if (!mNegLevCommaBitmap[-cur_level]) {
                        mNegLevCommaBitmap[-cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                    }
                    unsigned long mask = second - first;
                    if (!cb_mask) mask = mask | second;
                    unsigned long colon_mask = mask & colonbit;
                    if (colon_mask) {
                        mNegLevColonBitmap[-cur_level][top_word] |= colon_mask;
                    } else {
                        mNegLevCommaBitmap[-cur_level][top_word] |= (commabit & mask);
                    }
                    if (cb_mask) {
                        if (cb_bit == rb_bit) {
                            mNegLevColonBitmap[-cur_level][top_word] |= cb_bit;
                            mNegLevCommaBitmap[-cur_level][top_word] |= cb_bit;
                        }
                        else if (cb_bit == lb_bit) {
                            if (cur_level + 1 == 0) {
                                if (!mLevCommaBitmap[0]) {
                                    mLevCommaBitmap[0] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                                }
                                mLevCommaBitmap[0][top_word] |= cb_bit;
                            } else {
                                if (!mNegLevCommaBitmap[-(cur_level + 1)]) {
                                    mNegLevCommaBitmap[-(cur_level + 1)] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                                }
                                mNegLevCommaBitmap[-(cur_level + 1)][top_word] |= cb_bit;
                            }
                        }
                    }
                }
                if (cb_mask) {
                    if (cb_bit == lb_bit) {
                        lb_mask = lb_mask & (lb_mask - 1);
                        lb_bit = lb_mask & (-lb_mask);
                        ++cur_level;
                        if (mThreadId == 0 && cur_level == 0) {
                            // JSON record at the top level could be an array
                            if (!mLevCommaBitmap[cur_level]) {
                                mLevCommaBitmap[cur_level] = (unsigned long*)calloc(mNumWords, sizeof(unsigned long));
                            }
                            mLevCommaBitmap[cur_level][top_word] |= cb_bit;
                        }
                    } else if (cb_bit == rb_bit) {
                        rb_mask = rb_mask & (rb_mask - 1);
                        rb_bit = rb_mask & (-rb_mask);
                        --cur_level;
                    }
                    first = second;
                    cb_mask = cb_mask & (cb_mask - 1);
                    if (cur_level > mMaxPositiveLevel) {
                        mMaxPositiveLevel = cur_level;
                    } else if (cur_level < mMinNegativeLevel) {
                        mMinNegativeLevel = cur_level;
                    }
                } else {
                    first = 0;
                }
            }
        }
    }
    if (mDepth == MAX_LEVEL - 1) mDepth = mMaxPositiveLevel; 
    mEndLevel = cur_level;
}
