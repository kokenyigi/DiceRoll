#pragma once
#ifndef ECS_H
#define ECS_H

#include <vector>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp> // transzformációkhoz
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/quaternion.hpp>

#include "Utils.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "ShadowMap.h"

#include "Collision.h"




//Components for the ECS
struct TransformComponent
{
	glm::vec3 position;
	glm::quat rotation;
	float scale;
};

struct PhysicsComponent
{
	float mass;
	float invMass;
	float restitution;
	float friction;

	glm::vec3 velocity;

	glm::vec3 angularVelocity;

	//inertiatensor, torque
	int inertiaDataId;
};

struct ColliderComponent
{
	//empty yet
	float boundingRadius;
	int localHullId;
};

struct RenderComponent
{
	int meshId;
};




//Helping data structures for our ecs
struct LocalHullData
{
	std::vector<glm::vec3> Vertices;
};

struct LocalInertiaData
{

	glm::mat3 inertiaTensor;
	glm::mat3 inverseInertiaTensor;
};

struct LocalDirectionData
{
	std::vector<glm::vec3> directions;
};

struct EntityModel
{
	//Transform component is generated on the fly
	//Physics Component data
	float mass;
	float restitution;
	float friction;
	int inertiaDataId;

	//Collider Component Data
	float boundingRadius;
	int localHullId;
	
	//Render Component Data
	int meshId;

	int localDirectionId;


};



//An enumeration for entity models
enum EntityIdentifier
{
	TETRAHEDRON = 0,
	CUBE = 1,
	OCTAHEDRON = 2,
	DECAHEDRON1_10 = 3,
	DECAHEDRON0_9 = 4,
	DECAHEDRON00_90 = 5,
	DODECAHEDRON = 6,
	ICOSAHEDRON = 7
};





//Our main, game-object handling system, the Entity Component System
class ECS
{
private:
	//Virtual World parameters
	
	glm::vec3 worldSize;	//The world is a rectangular box, width = x, height = y, length = z
	std::vector<Plane> worldBorders;

	//Entity fields
	int m_entityCapacity;
	int m_entityCount;

	int* m_freeIds;
	int* m_entityIds;

	//Component fields
	TransformComponent* transforms;
	ColliderComponent* colliders;
	PhysicsComponent* physics;
	glm::mat4* worldTransforms;
	int* localDirectionIds;
	RenderComponent* renders;


	//Helper Memberfields
	std::vector<Mesh<VertexP3N3T2T2>> meshes;
	
	std::vector<LocalHullData> localHulls;
	std::vector<LocalInertiaData> localInertiaDatas;

	std::vector<LocalDirectionData> localDirections;

	std::vector<EntityModel> entityModels;

	ContactManifold* manifoldBuffer;
	int manifoldCount;
	int manifoldCapacity;
	
	
	//For rendering
	Shader baseShader;

	Texture materialTexture;
	Texture numberTexture;
	Texture boxTexture;
	Texture tableTexture;

	Mesh<VertexP3N3T2T2> cubeMesh;

	//The dices will be inside this box
	glm::mat4 boxWorldTransforms[7];
	glm::mat4 boxInvTranWorldTransforms[7]; // for normals

	//The box lies on top of the table
	//glm::mat4 tableWorldTransforms[5];
	//glm::mat4 tableInvTranWorldTransforms[5]; 


	//Data of our spotLight
	glm::vec3 spotLightPosition;
	glm::vec3 spotLightDirection;
	float spotLightInnerCosine;
	float spotLightOuterCosine;

	//Data members for our shadow Calculations
	Shader shadowShader;

	ShadowMap shadowMap;

	float lightNearPlane;
	float lightFarPlane;

	float lightFovy; //Should be equal to spotLight's outercos's degree


public:
	ECS();
	~ECS();

	void Init(int entityCapacity,const glm::vec3& worldSize); // add world size init here too

	void Update(float deltaTimem, int& diceRollSum);
	void Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,const glm::vec3& cameraPosition,
				float windowWidth, float windowHeight);

	void AddEntity(EntityIdentifier identifier); // needs more parameters use references
	void Clear();
private:
	void TransformSystem(); //writes the current transform data into the worldtransform components, for easier and faster access
	void PhysicsSystem(float deltaTime);
	void CollisionDetectionSystem(); 
	void CollisionSolverSystem();
	void SummationSystem(int& diceRollSum);
	void RenderSystem(const Shader& shader);
};



#endif 
