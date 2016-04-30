#pragma once

#include <string>
#include "Dictionary.h"

const int MAX_ADJACENT = 4;

struct BoardNode
{
    BoardNode(char value);

    char mValue;
    BoardNode* mUp = nullptr;
    BoardNode* mDown = nullptr;
    BoardNode* mLeft = nullptr;
    BoardNode* mRight = nullptr;
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
    BoardNode** board;
    //May contain doubles
    char** mAnswers;
};
