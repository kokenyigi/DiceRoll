#pragma once
#ifndef VERTEXLAYOUT_H
#define VERTEXLAYOUT_H

#include <vector>
#include <GL/glew.h>
#include <iostream>

#include "Vertex.h"
#include "Debug.h"

//#include <iostream>


struct VertexAttribute
{
	unsigned int count;
	unsigned int type;
	unsigned char is_normalized;
	unsigned int offset;
};

template<typename VertexType>
class VertexLayout
{
private:
	std::vector<VertexAttribute> m_layout;
	unsigned int m_stride;
public:
	VertexLayout();

	void Apply();
private:
	void Push(unsigned int gltype, unsigned int count, unsigned char is_normalized, unsigned int offset);
};


template<typename VertexType>
void VertexLayout<VertexType>::Push(unsigned int gltype, unsigned int count, unsigned char is_normalized, unsigned int offset)
{
	m_layout.push_back({ count,gltype,is_normalized,offset });
}

template<typename VertexType>
void VertexLayout<VertexType>::Apply()
{
	m_stride = sizeof(VertexType);

	for (unsigned int i = 0;i < m_layout.size();++i)
	{
		
		const VertexAttribute& attrib = m_layout[i];

		GLCall(glVertexAttribPointer(i, attrib.count, attrib.type, attrib.is_normalized, m_stride, (const void*)attrib.offset));
		GLCall(glEnableVertexAttribArray(i));
	}
}




//Actual vertexlayout implementation per type
template<>
inline VertexLayout<VertexP3C3>::VertexLayout()
{
	Push(GL_FLOAT, 3, GL_FALSE, offsetof(VertexP3C3, position));
	Push(GL_FLOAT, 3, GL_FALSE, offsetof(VertexP3C3, color));
}

template<>
inline VertexLayout<VertexP3N3>::VertexLayout()
{
	Push(GL_FLOAT, 3, GL_FALSE, offsetof(VertexP3N3, position));
	Push(GL_FLOAT, 3, GL_FALSE, offsetof(VertexP3N3, normal));
}

template<>
inline VertexLayout<VertexP3N3C3>::VertexLayout()
{
	Push(GL_FLOAT, 3, GL_FALSE, offsetof(VertexP3N3C3, position));
	Push(GL_FLOAT, 3, GL_FALSE, offsetof(VertexP3N3C3, normal));
	Push(GL_FLOAT, 3, GL_FALSE, offsetof(VertexP3N3C3, color));
}

template<>
inline VertexLayout<VertexP3N3T2>::VertexLayout()
{
	Push(GL_FLOAT, 3, GL_FALSE, offsetof(VertexP3N3T2, position));
	Push(GL_FLOAT, 3, GL_FALSE, offsetof(VertexP3N3T2, normal));
	Push(GL_FLOAT, 2, GL_FALSE, offsetof(VertexP3N3T2, textureCoord));
}

template<>
inline VertexLayout<VertexP3N3T2T2>::VertexLayout()
{
	Push(GL_FLOAT, 3, GL_FALSE, offsetof(VertexP3N3T2T2, position));
	Push(GL_FLOAT, 3, GL_FALSE, offsetof(VertexP3N3T2T2, normal));
	Push(GL_FLOAT, 2, GL_FALSE, offsetof(VertexP3N3T2T2, numberTexCoord));
	Push(GL_FLOAT, 2, GL_FALSE, offsetof(VertexP3N3T2T2, materialTexCoord));
}

template<>
inline VertexLayout<VertexP2T2>::VertexLayout()
{
	Push(GL_FLOAT, 2, GL_FALSE, offsetof(VertexP2T2, position));
	Push(GL_FLOAT, 2, GL_FALSE, offsetof(VertexP2T2, textureCoords));
}


//If we try to ever call the constructor of vertexLayout with an unspcified vertexType, then we get an error
template<typename UnWrittenVertexType>
VertexLayout<UnWrittenVertexType>::VertexLayout()
{
	static_assert(sizeof(UnWrittenVertexType) == 0, "[STATIC_ASSERT_ERROR]: VertexLayout for this specific type hasn't been declared yet!\n");
}

#endif