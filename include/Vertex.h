#pragma once
#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

struct VertexP3C3
{
	glm::vec3 position;
	glm::vec3 color;
};

struct VertexP3N3
{
	glm::vec3 position;
	glm::vec3 normal;
};

struct VertexP3N3C3
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
};

struct VertexP3N3T2
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoord;
};

struct VertexP3N3T2T2
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 numberTexCoord;
	glm::vec2 materialTexCoord;
};


struct VertexP2T2
{
	glm::vec2 position;
	glm::vec2 textureCoords;
};

#endif