//Memory leak detection, comment out this and _CrtDumpMemoryLeaks when ready for production
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//-----------------------------------------------------------------------------------------
#include <conio.h>
#include <iostream>
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
    if (params->mDictPath == "" || params->mBoardPath == "") {
        printf("Failed to load dictionary and/or board.\n");
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
    Solver* solver = new Solver(dictionary, board, params->mOutfile);
    printf("%f milliseconds.\n", boardTimer.stop());

    printf("\nLoading complete! Hit any key to start solving!\n");
    _getch();

    Timer solveTimer;
    solver->solve();
    printf("\nSolved in %f milliseconds.\n", solveTimer.stop());

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

