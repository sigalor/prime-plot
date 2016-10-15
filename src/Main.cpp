#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <algorithm>
#include <functional>

#include "PrimeGenerator.hpp"
#include "NumberStringOperations.hpp"
#include "OptionProcessing.hpp"
#include "CPPUtilities.hpp"
#include "BraillePixelsScreen.hpp"

void processAllPrimes(std::function<void(long)> worker, bool withInputFile)
{
	std::vector<long> primes;
	long lastCurrLimit = 0, currLimit = 0, chunkSize = 50000, lastInpNum = 1;
	
	if(withInputFile)
	{
		while(currLimit < OptionProcessing::primeInputBeginIndex)												//generate primes before prime input
		{
			primes = PrimeGenerator::findPrimes(primes.size()==0 ? 2 : primes.back(), std::min(OptionProcessing::primeInputBeginIndex-currLimit, chunkSize), &lastCurrLimit);
			for(long& p : primes)
				worker(p);
			currLimit += primes.size();
			lastInpNum = primes.back() + 1;
		}
	
		if(OptionProcessing::primeInput.is_open())
		{
			while(!OptionProcessing::primeInput.eof()  &&  currLimit < OptionProcessing::limit)
			{
				std::string inpLine;
				std::getline(OptionProcessing::primeInput, inpLine);
				if(inpLine != ""  &&  [&]() { try { lastInpNum = NumberStringOperations::stolCustom(inpLine, OptionProcessing::primeInputBase, OptionProcessing::primeInputBaseAlphabet); } catch(...) { std::cerr << "warning: could not interpret number '" << inpLine << "' from prime input file\n"; return false; } return true; }())
				{
					worker(lastInpNum);
					currLimit++;
				}
			}
		}
		
		lastCurrLimit = 0;
		primes.clear();
	}

	lastInpNum++;
	while(currLimit < OptionProcessing::limit)																	//generate primes after prime input
	{
		primes = PrimeGenerator::findPrimes(primes.size()==0 ? lastInpNum : primes.back(), std::min(OptionProcessing::limit-currLimit, chunkSize), &lastCurrLimit);
		for(long& p : primes)
			worker(p);
		currLimit += primes.size();
	}
}

void outputLastDigits(std::ostream& output, const std::vector<std::vector<int>>& lastDigitCount, int maxLastDigitCount, int asciiArtNumLength)
{
	if(OptionProcessing::action == OptionProcessing::Action::LastDigitAsciiArt)
	{
		if(OptionProcessing::base-1 > maxLastDigitCount)
			maxLastDigitCount = OptionProcessing::base-1;
		if(maxLastDigitCount != 0)
			asciiArtNumLength = floor(log10(maxLastDigitCount)) + 1;
		output << " " << std::string(asciiArtNumLength, ' ') << " |";
		for(int i=OptionProcessing::columnBegin; i<OptionProcessing::base; i+=OptionProcessing::columnInterval)
			output << " " << NumberStringOperations::print("%*d", asciiArtNumLength, i) << " " << (i+OptionProcessing::columnInterval>=OptionProcessing::base ? "\n" : "|");
		for(int i=OptionProcessing::columnBegin-OptionProcessing::columnInterval; i<OptionProcessing::base; i+=OptionProcessing::columnInterval)
			output << "-" << std::string(asciiArtNumLength, '-') << "-" << (i+OptionProcessing::columnInterval>=OptionProcessing::base ? "\n" : "+");
	}
	else if(OptionProcessing::action == OptionProcessing::Action::LastDigitCsvWithHeaders)
	{
		output << ",";
		for(int i=OptionProcessing::columnBegin; i<OptionProcessing::base; i+=OptionProcessing::columnInterval)
			output << i << (i+OptionProcessing::columnInterval>=OptionProcessing::base ? "\n" : "|");
	}
	
	for(std::size_t i=0; i<lastDigitCount.size(); i++)
	{
		int actualI = i*OptionProcessing::rowInterval + OptionProcessing::rowBegin;
		if(OptionProcessing::action == OptionProcessing::Action::LastDigitAsciiArt)
			output << " " << NumberStringOperations::print("%*d", asciiArtNumLength, actualI) << " |";
		else if(OptionProcessing::action == OptionProcessing::Action::LastDigitCsvWithHeaders)
			output << actualI << ",";
	
		for(std::size_t j=0; j<lastDigitCount[i].size(); j++)
		{
			if(OptionProcessing::action == OptionProcessing::Action::LastDigitAsciiArt)
			{
				int brightness = 255 - static_cast<int>(static_cast<float>(lastDigitCount[i][j])/static_cast<float>(maxLastDigitCount)*255.0f);
				if(OptionProcessing::enableAsciiArtColors  &&  output.rdbuf() == std::cout.rdbuf())
					output << "\x1b[48;2;255;" << brightness << ";" << brightness << "m";							//rgb color in unix terminal, from http://stackoverflow.com/a/26665998
				output << " " << NumberStringOperations::print("%*d", asciiArtNumLength, lastDigitCount[i][j]) << " ";
				if(OptionProcessing::enableAsciiArtColors  &&  output.rdbuf() == std::cout.rdbuf())
					output << "\x1b[0m";
				output << (j==lastDigitCount[i].size()-1 ? "\n" : "|");
			}
			else if(OptionProcessing::action == OptionProcessing::Action::LastDigitCsv  ||  OptionProcessing::action == OptionProcessing::Action::LastDigitCsvWithHeaders)
				output << lastDigitCount[i][j] << (j==lastDigitCount[i].size()-1 ? "\n" : ",");
		}
	}
}

float getDiagonalAverage(const std::vector<std::vector<int>>& lastDigitCount, std::size_t beginX, std::size_t beginY)
{
	int x = static_cast<int>(beginX);
	int y = static_cast<int>(beginY);
	float total = 0.0f;
	std::size_t num = 0;

	while(x >= 0  &&  y >= 0)
	{
		total += lastDigitCount[y][x];
		x--;
		y--;
		num++;
	}
	
	if(num == 0)
		return 0.0f;
	return total/static_cast<float>(num);
}

std::vector<float> getDiagonalAverages(const std::vector<std::vector<int>>& lastDigitCount)
{
	std::vector<float> ret;
	
	if(lastDigitCount.size() > 0)
		for(std::size_t x=0; x<lastDigitCount[0].size(); x++)
			ret.push_back(getDiagonalAverage(lastDigitCount, x, lastDigitCount.size()-1));
	for(int y=static_cast<int>(lastDigitCount.size()-2); y>=0; y--)
		ret.push_back(getDiagonalAverage(lastDigitCount, lastDigitCount.size()-1, static_cast<std::size_t>(y)));
	
	return ret;
}

void outputDiagonalAverages(std::ostream& output, const std::vector<float>& diagonalAverages)
{
	for(auto& f : diagonalAverages)
		output << f << "\n";
}

void plotDiagonalAverages(std::ostream& output, const std::vector<float>& diagonalAverages)
{
	#ifdef __unix__
	BraillePixelsScreen bps;
	#else
	if(terminalWidth <= 0  ||  terminalHeight <= 0)
	{
		std::cerr << "error: needed option --terminal-size not given\n";
		std::exit(1);
	}
	BraillePixelsScreen bps(OptionProcessing::terminalWidth*2, OptionProcessing::terminalHeight*4);
	#endif
	
	float maxX = static_cast<float>(diagonalAverages.size()-1);
	float maxY = *std::max_element(diagonalAverages.begin(), diagonalAverages.end());
	float xDisplace = std::to_string(static_cast<int>(maxY)).size() + 2;
	float yDisplace = 2;
	float heightX = static_cast<float>(bps.getWidth()/2 - xDisplace - (std::to_string(diagonalAverages.size()-1).size()+1) + 1);
	float heightY = static_cast<float>(bps.getHeight()/4 - yDisplace);
	float xAxisSpacing = heightX / maxX;
	float yAxisSpacing = heightY / maxY;
	
	float currX, prevX = -10.0f, nextMinX = 2.0f;
	
	bps.clear();
	bps.drawLine(xDisplace*2, bps.getHeight()-yDisplace*4-1, bps.getWidth()-1, bps.getHeight()-yDisplace*4-1);		//x axis
	bps.drawLine(xDisplace*2, bps.getHeight()-yDisplace*4-1, xDisplace*2, 0);										//y axis
	for(std::size_t x=0; x<diagonalAverages.size(); x++)
	{
		currX = xDisplace + static_cast<float>(x)*xAxisSpacing;
		if(currX-prevX >= nextMinX)
		{
			std::string xStr = std::to_string(x);
			bps.setString(currX, heightY, xStr);
			nextMinX = xStr.size() + 1;
			prevX = floor(currX);
		}
		
		if(x >= 1)
			bps.drawLine((xDisplace + static_cast<float>(x-1)*xAxisSpacing)*2, ((1.0f - diagonalAverages[x-1]/maxY)*(heightY-1.0f))*4+3, currX*2, ((1.0f - diagonalAverages[x]/maxY)*(heightY-1.0f))*4+3);
	}
	for(float y=0.0f; y<heightY; y+=yAxisSpacing)
	{
		bps.setStringRightAligned(xDisplace-2, heightY-y-1.0f, std::to_string(static_cast<int>(y/heightY*maxY)));
		bps.setPixel(static_cast<int>(xDisplace-2)*2 + 3, static_cast<int>(heightY-y-1.0f)*4 + 3);
		bps.setPixel(static_cast<int>(xDisplace-2)*2 + 2, static_cast<int>(heightY-y-1.0f)*4 + 3);
	}

	bps.flush(output);
}

int main(int argc, char** argv)
{
	OptionProcessing::process(argc, argv);
	std::ostream output(OptionProcessing::outputBuf);
	if(OptionProcessing::action == OptionProcessing::Action::Generate)
	{
		processAllPrimes(std::function<void(long)>([&](long p) { output << NumberStringOperations::ltosCustom(p, OptionProcessing::base, OptionProcessing::baseAlphabet) << "\n"; }), true);
	}
	else if(OptionProcessing::action == OptionProcessing::Action::LastDigitAsciiArt  ||  OptionProcessing::action == OptionProcessing::Action::LastDigitCsv  ||  OptionProcessing::action == OptionProcessing::Action::LastDigitCsvWithHeaders  ||  OptionProcessing::action == OptionProcessing::Action::DiagonalAverages  ||  OptionProcessing::action == OptionProcessing::Action::PlotDiagonalAverages)
	{
		std::vector<std::vector<int>> lastDigitCount;																//first dimension/index and left column: 'from' digit, second dimension/index and top row: 'to' digit
		int maxLastDigitCount = 0;
		long prevLastDigit = -1, currLastDigit;
		int asciiArtNumLength = 1;
		
		lastDigitCount = CPPUtilities::create2DVector<int>(ceil(static_cast<float>(OptionProcessing::base-OptionProcessing::rowBegin) / static_cast<float>(OptionProcessing::rowInterval)),
														   ceil(static_cast<float>(OptionProcessing::base-OptionProcessing::columnBegin) / static_cast<float>(OptionProcessing::columnInterval)));
		processAllPrimes(std::function<void(long)>([&](long p)
		{
			currLastDigit = p % OptionProcessing::base;
			if(prevLastDigit != -1  &&  prevLastDigit-OptionProcessing::rowBegin >= 0  &&  currLastDigit-OptionProcessing::columnBegin >= 0  &&  (prevLastDigit-OptionProcessing::rowBegin)%OptionProcessing::rowInterval == 0  &&  (currLastDigit-OptionProcessing::columnBegin)%OptionProcessing::columnInterval == 0)
			{
				int& cell = lastDigitCount[(prevLastDigit-OptionProcessing::rowBegin)/OptionProcessing::rowInterval][(currLastDigit-OptionProcessing::columnBegin)/OptionProcessing::columnInterval];
				cell++;
				maxLastDigitCount = std::max(maxLastDigitCount, cell);
			}
			prevLastDigit = currLastDigit;
		}), true);
		
		if(OptionProcessing::action == OptionProcessing::Action::DiagonalAverages)
			outputDiagonalAverages(output, getDiagonalAverages(lastDigitCount));
		else if(OptionProcessing::action == OptionProcessing::Action::PlotDiagonalAverages)
			plotDiagonalAverages(output, getDiagonalAverages(lastDigitCount));
		else
			outputLastDigits(output, lastDigitCount, maxLastDigitCount, asciiArtNumLength);
	}
	
	return 0;
}
