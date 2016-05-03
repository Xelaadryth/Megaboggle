#pragma once

struct Board
{
    Board(const std::string filename);
    ~Board();

    unsigned int mWidth;
    unsigned int mHeight;
    char* mBoard;
};
