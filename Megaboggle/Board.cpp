#include <fstream>
#include <vector>
#include <string>

#include "Board.h"

Board::Board(const std::string filename)
{
    //Add all the words!
    std::string line;
    std::ifstream boardFile(filename);
    if (boardFile.is_open())
    {
        //Make a list of lists, which we'll then convert into an array of BoardNodes
        std::vector<std::vector<char>> letterRows;
        while (std::getline(boardFile, line))
        {
            //Make a list of letters
            std::vector<char> letterRow;
            for (unsigned int i = 0; i < line.length(); i++)
            {
                if (i % 2 == 0)
                {
                    letterRow.push_back(line.at(i));
                }
            }

            // The final row of the file may or may not be a blank line
            if (letterRow.size() > 0)
            {
                //Add this row to our list of rows
                letterRows.push_back(letterRow);
            }
        }

        //Assume that the file is at least a 1x1 grid
        mNumRows = letterRows.size();
        mNumColumns = letterRows[0].size();

        mBoard = new BoardNode*[mNumRows];

        //Construct the m x n grid of BoardNodes
        for (unsigned int j = 0; j < mNumRows; ++j)
        {
            //Initialize an empty "row" in our board, where a "row" is an array of BoardNodes
            mBoard[j] = new BoardNode[mNumColumns];

            //Fill in our row
            for (unsigned int i = 0; i < mNumColumns; ++i)
            {
                mBoard[j][i].mValue = letterRows[j][i];

                //Add our left neighbor if they exist
                if (i > 0)
                {
                    mBoard[j][i].mLeft = &(mBoard[j][i - 1]);
                    mBoard[j][i - 1].mRight = &(mBoard[j][i]);
                }

                //Add our up neighbor if they exist
                if (j > 0)
                {
                    mBoard[j][i].mUp = &(mBoard[j - 1][i]);
                    mBoard[j - 1][i].mDown = &(mBoard[j][i]);
                }
            }
        }
    }
    else
    {
        printf("\n\nFailed to open board file: %s\n", filename.c_str());
    }
}

Board::~Board()
{
    for (unsigned int j = 0; j < mNumRows; ++j)
    {
        delete mBoard[j];
    }

    delete mBoard;
}

void Board::solve()
{
}

void Board::writeAnswers()
{
}
