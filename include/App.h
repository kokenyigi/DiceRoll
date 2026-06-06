#pragma once
#ifndef APP_H
#define APP_H

#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include <iostream>

#include "Utils.h"

#include "Shader.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Camera.h"
#include "Texture.h"

#include "ECS.h"

#include "GUI.h"
#include "Controls.h"

class App
{
private:
	//Window data
	GLFWwindow* m_window;
	int m_windowHeight;
	int m_windowWidth;

	bool isWindowMinimized;

	//Game Data

	float deltatime = 0.0f;

	ECS Ecs;



	bool is_free_cam = true;
	Camera m_camera;

	//Shaders
	
	

	//uniforms
	glm::mat4 projectionTransform;
	glm::mat4 viewTransform;

	
	
	//GUI
	GUI Gui;

	
	

	//Helper variables
	float last_frame_time = 0.0f;

	bool is_mouse_first_pos = true;
	float last_mouse_x;
	float last_mouse_y;

	bool showGui = true;

	int rollAmounts[7];
	std::string rollAmountsStrings[7];

	std::string rollSumAmountString;

public:
	App(int windowWidth = 1000, int windowHeight = 800, const char* windowTitle = "_debugTitle");
	
	~App();
	void Run();

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void WindowSizeCallback(GLFWwindow* window, int width, int height);
	static void WindowPosCallback(GLFWwindow* window, int xpos, int ypos);
	static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void WindowIconifiedCallback(GLFWwindow* window, int isIconified);

	

	template<int index,bool isIncrement>
	static void GuiIncrementDecrementCallback(void* context);

	static void GuiRollEnquedDiceCallback(void* context);


private:
	void GlfwInit(int windowWidth, int windowHeight, const char* windowTitle);
	void GlewInit(int windowWidth, int windowHeight);
	void Update();
	void Render();
	


};







template<int index, bool isIncrement>
inline void App::GuiIncrementDecrementCallback(void* context)
{
	App* app = (App*)context;

	int maxAmount = 20;
	if (isIncrement)
	{
		if (app->rollAmounts[index] < maxAmount)
		{
			++app->rollAmounts[index];
		}
	}
	else
	{
		if (app->rollAmounts[index] > 0)
		{
			--app->rollAmounts[index];
		}
	}

	app->rollAmountsStrings[index] = IntegerToString(app->rollAmounts[index]);
	//std::cout << app->rollAmountsStrings[index] << std::endl;
}


#endif