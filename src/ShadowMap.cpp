#include "ShadowMap.h"

#include "Debug.h"
	
ShadowMap::ShadowMap() 
{
	
	m_width = 0;
	m_height = 0;
	
	m_FBOId = 0;
	m_depthMapTextureId = 0;
}

ShadowMap::~ShadowMap()
{
	if (m_FBOId != 0)
	{
		GLCall(glDeleteFramebuffers(1, &m_FBOId));
	}
	if (m_depthMapTextureId != 0)
	{
		GLCall(glDeleteTextures(1, &m_depthMapTextureId));
	}
}

void ShadowMap::Set(unsigned int width, unsigned int height)
{
	
	m_width = width;
	m_height = height;
	


	GLCall(glGenFramebuffers(1, &m_FBOId));

	//Creation of the depth texture
	GLCall(glGenTextures(1, &m_depthMapTextureId));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_depthMapTextureId));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	float borderColor[] = { 1.0,1.0,1.0,1.0 };
	GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

	//Attach the depth texture as the frame buffer object's data
	//Specifies the frame buffer object, then unbinds it
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMapTextureId, 0));
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void ShadowMap::BindForWriting()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId));
}

void ShadowMap::UnbindFromWriting()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void ShadowMap::BindForReading(unsigned int slot)
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_depthMapTextureId));
}

void ShadowMap::UnbindFromReading()
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
