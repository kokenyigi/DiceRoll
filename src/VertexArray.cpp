#include "Debug.h"
#include "VertexArray.h"
//#include <iostream>

VertexArray::VertexArray() : m_vaoID(0)
{
	
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_vaoID));
}

void VertexArray::Set(const VertexBuffer& vbo, const IndexBuffer& ibo)
{
	GLCall(glGenVertexArrays(1, &m_vaoID));
	Bind();

	vbo.Bind();
	ibo.Bind();

	
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_vaoID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}