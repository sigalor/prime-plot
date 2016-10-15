#include "BraillePixelsScreen.hpp"



void BraillePixelsScreen::initPixels()
{
	pixels.resize(height);
	for(auto& r : pixels)
	{
		r.resize(width);
		for(auto it = r.begin(); it != r.end(); ++it)
			*it = false;
	}
}

void BraillePixelsScreen::initChars()
{
	chars.resize(height/4);
	for(auto& r : chars)
	{
		r.resize(width/2);
		for(auto& c : r)
			c = '\0';
	}
}

void BraillePixelsScreen::init()
{
	initPixels();
	initChars();
}

int BraillePixelsScreen::hiBit(unsigned int n)																		//principle from http://stackoverflow.com/a/53175
{
	int ret = 0;
	while(n >>= 1  &&  n)
		ret++;
	return ret;
}

unsigned int BraillePixelsScreen::greaterAbs(int x, int y)
{
	unsigned int absX = x<0 ? -x : x;																				//for only three instructions, check out https://graphics.stanford.edu/~seander/bithacks.html#IntegerAbs
	unsigned int absY = y<0 ? -y : y;
	return (absX > absY) ? absX : absY;
}

#ifdef __unix__																										//more preprocessor operating system dependent defines at http://stackoverflow.com/a/8249232
BraillePixelsScreen::BraillePixelsScreen()
{
	struct winsize w;																								//from http://stackoverflow.com/a/1022961
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	width = w.ws_col * 2;
	height = w.ws_row * 4;
	init();
}
#endif

BraillePixelsScreen::BraillePixelsScreen(unsigned int newWidth, unsigned int newHeight) : width(newWidth), height(newHeight)
{
	width -= width % 2;																								//width has to be a multiple of 2, because braille symbols have a width of 2 points
	height -= height % 4;																							//the same with the height
	init();
}

void BraillePixelsScreen::flush(std::ostream& output, bool cls)
{
	if(cls)
		output << "\x1b[2J";																						//clear screen (ANSI sequence)
	for(unsigned int y=0; y<height; y+=4)
	{
		for(unsigned int x=0; x<width; x+=2)
		{
			if(chars[y/4][x/2] != '\0')																				//if a character is set at the current position, it always replaces a 4x2 braille pixel block
				output << chars[y/4][x/2];
			else
			{
				unsigned int braille = 0;
				braille |= pixels[y][x]     ?   1 : 0;																//Braille points in Unicode are arranged like this (the numbers are the bit positions):
				braille |= pixels[y+1][x]   ?   2 : 0;																// 0  3
				braille |= pixels[y+2][x]   ?   4 : 0;																// 1  4
				braille |= pixels[y][x+1]   ?   8 : 0;																// 2  5
				braille |= pixels[y+1][x+1] ?  16 : 0;																// 6  7
				braille |= pixels[y+2][x+1] ?  32 : 0;
				braille |= pixels[y+3][x]   ?  64 : 0;
				braille |= pixels[y+3][x+1] ? 128 : 0;
				braille += 0x2800;																					//this is where the Unicode Braille block begins
				UnicodeUtilities::outputUnicodeAsUtf8(output, braille);
			}
		}
		if(y != height-4)
			output << "\n";
	}
	output << std::flush;
}

void BraillePixelsScreen::clearPixels()
{
	for(auto& r : pixels)
		for(auto it = r.begin(); it != r.end(); ++it)
			*it = false;
}

void BraillePixelsScreen::clearChars()
{
	for(auto& r : chars)
		for(auto& c : r)
			c = '\0';
}

void BraillePixelsScreen::clear()
{
	clearPixels();
	clearChars();
}

void BraillePixelsScreen::setPixel(unsigned int x, unsigned int y, bool set)
{
	if(x >= width  ||  y >= height)
		return;
	pixels[y][x] = set;
}

void BraillePixelsScreen::setChar(unsigned int x, unsigned int y, char c)											//x has to be in interval [0; width/2[, y in [0; height/4[
{
	if(x >= width/2  ||  y >= height/4)
		return;
	chars[y][x] = c;
}

void BraillePixelsScreen::setString(unsigned int x, unsigned int y, std::string s)
{
	while(s.size() > 0)
	{
		setChar(x, y, s.front());
		x++;
		s.erase(s.begin());
	}
}

void BraillePixelsScreen::setStringRightAligned(unsigned int x, unsigned int y, std::string s)
{
	while(s.size() > 0)
	{
		setChar(x, y, s.back());
		x--;
		s.pop_back();
	}
}

void BraillePixelsScreen::setStringVertical(unsigned int x, unsigned int y, std::string s)
{
	while(s.size() > 0)
	{
		setChar(x, y, s.front());
		y++;
		s.erase(s.begin());
	}
}

void BraillePixelsScreen::drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, bool set)	//Bresenham algorithm, slighly optimized to prevent line gaps
{
	int dx = (int)x2 - (int)x1;
	int dy = (int)y2 - (int)y1;
	int density = hiBit(greaterAbs(dx, dy)) + 1;
	unsigned int x = (x1 << density) + (1 << (density-1));
	unsigned int y = (y1 << density) + (1 << (density-1));

	for(unsigned int l=0; l<(1u << density); l++)
	{
		setPixel(x>>density, y>>density, set);
		x += dx;
		y += dy;
	}
	setPixel(x1, y1, set);																							//the algorithm may have omitted the first and/or the last pixel
	setPixel(x2, y2, set);
}

void BraillePixelsScreen::plot(unsigned int origX, unsigned int origY, float scaleX, float scaleY, std::function<float(float)> func, bool set)
{
	for(unsigned int x=0; x<width; x++)
	{
		float plotX = static_cast<float>((int)x-(int)origX)*scaleX;
		float plotY = -(func(plotX) * scaleY) + origY;
		if(plotY >= 0)
			setPixel(x, static_cast<unsigned int>(plotY), set);
	}
}

unsigned int BraillePixelsScreen::getWidth()
{
	return width;
}

unsigned int BraillePixelsScreen::getHeight()
{
	return height;
}
