#pragma once

#include <string>

class CommandLineParameters
{
public:
    CommandLineParameters(int argc, char** argv);

    std::string mDictPath = "";
    std::string mBoardPath = "";
    std::string mOutfile = "";
    unsigned int mNumRuns = 1;
    unsigned int mGenerateBoardSize = 0;
    bool mHelp = false;
    bool mVerbose = false;
};
