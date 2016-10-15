#pragma once



#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>



namespace OptionProcessing
{
	enum class Action
	{
		Invalid = -1,
		Generate,
		LastDigitAsciiArt,
		LastDigitCsv,
		LastDigitCsvWithHeaders,
		DiagonalAverages,
		PlotDiagonalAverages
	};

	extern std::string							programName;
	extern Action								action;
	extern int									base;
	extern std::string							baseAlphabet;
	extern int									baseAlphabetPending;
	extern long									limit;
	extern std::ofstream						outputFile;
	extern std::streambuf*						outputBuf;
	extern std::ifstream						primeInput;
	extern int									primeInputBeginIndex;
	extern int									primeInputBase;
	extern std::string							primeInputBaseAlphabet;
	extern int									rowBegin, rowInterval;
	extern int									columnBegin, columnInterval;
	extern bool									enableAsciiArtColors;
	//extern int								numGeneratorThreads;
	extern int									terminalWidth, terminalHeight;
	extern bool									terminalSizeOptional;

	void										printUsage();
	void										process(int argc, char** argv);
}
