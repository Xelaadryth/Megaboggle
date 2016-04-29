#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "CommandLine.h"
#include "Dictionary.h"
#include "Timer.h"


int main(int argc, char** argv)
{

	CommandLineParameters params = CommandLineParameters(argc, argv);
	if (params.dictPath == "" || params.boardPath == "") {
		printf("Failed to load dictionary and/or board.\n");
		_getch();
		return -1;
	}

	printf("Loading dictionary...");
	Timer dictionaryTimer;
	Dictionary dictionary(params.dictPath);
	printf("%li milliseconds.\n", dictionaryTimer.stop());

	printf("Loading board...");
	Timer boardTimer;
	//TODO: Load the board
	printf("%li milliseconds.\n", boardTimer.stop());

	printf("\nLoading complete! Hit any key to start solving!\n");
	_getch();

	Timer solveTimer;
	//TODO: Solve board
	printf("Solved in %li milliseconds.\n", solveTimer.stop());
	_getch();

	return 0;
}

