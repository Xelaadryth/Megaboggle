#pragma once

#include <string>

struct Board
{
    Board(const std::string filename);
    ~Board();

    unsigned int mHeight;
    unsigned int mWidth;
    char* mBoard;
};
