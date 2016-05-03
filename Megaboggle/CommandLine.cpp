#include "CommandLine.h"

CommandLineParameters::CommandLineParameters(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (strlen(argv[i]) < 2 || argc - i < 1)
        {
            continue;
        }
        switch (argv[i][0])
        {

        case '-':
            switch (argv[i][1])
            {
            case 'd':
                mDictPath = argv[++i];
                break;
            case 'i':
                mBoardPath = argv[++i];
                break;
            case 'h':
                mHelp = true;
                break;
            case 'n':
                mNumRuns = atoi(argv[++i]);
                break;
            case 'o':
                mOutfile = argv[++i];
                break;
            case 'v':
                mVerbose = true;
                break;
            }
        }
    }

    if (mHelp || mDictPath == "" || mBoardPath == "" || mOutfile == "")
    {
        printf("Usage:\n\n  Required:\n    -d    [DICTIONARY PATH]    Path to dictionary file.\n    -i    [BOARD PATH]         Path to Boggle board file.\n    -o    [OUT PATH]           Path to found words output file.\n  Optional:\n    -h                         Display these descriptions.\n    -n    [NUM ITERATIONS]     Number of times to run the solver.\n    -v                         Display runtimes for all sections.\n\n  Example:\n    Megaboggle.exe -d \"D:\\Megaboggle\\Dictionaries\\words.txt\" -i \"D:\\Megaboggle\\Boards\\1000.txt\" -o \"D:\\Megaboggle\\output.txt\" -n 2 -v\n\n  Threading options in top of Solver.h\n");
    }
}
