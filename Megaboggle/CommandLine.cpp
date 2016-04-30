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
			case 'o':
				mBenchmark = false;
				break;
            }
        }
    }

    if (mDictPath == "")
    {
        printf("Please input -d [DICTIONARY PATH].\n");
    }
    if (mBoardPath == "")
    {
        printf("Please input -i [BOARD PATH].\n");
    }
}
