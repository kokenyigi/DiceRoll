#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>
#include <string>

struct Plane
{
	float offset;
	glm::vec3 normal;
};


/**
* 
*/
struct Rectangle
{
	float x;
	float y;
	float width;
	float height;
};

//This functions takes in vec2 raw screen coordinates,
//And spits out the correct NDC coordinates
static glm::vec2 ScreenToNDC(const glm::vec2& rawScreenCoords, int windowWidth, int windowHeight)
{
	float normalizedDeviceX = (rawScreenCoords.x / (float)windowWidth) * 2 - 1;
	float normalizedDeviceY = ((rawScreenCoords.y / (float)windowHeight) * 2 - 1) * -1;

	return glm::vec2(normalizedDeviceX, normalizedDeviceY);
}

static glm::vec2 NDCToScreen(const glm::vec2& normalizedDeviceCoords, int windowWidth, int windowHeight)
{
	float screenX = (normalizedDeviceCoords.x + 1.0f) / 2.0f * windowWidth;
	float screenY = (-normalizedDeviceCoords.y + 1.0f) / 2.0f * windowHeight;

	return glm::vec2(screenX, screenY);
}

//Has numerical stability(division by zero avoidance)
//Calculates u interpolated: p1*(1-u) + p2 * u
static float HorizontalRayToLineIntersect(const glm::vec2& rayOrigin, const glm::vec2& p1, const glm::vec2& p2)
{
	//we know our ray's direction is: vec2(1,0)

	//If this happens we know that the line segment is almost paralell to our ray
	//Meaning no collision, therefore we return -1
	if (fabsf(p1.y - p2.y) < 0.0000001f)
	{
		return -1;
	}

	float u = (rayOrigin.y - p1.y) / (p2.y - p1.y);
	if (u >= 0 && u <= 1)
	{
		float t = p1.x - rayOrigin.x + u * (p2.x - p1.x);
		return t;
	}
	
	return -1;
	
}

//Returns 0<=t<=1 if ray intersects the line segment between p1 and p2
//Yet to be written
static float RayLineIntersect(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, const glm::vec2& p1, const glm::vec2& p2)
{
	return -1.0f;
}

//Turns a positive (0<= integer) into string
static std::string IntegerToString(int integer)
{
	if (integer < 0)
	{
		return "";
	}
	if (integer == 0)
	{
		return "0";
	}
	char zeroAscii = '0';
	std::string retval = "";
	int number = integer;
	while (number > 0)
	{
		int currentNumber = number % 10;
		number /= 10;

		retval = (char)(zeroAscii + currentNumber) + retval;
	}

	


	return retval;
}

//Return -1 if the string is invalid
static int StringToInteger(const std::string& string)
{
	if (string.size() < 1)
	{
		return -1;
	}
	if (string.size() == 1 && string[0] == '0')
	{
		return 0;
	}

	if (string[0] == '0')
	{
		//cant start with 0
		return -1;
	}
	
	char zeroAscii = '0';
	int retval = 0;
	for (int i = 0;i < string.size();++i)
	{
		if (string[i] > '9' || string[i] < '0')
		{
			return -1;
		}
		retval = retval * 10 + (int)(string[i] - zeroAscii);
	}
		
	return retval;

	
}

#endif
