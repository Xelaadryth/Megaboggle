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

	printf("Loading dictionary...\n");
	Dictionary dictionary(params.dictPath);

	printf("Loading board...\n");
	// TODO: Load the board
	std::cout << params.boardPath << "\n";

	printf("\nLoading complete! Hit any key to start solving!\n");
	_getch();

	Timer solveTimer;

	// TODO: Solve board
	_getch();

	long stopTime = solveTimer.stop();
	printf("Solved in %li milliseconds.\n", solveTimer.stop());

	_getch();
	return 0;
}

