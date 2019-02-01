#include "pch.h"
#include "Evolution.h"
#include <iostream>
#include <vld.h>

using namespace std;

int main(int argc, char** argv)
{
	Evolution evo;

	for (int i = 1; i < argc; i += 2)
	{
		if (strcmp(argv[i], "-i") == 0)
		{
			evo.pathIN = argv[i + 1];
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			evo.pathOUT = argv[i + 1];
		}
		else if (strcmp(argv[i], "-r") == 0)
		{
			evo.r = atof(argv[i + 1]);
		}
		else if (strcmp(argv[i], "-w") == 0)
		{
			evo.w = atof(argv[i + 1]);
		}
		else if (strcmp(argv[i], "-p") == 0)
		{
			evo.iterations = atoll(argv[i + 1]);
		}
		else if (strcmp(argv[i], "-k") == 0)
		{
			evo.reproductionFactor = atof(argv[i + 1]);
		}
	}
	evo.Evolve();
}
