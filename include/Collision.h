#pragma once
#ifndef COLLISION_H
#define COLLISION_H

#include "glm/glm.hpp"
#include <vector>

struct ContactManifold
{
	int entityId1;
	int entityId2; //This piece of ass is -1 if we entity is hitting a plane
	glm::vec3 normal;
	float penetrationDepth;
	int contactPointCount;
	glm::vec3 contactPoints[4];
};

bool RigidbodyCollision(const std::vector<glm::vec3>& localVertices1, const glm::mat4& worldTransform1,
						const std::vector<glm::vec3>& localVertices2, const glm::mat4& worldTransform2,
						ContactManifold& contactManifold);

#endif
