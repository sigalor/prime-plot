#pragma once



#include <string>
#include <vector>
#include <functional>

#ifdef __unix__
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "UnicodeUtilities.hpp"



class BraillePixelsScreen																							//idea from https://github.com/asciimoo/drawille
{
	private:
		std::size_t								width, height;
		std::vector<std::vector<bool>>			pixels;
		std::vector<std::vector<char>>			chars;
	
		void									initPixels();
		void									initChars();
		void									init();
		int										hiBit(unsigned int n);
		unsigned int							greaterAbs(int x, int y);
	
	public:
		#ifdef __unix__
		BraillePixelsScreen();
		#endif
		BraillePixelsScreen(unsigned int newWidth, unsigned int newHeight);
		
		void									flush(std::ostream& output, bool cls=false);
		void									clearPixels();
		void									clearChars();
		void									clear();
		void									setPixel(unsigned int x, unsigned int y, bool set=true);
		void									setChar(unsigned int x, unsigned int y, char c);
		void									setString(unsigned int x, unsigned int y, std::string s);
		void									setStringRightAligned(unsigned int x, unsigned int y, std::string s);
		void									setStringVertical(unsigned int x, unsigned int y, std::string s);
		void									drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, bool set=true);
		void									plot(unsigned int origX, unsigned int origY, float scaleX, float scaleY, std::function<float(float)> func, bool set=true);
		unsigned int							getWidth();
		unsigned int							getHeight();
};
