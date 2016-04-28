#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "Dictionary.h"
#include "Timer.h"


void parseCommandLine(int argc, char** argv, std::string* dictPath, std::string* boardPath)
{
	for (int i = 1; i < argc; ++i)
	{
		if (strlen(argv[i]) < 2 || argc - i < 2)
		{
			continue;
		}
		switch (argv[i][0])
		{

		case '-':
			switch (argv[i][1])
			{
			case 'd':
				*dictPath = argv[++i];
				break;
			case 'i':
				*boardPath = argv[++i];
			}
		}
	}

	if (!dictPath)
	{
		printf("Please input -d [DICTIONARY PATH].\n");
	}
	if (!boardPath)
	{
		printf("Please input -i [BOARD PATH].\n");
	}
}


int main(int argc, char** argv)
{
	std::string dictPath;
	std::string boardPath;
	std::string* dictPathAddr = &dictPath;
	std::string* boardPathAddr = &boardPath;

	parseCommandLine(argc, argv, dictPathAddr, boardPathAddr);
	if (dictPath == "" || boardPath == "") {
		_getch();
		return -1;
	}

	printf("Loading dictionary...\n");
	Dictionary dictionary(dictPath);

	printf("Loading board...\n");
	// TODO: Load the board
	std::cout << boardPath << "\n";

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

