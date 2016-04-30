#include <fstream>
#include <forward_list>
#include <string>

#include "Board.h"

BoardNode::BoardNode(char value) :
    mValue(value) {}

Board::Board(const std::string filename)
{
    //Add all the words!
    std::string line;
    std::ifstream boardFile(filename);
    if (boardFile.is_open())
    {
        unsigned int m = 0;
        unsigned int n = 0;
        bool isFirst = true;

        //Make a list of lists, which we'll then convert into an array of BoardNodes
        std::forward_list<std::forward_list<char>> rows;
        while (std::getline(boardFile, line))
        {
            //Count how many rows there are
            ++m;

            //Make a list of letters
            std::forward_list<char> row;
            for (unsigned int i = 0; i < line.length(); i++)
            {
                if (i % 2 == 0)
                {
                    row.push_front(line.at(i));

                    //Count how many letters are in a row
                    if (isFirst)
                    {
                        ++n;
                    }
                }
            }

            //Add this row to our list of rows
            rows.push_front(row);

            isFirst = false;
        }

        //TODO: Construct the m x n grid of BoardNodes
    }
    else
    {
        printf("\n\nFailed to open board file: %s\n", filename.c_str());
    }
}

Board::~Board()
{
}

void Board::solve()
{
}

void Board::writeAnswers()
{
}
