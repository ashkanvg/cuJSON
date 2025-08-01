#ifndef TOKENIZER_H
#define TONENIZER_H

#define IN 10
#define OUT 11

#define TRUE 100
#define ERROR 101
#define END 102

// token type
#define LCB 1   //'{'
#define RCB 2   //'}'
#define LB 3    //'['
#define RB 4    //']'
#define COM 5   //','
#define COLON 6 //':'
#define PRI 7   //primitive
#define STRING 8 // "abc"
#define UNKNOWN 9 
#define INVALID -1  //invalid

class Tokenizer {
  private:
    char* mChunk;
    // starting state of the separated chunk
    // IN -- inside string
    // OUT -- outside string
    int mStartState;
    // current position of mText
    long mCurPos;
    // current token type
    int mCurTknType;
    // starting position of the next token
    long mNextTknPos;
  public:
    Tokenizer() {}

    Tokenizer(char* chunk, int state) {
        mChunk = chunk;
        mStartState = state;
        mCurPos = 0;
        mCurTknType = UNKNOWN;
        mNextTknPos = 0;
    }

    // create iterator based on start state
    void createIterator(char* chunk, int state) {
        mChunk = chunk;
        mStartState = state;
        mCurPos = 0;
        mCurTknType = UNKNOWN;
        mNextTknPos = 0;
    }

    int oppositeState(int state) {
        if (state == IN) return OUT;
        if (state == OUT) return IN;
        return UNKNOWN;
    }

    // whether the next valid token exists, only iterates through the first word (64 bits)
    int hasNextToken();
    // returns the type of the next token, only iterates through the first word (64 bits)
    int nextToken();

  private:
    int getStringToken(int& pos);
};
#endif
