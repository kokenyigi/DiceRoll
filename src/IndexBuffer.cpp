#include "Debug.h"
#include "IndexBuffer.h"
#include <iostream>

IndexBuffer::IndexBuffer():m_iboID(0)
{
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_iboID));
}

void IndexBuffer::Set(const unsigned int* indices, int count)
{
	GLCall(glGenBuffers(1, &m_iboID));
	
	Bind();
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, indices, GL_STATIC_DRAW));
	Unbind();
}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID));
}

void IndexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}