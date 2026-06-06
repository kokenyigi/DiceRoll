#include "Camera.h"
#include <iostream>

Camera::Camera()
{
	//std::cout << "Camera" << std::endl;
	position = glm::vec3(0.0f, 0.0f, 20);
	front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_world_up = glm::vec3(0.0f, 1.0f, 0.0f);

	CalculateUVW();
}

void Camera::Move(float deltatime, glm::vec3 directions,bool isSpeedy)
{
	float deltaspeed = velocity * deltatime;
	if (isSpeedy)
	{
		deltaspeed = speedyVelocity * deltatime;
	}

	

	position += u * deltaspeed * directions.x;
	position += v * deltaspeed * directions.y;
	position += w * deltaspeed * directions.z;

	//No extra calculation needed
}

void Camera::Rotate(float dx, float dy)
{
	SetYaw(m_yaw + dx * sensitivity);
	SetPitch(m_pitch + dy * sensitivity);

	CalculateFront();//Caluclates uvw too
}

Camera::~Camera()
{
	//implement
}

void Camera::SetFovy(float fovy)
{
	if (fovy < 1.0f)
	{
		m_fovy = 1.0f;
	}
	else if (fovy > 45.0f)
	{
		m_fovy = 45.0f;
	}
	else
	{
		m_fovy = fovy;
	}
}

void Camera::SetYaw(float yaw)
{
	m_yaw = yaw;
}

glm::vec3 Camera::GetPosition()
{
	return this->position;
}

void Camera::SetPitch(float pitch)
{
	if (pitch >= 89.5f)
	{
		m_pitch = 89.5f;
	}
	else if (pitch <= -89.5f)
	{
		m_pitch = -89.5f;
	}
	else
	{
		m_pitch = pitch;
	}
}

void Camera::SetRoll(float roll)
{
	m_roll = roll;
}

void Camera::CalculateUVW()
{
	w = front;
	v = glm::normalize(glm::cross(w, m_world_up));
	u = glm::cross(w, v);
}

void Camera::CalculateFront()
{
	glm::vec3 new_front;
	new_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	new_front.y = sin(glm::radians(m_pitch));
	new_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

	front = glm::normalize(new_front);

	CalculateUVW();
}