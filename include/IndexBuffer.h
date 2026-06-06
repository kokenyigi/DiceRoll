#pragma once
#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

class IndexBuffer
{
private:
	unsigned int m_iboID;
public:
	IndexBuffer();
	~IndexBuffer();

	void Set(const unsigned int* indices, int count);

	void Bind() const;
	void Unbind() const;
};


#endif
