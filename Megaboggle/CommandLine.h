#pragma once

#include <string>

struct CommandLineParameters
{
    std::string mDictPath = "";
    std::string mBoardPath = "";
    std::string mOutfile = "";
    unsigned int mNumRuns = 1;
    unsigned int mGenerateBoardSize = 0;
    bool mHelp = false;
    bool mVerbose = false;

    CommandLineParameters(int argc, char** argv);
};
