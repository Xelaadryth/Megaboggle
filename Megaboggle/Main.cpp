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


void run(Dictionary* dictionary, CommandLineParameters* params)
{
    if (params->mVerbose)
    {
        printf("Loading board...");
    }
    Timer boardTimer;
    Board* board = new Board(params->mBoardPath);
    Solver* solver = new Solver(dictionary, board);
    if (params->mVerbose)
    {
        printf("%f milliseconds\n", boardTimer.stop());
    }

    if (params->mVerbose)
    {
        printf("\nSolving...");
    }

    Timer solveTimer;
    solver->solve();
    printf("%f milliseconds\n", solveTimer.stop());

    if (params->mVerbose)
    {
        printf("\nFormatting output for results...");
    }
    Timer sortTimer;
    dictionary->outputResults(params->mOutfile);
    if (params->mVerbose)
    {
        printf("%f milliseconds\n", sortTimer.stop());
    }

    //Cleanup!
    delete board;
    delete solver;
}


int main(int argc, char** argv)
{
    CommandLineParameters* params = new CommandLineParameters(argc, argv);
    if (params->mHelp || params->mDictPath == "" || params->mBoardPath == "" || params->mOutfile == "") {
        _getch();
        return -1;
    }

    if (params->mVerbose)
    {
        printf("Loading dictionary...");
    }
    Timer dictionaryTimer;
    Dictionary* dictionary = new Dictionary(params->mDictPath);
    if (params->mVerbose)
    {
        printf("%f milliseconds\n", dictionaryTimer.stop());
    }

    for (unsigned int i = 0; i < params->mNumRuns; ++i)
    {
        if (params->mVerbose && i)
        {
            printf("\n    ----------------------------\n\n");
        }

        run(dictionary, params);
    }

    //Cleanup!
    delete dictionary;
    delete params;

    //Print out memory leaks
    //_CrtDumpMemoryLeaks();

    printf("\nPress any key to exit the program.\n");
    _getch();

    return 0;
}

