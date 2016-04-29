#include "CommandLine.h"

CommandLineParameters::CommandLineParameters(int argc, char** argv)
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
				dictPath = argv[++i];
				break;
			case 'i':
				boardPath = argv[++i];
				break;
			}
		}
	}

	if (dictPath == "")
	{
		printf("Please input -d [DICTIONARY PATH].\n");
	}
	if (boardPath == "")
	{
		printf("Please input -i [BOARD PATH].\n");
	}
}
