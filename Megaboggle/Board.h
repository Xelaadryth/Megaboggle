#pragma once

#include <string>
#include "Dictionary.h"

const int MAX_ADJACENT = 4;

struct BoardNode
{
    BoardNode* mUp = nullptr;
    BoardNode* mDown = nullptr;
    BoardNode* mLeft = nullptr;
    BoardNode* mRight = nullptr;
    char mValue;
};

class Board
{
public:
    Board(const std::string filename);
    ~Board();

    void solve();
    //Only run this after solving
    void writeAnswers();

private:
    BoardNode** mBoard;
    unsigned int mNumRows;
    unsigned int mNumColumns;
    //May contain doubles
    char** mAnswers;
};
