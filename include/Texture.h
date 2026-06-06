#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>

class Texture
{
private:

	unsigned int m_textureId;
	std::string m_textureFilePath;
	unsigned char* m_localTextureBuffer;
	int m_width;
	int m_height;
	int m_bitsPerPixel;

public:
	Texture();
	Texture(const std::string& textureFilePath);
	~Texture();

	void Set(const std::string& textureFilePath);

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_width; }
	inline int GetHeight() const { return m_height; }

};


#endif