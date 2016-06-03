//Memory leak detection, comment out this and _CrtDumpMemoryLeaks when ready for production
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//-----------------------------------------------------------------------------------------
#include <conio.h>
#include <fstream>
#include <string>

#include "Board.h"
#include "CommandLine.h"
#include "Dictionary.h"
#include "Solver.h"
#include "Timer.h"


void Run(Dictionary& dictionary, CommandLineParameters& params)
{
    if (params.mVerbose)
    {
        printf("Loading board...");
    }
    Timer boardTimer;
    Board board(params.mBoardPath);
    Solver solver(dictionary, board);
    if (params.mVerbose)
    {
        printf("%f milliseconds\n", boardTimer.stop());
    }

    if (params.mVerbose)
    {
        printf("\nSolving...");
    }

    Timer solveTimer;
    solver.Solve();
    printf("%f milliseconds\n", solveTimer.stop());

    if (params.mVerbose)
    {
        printf("\nFormatting output for results...");
    }
    Timer sortTimer;
    dictionary.OutputResults(params.mOutfile);
    if (params.mVerbose)
    {
        printf("%f milliseconds\n", sortTimer.stop());
    }
}


int main(int argc, char** argv)
{
    {
        CommandLineParameters params(argc, argv);
        if (params.mHelp || params.mDictPath == "" || params.mBoardPath == "" || params.mOutfile == "")
        {
            _getch();
            return -1;
        }

        if (params.mVerbose)
        {
            printf("\nLoading dictionary...");
        }
        Timer dictionaryTimer;
        Dictionary dictionary(params.mDictPath);
        if (params.mVerbose)
        {
            printf("%f milliseconds\n", dictionaryTimer.stop());
        }

        for (unsigned int i = 0; i < params.mNumRuns; ++i)
        {
            if (params.mVerbose && i)
            {
                printf("\n    ----------------------------\n\n");
            }

            Run(dictionary, params);
        }

        //TODO: Make this a command line argument function
        //Generate a randomized board
        //if (params.mGenerateBoardSize)
        //{
        //    if (params.mVerbose)
        //    {
        //        printf("\nGenerating board at C:\\board.csv...");
        //    }
        //    Timer generateTimer;

        //    FILE* outfile;
        //    fopen_s(&outfile, "C:\\board.csv", "w");

        //    for (unsigned int y = 0; y < params.mGenerateBoardSize; ++y)
        //    {
        //        for (unsigned int x = 0; x < params.mGenerateBoardSize - 1; ++x)
        //        {
        //            fprintf(outfile, "%c,", 'a' + rand() % 26);
        //        }
        //        fprintf(outfile, "%c\n", 'a' + rand() % 26);
        //    }

        //    fclose(outfile);

        //    if (params.mVerbose)
        //    {
        //        printf("%f milliseconds\n", generateTimer.stop());
        //    }
        //}
    }

    //Print out memory leaks
    //_CrtDumpMemoryLeaks();

    printf("\nPress any key to exit the program.\n");
    _getch();

    return 0;
}

