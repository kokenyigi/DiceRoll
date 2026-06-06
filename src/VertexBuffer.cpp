#include "Debug.h"
#include "VertexBuffer.h"
//#include <iostream>

VertexBuffer::VertexBuffer():m_vboID(0)
{
	
}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_vboID));
}

void VertexBuffer::Set(const void* data, int size) 
{
	GLCall(glGenBuffers(1, &m_vboID));

	Bind();
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
	Unbind();
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vboID));
}

void VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}