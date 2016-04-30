#pragma once

#include <string>

struct CommandLineParameters
{
    std::string mDictPath = "";
    std::string mBoardPath = "";
	bool mBenchmark = true;

    CommandLineParameters(int argc, char** argv);
};