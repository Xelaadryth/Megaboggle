#pragma once

#include <string>

struct CommandLineParameters
{
    std::string mDictPath = "";
    std::string mBoardPath = "";
	std::string mOutfile = "";

    CommandLineParameters(int argc, char** argv);
};