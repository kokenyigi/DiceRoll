#include "Collision.h"

#include <iostream>
#include <stack>

struct SupportPoint
{
	glm::vec3 point;
	int indA;
	int indB;
};

static glm::vec3 FurthestPointInDirection(const std::vector<glm::vec3>& localVertices, const glm::mat4& worldTransform,
											const glm::vec3& dir,int& index)
{
	glm::vec3 maxPoint  = glm::vec3(worldTransform * glm::vec4(localVertices[0],1));
	float maxValue = glm::dot(maxPoint, dir);
	index = 0;

	for (int i = 1;i < localVertices.size();++i)
	{
		glm::vec3 currentPoint = glm::vec3(worldTransform * glm::vec4(localVertices[i], 1));
		float currentValue = glm::dot(currentPoint, dir);

		if (currentValue > maxValue)
		{
			maxValue = currentValue;
			maxPoint = currentPoint;
			index = i;
		}
	}

	return maxPoint;
}

static SupportPoint GetSupportPoint(const std::vector<glm::vec3>& localVertices1, const glm::mat4& worldTransform1,
							  const std::vector<glm::vec3>& localVertices2, const glm::mat4& worldTransform2,
							  const glm::vec3& direction)
{
	int indexA;
	int indexB;

	glm::vec3 furthestA = FurthestPointInDirection(localVertices1, worldTransform1, direction, indexA);
	glm::vec3 furthestB = FurthestPointInDirection(localVertices2, worldTransform2, -direction,indexB);
	return { furthestA - furthestB,indexA,indexB };
}

static bool IsSameDirection(const glm::vec3& firstDirection, const glm::vec3& otherDirection)
{
	return glm::dot(firstDirection, otherDirection) > 0;
}

static bool LineCase(SupportPoint simplexPoints[4], int& simplexPointCount, glm::vec3& direction)
{
	SupportPoint a = simplexPoints[1];
	SupportPoint b = simplexPoints[0];

	glm::vec3 ab = b.point - a.point;
	glm::vec3 ao = -a.point;

	if (IsSameDirection(ab, ao))
	{
		direction = glm::cross(glm::cross(ab, ao), ab);
	}
	else
	{
		simplexPoints[0] = a;
		simplexPointCount = 1;
		direction = ao;
	}
	

	return false;
}

static bool TriangleCase(SupportPoint simplexPoints[4], int& simplexPointCount, glm::vec3& direction)
{
	SupportPoint a = simplexPoints[2];
	SupportPoint b = simplexPoints[1];
	SupportPoint c = simplexPoints[0];

	glm::vec3 ab = b.point - a.point;
	glm::vec3 ac = c.point - a.point;
	glm::vec3 ao = -a.point;

	glm::vec3 abc = glm::cross(ab, ac);

	if (IsSameDirection(glm::cross(abc, ac), ao))
	{
		//Here we know, that the origin lies outside, on the AC face
		//B will be discarded anyhow
		if (IsSameDirection(ao, ac))
		{
			//Here, we will keep C also, as a support point since this way the closest feature remaines
			simplexPoints[1] = a;
			simplexPointCount = 2;

			direction = glm::cross(glm::cross(ac, ao), ac);
		}
		else
		{
			//Here we will discard both B and C, since none of them provide to the closest feature
			//We will use a line-case for simplicity reasons
			simplexPoints[1] = a;
			simplexPoints[0] = b;
			simplexPointCount = 2;
			return LineCase(simplexPoints, simplexPointCount, direction);
		}
	}
	else
	{
		if (IsSameDirection(glm::cross(ab, abc), ao))
		{
			//Here, we know that the origin is outside, on the AB face
			//we will use a line-case to determine if B is a good support point or not
			//C point will be discarded here nonetheless
			simplexPoints[1] = a;
			simplexPoints[0] = b;
			simplexPointCount = 2;
			return LineCase(simplexPoints, simplexPointCount, direction);
		}
		else
		{
			//if we got here, that means the origin must lie "inside" our triangle-shaped bar of space
			//we still ought to check if the origin is above or below our triangular face
			if (IsSameDirection(abc, ao))
			{
				//origin is above the triangle
				direction = abc;
			}
			else
			{
				//origin is below the triangle
				//we have to shuffle our support points a bit, since our tetrahedron case works universally this way
				//and by shuffling we obey the rules of our right-handed coordinate system
				simplexPoints[0] = b;
				simplexPoints[1] = c;
				
				//a remains
				direction = -abc;
			}
		}
	}

	return false;
}

static bool TetrahedronCase(SupportPoint simplexPoints[4], int& simplexPointCount, glm::vec3& direction)
{
	SupportPoint a = simplexPoints[3];
	SupportPoint b = simplexPoints[2];
	SupportPoint c = simplexPoints[1];
	SupportPoint d = simplexPoints[0];

	glm::vec3 ab = b.point - a.point;
	glm::vec3 ac = c.point - a.point;
	glm::vec3 ad = d.point - a.point;
	glm::vec3 ao = -a.point;

	glm::vec3 abc = glm::cross(ab, ac);
	glm::vec3 acd = glm::cross(ac, ad);
	glm::vec3 adb = glm::cross(ad, ab);

	//each case we check if the origin is outside towards any triangular face of the tetrahedron
	//if its outside on a face, we discard the furthest triangle and then assign the new points properly
	if (IsSameDirection(abc, ao))
	{
		simplexPoints[0] = c;
		simplexPoints[1] = b;
		simplexPoints[2] = a;
		simplexPointCount = 3;
		return TriangleCase(simplexPoints, simplexPointCount, direction);
	}

	if (IsSameDirection(acd, ao))
	{
		simplexPoints[0] = d;
		simplexPoints[1] = c;
		simplexPoints[2] = a;
		simplexPointCount = 3;
		return TriangleCase(simplexPoints, simplexPointCount, direction);
	}
	if (IsSameDirection(adb, ao))
	{
		simplexPoints[0] = b;
		simplexPoints[1] = d;
		simplexPoints[2] = a;
		simplexPointCount = 3;
		return TriangleCase(simplexPoints, simplexPointCount, direction);
	}

	return true;
}



static bool ProcessSimplex(SupportPoint simplexPoints[4],int& simplexPointCount, glm::vec3& direction)
{
	switch (simplexPointCount)
	{
		case 2:
			return LineCase(simplexPoints, simplexPointCount, direction);
			break;
		case 3:
			return TriangleCase(simplexPoints, simplexPointCount, direction);
			break;
		case 4:
			return TetrahedronCase(simplexPoints, simplexPointCount, direction);
			break;
	}

	//This shall never be reached
	std::cout << "Simplex has weird point count: " << simplexPointCount << std::endl;
	return false;
}

/*
* My implementation of the famous Gilbert-Jonathan-Keerthi algorithm
* 
*/
static bool GJK(const std::vector<glm::vec3>& localVertices1, const glm::mat4& worldTransform1,
		 const std::vector<glm::vec3>& localVertices2, const glm::mat4& worldTransform2,
		 SupportPoint simplex[4])
{
	//glm::vec3 simplexPoints[4];
	int simplexPointCount = 0;

	SupportPoint support = GetSupportPoint(localVertices1, worldTransform1, localVertices2, worldTransform2, glm::vec3(1, 0, 0));

	simplex[0] = support;
	++simplexPointCount;

	glm::vec3 direction = - support.point;

	int maximumIteration = 50;
	int currentIterationCount = 0;

	while (currentIterationCount < maximumIteration)
	{
		support = GetSupportPoint(localVertices1, worldTransform1, localVertices2, worldTransform2, direction);

		if (glm::dot(support.point, direction) < 0)
		{
			return false;
		}

		simplex[simplexPointCount] = support;
		++simplexPointCount;

		bool isSimplexContainingOrigin = ProcessSimplex(simplex,simplexPointCount,direction);
		if (isSimplexContainingOrigin)
		{
			return true;
		}

		++currentIterationCount;
	}

	return false;
}









//Data structures for Expanding Polytope Algorithm
struct TriangleFace
{
	int vertices[3]; //indices to vertices inside the polytope's vertex array
	int neighbors[3]; //indices to neighboring faces, since face is a triangular face, it can have 3 neighbors at max
	glm::vec3 normal;
	float distance;

	bool isMarked;
	bool isAlive;
};

struct HorizonEdge
{
	int invisibleFaceInd;
	int invisibleFaceMissingNeighborInd;
	int vertices[2];
};

static void CreatePolytopeFromSimplex(SupportPoint simplex[4],std::vector<SupportPoint>& vertices, std::vector<TriangleFace>& faces)
{
	SupportPoint a = simplex[3];
	SupportPoint b = simplex[2];
	SupportPoint c = simplex[1];
	SupportPoint d = simplex[0];

	vertices.push_back(d);
	vertices.push_back(c);
	vertices.push_back(b);
	vertices.push_back(a);

	glm::vec3 abcN = glm::normalize(glm::cross(b.point - a.point, c.point - a.point));
	glm::vec3 acdN = glm::normalize(glm::cross(c.point - a.point, d.point - a.point));
	glm::vec3 adbN = glm::normalize(glm::cross(d.point - a.point, b.point - a.point));
	glm::vec3 bdcN = glm::normalize(glm::cross(d.point - b.point, c.point - b.point));


	//The simplex contains the origin because of GJK
	//Therefore the origin is an inner point of the tetrahedron
	float distance0 = glm::dot(a.point, abcN);
	//std::cout << "Distance0 = " << distance0 << std::endl;

	float distance1 = glm::dot(a.point, acdN);
	//std::cout << "Distance1 = " << distance1 << std::endl;

	float distance2 = glm::dot(a.point, adbN);
	//std::cout << "Distance2 = " << distance2 << std::endl;

	float distance3 = glm::dot(b.point, bdcN);
	//std::cout << "Distance3 = " << distance3 << std::endl;


	//Very importantus assumptionus!
	//order matters! we have to enlist the neighbors in the same order as we did the vertices
	//we assume an order when we are expanding the polytope later on!
	faces.push_back({{ 3,2,1 }, { 2,3,1 }, abcN,distance0 , false,true});
	faces.push_back({ { 3,1,0 }, { 0,3,2 }, acdN,distance1, false,true });
	faces.push_back({ { 3,0,2 }, { 1,3,0 }, adbN,distance2, false,true });
	faces.push_back({ { 2,0,1 }, { 2,1,0 }, bdcN,distance3, false,true });

	/*
	std::cout << "Distance0 = " << distance0 << std::endl;
	std::cout << "Distance1 = " << distance1 << std::endl;
	std::cout << "Distance2 = " << distance2 << std::endl;
	std::cout << "Distance3 = " << distance3 << std::endl;
	*/
}


static void GetMinFaceData(const std::vector<TriangleFace>& faces, glm::vec3& minNormal, float& minDistance, int& minIndex)
{
	
	minDistance = FLT_MAX;
	minIndex = 0;

	for (int i = 0;i < faces.size();++i)
	{
		if (faces[i].isAlive == true)
		{
			//std::cout << "Current Face Distance = " << faces[i].distance << std::endl;
			if (faces[i].distance < minDistance)
			{
				minDistance = faces[i].distance;
				minNormal = faces[i].normal;
				minIndex = i;
			}
		}
	}
}

static bool IsFaceVisible(const TriangleFace& face,const std::vector<SupportPoint>& vertices,  const SupportPoint& sourcePoint)
{
	return (glm::dot(face.normal, sourcePoint.point-vertices[face.vertices[0]].point) > 0.000001f);
}

static void DebugVector(const glm::vec3& vec,const char* captcha)
{
	std::cout << captcha << " : (x: " << vec.x << " y: " << vec.y << " z: " << vec.z << " )\n";
}

static void DebugFace(const TriangleFace& face)
{
	std::cout << "TriangularFace Entry:\n";
	DebugVector(face.normal, "	Face normal");
	std::cout << "	Face distance: " << face.distance << std::endl;
	std::cout << "	Face vertices: \n";
	for (int i = 0;i < 3;++i)
	{
		std::cout <<"		" << i << ".th vertex: " << face.vertices[i] << std::endl;
	}
	std::cout << "	Face neighbours: \n";
	for (int i = 0;i < 3;++i)
	{
		std::cout << "		" << i << ".th neighbour: " << face.neighbors[i] << std::endl;
	}
}



static void DebugPolytope(const std::vector<glm::vec3>& vertices, const std::vector<TriangleFace>& faces)
{
	std::cout << "------------------Polytope Entry: ---------------------\n";
	std::cout << "Vertices: \n";
	for (int i = 0;i < vertices.size();++i)
	{
		DebugVector(vertices[i], "	Vertex: ");
	}
	std::cout << "Faces:\n";
	for (int i = 0;i < faces.size();++i)
	{
		if (faces[i].isAlive)
		{
			DebugFace(faces[i]);
		}
	}
}

static void ExpandPolytope(std::vector<SupportPoint>& vertices, std::vector<TriangleFace>& faces, 
						   int minfaceIndex, SupportPoint& supportPoint)
{
	//DebugPolytope(vertices, faces);
	//Before DFS runs we mark all faces unmarked
	for (int i = 0;i < faces.size();++i)
	{
		if (faces[i].isAlive)
		{
			faces[i].isMarked = false;
		}
	}

	std::vector<HorizonEdge> horizonEdges;

	std::stack<int> faceStack;
	faceStack.push(minfaceIndex);
	faces[minfaceIndex].isMarked = true;

	int visibleFaceCount = 0;
	//Flood fill-like DFS for the faces
	while (!faceStack.empty())
	{
		int currentFaceIndex = faceStack.top();
		faceStack.pop();

		if (IsFaceVisible(faces[currentFaceIndex],vertices, supportPoint))
		{
			++visibleFaceCount;
			//Here we know, that during the flood fill we have found an alive, visible face
			//Due to the reasons above, we have to expand in DFS all of its alive children
			//this way, the DFS goes through not just visible faces, but also horizon faces
			faces[currentFaceIndex].isAlive = false;

			for (int i = 0;i < 3;++i)
			{
				int subFaceIndex = faces[currentFaceIndex].neighbors[i];
				if (!faces[subFaceIndex].isMarked)
				{
					faces[subFaceIndex].isMarked = true;
					faceStack.push(subFaceIndex);
					
				}
			}
		}
		else
		{
			//Here we know, that during the flood fill, we have encounterred with a horizon face
			//this is because, this face is invisible, but we can only get to invisible faces from visible ones
			//Therefore, we have to loop through this invisFace's neighbours, in search for visible faces, because then we can add
			//HORIZON EDGES
			for (int i = 0;i < 3;++i)
			{
				int subFaceIndex = faces[currentFaceIndex].neighbors[i];

				if (IsFaceVisible(faces[subFaceIndex],vertices, supportPoint))
				{
					//Here, we have found a invis-visible connection, therefore a horizon edge
					int vert1 = faces[currentFaceIndex].vertices[i];
					int vert2 = faces[currentFaceIndex].vertices[(i + 1) % 3];

					horizonEdges.push_back({ currentFaceIndex,i,{vert1,vert2} });
				}
				else
				{
					//This case is never handled, because this is a connection between
					//invis- invis
				}
			}

		}

		

		
	}
	//std::cout << "Visible face count = " << visibleFaceCount << std::endl;
	//std::cout << "Horizon edge count = " << horizonEdges.size() << std::endl;
	//During the flood fill we killed many-many faces,
	//Now, we have to reconstruct our polytope, in a convex manner
	//We will utilize horizonedges to do this, they look like this:
	//vert1-vert2 and the faces[0] is the face where the flood fill stopped, since its invisible
	/*
	std::cout << "Horizon Edges: \n";
	for (int i = 0;i < horizonEdges.size();++i)
	{
		std::cout << "	v0: " << horizonEdges[i].vertices[0] << " v1: " << horizonEdges[i].vertices[1] << std::endl;
	}
	*/
	
	//Important! At this point we don't know the order the horizon edges come after one an other
	//For the reason above, we have to make an ordered edge ring, or else
	
	//std::cout << startToEdgesMap.size() << std::endl;
	//this container has the original horizonedge indices, in the order they come after one an other
	std::vector<int> edgeOrderIndices;
	edgeOrderIndices.reserve(horizonEdges.size());
	edgeOrderIndices.push_back(0);

	while (edgeOrderIndices.size() < horizonEdges.size())
	{
		int lastInsertedEdgeInd = edgeOrderIndices[edgeOrderIndices.size() - 1];
		int lastVertexInd = horizonEdges[lastInsertedEdgeInd].vertices[1];

		bool found = false;
		for (int i = 0;i < horizonEdges.size() && !found;++i)
		{
			if (i != lastInsertedEdgeInd)
			{
				int vert0 = horizonEdges[i].vertices[0];
				int vert1 = horizonEdges[i].vertices[1];

				int debug0 = horizonEdges[lastInsertedEdgeInd].vertices[0];
				int debug1 = horizonEdges[lastInsertedEdgeInd].vertices[1];

				



				if (vert0 == lastVertexInd)
				{
					found = true;
					edgeOrderIndices.push_back(i);
					
					
				}
				else if (vert1 == lastVertexInd)
				{
					found = true;
					edgeOrderIndices.push_back(i);
					horizonEdges[i].vertices[0] = vert1;
					horizonEdges[i].vertices[1] = vert0;
				}
			}
		}
	}

	if (horizonEdges.size() != edgeOrderIndices.size())
	{
		for (int i = 0;i < horizonEdges.size();++i)
		{
			std::cout << horizonEdges[i].vertices[0] << " , " << horizonEdges[i].vertices[1] << std::endl;
		}
	}
	
	assert(horizonEdges.size() == edgeOrderIndices.size());
	int oldFaceCount = faces.size();

	//DebugPolytope(vertices, faces);
	//std::cout << "EdgeOrder: \n";
	for (int i = 0;i < edgeOrderIndices.size();++i)
	{
		
		int currentEdgeInd = edgeOrderIndices[i];

		int vert1 = horizonEdges[currentEdgeInd].vertices[0];
		int vert2 = horizonEdges[currentEdgeInd].vertices[1];

		//std::cout << "	v0: " << vert1 << 
		//	" v1: " << vert2 << std::endl;


		int invisFaceInd = horizonEdges[currentEdgeInd].invisibleFaceInd;
		int invisFaceMissingNeighborInd = horizonEdges[currentEdgeInd].invisibleFaceMissingNeighborInd;

		glm::vec3 faceNormal =glm::normalize(glm::cross(supportPoint.point-vertices[vert1].point,supportPoint.point-vertices[vert2].point));
		

		//Quick flip of the normal and signedDistance in the case the normal is messed up
		
		if (glm::dot(vertices[vert1].point, faceNormal) <0)
		{
			faceNormal *= -1;
		}
		float faceDistance = glm::dot(vertices[vert1].point, faceNormal);

		int newPointInd = vertices.size();

		int precedeInd = (i-1) < 0 ? (i-1) + edgeOrderIndices.size() : (i-1);
		int nextInd = (i + 1) % edgeOrderIndices.size();

		int precedeFaceNeighbor = oldFaceCount + precedeInd;
		int nextFaceNeighbor = oldFaceCount + nextInd;

		//Order of vertices matters!
		//And so does neighboring face indices!
		faces.push_back({ {vert2,vert1,newPointInd},{invisFaceInd, precedeFaceNeighbor, nextFaceNeighbor },faceNormal,faceDistance,false,true });
		//std::cout << "new face distance: " << faceDistance << std::endl;
		//We also have to fix the invisFace's neighbor
		faces[invisFaceInd].neighbors[invisFaceMissingNeighborInd] = oldFaceCount + i;
	}

	//finally, just for some simplicity's sake, we only pushback the new point here
	vertices.push_back(supportPoint);

	//DebugPolytope(vertices, faces);

}

//parameters: technically normal isnt needed but watevah
static glm::vec3 Barycentric(const glm::vec3& p, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,const glm::vec3 normal)
{
	float tABC = glm::dot(normal, glm::cross(v1 - v0, v2 - v0));
	float tABP = glm::dot(normal, glm::cross(v1 - v0, p - v0));
	float tAPC = glm::dot(normal, glm::cross(p - v0, v2 - v0));

	glm::vec3 weights;
	weights.z = tABP / tABC;
	weights.y = tAPC / tABC;
	weights.x = 1 - weights.y - weights.z;

	return weights;
}




static void EPA(SupportPoint simplex[4],
				const std::vector<glm::vec3>& localVertices1, const glm::mat4& worldTransform1,
				const std::vector<glm::vec3>& localVertices2, const glm::mat4& worldTransform2,
				ContactManifold& contactManifold) // needs extra parameters
{
	//std::cout << "Epa starts\n";
	//Takes the simplex made by GJK, and constructs a polytope adjecency structure
	//Consists of faces & vertices
	std::vector<SupportPoint> vertices;
	std::vector<TriangleFace> faces;

	vertices.reserve(50);
	faces.reserve(50);

	CreatePolytopeFromSimplex(simplex, vertices, faces);
	

	glm::vec3 minFaceNormal;
	float minFaceDistance;
	int minFaceIndex;

	int maxIterations = 50;
	int currentIterationCount = 0;
	while (currentIterationCount < maxIterations)
	{
		GetMinFaceData(faces, minFaceNormal, minFaceDistance,minFaceIndex);

		SupportPoint support = GetSupportPoint(localVertices1, worldTransform1, localVertices2, worldTransform2, minFaceNormal);
		float supportDistance = glm::dot(support.point, minFaceNormal);

		if (fabsf(supportDistance - minFaceDistance) < 0.001f)
		{
			//We succeeded in finding the closest minkowski difference face to the origin,
			//this face's normal will be the contact normal
			//and the penetration depth will be the distance of this plane from the origin
			contactManifold.normal = minFaceNormal;
			contactManifold.penetrationDepth = minFaceDistance;

			glm::vec3 point = minFaceNormal * minFaceDistance;
			glm::vec3 weights = Barycentric(point, vertices[faces[minFaceIndex].vertices[0]].point,
				vertices[faces[minFaceIndex].vertices[1]].point,
				vertices[faces[minFaceIndex].vertices[2]].point,minFaceNormal);

			//std::cout << "Weights: u: " << weights.x << " v: " << weights.y << " w: " << weights.z << std::endl;

			glm::vec3 localPointA0 = localVertices1[vertices[faces[minFaceIndex].vertices[0]].indA];			
			glm::vec3 localPointA1 = localVertices1[vertices[faces[minFaceIndex].vertices[1]].indA];
			glm::vec3 localPointA2 = localVertices1[vertices[faces[minFaceIndex].vertices[2]].indA];
			glm::vec3 localPointB0 = localVertices2[vertices[faces[minFaceIndex].vertices[0]].indB];
			glm::vec3 localPointB1 = localVertices2[vertices[faces[minFaceIndex].vertices[1]].indB];
			glm::vec3 localPointB2 = localVertices2[vertices[faces[minFaceIndex].vertices[2]].indB];

			glm::vec3 worldPointA0 = glm::vec3(worldTransform1 * glm::vec4(localPointA0, 1));
			glm::vec3 worldPointA1 = glm::vec3(worldTransform1 * glm::vec4(localPointA1, 1));
			glm::vec3 worldPointA2 = glm::vec3(worldTransform1 * glm::vec4(localPointA2, 1));
			glm::vec3 worldPointB0 = glm::vec3(worldTransform2 * glm::vec4(localPointB0, 1));
			glm::vec3 worldPointB1 = glm::vec3(worldTransform2 * glm::vec4(localPointB1, 1));
			glm::vec3 worldPointB2 = glm::vec3(worldTransform2 * glm::vec4(localPointB2, 1));

			glm::vec3 contactA = weights.x * worldPointA0 + weights.y * worldPointA1 + weights.z * worldPointA2;
			glm::vec3 contactB = weights.x * worldPointB0 + weights.y * worldPointB1 + weights.z * worldPointB2;

			glm::vec3 contactPoint = (contactA + contactB) * 0.5f;

			contactManifold.contactPointCount = 1;
			contactManifold.contactPoints[0] = contactPoint;
			//std::cout << "Epa finishes\n";
			return;
		}
		else
		{
			//This is the usual case, where we havent found the closest property yet
			//Here, we will have to expand our polytope, by deleting the minFace and all faces visible from support
			//We will construct new faces, all while keeping the polytope konvex
			//std::cout << "Before Expansion still running...\n";
			
			ExpandPolytope(vertices, faces, minFaceIndex, support);

			//std::cout << "After Expansion still running...\n";
		}

		++currentIterationCount;
	}

	return;
}


bool RigidbodyCollision(const std::vector<glm::vec3>& localVertices1, const glm::mat4& worldTransform1,
						const std::vector<glm::vec3>& localVertices2, const glm::mat4& worldTransform2,
						ContactManifold& contactManifold)
{
	SupportPoint initialSimplex[4];
	//std::cout << "Gilbert-Jonathan-Keerthi algorithm starts...\n";
	bool areColliding = GJK(localVertices1, worldTransform1, localVertices2, worldTransform2, initialSimplex);
	//std::cout << "GJK finishes\n";
	if (areColliding)
	{
		//std::cout << "Expanding Polytope algorithm start...\n";
		EPA(initialSimplex, localVertices1, worldTransform1, localVertices2, worldTransform2, contactManifold);
		//std::cout << "EPA finishes\n";
	}
	
	return areColliding;
	
}
