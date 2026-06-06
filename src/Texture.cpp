#include "Texture.h"

#include <GL/glew.h>

#include "stb_image.h"

#include "Debug.h"


Texture::Texture() :m_textureFilePath(""),
	m_localTextureBuffer(nullptr),
	m_width(0), m_height(0), m_bitsPerPixel(0),m_textureId(0)
{
	
}


Texture::Texture(const std::string& textureFilePath)
	:m_textureFilePath(textureFilePath),
	m_localTextureBuffer(nullptr),
	m_width(0), m_height(0), m_bitsPerPixel(0)
{
	Set(textureFilePath);
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(0, &m_textureId));
}

void Texture::Set(const std::string& textureFilePath)
{
	m_textureFilePath = textureFilePath;
	stbi_set_flip_vertically_on_load(1);
	m_localTextureBuffer = stbi_load(m_textureFilePath.c_str(), &m_width, &m_height, &m_bitsPerPixel, 4);

	GLCall(glGenTextures(1, &m_textureId));
	
	Bind(0);

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localTextureBuffer));

	Unbind();

	if (m_localTextureBuffer)
	{
		stbi_image_free(m_localTextureBuffer);
	}
}


void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_textureId));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}