#include "TextureManager.h"



TextureManager::TextureManager()
{
	m_Width = 2;
	m_Height = 2;
	m_PixelByte = 4;

	m_Buffer.push_back(0);
	m_Buffer.push_back(0);
	m_Buffer.push_back(255);
	m_Buffer.push_back(255);

	m_Buffer.push_back(0);
	m_Buffer.push_back(255);
	m_Buffer.push_back(0);
	m_Buffer.push_back(255);

	m_Buffer.push_back(255);
	m_Buffer.push_back(0);
	m_Buffer.push_back(0);
	m_Buffer.push_back(255);

	m_Buffer.push_back(255);
	m_Buffer.push_back(255);
	m_Buffer.push_back(255);
	m_Buffer.push_back(255);
}


TextureManager::~TextureManager()
{
	m_Buffer.clear();
	m_Width = 0;
	m_Height = 0;
}

int TextureManager::getWidth(void)
{
	return m_Width;
}

int TextureManager::getHeight(void)
{
	return m_Height;
}

int TextureManager::getPixelByte(void)
{
	return m_PixelByte;
}

const unsigned char* TextureManager::getTextureBuffer(void)
{
	return &m_Buffer[0];
}