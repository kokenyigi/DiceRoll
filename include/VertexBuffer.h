#pragma once
#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H



class VertexBuffer
{
private:
	unsigned int m_vboID;
public:
	VertexBuffer();
	~VertexBuffer();

	void Set(const void* data, int size);

	void Bind() const;
	void Unbind() const;
};


#endif
