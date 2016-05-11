#pragma once

struct Board
{
    Board(const std::string filename);
    ~Board();
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

    unsigned int mWidth;
    unsigned int mHeight;
    char* mBoard;
};
