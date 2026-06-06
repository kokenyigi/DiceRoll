#include "ECS.h"

#include "Utils.h"

#include <iostream>

//Public ECS function definitions
//--------------------------------------------------------------------------------------------------------------------------------------

ECS::ECS()
{
	worldSize = glm::vec3(0, 0, 0);

	m_entityCount = 0;
	m_entityCapacity = 0;

	m_freeIds = nullptr;
	m_entityIds = nullptr;

	transforms = nullptr;
	physics = nullptr;
	colliders = nullptr;
	worldTransforms = nullptr;
	localDirectionIds = nullptr;
	renders = nullptr;

	manifoldCount = 0;
	manifoldCapacity = 0;
	manifoldBuffer = nullptr;
}

ECS::~ECS()
{
	if (m_entityCapacity > 0)
	{
		delete[] m_freeIds;
		delete[] m_entityIds;

		
		delete[] transforms;
		delete[] physics;
		delete[] colliders;
		delete[] worldTransforms;
		delete[] localDirectionIds;
		delete[] renders;
	}
}

//returns the left bottom corner in UV of the given number
static glm::vec2 NumberToUV(int number)
{
	float uW = 1.0 / 6;
	float uH = 1.0 / 8;
	if (number > 0 && number <= 20)
	{

		glm::vec2 retval = glm::vec2(0, uH * 7);

		int index = number - 1;
		int wMult = index % 5;
		int hMult = index / 5;

		glm::vec2 offset = glm::vec2(wMult * uW, -hMult * uH);

		return retval + offset;
	}
	else if (number == 0)
	{
		return glm::vec2(3 * uW, uH);
	}
	else if (number >= 30 && number <= 90 && number % 10 == 0)
	{
		glm::vec2 retval = glm::vec2(uW * 5, uH * 7);

		int index = number / 10 - 3;

		glm::vec2 offset = glm::vec2(0, -index * uH);

		return retval + offset;
	}
}

//this function calculates the inertiaTensor of a tetrahedron and mass and volume in its parameter references
//Very importantus thing: this function doesnt deal with tetrahedrons of bodies, that arent origo-centerred mass wise
//COM calculations are needed for these kinds of general bodies
static void CalculateTetraHedronData(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,float density,
									 float& massRef, glm::mat3& inertiaTensorRef)
{
	massRef = 0.0f;
	inertiaTensorRef = glm::mat3(0.0f);

	float signedVolume = glm::dot(v0, glm::cross(v1, v2)) / 6.0f;
	float volume = glm::abs(signedVolume);

	float mass = density * volume;

	float signedMass = density * signedVolume;

	//writing into out reference mass
	massRef = mass;
	
	//long inertia calculation

	//diagonal elements
	float Ixx = mass / 10.0f * (v0.y * v0.y + v1.y * v1.y + v2.y * v2.y + v0.y * v1.y + v0.y * v2.y + v1.y * v2.y +
		v0.z * v0.z + v1.z * v1.z + v2.z * v2.z + v0.z * v1.z + v0.z * v2.z + v1.z * v2.z);
	float Iyy = mass / 10.0f * (v0.x * v0.x + v1.x * v1.x + v2.x * v2.x + v0.x * v1.x + v0.x * v2.x + v1.x * v2.x +
		v0.z * v0.z + v1.z * v1.z + v2.z * v2.z + v0.z * v1.z + v0.z * v2.z + v1.z * v2.z);
	float Izz = mass / 10.0f * (v0.x * v0.x + v1.x * v1.x + v2.x * v2.x + v0.x * v1.x + v0.x * v2.x + v1.x * v2.x +
		v0.y * v0.y + v1.y * v1.y + v2.y * v2.y + v0.y * v1.y + v0.y * v2.y + v1.y * v2.y);

	//off-diagonal elements
	float Ixy = -mass / 20.0f * (2*v0.x*v0.y + 2*v1.x*v1.y + 2*v2.x*v2.y + v0.x*v1.y + v1.x*v0.y + v0.x*v2.y + v2.x*v0.y +
		v1.x*v2.y + v2.x*v1.y);

	float Ixz = -mass / 20.0f * (2 * v0.x * v0.z + 2 * v1.x * v1.z + 2 * v2.x * v2.z + v0.x * v1.z + v1.x * v0.z + v0.x * v2.z + v2.x * v0.z +
		v1.x * v2.z + v2.x * v1.z);

	float Iyz = -mass / 20.0f * (2 * v0.y * v0.z + 2 * v1.y * v1.z + 2 * v2.y * v2.z + v0.y * v1.z + v1.y * v0.z + v0.y * v2.z + v2.y * v0.z +
		v1.y * v2.z + v2.y * v1.z);

	//writing into out reference inertiatensor
	inertiaTensorRef[0][0] += Ixx;
	inertiaTensorRef[1][1] += Iyy;
	inertiaTensorRef[2][2] += Izz;

	inertiaTensorRef[0][1] += Ixy;
	inertiaTensorRef[0][2] += Ixz;
	inertiaTensorRef[1][2] += Iyz;

	inertiaTensorRef[1][0] += Ixy;
	inertiaTensorRef[2][0] += Ixz;
	inertiaTensorRef[2][1] += Iyz;


}

void ECS::Init(int entityCapacity, const glm::vec3& worldSize)
{
	m_entityCount = 0;
	m_entityCapacity = entityCapacity;

	m_freeIds = new int[entityCapacity];
	for (int i = 0;i < entityCapacity;++i)
	{
		m_freeIds[i] = entityCapacity - 1 - i;
	}
	m_entityIds = new int[entityCapacity];

	transforms = new TransformComponent[entityCapacity];
	physics = new PhysicsComponent[entityCapacity];
	colliders = new ColliderComponent[entityCapacity];
	worldTransforms = new glm::mat4[entityCapacity];
	localDirectionIds = new int[entityCapacity];
	renders = new RenderComponent[entityCapacity];

	manifoldBuffer = new ContactManifold[4096];
	manifoldCapacity = 4096;
	manifoldCount = 0;

	//we have 8 distinct meshes to load
	//in this order: 0 - d4 , 1 - d6 , 2 - d8 , 3 - d10[1-10] , 4 - d10[0-9] , 5 - d10[00-90] , 6 - d12 , 7 - d20
	meshes.resize(8); 

	//Disgustingly long geometry initialization - future: load from file
	//Should also intialize our localHulls, and localInertias
	localHulls.resize(6);
	localInertiaDatas.resize(6);

	localDirections.resize(6);

	entityModels.resize(8);

	float constantDensity = 1.3f;

#pragma region TetraHedron
	//Tetrahedron Mesh
	float sqrt2 = glm::sqrt(2);
	float sqrt3 = glm::sqrt(3);
	constexpr float pi = glm::pi<float>();
	
	{
		float asp = 2.0 / 6 / sqrt3;

		glm::vec3 vert1 = glm::vec3(0, 1, 0);
		glm::vec3 vert2 = glm::vec3(-2 * sqrt2 / 3.0f, -1.0 / 3, 0);
		glm::vec3 vert3 = glm::vec3(sqrt2 / 3, -1.0 / 3, sqrt2 / sqrt3);
		glm::vec3 vert4 = glm::vec3(sqrt2 / 3, -1.0 / 3, -sqrt2 / sqrt3);

		localDirections[0].directions.push_back(vert1);
		localDirections[0].directions.push_back(vert2);
		localDirections[0].directions.push_back(vert3);
		localDirections[0].directions.push_back(vert4);
		

		std::vector<VertexP3N3T2T2> tetraVertices =
		{
			/*
			{glm::vec3(0,1,0),glm::vec3(1,1,0)},
			{glm::vec3(-2 * sqrt2 / 3.0f,-1.0 / 3,0),glm::vec3(1,0,0)},
			{glm::vec3(sqrt2 / 3,-1.0 / 3,sqrt2 / sqrt3),glm::vec3(0,1,0)},
			{glm::vec3(sqrt2 / 3,-1.0 / 3,-sqrt2 / sqrt3),glm::vec3(0,0,1)}
			*/
			{vert1,-vert4,glm::vec2(asp,0.5),glm::vec2(0.5,1.0)},
			{vert2,-vert4,glm::vec2(0,1.0 / 4),glm::vec2(0.0,0.0)},
			{vert3,-vert4,glm::vec2(2 * asp,0.25),glm::vec2(1.0,0.0)},


			{vert1,-vert2,glm::vec2(2 * asp,0.25),glm::vec2(0.5,1.0)},
			{vert3,-vert2,glm::vec2(3 * asp,0.5),glm::vec2(0.0,0.0)},
			{vert4,-vert2,glm::vec2(asp,0.5),glm::vec2(1.0,0.0)},


			{vert1,-vert3,glm::vec2(3 * asp,0.5),glm::vec2(0.5,1.0)},
			{vert4,-vert3,glm::vec2(2 * asp,0.25),glm::vec2(0.0,0.0)},
			{vert2,-vert3,glm::vec2(4 * asp,0.25),glm::vec2(1.0,0.0)},

			{vert2,-vert1,glm::vec2(asp,0.25),glm::vec2(0.5,1.0)},
			{vert3,-vert1,glm::vec2(2 * asp,0),glm::vec2(0.0,0.0)},
			{vert4,-vert1,glm::vec2(0,0),glm::vec2(1.0,0.0)}

		};

		std::vector<unsigned int> tetraIndices =
		{
			0,1,2,
			3,4,5,
			6,7,8,
			11,10,9
		};

		meshes[0].Set(tetraVertices, tetraIndices);

		//inertia + hull generation
		localHulls[0].Vertices.push_back(glm::vec3(0,1,0));

		

		glm::mat3 tetraHedronInertiaTensor = glm::mat3(0.0f);
		float tetraHedronMass = 0.0f;

		for (int i = 0;i < 3;++i)
		{
			float a = -1.0 / 3;
			float b = sqrtf(1 - a * a);
			glm::vec3 topVert0 = glm::vec3(0,1,0);
			glm::vec3 ringvert1 = glm::vec3(b * cosf(i * 2 * pi / 3 + pi), a, b * sinf(i * 2 * pi / 3 + pi));
			localHulls[0].Vertices.push_back(ringvert1); //we add this ringvertex to the hull, 3 in total
			glm::vec3 ringvert2 = glm::vec3(b * cosf(i * 2 * pi / 3 + pi + 2*pi/3), a, b * sinf(i * 2 * pi / 3 + pi + 2 * pi / 3));

			//calculation of the TetraHedron's inertiaTensor
			float mass;
			glm::mat3 inertiaTensorPart;
			CalculateTetraHedronData(topVert0, ringvert1, ringvert2, constantDensity, mass, inertiaTensorPart);

			tetraHedronMass += mass;
			tetraHedronInertiaTensor += inertiaTensorPart;


		}

		float mass;
		glm::mat3 inertiaTensorPart;
		CalculateTetraHedronData(vert4, vert3, vert2, constantDensity, mass, inertiaTensorPart);

		tetraHedronMass += mass;
		tetraHedronInertiaTensor += inertiaTensorPart;

		localInertiaDatas[0].inertiaTensor = tetraHedronInertiaTensor;
		localInertiaDatas[0].inverseInertiaTensor = glm::inverse(tetraHedronInertiaTensor);

		entityModels[0] = {tetraHedronMass,0.8f,0.3f,0,1.0f,0,0,0};
	}
#pragma endregion

#pragma region Cube
	//Cube mesh

	float cubeA = 1.0 / sqrt3;

	//if the texture changes so do these!
	float uW = 1.0 / 6;
	float uH = 1.0 / 8;

	std::vector<VertexP3N3T2T2> cubeVertices =
	{
		{glm::vec3(cubeA,cubeA,cubeA),glm::vec3(0,1,0),glm::vec2(0,uH * 7),glm::vec2(0,0)},
		{glm::vec3(cubeA,cubeA,-cubeA),glm::vec3(0,1,0),glm::vec2(uW,uH * 7),glm::vec2(1,0)},
		{glm::vec3(-cubeA,cubeA,-cubeA),glm::vec3(0,1,0),glm::vec2(uW,uH * 8),glm::vec2(1,1)},
		{glm::vec3(-cubeA,cubeA,cubeA),glm::vec3(0,1,0),glm::vec2(0,uH * 8),glm::vec2(0,1)},

		{glm::vec3(cubeA,-cubeA,cubeA),glm::vec3(0,-1,0),glm::vec2(0,uH * 6),glm::vec2(0,0)},
		{glm::vec3(-cubeA,-cubeA,cubeA),glm::vec3(0,-1,0),glm::vec2(uW,uH * 6),glm::vec2(1,0)},
		{glm::vec3(-cubeA,-cubeA,-cubeA),glm::vec3(0,-1,0),glm::vec2(uW,uH * 7),glm::vec2(1,1)},
		{glm::vec3(cubeA,-cubeA,-cubeA),glm::vec3(0,-1,0),glm::vec2(0,uH * 7),glm::vec2(0,1)},

		{glm::vec3(-cubeA,-cubeA,cubeA),glm::vec3(0,0,1),glm::vec2(uW,uH * 7),glm::vec2(0,0)},
		{glm::vec3(cubeA,-cubeA,cubeA),glm::vec3(0,0,1),glm::vec2(uW * 2,uH * 7),glm::vec2(1,0)},
		{glm::vec3(cubeA,cubeA,cubeA),glm::vec3(0,0,1),glm::vec2(uW * 2,uH * 8),glm::vec2(1,1)},
		{glm::vec3(-cubeA,cubeA,cubeA),glm::vec3(0,0,1),glm::vec2(uW,uH * 8),glm::vec2(0,1)},

		{glm::vec3(cubeA,-cubeA,-cubeA),glm::vec3(0,0,-1),glm::vec2(uW * 4,uH * 7),glm::vec2(0,0)},
		{glm::vec3(-cubeA,-cubeA,-cubeA),glm::vec3(0,0,-1),glm::vec2(uW * 5,uH * 7),glm::vec2(1,0)},
		{glm::vec3(-cubeA,cubeA,-cubeA),glm::vec3(0,0,-1),glm::vec2(uW * 5,uH * 8),glm::vec2(1,1)},
		{glm::vec3(cubeA,cubeA,-cubeA),glm::vec3(0,0,-1),glm::vec2(uW * 4,uH * 8),glm::vec2(0,1)},

		{glm::vec3(cubeA,-cubeA,cubeA),glm::vec3(1,0,0),glm::vec2(uW * 2,uH * 7),glm::vec2(0,0)},
		{glm::vec3(cubeA,-cubeA,-cubeA),glm::vec3(1,0,0),glm::vec2(uW * 3,uH * 7),glm::vec2(1,0)},
		{glm::vec3(cubeA,cubeA,-cubeA),glm::vec3(1,0,0),glm::vec2(uW * 3,uH * 8),glm::vec2(1,1)},
		{glm::vec3(cubeA,cubeA,cubeA),glm::vec3(1,0,0),glm::vec2(uW * 2,uH * 8),glm::vec2(0,1)},

		{glm::vec3(-cubeA,-cubeA,-cubeA),glm::vec3(-1,0,0),glm::vec2(uW * 3,uH * 7),glm::vec2(0,0)},
		{glm::vec3(-cubeA,-cubeA,cubeA),glm::vec3(-1,0,0),glm::vec2(uW * 4,uH * 7),glm::vec2(1,0)},
		{glm::vec3(-cubeA,cubeA,cubeA),glm::vec3(-1,0,0),glm::vec2(uW * 4,uH * 8),glm::vec2(1,1)},
		{glm::vec3(-cubeA,cubeA,-cubeA),glm::vec3(-1,0,0),glm::vec2(uW * 3,uH * 8),glm::vec2(0,1)},
	};

	localDirections[1].directions.push_back(glm::vec3(0,1,0));
	localDirections[1].directions.push_back(glm::vec3(0, 0, 1));
	localDirections[1].directions.push_back(glm::vec3(1, 0, 0));
	localDirections[1].directions.push_back(glm::vec3(-1, 0, 0));
	localDirections[1].directions.push_back(glm::vec3(0, 0, -1));
	localDirections[1].directions.push_back(glm::vec3(0, -1, 0));



	std::vector<unsigned int> cubeIndices =
	{
		0,1,2,
		2,3,0,

		4,5,6,
		6,7,4,

		8,9,10,
		10,11,8,

		12,13,14,
		14,15,12,

		16,17,18,
		18,19,16,

		20,21,22,
		22,23,20
	};

	meshes[1].Set(cubeVertices, cubeIndices);


	{

		

		glm::mat3 cubeInertiaTensor = glm::mat3(0.0f);
		float cubeMass = 0;

		float a = cubeA;
		float b = sqrtf(1 - a*a);

		for (int i = 0;i < 4;++i)
		{
			glm::vec3 topRingVert0 = glm::vec3(b * cosf(i * 2 * pi / 4.0f + pi / 4.0f), a, b * sinf(i * 2 * pi / 4.0f + pi / 4.0f));
			glm::vec3 topRingVert1 = glm::vec3(b * cosf(i * 2 * pi / 4.0f + pi / 4.0f + pi/2.0f), a, b * sinf(i * 2 * pi / 4.0f + pi / 4.0f + pi / 2.0f));


			localHulls[1].Vertices.push_back(topRingVert0);

			glm::vec3 bottomRingVert0 = glm::vec3(b * cosf(i * 2 * pi / 4.0f + pi / 4.0f), -a, b * sinf(i * 2 * pi / 4.0f + pi / 4.0f));
			glm::vec3 bottomRingVert1 = glm::vec3(b * cosf(i * 2 * pi / 4.0f + pi / 4.0f + pi / 2.0f), -a, b * sinf(i * 2 * pi / 4.0f + pi / 4.0f + pi / 2.0f));

			localHulls[1].Vertices.push_back(bottomRingVert0);

			float currentMass = 0.0f;
			glm::mat3 currentInertiaPart = glm::mat3(0.0f);
			CalculateTetraHedronData(topRingVert0, bottomRingVert0, bottomRingVert1, constantDensity, currentMass, currentInertiaPart);

			cubeMass += currentMass;
			cubeInertiaTensor += currentInertiaPart;

			CalculateTetraHedronData(bottomRingVert1, topRingVert1, topRingVert0, constantDensity, currentMass, currentInertiaPart);

			cubeMass += currentMass;
			cubeInertiaTensor += currentInertiaPart;
		}


		float currentMass = 0.0f;
		glm::mat3 currentInertiaPart = glm::mat3(0.0f);

		glm::vec3 topVert0 = glm::vec3(a, a, a);
		glm::vec3 topVert1 = glm::vec3(a, a, -a);
		glm::vec3 topVert2 = glm::vec3(-a, a, -a);
		glm::vec3 topVert3 = glm::vec3(-a, a, a);

		glm::vec3 bottomVert0 = glm::vec3(a, -a, a);
		glm::vec3 bottomVert1 = glm::vec3(-a, -a, a);
		glm::vec3 bottomVert2 = glm::vec3(-a, -a, -a);
		glm::vec3 bottomVert3 = glm::vec3(a, -a, -a);

		CalculateTetraHedronData(topVert0, topVert1, topVert2, constantDensity, currentMass, currentInertiaPart);
		cubeMass += currentMass;
		cubeInertiaTensor += currentInertiaPart;
		CalculateTetraHedronData(topVert2, topVert3, topVert0, constantDensity, currentMass, currentInertiaPart);
		cubeMass += currentMass;
		cubeInertiaTensor += currentInertiaPart;
		CalculateTetraHedronData(bottomVert0, bottomVert1, bottomVert2, constantDensity, currentMass, currentInertiaPart);
		cubeMass += currentMass;
		cubeInertiaTensor += currentInertiaPart;
		CalculateTetraHedronData(bottomVert2, bottomVert3, bottomVert0, constantDensity, currentMass, currentInertiaPart);
		cubeMass += currentMass;
		cubeInertiaTensor += currentInertiaPart;

		localInertiaDatas[1].inertiaTensor = cubeInertiaTensor;
		localInertiaDatas[1].inverseInertiaTensor = glm::inverse(cubeInertiaTensor);
		//top and bottom faces
		entityModels[1] = { cubeMass,0.6f,0.3f,1,1.0f,1,1 ,1};
	}

#pragma endregion Cube

#pragma region OctaHedron
	//Octahedron mesh

	std::vector<VertexP3N3T2T2> octaVertices;

	std::vector<unsigned int> octaIndices;

	float octaMass = 0.0f;
	glm::mat3 octaInertiaTensor = glm::mat3(0.0f);
	
	localHulls[2].Vertices.push_back(glm::vec3(0, 1, 0));
	localHulls[2].Vertices.push_back(glm::vec3(0,-1,0));

	localDirections[2].directions.resize(8);

	for (int i = 0;i < 4;++i)
	{
		int upNumber = i + 1;
		int downNumber = 8 - i;

		

		glm::vec2 uvOffset0 = glm::vec2(0, uH * (sqrt3 - 1) / 2.0 / sqrt3);
		glm::vec2 uvOffset1 = glm::vec2(uW, uvOffset0.y);
		glm::vec2 uvOffset2 = glm::vec2(uW / 2.0, uH * (2 + sqrt3) / sqrt3 / 2.0);

		glm::vec2 upNumberUV = NumberToUV(upNumber);
		glm::vec2 downNumberUV = NumberToUV(downNumber);

		float normalPolarArg = pi / 4;
		float normalAzimArg = pi / 4 + i * pi / 2;

		glm::vec3 octaVert1 = glm::vec3(sinf(pi / 2 * i), 0, cosf(pi / 2 * i));

		glm::vec3 octaVert2 = glm::vec3(sinf(pi / 2 * i + pi / 2), 0, cosf(pi / 2 * i + pi / 2));


		glm::vec3 upNormal = glm::normalize(glm::cross(octaVert1-glm::vec3(0,1,0),octaVert2-glm::vec3(0,1,0)));

		localDirections[2].directions[i] = upNormal;
		localDirections[2].directions[7-i] = -upNormal;

		octaVertices.push_back({ glm::vec3(0,1,0),upNormal,upNumberUV + uvOffset2,glm::vec2(0.5,1) });

		

		localHulls[2].Vertices.push_back(octaVert1);


		octaVertices.push_back({ octaVert1,upNormal,upNumberUV + uvOffset0,glm::vec2(0.0,0.0) });

		
		octaVertices.push_back({ octaVert2,upNormal,upNumberUV + uvOffset1,glm::vec2(1,0.0) });


		float currentMass = 0.0f;
		glm::mat3 currentInertiaPart = glm::mat3(0.0f);
		CalculateTetraHedronData(glm::vec3(0, 1, 0), octaVert1, octaVert2, constantDensity, currentMass, currentInertiaPart);

		octaMass += currentMass;
		octaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(glm::vec3(0, -1, 0), octaVert2, octaVert1, constantDensity, currentMass, currentInertiaPart);

		octaMass += currentMass;
		octaInertiaTensor += currentInertiaPart;


		octaIndices.push_back(6 * i);
		octaIndices.push_back(6 * i + 1);
		octaIndices.push_back(6 * i + 2);


		octaVertices.push_back({ glm::vec3(0,-1,0),-upNormal,downNumberUV + uvOffset2,glm::vec2(0.5,1) });


		octaVertices.push_back({ -octaVert2,-upNormal,downNumberUV + uvOffset0,glm::vec2(0.0,0.0) });


		octaVertices.push_back({ -octaVert1,-upNormal,downNumberUV + uvOffset1,glm::vec2(1,0.0) });

		octaIndices.push_back(6 * i + 3);
		octaIndices.push_back(6 * i + 4);
		octaIndices.push_back(6 * i + 5);





	}

	meshes[2].Set(octaVertices, octaIndices);

	localInertiaDatas[2].inertiaTensor = octaInertiaTensor;
	localInertiaDatas[2].inverseInertiaTensor = glm::inverse(octaInertiaTensor);

	entityModels[2] = { octaMass,0.7f,0.3f,2,1.0f,2,2 ,2};


#pragma endregion OctaHedron

#pragma region DecaHedrons

	//Decahedron mesh
	std::vector<VertexP3N3T2T2> decaVertices;

	std::vector<unsigned int> decaIndices;

	float decaMass = 0.0f;
	glm::mat3 decaInertiaTensor = glm::mat3(0.0f);

	localHulls[3].Vertices.push_back(glm::vec3(0, 1, 0));
	localHulls[3].Vertices.push_back(glm::vec3(0, -1, 0));

	localDirections[3].directions.resize(10);

	for (int i = 0;i < 5;++i)
	{





		float a = 0.1056; //magic number
		float b = sqrtf(1 - a * a);

		glm::vec3 topVert = glm::vec3(0, 1, 0);
		glm::vec3 upVert1 = glm::vec3(b * sinf(2 * pi / 5 * i), a, b * cosf(2 * pi / 5 * i));
		glm::vec3 upVert2 = glm::vec3(b * sinf(2 * pi / 5 * i + 2 * pi / 10), -a, b * cosf(2 * pi / 5 * i + 2 * pi / 10));
		glm::vec3 upVert3 = glm::vec3(b * sinf(2 * pi / 5 * i + 2 * pi / 5), a, b * cosf(2 * pi / 5 * i + 2 * pi / 5));

		glm::vec3 upNormal = glm::normalize(glm::cross( upVert3 - upVert2, upVert1 - upVert2)); // may have to change order

		localDirections[3].directions[i] = upNormal;
		localDirections[3].directions[9-i] = -upNormal;

		float aspectRatio = glm::length(upVert1 - topVert) / glm::length(upVert1 - upVert2);
		//std::cout << aspectRatio << std::endl;


		//texturing
		int upNumber = i + 1;
		int downNumber = 10 - i;

		float SinCosArg = atanf(aspectRatio);

		float uvB = sqrtf(uH * uH / (aspectRatio * aspectRatio + 1));
		//a bit of extra offset so that deltoids are higher
		glm::vec2 extraOffset = glm::vec2(0, 0.01);

		glm::vec2 uvOffset0 = glm::vec2(uW / 2, uH) + extraOffset;
		glm::vec2 uvOffset1 = glm::vec2(uW / uH * (uH / 2 - uvB * sinf(SinCosArg)), uvB * cosf(SinCosArg)) + extraOffset;
		glm::vec2 uvOffset2 = glm::vec2(uW / 2, 0) + extraOffset;
		glm::vec2 uvOffset3 = glm::vec2(uW / uH * (uH / 2 + uvB * sinf(SinCosArg)), uvB * cosf(SinCosArg)) + extraOffset;

		glm::vec2 topNumUV = NumberToUV(upNumber);


		float matUVB = sqrtf(1 / (aspectRatio * aspectRatio + 1));
		glm::vec2 materialUvOffset1 = glm::vec2(0.5 - matUVB * sinf(SinCosArg), matUVB * cosf(SinCosArg));
		glm::vec2 materialUvOffset3 = glm::vec2(0.5 + matUVB * sinf(SinCosArg), matUVB * cosf(SinCosArg));

		decaVertices.push_back({ topVert,upNormal,topNumUV + uvOffset0, glm::vec2(0.5,1) });
		decaVertices.push_back({ upVert1,upNormal,topNumUV + uvOffset1,materialUvOffset1 });
		decaVertices.push_back({ upVert2,upNormal,topNumUV + uvOffset2,glm::vec2(0.5,0) });
		decaVertices.push_back({ upVert3,upNormal,topNumUV + uvOffset3,materialUvOffset3 });


		decaIndices.push_back(8 * i + 0);
		decaIndices.push_back(8 * i + 1);
		decaIndices.push_back(8 * i + 2);

		decaIndices.push_back(8 * i + 2);
		decaIndices.push_back(8 * i + 3);
		decaIndices.push_back(8 * i + 0);


		glm::vec3 bottomVert = glm::vec3(0, -1, 0);
		glm::vec3 downVert1 = -upVert3;
		glm::vec3 downVert2 = -upVert2;
		glm::vec3 downVert3 = -upVert1;

		localHulls[3].Vertices.push_back(upVert2);
		localHulls[3].Vertices.push_back(downVert2);

		float currentMass = 0.0f;
		glm::mat3 currentInertiaPart = glm::mat3(0.0f);

		CalculateTetraHedronData(topVert, upVert1, upVert2, constantDensity, currentMass, currentInertiaPart);
		decaMass += currentMass;
		decaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(upVert2, upVert3, topVert, constantDensity, currentMass, currentInertiaPart);
		decaMass += currentMass;
		decaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(bottomVert, downVert1, downVert2, constantDensity, currentMass, currentInertiaPart);
		decaMass += currentMass;
		decaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(downVert2, downVert3, bottomVert, constantDensity, currentMass, currentInertiaPart);
		decaMass += currentMass;
		decaInertiaTensor += currentInertiaPart;

		glm::vec3 downNormal = -upNormal;

		glm::vec2 bottomNumUV = NumberToUV(downNumber);

		decaVertices.push_back({ bottomVert,downNormal,bottomNumUV + uvOffset0,glm::vec2(0.5,1) });
		decaVertices.push_back({ downVert1,downNormal,bottomNumUV + uvOffset1,materialUvOffset1 });
		decaVertices.push_back({ downVert2,downNormal,bottomNumUV + uvOffset2,glm::vec2(0.5,0) });
		decaVertices.push_back({ downVert3,downNormal,bottomNumUV + uvOffset3,materialUvOffset3 });

		decaIndices.push_back(8 * i + 4);
		decaIndices.push_back(8 * i + 5);
		decaIndices.push_back(8 * i + 6);

		decaIndices.push_back(8 * i + 6);
		decaIndices.push_back(8 * i + 7);
		decaIndices.push_back(8 * i + 4);
	}


	meshes[3].Set(decaVertices, decaIndices);

	localInertiaDatas[3].inertiaTensor = decaInertiaTensor;
	localInertiaDatas[3].inverseInertiaTensor = glm::inverse(decaInertiaTensor);

	entityModels[3] = { decaMass,0.7f,0.3f,3,1.0f,3,3 ,3};

	//Deca1hedron mesh
	std::vector<VertexP3N3T2T2> deca1Vertices;

	std::vector<unsigned int> deca1Indices;

	for (int i = 0;i < 5;++i)
	{





		float a = 0.1056; //magic number
		float b = sqrtf(1 - a * a);

		glm::vec3 topVert = glm::vec3(0, 1, 0);
		glm::vec3 upVert1 = glm::vec3(b * sinf(2 * pi / 5 * i), a, b * cosf(2 * pi / 5 * i));
		glm::vec3 upVert2 = glm::vec3(b * sinf(2 * pi / 5 * i + 2 * pi / 10), -a, b * cosf(2 * pi / 5 * i + 2 * pi / 10));
		glm::vec3 upVert3 = glm::vec3(b * sinf(2 * pi / 5 * i + 2 * pi / 5), a, b * cosf(2 * pi / 5 * i + 2 * pi / 5));

		glm::vec3 upNormal = glm::normalize(glm::cross(upVert3 - upVert2, upVert1 - upVert2)); // may have to change order


		float aspectRatio = glm::length(upVert1 - topVert) / glm::length(upVert1 - upVert2);
		//std::cout << aspectRatio << std::endl;


		//texturing
		int upNumber = i;
		int downNumber = 9 - i;

		float SinCosArg = atanf(aspectRatio);

		float uvB = sqrtf(uH * uH / (aspectRatio * aspectRatio + 1));
		//a bit of extra offset so that deltoids are higher
		glm::vec2 extraOffset = glm::vec2(0, 0.01);

		glm::vec2 uvOffset0 = glm::vec2(uW / 2, uH) + extraOffset;
		glm::vec2 uvOffset1 = glm::vec2(uW / uH * (uH / 2 - uvB * sinf(SinCosArg)), uvB * cosf(SinCosArg)) + extraOffset;
		glm::vec2 uvOffset2 = glm::vec2(uW / 2, 0) + extraOffset;
		glm::vec2 uvOffset3 = glm::vec2(uW / uH * (uH / 2 + uvB * sinf(SinCosArg)), uvB * cosf(SinCosArg)) + extraOffset;

		glm::vec2 topNumUV = NumberToUV(upNumber);


		float matUVB = sqrtf(1 / (aspectRatio * aspectRatio + 1));
		glm::vec2 materialUvOffset1 = glm::vec2(0.5 - matUVB * sinf(SinCosArg), matUVB * cosf(SinCosArg));
		glm::vec2 materialUvOffset3 = glm::vec2(0.5 + matUVB * sinf(SinCosArg), matUVB * cosf(SinCosArg));

		deca1Vertices.push_back({ topVert,upNormal,topNumUV + uvOffset0, glm::vec2(0.5,1) });
		deca1Vertices.push_back({ upVert1,upNormal,topNumUV + uvOffset1,materialUvOffset1 });
		deca1Vertices.push_back({ upVert2,upNormal,topNumUV + uvOffset2,glm::vec2(0.5,0) });
		deca1Vertices.push_back({ upVert3,upNormal,topNumUV + uvOffset3,materialUvOffset3 });


		deca1Indices.push_back(8 * i + 0);
		deca1Indices.push_back(8 * i + 1);
		deca1Indices.push_back(8 * i + 2);

		deca1Indices.push_back(8 * i + 2);
		deca1Indices.push_back(8 * i + 3);
		deca1Indices.push_back(8 * i + 0);


		glm::vec3 bottomVert = glm::vec3(0, -1, 0);
		glm::vec3 downVert1 = -upVert3;
		glm::vec3 downVert2 = -upVert2;
		glm::vec3 downVert3 = -upVert1;

		glm::vec3 downNormal = -upNormal;

		glm::vec2 bottomNumUV = NumberToUV(downNumber);

		deca1Vertices.push_back({ bottomVert,downNormal,bottomNumUV + uvOffset0,glm::vec2(0.5,1) });
		deca1Vertices.push_back({ downVert1,downNormal,bottomNumUV + uvOffset1,materialUvOffset1 });
		deca1Vertices.push_back({ downVert2,downNormal,bottomNumUV + uvOffset2,glm::vec2(0.5,0) });
		deca1Vertices.push_back({ downVert3,downNormal,bottomNumUV + uvOffset3,materialUvOffset3 });

		deca1Indices.push_back(8 * i + 4);
		deca1Indices.push_back(8 * i + 5);
		deca1Indices.push_back(8 * i + 6);

		deca1Indices.push_back(8 * i + 6);
		deca1Indices.push_back(8 * i + 7);
		deca1Indices.push_back(8 * i + 4);
	}


	meshes[4].Set(deca1Vertices, deca1Indices);

	entityModels[4] = { decaMass,0.7f,0.3f,3,1.0f,3,4,3 };

	//Deca10hedron mesh
	std::vector<VertexP3N3T2T2> deca10Vertices;

	std::vector<unsigned int> deca10Indices;

	for (int i = 0;i < 5;++i)
	{





		float a = 0.1056; //magic number
		float b = sqrtf(1 - a * a);

		glm::vec3 topVert = glm::vec3(0, 1, 0);
		glm::vec3 upVert1 = glm::vec3(b * sinf(2 * pi / 5 * i), a, b * cosf(2 * pi / 5 * i));
		glm::vec3 upVert2 = glm::vec3(b * sinf(2 * pi / 5 * i + 2 * pi / 10), -a, b * cosf(2 * pi / 5 * i + 2 * pi / 10));
		glm::vec3 upVert3 = glm::vec3(b * sinf(2 * pi / 5 * i + 2 * pi / 5), a, b * cosf(2 * pi / 5 * i + 2 * pi / 5));

		glm::vec3 upNormal = glm::normalize(glm::cross(upVert3 - upVert2, upVert1 - upVert2)); // may have to change order


		float aspectRatio = glm::length(upVert1 - topVert) / glm::length(upVert1 - upVert2);
		//std::cout << aspectRatio << std::endl;


		//texturing
		int upNumber = 10 * i;
		int downNumber = 90 - 10 * i;

		float SinCosArg = atanf(aspectRatio);

		float uvB = sqrtf(uH * uH / (aspectRatio * aspectRatio + 1));
		//a bit of extra offset so that deltoids are higher
		glm::vec2 extraOffset = glm::vec2(0, 0.01);

		glm::vec2 uvOffset0 = glm::vec2(uW / 2, uH) + extraOffset;
		glm::vec2 uvOffset1 = glm::vec2(uW / uH * (uH / 2 - uvB * sinf(SinCosArg)), uvB * cosf(SinCosArg)) + extraOffset;
		glm::vec2 uvOffset2 = glm::vec2(uW / 2, 0) + extraOffset;
		glm::vec2 uvOffset3 = glm::vec2(uW / uH * (uH / 2 + uvB * sinf(SinCosArg)), uvB * cosf(SinCosArg)) + extraOffset;

		glm::vec2 topNumUV = NumberToUV(upNumber);


		float matUVB = sqrtf(1 / (aspectRatio * aspectRatio + 1));
		glm::vec2 materialUvOffset1 = glm::vec2(0.5 - matUVB * sinf(SinCosArg), matUVB * cosf(SinCosArg));
		glm::vec2 materialUvOffset3 = glm::vec2(0.5 + matUVB * sinf(SinCosArg), matUVB * cosf(SinCosArg));

		deca10Vertices.push_back({ topVert,upNormal,topNumUV + uvOffset0, glm::vec2(0.5,1) });
		deca10Vertices.push_back({ upVert1,upNormal,topNumUV + uvOffset1,materialUvOffset1 });
		deca10Vertices.push_back({ upVert2,upNormal,topNumUV + uvOffset2,glm::vec2(0.5,0) });
		deca10Vertices.push_back({ upVert3,upNormal,topNumUV + uvOffset3,materialUvOffset3 });


		deca10Indices.push_back(8 * i + 0);
		deca10Indices.push_back(8 * i + 1);
		deca10Indices.push_back(8 * i + 2);

		deca10Indices.push_back(8 * i + 2);
		deca10Indices.push_back(8 * i + 3);
		deca10Indices.push_back(8 * i + 0);


		glm::vec3 bottomVert = glm::vec3(0, -1, 0);
		glm::vec3 downVert1 = -upVert3;
		glm::vec3 downVert2 = -upVert2;
		glm::vec3 downVert3 = -upVert1;

		glm::vec3 downNormal = -upNormal;

		glm::vec2 bottomNumUV = NumberToUV(downNumber);

		deca10Vertices.push_back({ bottomVert,downNormal,bottomNumUV + uvOffset0,glm::vec2(0.5,1) });
		deca10Vertices.push_back({ downVert1,downNormal,bottomNumUV + uvOffset1,materialUvOffset1 });
		deca10Vertices.push_back({ downVert2,downNormal,bottomNumUV + uvOffset2,glm::vec2(0.5,0) });
		deca10Vertices.push_back({ downVert3,downNormal,bottomNumUV + uvOffset3,materialUvOffset3 });

		deca10Indices.push_back(8 * i + 4);
		deca10Indices.push_back(8 * i + 5);
		deca10Indices.push_back(8 * i + 6);

		deca10Indices.push_back(8 * i + 6);
		deca10Indices.push_back(8 * i + 7);
		deca10Indices.push_back(8 * i + 4);
	}


	meshes[5].Set(deca10Vertices, deca10Indices);

	entityModels[5] = { decaMass,0.7f,0.3f,3,1.0f,3,5 ,3};

#pragma endregion DecaHedrons

#pragma region DodecaHedron
	//Dodecahedron mesh
	std::vector<VertexP3N3T2T2> dodecaVertices;

	std::vector<unsigned int> dodecaIndices;

	float dodecaMass = 0.0f;
	glm::mat3 dodecaInertiaTensor = glm::mat3(0.0f);

	localDirections[4].directions.resize(12);


	for (int i = 0;i < 5;++i)
	{
		float midA = 0.18759247f;
		float midB = sqrtf(1 - midA * midA);

		float topA = 0.79465447f;
		float topB = sqrtf(1 - topA * topA);

		//top face
		glm::vec3 midTopVert0 = glm::vec3(midB * sinf(2 * pi / 5 * i), midA, midB * cosf(2 * pi / 5 * i));
		glm::vec3 midBottomVert1 = glm::vec3(midB * sinf(2 * pi / 5 * i + pi / 5), -midA, midB * cosf(2 * pi / 5 * i + pi / 5));
		glm::vec3 midTopVert2 = glm::vec3(midB * sinf(2 * pi / 5 * i + 2 * pi / 5), midA, midB * cosf(2 * pi / 5 * i + 2 * pi / 5));
		glm::vec3 topVert3 = glm::vec3(topB * sinf(2 * pi / 5 * i + 2 * pi / 5), topA, topB * cosf(2 * pi / 5 * i + 2 * pi / 5));
		glm::vec3 topVert4 = glm::vec3(topB * sinf(2 * pi / 5 * i), topA, topB * cosf(2 * pi / 5 * i));

		localHulls[4].Vertices.push_back(midTopVert0);
		localHulls[4].Vertices.push_back(topVert4);

		float currentMass = 0.0f;
		glm::mat3 currentInertiaPart = glm::mat3(0.0f);

		CalculateTetraHedronData(midTopVert0, midBottomVert1, midTopVert2, constantDensity, currentMass, currentInertiaPart);
		dodecaMass += currentMass;
		dodecaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(midTopVert0, midTopVert2,topVert3 ,constantDensity, currentMass, currentInertiaPart);
		dodecaMass += currentMass;
		dodecaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(midTopVert0, topVert3,topVert4, constantDensity, currentMass, currentInertiaPart);
		dodecaMass += currentMass;
		dodecaInertiaTensor += currentInertiaPart;



		glm::vec3 topNormal = glm::normalize(glm::cross(midTopVert2 - midBottomVert1, midTopVert0 - midBottomVert1));

		int topNumber = i + 1;
		int bottomNumber = 12 - i;

		localDirections[4].directions[i] = topNormal;
		localDirections[4].directions[11-i] = -topNormal;

		glm::vec2 topNumberUV = NumberToUV(topNumber);

		float uvA = uW / 2.0 / cosf(pi / 5);

		glm::vec2 UVoffset0 = glm::vec2(0, uH - uH / uW * uvA * cosf(pi / 10));
		glm::vec2 UVoffset1 = glm::vec2(uW / 2, uH - uH / uW * uvA * cosf(pi / 10) - uH / uW * uvA * tanf(pi / 5));
		glm::vec2 UVoffset2 = glm::vec2(uW, uH - uH / uW * uvA * cosf(pi / 10));
		glm::vec2 UVoffset4 = glm::vec2(uvA * sinf(pi / 10), uH);
		glm::vec2 UVoffset3 = glm::vec2(uW - uvA * sinf(pi / 10), uH);

		float matUVA = 0.5 / cosf(pi / 5);


		dodecaVertices.push_back({ midTopVert0,topNormal,topNumberUV + UVoffset0,glm::vec2(0,1 - matUVA * cosf(pi / 10)) });
		dodecaVertices.push_back({ midBottomVert1,topNormal,topNumberUV + UVoffset1,glm::vec2(0.5,1 - matUVA * cosf(pi / 10) - matUVA * tanf(pi / 5)) });
		dodecaVertices.push_back({ midTopVert2,topNormal,topNumberUV + UVoffset2,glm::vec2(1,1 - matUVA * cosf(pi / 10)) });
		dodecaVertices.push_back({ topVert3,topNormal,topNumberUV + UVoffset3,glm::vec2(1 - matUVA * sinf(pi / 10),1) });
		dodecaVertices.push_back({ topVert4,topNormal,topNumberUV + UVoffset4,glm::vec2(matUVA * sinf(pi / 10),1) });

		dodecaIndices.push_back(10 * i + 0);
		dodecaIndices.push_back(10 * i + 1);
		dodecaIndices.push_back(10 * i + 2);

		dodecaIndices.push_back(10 * i + 0);
		dodecaIndices.push_back(10 * i + 2);
		dodecaIndices.push_back(10 * i + 3);

		dodecaIndices.push_back(10 * i + 0);
		dodecaIndices.push_back(10 * i + 3);
		dodecaIndices.push_back(10 * i + 4);


		//bottom face

		glm::vec3 midBottomVert0 = -midTopVert2;
		glm::vec3 midTopVert1 = -midBottomVert1;
		glm::vec3 midBottomVert2 = -midTopVert0;
		glm::vec3 bottomVert3 = -topVert4;
		glm::vec3 bottomVert4 = -topVert3;



		localHulls[4].Vertices.push_back(midBottomVert0);
		localHulls[4].Vertices.push_back(bottomVert4);


		CalculateTetraHedronData(midBottomVert0,midTopVert1,midBottomVert2, constantDensity, currentMass, currentInertiaPart);
		dodecaMass += currentMass;
		dodecaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(midBottomVert0, midBottomVert2,bottomVert3, constantDensity, currentMass, currentInertiaPart);
		dodecaMass += currentMass;
		dodecaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(midBottomVert0,bottomVert3,bottomVert4, constantDensity, currentMass, currentInertiaPart);
		dodecaMass += currentMass;
		dodecaInertiaTensor += currentInertiaPart;




		glm::vec3 bottomNormal = -topNormal;

		glm::vec2 bottomNumberUV = NumberToUV(bottomNumber);

		dodecaVertices.push_back({ midBottomVert0,bottomNormal,bottomNumberUV + UVoffset0,glm::vec2(0,1 - matUVA * cosf(pi / 10)) });
		dodecaVertices.push_back({ midTopVert1,bottomNormal,bottomNumberUV + UVoffset1,glm::vec2(0.5,1 - matUVA * cosf(pi / 10) - matUVA * tanf(pi / 5)) });
		dodecaVertices.push_back({ midBottomVert2,bottomNormal,bottomNumberUV + UVoffset2,glm::vec2(1,1 - matUVA * cosf(pi / 10)) });
		dodecaVertices.push_back({ bottomVert3,bottomNormal,bottomNumberUV + UVoffset3,glm::vec2(1 - matUVA * sinf(pi / 10),1) });
		dodecaVertices.push_back({ bottomVert4,bottomNormal,bottomNumberUV + UVoffset4,glm::vec2(matUVA * sinf(pi / 10),1) });

		dodecaIndices.push_back(10 * i + 5);
		dodecaIndices.push_back(10 * i + 6);
		dodecaIndices.push_back(10 * i + 7);

		dodecaIndices.push_back(10 * i + 5);
		dodecaIndices.push_back(10 * i + 7);
		dodecaIndices.push_back(10 * i + 8);

		dodecaIndices.push_back(10 * i + 5);
		dodecaIndices.push_back(10 * i + 8);
		dodecaIndices.push_back(10 * i + 9);

	}

	{


		float topA = 0.79465447f;
		float topB = sqrtf(1 - topA * topA);

		//top face

		glm::vec3 topVert0 = glm::vec3(topB * sinf(2 * pi / 5 * 0), topA, topB * cosf(2 * pi / 5 * 0));
		glm::vec3 topVert1 = glm::vec3(topB * sinf(2 * pi / 5 * 1), topA, topB * cosf(2 * pi / 5 * 1));
		glm::vec3 topVert2 = glm::vec3(topB * sinf(2 * pi / 5 * 2), topA, topB * cosf(2 * pi / 5 * 2));
		glm::vec3 topVert3 = glm::vec3(topB * sinf(2 * pi / 5 * 3), topA, topB * cosf(2 * pi / 5 * 3));
		glm::vec3 topVert4 = glm::vec3(topB * sinf(2 * pi / 5 * 4), topA, topB * cosf(2 * pi / 5 * 4));



		float currentMass = 0.0f;
		glm::mat3 currentInertiaPart = glm::mat3(0.0f);

		CalculateTetraHedronData(topVert0, topVert1, topVert2, constantDensity, currentMass, currentInertiaPart);
		dodecaMass += currentMass;
		dodecaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(topVert0, topVert2, topVert3, constantDensity, currentMass, currentInertiaPart);
		dodecaMass += currentMass;
		dodecaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(topVert0, topVert3, topVert4, constantDensity, currentMass, currentInertiaPart);
		dodecaMass += currentMass;
		dodecaInertiaTensor += currentInertiaPart;



		glm::vec3 topNormal = glm::vec3(0, 1, 0);

		int topNumber = 6;
		int bottomNumber = 7;

		localDirections[4].directions[5] = glm::vec3(0, 1, 0);
		localDirections[4].directions[6] = glm::vec3(0, -1, 0);

		glm::vec2 topNumberUV = NumberToUV(topNumber);

		float uvA = uW / 2.0 / cosf(pi / 5);

		glm::vec2 UVoffset0 = glm::vec2(0, uH - uH / uW * uvA * cosf(pi / 10));
		glm::vec2 UVoffset1 = glm::vec2(uW / 2, uH - uH / uW * uvA * cosf(pi / 10) - uH / uW * uvA * tanf(pi / 5));
		glm::vec2 UVoffset2 = glm::vec2(uW, uH - uH / uW * uvA * cosf(pi / 10));
		glm::vec2 UVoffset4 = glm::vec2(uvA * sinf(pi / 10), uH);
		glm::vec2 UVoffset3 = glm::vec2(uW - uvA * sinf(pi / 10), uH);

		float matUVA = 0.5 / cosf(pi / 5);


		dodecaVertices.push_back({ topVert0,topNormal,topNumberUV + UVoffset0,glm::vec2(0,1 - matUVA * cosf(pi / 10)) });
		dodecaVertices.push_back({ topVert1,topNormal,topNumberUV + UVoffset1,glm::vec2(0.5,1 - matUVA * cosf(pi / 10) - matUVA * tanf(pi / 5)) });
		dodecaVertices.push_back({ topVert2,topNormal,topNumberUV + UVoffset2,glm::vec2(1,1 - matUVA * cosf(pi / 10)) });
		dodecaVertices.push_back({ topVert3,topNormal,topNumberUV + UVoffset3,glm::vec2(1 - matUVA * sinf(pi / 10),1) });
		dodecaVertices.push_back({ topVert4,topNormal,topNumberUV + UVoffset4,glm::vec2(matUVA * sinf(pi / 10),1) });

		dodecaIndices.push_back(50);
		dodecaIndices.push_back(51);
		dodecaIndices.push_back(52);

		dodecaIndices.push_back(50);
		dodecaIndices.push_back(52);
		dodecaIndices.push_back(53);

		dodecaIndices.push_back(50);
		dodecaIndices.push_back(53);
		dodecaIndices.push_back(54);


		//bottom face
		glm::vec3 bottomVert0 = -topVert0;
		glm::vec3 bottomVert1 = -topVert4;
		glm::vec3 bottomVert2 = -topVert3;
		glm::vec3 bottomVert3 = -topVert2;
		glm::vec3 bottomVert4 = -topVert1;



		CalculateTetraHedronData(bottomVert0, bottomVert1, bottomVert2, constantDensity, currentMass, currentInertiaPart);
		dodecaMass += currentMass;
		dodecaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(bottomVert0, bottomVert2, bottomVert3, constantDensity, currentMass, currentInertiaPart);
		dodecaMass += currentMass;
		dodecaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(bottomVert0, bottomVert3, bottomVert4, constantDensity, currentMass, currentInertiaPart);
		dodecaMass += currentMass;
		dodecaInertiaTensor += currentInertiaPart;




		glm::vec3 bottomNormal = -topNormal;

		glm::vec2 bottomNumberUV = NumberToUV(bottomNumber);

		dodecaVertices.push_back({ bottomVert0,bottomNormal,bottomNumberUV + UVoffset0,glm::vec2(0,1 - matUVA * cosf(pi / 10)) });
		dodecaVertices.push_back({ bottomVert1,bottomNormal,bottomNumberUV + UVoffset1,glm::vec2(0.5,1 - matUVA * cosf(pi / 10) - matUVA * tanf(pi / 5)) });
		dodecaVertices.push_back({ bottomVert2,bottomNormal,bottomNumberUV + UVoffset2,glm::vec2(1,1 - matUVA * cosf(pi / 10)) });
		dodecaVertices.push_back({ bottomVert3,bottomNormal,bottomNumberUV + UVoffset3,glm::vec2(1 - matUVA * sinf(pi / 10),1) });
		dodecaVertices.push_back({ bottomVert4,bottomNormal,bottomNumberUV + UVoffset4,glm::vec2(matUVA * sinf(pi / 10),1) });

		dodecaIndices.push_back(55);
		dodecaIndices.push_back(56);
		dodecaIndices.push_back(57);

		dodecaIndices.push_back(55);
		dodecaIndices.push_back(57);
		dodecaIndices.push_back(58);

		dodecaIndices.push_back(55);
		dodecaIndices.push_back(58);
		dodecaIndices.push_back(59);
	}


	meshes[6].Set(dodecaVertices, dodecaIndices);

	localInertiaDatas[4].inertiaTensor = dodecaInertiaTensor;
	localInertiaDatas[4].inverseInertiaTensor= glm::inverse(dodecaInertiaTensor);

	entityModels[6] = { dodecaMass,0.7f,0.3f,4,1.0f,4,6 ,4};

#pragma endregion DodecaHedron

#pragma region IcosaHedron

	//Icosahedron mesh
	std::vector<VertexP3N3T2T2> icosaVertices;

	std::vector<unsigned int> icosaIndices;

	localHulls[5].Vertices.push_back(glm::vec3(0, 1, 0));
	localHulls[5].Vertices.push_back(glm::vec3(0, -1, 0));

	float icosaMass = 0.0f;
	glm::mat3 icosaInertiaTensor = glm::mat3(0.0f);

	localDirections[5].directions.resize(20);

	for (int i = 0;i < 5;++i)
	{
		float a = 0.4472;
		float b = sqrtf(1 - a * a);

		glm::vec3 topVert0 = glm::vec3(0, 1, 0);
		glm::vec3 topVert1 = glm::vec3(b * sinf(2 * pi / 5.0 * i), a, b * cosf(2 * pi / 5.0 * i));
		glm::vec3 topVert2 = glm::vec3(b * sinf(2 * pi / 5.0 * i + 2 * pi / 5), a, b * cosf(2 * pi / 5.0 * i + 2 * pi / 5));
		glm::vec3 topVert3 = glm::vec3(b * sinf(2 * pi / 5.0 * i + pi / 5), -a, b * cosf(2 * pi / 5.0 * i + pi / 5));

		localHulls[5].Vertices.push_back(topVert1);
		localHulls[5].Vertices.push_back(topVert3);

		float currentMass = 0.0f;
		glm::mat3 currentInertiaPart = glm::mat3(0.0f);

		CalculateTetraHedronData(topVert0, topVert1, topVert2, constantDensity, currentMass, currentInertiaPart);
		icosaMass += currentMass;
		icosaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(topVert1,topVert3, topVert2, constantDensity, currentMass, currentInertiaPart);
		icosaMass += currentMass;
		icosaInertiaTensor += currentInertiaPart;


		glm::vec3 topNormal = glm::normalize(glm::cross(topVert1 - topVert0, topVert2 - topVert0));

		glm::vec3 topMidNormal = glm::normalize(glm::cross(topVert2 - topVert3, topVert1 - topVert3));

		glm::vec2 uvOffset0 = glm::vec2(0, uH * (sqrt3 - 1) / 2.0 / sqrt3);
		glm::vec2 uvOffset1 = glm::vec2(uW, uvOffset0.y);
		glm::vec2 uvOffset2 = glm::vec2(uW / 2.0, uH * (2 + sqrt3) / sqrt3 / 2.0);

		int topNumber = i + 1;
		int bottomNumber = 20 - i;
		int topMidNumber = 15 - i;
		int bottomMidNumber = i + 6;


		localDirections[5].directions[i] = topNormal;
		localDirections[5].directions[14-i] = topMidNormal;
		localDirections[5].directions[19-i] = -topNormal;
		localDirections[5].directions[5 + i] = -topMidNormal;

		glm::vec2 topNumberUV = NumberToUV(topNumber);
		glm::vec2 topMidNumberUV = NumberToUV(topMidNumber);


		icosaVertices.push_back({ topVert0,topNormal,topNumberUV + uvOffset2,glm::vec2(0.5,(2 + sqrt3) / 2.0 / sqrt3 - 0.1) });
		icosaVertices.push_back({ topVert1,topNormal,topNumberUV + uvOffset0,glm::vec2(0,(sqrt3 - 1) / 2.0 / sqrt3 - 0.1) });
		icosaVertices.push_back({ topVert2,topNormal,topNumberUV + uvOffset1,glm::vec2(1,(sqrt3 - 1) / 2.0 / sqrt3 - 0.1) });

		icosaVertices.push_back({ topVert1,topMidNormal,topMidNumberUV + uvOffset1,glm::vec2(1,(sqrt3 - 1) / 2.0 / sqrt3 - 0.1) });
		icosaVertices.push_back({ topVert3,topMidNormal,topMidNumberUV + uvOffset2,glm::vec2(0.5,(2 + sqrt3) / 2.0 / sqrt3 - 0.1) });
		icosaVertices.push_back({ topVert2,topMidNormal,topMidNumberUV + uvOffset0,glm::vec2(0,(sqrt3 - 1) / 2.0 / sqrt3 - 0.1) });

		icosaIndices.push_back(12 * i + 0);
		icosaIndices.push_back(12 * i + 1);
		icosaIndices.push_back(12 * i + 2);

		icosaIndices.push_back(12 * i + 3);
		icosaIndices.push_back(12 * i + 4);
		icosaIndices.push_back(12 * i + 5);

		glm::vec2 bottomNumberUV = NumberToUV(bottomNumber);
		glm::vec2 bottomMidNumberUV = NumberToUV(bottomMidNumber);

		glm::vec3 bottomVert0 = glm::vec3(0, -1, 0);
		glm::vec3 bottomVert1 = -topVert2;
		glm::vec3 bottomVert2 = -topVert1;
		glm::vec3 bottomVert3 = -topVert3;


		CalculateTetraHedronData(bottomVert0,bottomVert1, bottomVert2, constantDensity, currentMass, currentInertiaPart);
		icosaMass += currentMass;
		icosaInertiaTensor += currentInertiaPart;

		CalculateTetraHedronData(bottomVert1, bottomVert3, bottomVert2, constantDensity, currentMass, currentInertiaPart);
		icosaMass += currentMass;
		icosaInertiaTensor += currentInertiaPart;




		glm::vec3 bottomNormal = -topNormal;
		glm::vec3 bottomMidNormal = -topMidNormal;

		icosaVertices.push_back({ bottomVert0,bottomNormal,bottomNumberUV + uvOffset2,glm::vec2(0.5,(2 + sqrt3) / 2.0 / sqrt3 - 0.1) });
		icosaVertices.push_back({ bottomVert1,bottomNormal,bottomNumberUV + uvOffset0,glm::vec2(0,(sqrt3 - 1) / 2.0 / sqrt3 - 0.1) });
		icosaVertices.push_back({ bottomVert2,bottomNormal,bottomNumberUV + uvOffset1,glm::vec2(1,(sqrt3 - 1) / 2.0 / sqrt3 - 0.1) });

		icosaVertices.push_back({ bottomVert1,bottomMidNormal,bottomMidNumberUV + uvOffset1,glm::vec2(1,(sqrt3 - 1) / 2.0 / sqrt3 - 0.1) });
		icosaVertices.push_back({ bottomVert3,bottomMidNormal,bottomMidNumberUV + uvOffset2,glm::vec2(0.5,(2 + sqrt3) / 2.0 / sqrt3 - 0.1) });
		icosaVertices.push_back({ bottomVert2,bottomMidNormal,bottomMidNumberUV + uvOffset0,glm::vec2(0,(sqrt3 - 1) / 2.0 / sqrt3 - 0.1) });

		icosaIndices.push_back(12 * i + 6);
		icosaIndices.push_back(12 * i + 7);
		icosaIndices.push_back(12 * i + 8);

		icosaIndices.push_back(12 * i + 9);
		icosaIndices.push_back(12 * i + 10);
		icosaIndices.push_back(12 * i + 11);
	}

	meshes[7].Set(icosaVertices, icosaIndices);

	localInertiaDatas[5].inertiaTensor = icosaInertiaTensor;
	localInertiaDatas[5].inverseInertiaTensor += glm::inverse(icosaInertiaTensor);

	entityModels[7] = { icosaMass,0.7f,0.3f,5,1.0f,5,7,5 };

#pragma endregion IcosaHedron



	//Intialization of our 2 textures: (so far)
	numberTexture.Set("res/textures/number_texture.png");
	materialTexture.Set("res/textures/material.png");

	//Initialization of the worldsize and bordering planes
	this->worldSize = worldSize;
	worldBorders.push_back({ +worldSize.y / 2.0f,glm::vec3(0,1,0) });
	//worldBorders.push_back({ +worldSize.y / 2.0f,glm::vec3(0,-1,0) });

	worldBorders.push_back({ +worldSize.x / 2.0f,glm::vec3(1,0,0) });
	worldBorders.push_back({ +worldSize.x / 2.0f,glm::vec3(-1,0,0) });

	worldBorders.push_back({ +worldSize.z / 2.0f,glm::vec3(0,0,1) });
	worldBorders.push_back({ +worldSize.z / 2.0f,glm::vec3(0,0,-1) });





	baseShader.Set("res/shaders/vertex_shader.vert", "res/shaders/fragment_shader.frag");


	//Cube Mesh for box & table
	std::vector<VertexP3N3T2T2> cubeMeshVertices = {

		//Top
		{glm::vec3(0.5f,0.5f,0.5f),glm::vec3(0.0f,1.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(0.0f,0.0f)},
		{glm::vec3(-0.5f,0.5f,0.5f),glm::vec3(0.0f,1.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,0.0f)},
		{glm::vec3(-0.5f,0.5f,-0.5f),glm::vec3(0.0f,1.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,1.0f)},
		{glm::vec3(0.5f,0.5f,-0.5f),glm::vec3(0.0f,1.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(0.0f,1.0f)},

		//Bottom
		{glm::vec3(0.5,-0.5f,0.5f),glm::vec3(0.0f,-1.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(0.0f,0.0f)},
		{glm::vec3(-0.5,-0.5f,0.5f),glm::vec3(0.0f,-1.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,0.0f)},
		{glm::vec3(-0.5,-0.5f,-0.5f),glm::vec3(0.0f,-1.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,1.0f)},
		{glm::vec3(0.5,-0.5f,-0.5f),glm::vec3(0.0f,-1.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(0.0f,1.0f)},

		{glm::vec3(0.5f,0.5f,0.5f),glm::vec3(1.0f,0.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(0.0f,0.0f)},
		{glm::vec3(0.5f,-0.5f,0.5f),glm::vec3(1.0f,0.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,0.0f)},
		{glm::vec3(0.5f,-0.5f,-0.5f),glm::vec3(1.0f,0.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,1.0f)},
		{glm::vec3(0.5f,0.5f,-0.5f),glm::vec3(1.0f,0.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(0.0f,1.0f)},

		{glm::vec3(-0.5f,0.5f,0.5f),glm::vec3(-1.0f,0.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(0.0f,0.0f)},
		{glm::vec3(-0.5f,-0.5f,0.5f),glm::vec3(-1.0f,0.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,0.0f)},
		{glm::vec3(-0.5f,-0.5f,-0.5f),glm::vec3(-1.0f,0.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,1.0f)},
		{glm::vec3(-0.5f,0.5f,-0.5f),glm::vec3(-1.0f,0.0f,0.0f),glm::vec2(0.0f,0.0f),glm::vec2(0.0f,1.0f)},


		{glm::vec3(0.5f,0.5f,0.5f),glm::vec3(0.0f,0.0f,1.0f),glm::vec2(0.0f,0.0f),glm::vec2(0.0f,0.0f)},
		{glm::vec3(-0.5f,0.5f,0.5f),glm::vec3(0.0f,0.0f,1.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,0.0f)},
		{glm::vec3(-0.5f,-0.5f,0.5f),glm::vec3(0.0f,0.0f,1.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,1.0f)},
		{glm::vec3(0.5f,-0.5f,0.5f),glm::vec3(0.0f,0.0f,1.0f),glm::vec2(0.0f,0.0f),glm::vec2(0.0f,1.0f)},

		{glm::vec3(0.5f,0.5f,-0.5f),glm::vec3(0.0f,0.0f,-1.0f),glm::vec2(0.0f,0.0f),glm::vec2(0.0f,0.0f)},
		{glm::vec3(-0.5f,0.5f,-0.5f),glm::vec3(0.0f,0.0f,-1.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,0.0f)},
		{glm::vec3(-0.5f,-0.5f,-0.5f),glm::vec3(0.0f,0.0f,-1.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,1.0f)},
		{glm::vec3(0.5f,-0.5f,-0.5f),glm::vec3(0.0f,0.0f,-1.0f),glm::vec2(0.0f,0.0f),glm::vec2(0.0f,1.0f)}

	};

	std::vector<unsigned int> cubeMeshIndices = {
		2,1,0,
		0,3,2,

		4,5,6,
		6,7,4,

		8,9,10,
		10,11,8,

		14,13,12,
		12,15,14,

		16,17,18,
		18,19,16,

		22,21,20,
		20,23,22
	};

	cubeMesh.Set(cubeMeshVertices, cubeMeshIndices);

	boxTexture.Set("res/textures/woodenTexture1.png");
	//std::cout << boxTexture.GetHeight() << std::endl;

	


	//Worldsize has to be + in all directions
	float boxWidth = 1.0f;


	boxWorldTransforms[0] = glm::translate(glm::mat4(1.0f),glm::vec3(0,-worldSize.y/2 - boxWidth / 2,0))* 
							glm::scale(glm::mat4(1.0f), glm::vec3(worldSize.x,boxWidth,worldSize.z));

	

	boxWorldTransforms[1] = glm::translate(glm::mat4(1.0f),glm::vec3(0,-boxWidth/2-worldSize.y*3/8,+worldSize.z/2  + boxWidth/2)) *
							glm::scale(glm::mat4(1.0f), glm::vec3(worldSize.x + 2*boxWidth,worldSize.y/4 +boxWidth,boxWidth));

	boxWorldTransforms[2] = glm::translate(glm::mat4(1.0f), glm::vec3(-worldSize.x / 2 - boxWidth/2, -boxWidth / 2 - worldSize.y * 3 / 8, 0)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(worldSize.z, worldSize.y / 4 + boxWidth,boxWidth));

	boxWorldTransforms[3] = glm::translate(glm::mat4(1.0f), glm::vec3(0, -boxWidth / 2 - worldSize.y * 3 / 8, -worldSize.z / 2 - boxWidth / 2)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(worldSize.x + 2 * boxWidth, worldSize.y / 4 + boxWidth, boxWidth));

	boxWorldTransforms[4] = glm::translate(glm::mat4(1.0f), glm::vec3(+worldSize.x / 2 + boxWidth / 2, -boxWidth / 2 - worldSize.y * 3 / 8, 0)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(worldSize.z, worldSize.y / 4 + boxWidth, boxWidth));

	boxWorldTransforms[5] = glm::translate(glm::mat4(1.0f),glm::vec3(0,-worldSize.y/2 - boxWidth*1.5f,worldSize.z/2-boxWidth/2))*
		glm::scale(glm::mat4(1.0f), glm::vec3(worldSize.x, boxWidth, boxWidth));

	boxWorldTransforms[6] = glm::translate(glm::mat4(1.0f), glm::vec3(0, -worldSize.y/2 - boxWidth * 1.5f, -worldSize.z / 2 + boxWidth / 2)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(worldSize.x, boxWidth, boxWidth));

	boxInvTranWorldTransforms[0] = glm::transpose(glm::inverse(boxWorldTransforms[0]));
	boxInvTranWorldTransforms[1] = glm::transpose(glm::inverse(boxWorldTransforms[1]));
	boxInvTranWorldTransforms[2] = glm::transpose(glm::inverse(boxWorldTransforms[2]));
	boxInvTranWorldTransforms[3] = glm::transpose(glm::inverse(boxWorldTransforms[3]));
	boxInvTranWorldTransforms[4] = glm::transpose(glm::inverse(boxWorldTransforms[4]));
	boxInvTranWorldTransforms[5] = glm::transpose(glm::inverse(boxWorldTransforms[5]));
	boxInvTranWorldTransforms[6] = glm::transpose(glm::inverse(boxWorldTransforms[6]));

	
	//setting up Light data
	spotLightPosition = glm::vec3(10, 20, 10);
	spotLightDirection = glm::normalize( -spotLightPosition);
	spotLightInnerCosine = cosf(glm::radians(6.0f));
	spotLightOuterCosine = cosf(glm::radians(24.0f));

	//Setting up shadow data
	shadowShader.Set("res/shaders/shadow_vertex_shader.vert", "res/shaders/shadow_fragment_shader.frag");

	shadowMap.Set(2048, 2048); //specifying its resolution kinda

	lightFarPlane = 50.0f;
	lightNearPlane = 1.0f;

	lightFovy = 24.0f;

}

void ECS::Update(float deltaTime, int& diceRollSum)
{
	TransformSystem();
	CollisionDetectionSystem();
	CollisionSolverSystem();
	PhysicsSystem(deltaTime);
	SummationSystem(diceRollSum);
	//plus other update based systems
	
}

void ECS::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& cameraPosition,
				 float windowWidth, float windowHeight)
{
	/*
	* Firstly, we render our virtual world to a so called depth Buffer
	* To do this, we use our Spotlight's properties, to calculate the corrent lightView, and lightProj matrices
	* We will also use a special, simplified shader, that will just simply write the depth value to the shadowMap
	*/
	shadowShader.Bind();

	shadowMap.BindForWriting();

	glViewport(0, 0, shadowMap.GetWidth(), shadowMap.GetHeight());

	glClear(GL_DEPTH_BUFFER_BIT);

	float shadowWidth = (float)shadowMap.GetWidth();
	float shadowHeight = (float)shadowMap.GetHeight();

	glm::mat4 lightProjectionMatrix = glm::perspective(lightFovy, shadowWidth / shadowHeight, lightNearPlane, lightFarPlane);
	glm::mat4 lightViewMatrix = glm::lookAt(spotLightPosition, spotLightPosition + spotLightDirection, glm::vec3(0, 1.0f, 0));
	glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

	//Rendering all objects that are allowed to cast a shadow
	shadowShader.SetUniform<glm::mat4>("uLightSpaceTransform", lightSpaceMatrix);

	RenderSystem(shadowShader);

	for (int i = 0;i < 7;++i)
	{
		shadowShader.SetUniform<glm::mat4>("uWorldTransform", boxWorldTransforms[i]);
		cubeMesh.Draw();
	}




	shadowMap.UnbindFromWriting();

	shadowShader.Unbind();






	//Here happens the actual rendering to the screen's display buffer
	baseShader.Bind();

	glViewport(0, 0, windowWidth, windowHeight);

	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Projection & View transforms are the same for all objects in the virtual world
	baseShader.SetUniform<glm::mat4>("projectionTransform", projectionMatrix);
	baseShader.SetUniform<glm::mat4>("viewTransform", viewMatrix);

	baseShader.SetUniform<glm::vec3>("uCameraPosition", cameraPosition);

	//Setting up light related uniforms

	baseShader.SetUniform<glm::vec3>("uSpotLightPosition", spotLightPosition);
	baseShader.SetUniform<glm::vec3>("uSpotLightDirection", spotLightDirection);
	baseShader.SetUniform<float>("uSpotLightInnerCos", spotLightInnerCosine);
	baseShader.SetUniform<float>("uSpotLightOuterCos", spotLightOuterCosine);

	baseShader.SetUniform<glm::mat4>("uLightSpaceMatrix", lightSpaceMatrix);

	//Drawing of the entities
	numberTexture.Bind(0);
	baseShader.SetUniform<int>("uNumberTexture", 0);

	materialTexture.Bind(1);
	baseShader.SetUniform<int>("uMaterialTexture", 1);

	shadowMap.BindForReading(2);
	baseShader.SetUniform<int>("uShadowMapTexture", 2);



	baseShader.SetUniform<int>("uObjectTypeId", 0);

	

	RenderSystem(baseShader);

	//Here, the rendering of the box happens
	boxTexture.Bind(1);
	baseShader.SetUniform<int>("uMaterialTexture", 1);

	baseShader.SetUniform<int>("uObjectTypeId", 1);
	
	for (int i = 0;i < 7;++i)
	{
		baseShader.SetUniform<glm::mat4>("uWorldTransform", boxWorldTransforms[i]);
		baseShader.SetUniform<glm::mat4>("uInverseTransposedWorldTransform", boxInvTranWorldTransforms[i]);
		cubeMesh.Draw();
	}

	

	


	baseShader.Unbind();
}

void ECS::AddEntity(EntityIdentifier identifier)
{
	if (m_entityCount < m_entityCapacity)
	{
		int insertIdx = m_entityCount;
		int removeIdx = m_entityCapacity - m_entityCount - 1;

		int newEntityId = m_freeIds[removeIdx];

		m_entityIds[insertIdx] = newEntityId;


		//buggy start datas
		glm::vec3 startPos = glm::vec3((rand() % 1000 - 500) / 1000.0f * worldSize.x,
			(rand() % 1000 - 500)/1000.0f * worldSize.y, 
			(rand() % 1000 - 500)/1000.0f * worldSize.z);

		glm::quat startRotation = glm::quat(glm::vec3(glm::radians((float)(rand() % 360)),
			glm::radians((float)(rand() % 360)), glm::radians((float)(rand() % 360))));


		glm::vec3 startVelocity =glm::vec3((rand() % 1000 -500) / 30.0f, (rand() % 1000-500) / 30.0f, (rand() % 1000-500) / 30.0f);

		glm::vec3 startAngularVelocity = glm::vec3(glm::radians((float)(rand() % 720 -360)), glm::radians((float)(rand() % 720 -360)),
			glm::radians((float)(rand() % 720 - 360 )));

		


		transforms[newEntityId].position = startPos;
		transforms[newEntityId].rotation = startRotation;
		transforms[newEntityId].scale = 1.0f;


		physics[newEntityId].mass = entityModels[identifier].mass;
		physics[newEntityId].invMass = 1.0f / physics[newEntityId].mass;
		
		physics[newEntityId].restitution = entityModels[identifier].restitution;
		physics[newEntityId].friction = entityModels[identifier].friction;
		physics[newEntityId].inertiaDataId = entityModels[identifier].inertiaDataId;
		physics[newEntityId].velocity = startVelocity;
		physics[newEntityId].angularVelocity = startAngularVelocity;

		colliders[newEntityId].boundingRadius = entityModels[identifier].boundingRadius;
		colliders[newEntityId].localHullId = entityModels[identifier].localHullId;

		localDirectionIds[newEntityId] = entityModels[identifier].localDirectionId;

		renders[newEntityId].meshId = entityModels[identifier].meshId;

		++m_entityCount;
	}
}

void ECS::Clear()
{
	int currentIdCount = m_entityCount;
	int currentFreeIdCount = m_entityCapacity - m_entityCount;

	for (int i = 0;i < currentIdCount;++i)
	{
		m_freeIds[currentFreeIdCount + i] = m_entityIds[i];
	}

	m_entityCount = 0;
}


//Private ECS function definitions
//-------------------------------------------------------------------------------------------------------------------------------------

void ECS::PhysicsSystem(float deltaTime)
{
	for (int i = 0;i < m_entityCount;++i)
	{
		int entityId = m_entityIds[i];

		//Actual physics happening

		//gravitational acceleration;
		physics[entityId].velocity += glm::vec3(0, -9.81, 0) * deltaTime;





		//position change
		transforms[entityId].position += physics[entityId].velocity * deltaTime;

		//rotation change
		glm::vec3 angularVel = physics[entityId].angularVelocity;
		glm::quat deltaQuat = glm::quat(0.0f, angularVel.x, angularVel.y, angularVel.z);

		transforms[entityId].rotation += 0.5f * deltaQuat * deltaTime * transforms[entityId].rotation;

		transforms[entityId].rotation = glm::normalize(transforms[entityId].rotation);
	}
}



void ECS::CollisionDetectionSystem()
{
	//if two vertices/contact points have a difference of this much, we kinda ignore the diff and call it a day
	float errorMargin = 0.01f;

	manifoldCount = 0;

	for (int i = 0;i < m_entityCount;++i)
	{
		int entityId = m_entityIds[i];


		/*
		* Iterating through the planes one by one, and checking wether or not our entity collides with them
		*/
		for (int planeIdx = 0;planeIdx < worldBorders.size();++planeIdx)
		{
			if (manifoldCount < manifoldCapacity)
			{
				Plane plane = worldBorders[planeIdx];

				//Variables, based on which we will solve the collision later
				bool areColliding = false;
				
				manifoldBuffer[manifoldCount].entityId1 = entityId;
				manifoldBuffer[manifoldCount].entityId2 = -1;
				manifoldBuffer[manifoldCount].normal = plane.normal;
				manifoldBuffer[manifoldCount].contactPointCount = 0;
				manifoldBuffer[manifoldCount].penetrationDepth = errorMargin;


				/*
				* Every entity has a bounding sphere, represented by a radius
				* If even this sphere isnt colliding with the plane, then we discard the collision right away
				*/
				glm::vec3 contactPointAvarage = glm::vec3(0, 0, 0);
				int contactPointAvarageCount = 0;

				float dist = glm::dot(transforms[entityId].position, plane.normal) + plane.offset;
				if (dist < colliders[entityId].boundingRadius)
				{

					int localHullId = colliders[entityId].localHullId;

					/*
					* More of a maximum search for the deepest contact point(s)
					* If we find more than one in roughly the same depth, then we construct their avarage
					*/
					

					float minPenetration = 0.0f;
					for (int vertIdx = 0;vertIdx < localHulls[localHullId].Vertices.size();++vertIdx)
					{

						glm::vec3 worldVert = glm::vec3(worldTransforms[entityId] * glm::vec4(localHulls[localHullId].Vertices[vertIdx], 1));
						//std::cout << "World Vertex: (x: " << worldVert.x << " y: " << worldVert.y << " z: " << worldVert.z << " )\n";
						//the signed penetration of the world Vertex
						float currentPenetration = glm::dot(worldVert, plane.normal) + plane.offset;

						//check if the given vertex is actually penetrating our plane's surface and
						if (currentPenetration < 0.0f)
						{
							areColliding = true;
							

							if (glm::abs(currentPenetration - minPenetration) < errorMargin)
							{
								contactPointAvarage += worldVert;
								++contactPointAvarageCount;
								//we found a point that only differs by a little from the rest->we assume a face-plane or edge-plane contact
								/*
								if (manifoldBuffer[manifoldCount].contactPointCount < 4)
								{
									manifoldBuffer[manifoldCount].contactPoints[manifoldBuffer[manifoldCount].contactPointCount] = worldVert;
									++manifoldBuffer[manifoldCount].contactPointCount;
								}
								*/
							}
							else if (currentPenetration < minPenetration)
							{
								minPenetration = currentPenetration;

								contactPointAvarage = worldVert;
								contactPointAvarageCount = 1;
								manifoldBuffer[manifoldCount].penetrationDepth = glm::abs( minPenetration);
								/*
								manifoldBuffer[manifoldCount].penetrationDepth = glm::abs(currentPenetration);
								manifoldBuffer[manifoldCount].contactPointCount = 1;
								manifoldBuffer[manifoldCount].contactPoints[0] = worldVert;
								*/
							}
						}
					}
				}

				if (areColliding)
				{
					//with this increment we basically save the contact manifold,
					// since we would be writing the next collision into another one
					manifoldBuffer[manifoldCount].contactPointCount = 1;
					manifoldBuffer[manifoldCount].contactPoints[0] = contactPointAvarage / (float)contactPointAvarageCount;
					


					++manifoldCount;
				}
			}
		}


		/*
		* Iterating through other entities it could collide with
		*/
		for (int j = 0;j < m_entityCount;++j)
		{
			int otherEntityId = m_entityIds[j];

			//quick check to only consider a pair once
			if (entityId < otherEntityId && manifoldCount<manifoldCapacity)
			{
				//Bounding sphere optimization check
				glm::vec3 distanceVec = transforms[entityId].position - transforms[otherEntityId].position;
				float distSqr = glm::dot(distanceVec,distanceVec);
				float radiusesSqr = (colliders[entityId].boundingRadius + colliders[otherEntityId].boundingRadius) *
									(colliders[entityId].boundingRadius + colliders[otherEntityId].boundingRadius);
				if (distSqr < radiusesSqr)
				{
					
					//Running GJK + potential EPA for contact information
					int localHullId1 = colliders[entityId].localHullId;
					int localHullId2 = colliders[otherEntityId].localHullId;
					 //TODO: collision

					ContactManifold contactManifold;
					contactManifold.entityId1 = entityId;
					contactManifold.entityId2 = otherEntityId;
						
					//We check for collision using GJK
					//If collision is found we use EPA to get the contact information
					bool areColliding = RigidbodyCollision(localHulls[localHullId1].Vertices, worldTransforms[entityId],
														   localHulls[localHullId2].Vertices, worldTransforms[otherEntityId],
														   contactManifold);

					if (areColliding)
					{
						/*
						std::cout << "Collision found between entities: " << contactManifold.entityId1 << " and " << contactManifold.entityId2 << std::endl;
						std::cout << "The penetration normal: (x: " << contactManifold.normal.x << " y: " << contactManifold.normal.y <<
							" z: " << contactManifold.normal.z << " ) penetration depth: " << contactManifold.penetrationDepth << std::endl;
						*/
						manifoldBuffer[manifoldCount] = contactManifold;
						++manifoldCount;
					}
				}

			}
		}
	}
}

void ECS::CollisionSolverSystem()
{
	for (int i = 0;i < manifoldCount;++i)
	{
		int entityId1 = manifoldBuffer[i].entityId1;
		int entityId2 = manifoldBuffer[i].entityId2;

		/*
		* In this part we solve for the collision manifold generated above
		* we  are only solving the entity-plane case
		*/
		if (entityId2 == -1)
		{
			//std::cout << "Contact Points found: " << contactManifold.contactPointCount << " \n";
				//this is where the avaraging happens
			



			//we only have one point here, due to stability, later we could expand to more points
			for (int pointIdx = 0;pointIdx < manifoldBuffer[i].contactPointCount;++pointIdx)
			{
				glm::vec3 contactPoint = manifoldBuffer[i].contactPoints[pointIdx];

				glm::vec3 toContact = contactPoint - transforms[entityId1].position;

				glm::vec3 angularContactVelocity = glm::cross(physics[entityId1].angularVelocity, toContact);


				//This vec3 tells us the entity's velocity towards the avaraged contact point
				glm::vec3 contactVelocity = physics[entityId1].velocity + angularContactVelocity;
				//Because we are checking against a plane, then relativeVelcotiy = contactVelocity

				//we calculate the relative velocity's length in the plane normal direction
				float normalVelocity = glm::dot(contactVelocity, manifoldBuffer[i].normal);

				glm::vec3 tangentVelocity = contactVelocity - manifoldBuffer[i].normal * normalVelocity;

				float tangentSpeed = glm::length(tangentVelocity);
				glm::vec3 tangentDir = glm::vec3(0.0f);
				if (tangentSpeed > 0.0001f)
				{
					tangentDir = glm::normalize(tangentVelocity);
				}
				/*
				* if normalvelocity was more then 0, then the entity would be moving away from the plane
				* in this case, it would be unstable to solve for any further collisions
				*/
				if (normalVelocity < 0)
				{
					//calculation of the impulse
					glm::vec3 r_cross_n = glm::cross(toContact, manifoldBuffer[i].normal);


					glm::mat3 R = glm::mat3_cast(transforms[entityId1].rotation);

					int localInertiaId = physics[entityId1].inertiaDataId;
					glm::mat3 inverseInertiaWorld = R * localInertiaDatas[localInertiaId].inverseInertiaTensor * glm::transpose(R);

					glm::vec3 inversInertia_rn = inverseInertiaWorld * r_cross_n;


					float angularTerm = glm::dot(manifoldBuffer[i].normal, glm::cross(inversInertia_rn, toContact));

					float denominator = physics[entityId1].invMass + angularTerm;

					float restitution = physics[entityId1].restitution;
					if (normalVelocity > -0.5f)
					{
						restitution = 0.0f;
					}



					float impulseMagnitude = (-(1.0f + restitution) * normalVelocity / denominator)
						/ manifoldBuffer[i].contactPointCount;

					glm::vec3 normalImpulse = manifoldBuffer[i].normal * impulseMagnitude * 0.90f;


					//friction
					float frictionDenominator = denominator;

					float tangentVelocityAlongTangentDir = glm::dot(contactVelocity, tangentDir);

					float frictionImpulseMagnitude = 0.0f;

					if (tangentSpeed < 0.1f)
					{
						frictionImpulseMagnitude = -tangentVelocityAlongTangentDir / physics[entityId1].invMass;
					}
					else
					{
						frictionImpulseMagnitude = -tangentVelocityAlongTangentDir / physics[entityId1].invMass * 0.5f;
					}

					float mu = physics[entityId1].friction;

					float maximumFrictionImpulseMagnitude = mu * glm::abs(impulseMagnitude * 0.5f);

					if (glm::abs(frictionImpulseMagnitude) > maximumFrictionImpulseMagnitude)
					{
						frictionImpulseMagnitude = glm::sign(frictionImpulseMagnitude) * maximumFrictionImpulseMagnitude;
					}

					glm::vec3 frictionImpulse = tangentDir * frictionImpulseMagnitude;

					glm::vec3 frictionTorque = inverseInertiaWorld * glm::cross(toContact, frictionImpulse);

					float angularVelocityMagnitude = glm::length(physics[entityId1].angularVelocity);

					float damping = (angularVelocityMagnitude < 0.1f) ? 0.1f : 0.5f;



					physics[entityId1].velocity += normalImpulse * physics[entityId1].invMass;
					physics[entityId1].angularVelocity += inverseInertiaWorld * glm::cross(toContact, normalImpulse);


					physics[entityId1].velocity += frictionImpulse * physics[entityId1].invMass;
					physics[entityId1].angularVelocity += frictionTorque * damping;


					angularVelocityMagnitude = glm::length(physics[entityId1].angularVelocity);
					if (angularVelocityMagnitude < 0.1f)
					{
						physics[entityId1].angularVelocity = glm::vec3(0.0f);
					}
					else
					{
						physics[entityId1].angularVelocity *= 0.98f;
					}



					//lastly, beumgarte poisitin correction

					//transforms[entityId].position += contactManifold.penetrationDepth * contactManifold.normal;
				}

				//position correction

				float correctionPercent = 0.2f;
				float penetrationSlop = 0.005f;

				float approvedPenetration = manifoldBuffer[i].penetrationDepth - penetrationSlop;
				if (approvedPenetration > 0.0f)
				{
					transforms[entityId1].position += correctionPercent * approvedPenetration * manifoldBuffer[i].normal;
				}

				//This Shouldn't be done here, and is very undereperformant, we only have it here for stability
				worldTransforms[entityId1] = glm::translate(glm::mat4(1.0f), transforms[entityId1].position) *
					glm::mat4_cast(transforms[entityId1].rotation) *
					glm::scale(glm::mat4(1.0f), glm::vec3(transforms[entityId1].scale, transforms[entityId1].scale, transforms[entityId1].scale));
			}
		}

		/*
		* In this part we solve collisions for entity-entity pairs
		* "This ought to be fukin hard"
		*/
		else
		{
			//std::cout << "Not yet defined" << std::endl;
			glm::vec3 normal = manifoldBuffer[i].normal;
			float penetrationDepth = manifoldBuffer[i].penetrationDepth;

			glm::vec3 toContactA = manifoldBuffer[i].contactPoints[0]-transforms[entityId1].position;
			glm::vec3 toContactB = manifoldBuffer[i].contactPoints[0]-transforms[entityId2].position;

			glm::vec3 angularVelocityComponentA = glm::cross(physics[entityId1].angularVelocity, toContactA);
			glm::vec3 angularVelocityComponentB = glm::cross(physics[entityId2].angularVelocity, toContactB);

			glm::vec3 relativeVelocity = physics[entityId1].velocity + angularVelocityComponentA -
										(physics[entityId2].velocity + angularVelocityComponentB);

			float relativeVelocityMagnitude = glm::dot(relativeVelocity, normal);
			if (relativeVelocityMagnitude > 0.0f)
			{
				glm::vec3 toContactAxN = glm::cross(toContactA, normal);
				glm::vec3 toContactBxN = glm::cross(toContactB, normal);

				glm::mat3 rotationA = glm::mat3_cast(transforms[entityId1].rotation);
				glm::mat3 rotationB = glm::mat3_cast(transforms[entityId2].rotation);

				int localInertiaIdA = physics[entityId1].inertiaDataId;
				int localInertiaIdB = physics[entityId2].inertiaDataId;

				glm::mat3 inverseInertiaWorldA = rotationA * 
												localInertiaDatas[localInertiaIdA].inverseInertiaTensor *
												glm::transpose(rotationA);

				glm::mat3 inverseInertiaWorldB = rotationB *
												 localInertiaDatas[localInertiaIdB].inverseInertiaTensor *
												 glm::transpose(rotationB);

				glm::vec3 toContactInertiaA = glm::cross(inverseInertiaWorldA * toContactAxN, toContactA);
				glm::vec3 toContactInertiaB = glm::cross(inverseInertiaWorldB * toContactBxN, toContactB);

				float invMassSum = physics[entityId1].invMass + physics[entityId2].invMass +
								   glm::dot(normal, toContactInertiaA + toContactInertiaB);
				float mutualRestitution = fminf(physics[entityId1].restitution, physics[entityId2].restitution);

				float impulseMagnitude = -(1.0f + mutualRestitution) * relativeVelocityMagnitude / invMassSum;

				glm::vec3 normalImpulse = impulseMagnitude * normal;

				physics[entityId1].velocity += normalImpulse * physics[entityId1].invMass;
				physics[entityId2].velocity -= normalImpulse * physics[entityId2].invMass;

				physics[entityId1].angularVelocity += inverseInertiaWorldA * glm::cross(toContactA, normalImpulse);
				physics[entityId2].angularVelocity -= inverseInertiaWorldB * glm::cross(toContactB, normalImpulse);




			}


			transforms[entityId1].position += normal * penetrationDepth * -0.5f;
			transforms[entityId2].position += normal * penetrationDepth * 0.5f;

			worldTransforms[entityId1] = glm::translate(glm::mat4(1.0f), transforms[entityId1].position) *
				glm::mat4_cast(transforms[entityId1].rotation) *
				glm::scale(glm::mat4(1.0f), glm::vec3(transforms[entityId1].scale, transforms[entityId1].scale, transforms[entityId1].scale));

			worldTransforms[entityId2] = glm::translate(glm::mat4(1.0f), transforms[entityId2].position) *
				glm::mat4_cast(transforms[entityId2].rotation) *
				glm::scale(glm::mat4(1.0f), glm::vec3(transforms[entityId2].scale, transforms[entityId2].scale, transforms[entityId2].scale));
		}
	}
}

void ECS::TransformSystem()
{
	for (int i = 0;i < m_entityCount;++i)
	{
		int entityId = m_entityIds[i];

		worldTransforms[entityId] = glm::translate(glm::mat4(1.0f), transforms[entityId].position) *
			glm::mat4_cast(transforms[entityId].rotation) *
			glm::scale(glm::mat4(1.0f), glm::vec3(transforms[entityId].scale, transforms[entityId].scale, transforms[entityId].scale));
	}
}

void ECS::SummationSystem(int& diceRollSum)
{
	diceRollSum = 0;

	glm::vec3 worldUp = glm::vec3(0, 1, 0);

	for (int i = 0;i < m_entityCount;++i)
	{
		int entityId = m_entityIds[i];

		int localDirectionId = localDirectionIds[entityId];

		glm::mat3 entityRotation = glm::mat3_cast(transforms[entityId].rotation);

		int numberPointingUp;

		float maxSameDirection = -1.0f;
		int maxIndex = 0;

		for (int j = 0;j < localDirections[localDirectionId].directions.size();++j)
		{
			glm::vec3 worldDirection = entityRotation * localDirections[localDirectionId].directions[j];

			float currentSameDirection = glm::dot(worldDirection, worldUp);

			if (currentSameDirection > maxSameDirection)
			{
				maxSameDirection = currentSameDirection;

				maxIndex = j;
			}
		}

		if (renders[entityId].meshId == 4)
		{
			numberPointingUp = maxIndex;
		}
		else if (renders[entityId].meshId == 5)
		{
			numberPointingUp = maxIndex * 10;
		}
		else
		{
			numberPointingUp = maxIndex + 1;
		}

		diceRollSum += numberPointingUp;
	}
}

void ECS::RenderSystem(const Shader& shader)
{
	

	for (int i = 0;i < m_entityCount;++i)
	{
		int entityId = m_entityIds[i];

		shader.SetUniform<glm::mat4>("uWorldTransform", worldTransforms[entityId]);
		
		shader.SetUniform<glm::mat4>("uInverseTransposedWorldTransform", (worldTransforms[entityId]));

		int meshId = renders[entityId].meshId;
		meshes[meshId].Draw();
	}
}

