#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <fstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // transzformációkhoz
#include <glm/gtc/type_ptr.hpp>   

#include <vector>

class Camera
{

public:

private:
	float m_yaw = -90.f;
	float m_pitch = 0.0f;
	float m_roll = 0.0f;

	float m_fovy = 45.0f;

	float sensitivity = 0.1f;

	float velocity = 2.5f;
	float speedyVelocity = 10.0f;

	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 m_world_up;

	glm::vec3 u;
	glm::vec3 v;
	glm::vec3 w;

public:
	Camera();
	~Camera();

	void SetYaw(float yaw);
	float GetYaw() const { return m_yaw; }
	void SetPitch(float pitch);
	float GetPitch() const { return m_pitch; }
	void SetRoll(float roll);
	float GetRoll() const { return m_roll; }

	void SetFovy(float fovy);
	float GetFovy() const { return m_fovy; }

	glm::vec3 GetPosition();

	void Move(float deltatime, glm::vec3 directions,bool isSpeedy);
	void Rotate(float dx, float dy);

	glm::mat4 GetViewMatrix() const { return glm::lookAt(position, position + front, m_world_up); }
	glm::mat4 GetPerspectiveMatrix(float aspect, float near, float far) const { return glm::perspective(glm::radians(m_fovy), aspect, near, far); }

private:
	void CalculateUVW();
	void CalculateFront();
};

#endif