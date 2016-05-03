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


int main(int argc, char** argv)
{
    CommandLineParameters* params = new CommandLineParameters(argc, argv);
    if (params->mDictPath == "" || params->mBoardPath == "" || params->mOutfile == "") {
        printf("Additional options in top of Solver.h\n");
        _getch();
        return -1;
    }

    printf("Loading dictionary...");
    Timer dictionaryTimer;
    Dictionary* dictionary = new Dictionary(params->mDictPath);
    printf("%f milliseconds.\n", dictionaryTimer.stop());

    printf("Loading board...");
    Timer boardTimer;
    Board* board = new Board(params->mBoardPath);
    Solver* solver = new Solver(dictionary, board);
    printf("%f milliseconds.\n", boardTimer.stop());

    printf("\nLoading complete! Hit any key to start solving!\n");
    _getch();
    printf("\nSolving...");

    Timer solveTimer;
    solver->solve();
    printf("%f milliseconds.\n", solveTimer.stop());

    printf("\nFormatting output for results...");
    Timer sortTimer;
    dictionary->outputResults(params->mOutfile);
    printf("%f milliseconds.\n", sortTimer.stop());

    //Cleanup!
    delete params;
    delete dictionary;
    delete board;
    delete solver;

    //Print out memory leaks
    //_CrtDumpMemoryLeaks();

    printf("\nPress any key to exit the program.\n");
    _getch();

    return 0;
}

