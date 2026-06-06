#pragma once
#ifndef MESH_H
#define MESH_H

//#include <iostream>

#include <vector>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // transzformációkhoz
#include <glm/gtc/type_ptr.hpp>   

#include "Debug.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexLayout.h"

template <typename VertexType>
class Mesh
{
private:
	VertexArray m_VAO;
	VertexBuffer m_VBO;
	IndexBuffer m_IBO;
	
	int m_vertexCount;
	int m_indexCount;
public:
	Mesh();
	Mesh(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices);

	void Set(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices);
	void Draw();
};

template<typename VertexType>
Mesh<VertexType>::Mesh() : m_vertexCount(0),m_indexCount(0)
{
	//std::cout << "Mesh" << std::endl;
}

template<typename VertexType>
Mesh<VertexType>::Mesh(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices)
{
	Set(vertices, indices);
}

template<typename VertexType>
void Mesh<VertexType>::Set(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices)
{
	m_VBO.Set(vertices.data(), sizeof(VertexType) * vertices.size());
	m_IBO.Set(indices.data(), sizeof(unsigned int) * indices.size());
	m_indexCount = indices.size();

	m_VAO.Set(m_VBO, m_IBO);

	VertexLayout<VertexType> layout;

	layout.Apply();

	m_VAO.Unbind();

	m_VBO.Unbind();
	m_IBO.Unbind();
}

template<typename VertexType>
void Mesh<VertexType>::Draw()
{
	m_VAO.Bind();

	GLCall(glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr));

	m_VAO.Unbind();
}

#endif