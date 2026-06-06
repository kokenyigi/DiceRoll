#pragma once
#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "VertexBuffer.h"
#include "IndexBuffer.h"

class VertexArray
{
private:
	unsigned int m_vaoID;

public:
	VertexArray();
	~VertexArray();

	void Set(const VertexBuffer& vbo, const IndexBuffer& ibo);

	void Bind() const;
	void Unbind() const;
};


#endif
