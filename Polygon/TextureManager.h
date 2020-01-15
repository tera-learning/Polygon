#pragma once
#include <vector>

class TextureManager final
{
private:
	std::vector<unsigned char> m_Buffer;
	int m_Width;
	int m_Height;
	int m_PixelByte;

public:
	TextureManager();
	/* virtual */ ~TextureManager();

	int getWidth(void);
	int getHeight(void);
	int getPixelByte(void);
	const unsigned char* getTextureBuffer(void);
};

