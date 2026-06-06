#pragma once

#include<GL/glew.h>

#ifndef SHADOWMAP_H
#define SHADOWMAP_H


//Kind of a texture, kind of a framebuffer object
//I really just wanted to abstract this away, cuz gl functions are shit to read

class ShadowMap
{
private:
	unsigned int m_FBOId;
	unsigned int m_depthMapTextureId;
	unsigned int m_width;
	unsigned int m_height;

public:
	ShadowMap();
	~ShadowMap();

	void Set(unsigned int width, unsigned int height);

	void BindForWriting();
	void UnbindFromWriting();

	void BindForReading(unsigned int slot);
	void UnbindFromReading();

	inline unsigned int GetWidth() { return m_width; }
	inline unsigned int GetHeight() { return m_height; }




};

#endif