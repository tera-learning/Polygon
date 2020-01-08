#pragma once
#include <vector>

class TextureManager
{
private:
	std::vector<unsigned char> m_Buffer;
	int m_Width;
	int m_Height;
	int m_PixelByte;

public:
	TextureManager();
	~TextureManager();

	int getWidth(void);
	int getHeight(void);
	int getPixelByte(void);
	const unsigned char* getTextureBuffer(void);
};

