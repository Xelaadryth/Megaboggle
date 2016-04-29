#pragma once

#include <string>

struct CommandLineParameters
{
    std::string dictPath = "";
    std::string boardPath = "";

    CommandLineParameters(int argc, char** argv);
};