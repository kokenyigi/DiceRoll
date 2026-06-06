#include "App.h"
#include <iostream>




App::App(int windowWidth, int windowHeight, const char* windowTitle)
{
	//std::cout << " App constructor" << std::endl;
	//Mandatorily intialization of glfw and glew
	GlfwInit(windowWidth, windowHeight, windowTitle);
	GlewInit(windowWidth, windowHeight);

	//GeometryInit();
	Ecs.Init(500,glm::vec3(15,10,20));
	//TextureInit();

	Gui.Init(windowWidth, windowHeight);

	Button* buttonD4 = new Button(Gui.GetContext());
	buttonD4->SetScreenRect({ 100,40,50,50 });
	buttonD4->SetTextureRect({ 0,0,100,100 });
	std::vector<glm::vec2> squareOffsets =
	{
		glm::vec2(-25,-25),
		glm::vec2(-25,25),
		glm::vec2(25,25),
		glm::vec2(25,-25),
	};
	buttonD4->SetHitboxOffsets(squareOffsets);
	buttonD4->SetOnClickCallback(GuiIncrementDecrementCallback<0, true>, (void*)this);

	Gui.AddControl(buttonD4);

	Button* buttonD6 = new Button(Gui.GetContext());
	buttonD6->SetScreenRect({ 100, 120,50,50 });
	buttonD6->SetTextureRect({0, 100,100,100 });
	
	buttonD6->SetHitboxOffsets(squareOffsets);
	buttonD6->SetOnClickCallback(GuiIncrementDecrementCallback<1, true>, (void*)this);

	Button* buttonD8 = new Button(Gui.GetContext());
	buttonD8->SetScreenRect({ 200,40,50,50 });
	buttonD8->SetTextureRect({ 0,200,100,100 });

	buttonD8->SetHitboxOffsets(squareOffsets);
	buttonD8->SetOnClickCallback(GuiIncrementDecrementCallback<2, true>, (void*)this);

	Button* buttonD10 = new Button(Gui.GetContext());
	buttonD10->SetScreenRect({ 200,120,50,50 });
	buttonD10->SetTextureRect({ 0,300,100,100 });

	buttonD10->SetHitboxOffsets(squareOffsets);
	buttonD10->SetOnClickCallback(GuiIncrementDecrementCallback<3, true>, (void*)this);

	Button* buttonD12 = new Button(Gui.GetContext());
	buttonD12->SetScreenRect({ 300,40,50,50 });
	buttonD12->SetTextureRect({ 0,400,100,100 });

	buttonD12->SetHitboxOffsets(squareOffsets);
	buttonD12->SetOnClickCallback(GuiIncrementDecrementCallback<4, true>, (void*)this);
	
	Button* buttonD20 = new Button(Gui.GetContext());
	buttonD20->SetScreenRect({ 300,120,50,50 });
	buttonD20->SetTextureRect({ 0,500,100,100 });

	buttonD20->SetHitboxOffsets(squareOffsets);
	buttonD20->SetOnClickCallback(GuiIncrementDecrementCallback<5, true>, (void*)this);
	
	Button* buttonD100 = new Button(Gui.GetContext());
	buttonD100->SetScreenRect({ 400,80,50,50 });
	buttonD100->SetTextureRect({ 0, 600,100,100 });

	buttonD100->SetHitboxOffsets(squareOffsets);
	buttonD100->SetOnClickCallback(GuiIncrementDecrementCallback<6, true>, (void*)this);

	Gui.AddControl(buttonD6);
	Gui.AddControl(buttonD8);
	Gui.AddControl(buttonD10);
	Gui.AddControl(buttonD12);
	Gui.AddControl(buttonD20);
	Gui.AddControl(buttonD100);

	std::vector<glm::vec2> arrowOffsets =
	{
		glm::vec2(-12,12),
		glm::vec2(-12,-12),
		glm::vec2(12,-12),
		glm::vec2(12,12)
	};
	Button* buttonD4Increment = new Button(Gui.GetContext());
	buttonD4Increment->SetScreenRect({ 140,25,24,24 });
	buttonD4Increment->SetTextureRect({ 0,700,100,100 });
	buttonD4Increment->SetHitboxOffsets(arrowOffsets);
	buttonD4Increment->SetOnClickCallback(GuiIncrementDecrementCallback<0, true>, (void*)this);

	Button* buttonD4Decrement = new Button(Gui.GetContext());
	buttonD4Decrement->SetScreenRect({ 140,55,24,24 });
	buttonD4Decrement->SetTextureRect({ 0,800,100,100 });
	buttonD4Decrement->SetHitboxOffsets(arrowOffsets);
	buttonD4Decrement->SetOnClickCallback(GuiIncrementDecrementCallback<0, false>, (void*)this);

	Button* buttonD6Increment = new Button(Gui.GetContext());
	buttonD6Increment->SetScreenRect({ 140,105,24,24 });
	buttonD6Increment->SetTextureRect({ 0,700,100,100 });
	buttonD6Increment->SetHitboxOffsets(arrowOffsets);
	buttonD6Increment->SetOnClickCallback(GuiIncrementDecrementCallback<1, true>, (void*)this);

	Button* buttonD6Decrement = new Button(Gui.GetContext());
	buttonD6Decrement->SetScreenRect({ 140,135,24,24 });
	buttonD6Decrement->SetTextureRect({ 0,800,100,100 });
	buttonD6Decrement->SetHitboxOffsets(arrowOffsets);
	buttonD6Decrement->SetOnClickCallback(GuiIncrementDecrementCallback<1, false>, (void*)this);

	Button* buttonD8Increment = new Button(Gui.GetContext());
	buttonD8Increment->SetScreenRect({ 240,25,24,24 });
	buttonD8Increment->SetTextureRect({ 0,700,100,100 });
	buttonD8Increment->SetHitboxOffsets(arrowOffsets);
	buttonD8Increment->SetOnClickCallback(GuiIncrementDecrementCallback<2, true>, (void*)this);

	Button* buttonD8Decrement = new Button(Gui.GetContext());
	buttonD8Decrement->SetScreenRect({ 240,55,24,24 });
	buttonD8Decrement->SetTextureRect({ 0,800,100,100 });
	buttonD8Decrement->SetHitboxOffsets(arrowOffsets);
	buttonD8Decrement->SetOnClickCallback(GuiIncrementDecrementCallback<2, false>, (void*)this);

	Button* buttonD10Increment = new Button(Gui.GetContext());
	buttonD10Increment->SetScreenRect({ 240,105,24,24 });
	buttonD10Increment->SetTextureRect({ 0,700,100,100 });
	buttonD10Increment->SetHitboxOffsets(arrowOffsets);
	buttonD10Increment->SetOnClickCallback(GuiIncrementDecrementCallback<3, true>, (void*)this);

	Button* buttonD10Decrement = new Button(Gui.GetContext());
	buttonD10Decrement->SetScreenRect({ 240,135,24,24 });
	buttonD10Decrement->SetTextureRect({ 0,800,100,100 });
	buttonD10Decrement->SetHitboxOffsets(arrowOffsets);
	buttonD10Decrement->SetOnClickCallback(GuiIncrementDecrementCallback<3, false>, (void*)this);

	Button* buttonD12Increment = new Button(Gui.GetContext());
	buttonD12Increment->SetScreenRect({ 340,25,24,24 });
	buttonD12Increment->SetTextureRect({ 0,700,100,100 });
	buttonD12Increment->SetHitboxOffsets(arrowOffsets);
	buttonD12Increment->SetOnClickCallback(GuiIncrementDecrementCallback<4, true>, (void*)this);

	Button* buttonD12Decrement = new Button(Gui.GetContext());
	buttonD12Decrement->SetScreenRect({ 340,55,24,24 });
	buttonD12Decrement->SetTextureRect({ 0,800,100,100 });
	buttonD12Decrement->SetHitboxOffsets(arrowOffsets);
	buttonD12Decrement->SetOnClickCallback(GuiIncrementDecrementCallback<4, false>, (void*)this);

	Button* buttonD20Increment = new Button(Gui.GetContext());
	buttonD20Increment->SetScreenRect({ 340,105,24,24 });
	buttonD20Increment->SetTextureRect({ 0,700,100,100 });
	buttonD20Increment->SetHitboxOffsets(arrowOffsets);
	buttonD20Increment->SetOnClickCallback(GuiIncrementDecrementCallback<5, true>, (void*)this);

	Button* buttonD20Decrement = new Button(Gui.GetContext());
	buttonD20Decrement->SetScreenRect({ 340,135,24,24 });
	buttonD20Decrement->SetTextureRect({ 0,800,100,100 });
	buttonD20Decrement->SetHitboxOffsets(arrowOffsets);
	buttonD20Decrement->SetOnClickCallback(GuiIncrementDecrementCallback<5, false>, (void*)this);

	Button* buttonD100Increment = new Button(Gui.GetContext());
	buttonD100Increment->SetScreenRect({ 440,65,24,24 });
	buttonD100Increment->SetTextureRect({ 0,700,100,100 });
	buttonD100Increment->SetHitboxOffsets(arrowOffsets);
	buttonD100Increment->SetOnClickCallback(GuiIncrementDecrementCallback<6, true>, (void*)this);

	Button* buttonD100Decrement = new Button(Gui.GetContext());
	buttonD100Decrement->SetScreenRect({ 440,95,24,24 });
	buttonD100Decrement->SetTextureRect({ 0,800,100,100 });
	buttonD100Decrement->SetHitboxOffsets(arrowOffsets);
	buttonD100Decrement->SetOnClickCallback(GuiIncrementDecrementCallback<6, false>, (void*)this);


	Gui.AddControl(buttonD4Increment);
	Gui.AddControl(buttonD4Decrement);
	Gui.AddControl(buttonD6Increment);
	Gui.AddControl(buttonD6Decrement);
	Gui.AddControl(buttonD8Increment);
	Gui.AddControl(buttonD8Decrement);
	Gui.AddControl(buttonD10Increment);
	Gui.AddControl(buttonD10Decrement);
	Gui.AddControl(buttonD12Increment);
	Gui.AddControl(buttonD12Decrement);
	Gui.AddControl(buttonD20Increment);
	Gui.AddControl(buttonD20Decrement);
	Gui.AddControl(buttonD100Increment);
	Gui.AddControl(buttonD100Decrement);


	std::vector<glm::vec2> labelOffsets =
	{
		glm::vec2(-25,10),
		glm::vec2(-25,-10),
		glm::vec2(25,-10),
		glm::vec2(25,10)
	};


	Label* labelD4 = new Label(Gui.GetContext());
	labelD4->SetScreenRect({100,80,40,75});
	
	labelD4->SetTextureRect({ 0,900,100,100 });
	labelD4->SetDisplayRect({ 0,0,20,20 });
	labelD4->SetHitboxOffsets(labelOffsets);
	labelD4->SetLabelString(&rollAmountsStrings[0]);

	Label* labelD6 = new Label(Gui.GetContext());
	labelD6->SetScreenRect({ 100,160,40,75 });

	labelD6->SetTextureRect({ 0,900,100,100 });
	labelD6->SetDisplayRect({ 0,0,20,20 });
	labelD6->SetHitboxOffsets(labelOffsets);
	labelD6->SetLabelString(&rollAmountsStrings[1]);

	Label* labelD8 = new Label(Gui.GetContext());
	labelD8->SetScreenRect({ 200,80,40,75 });

	labelD8->SetTextureRect({ 0,900,100,100 });
	labelD8->SetDisplayRect({ 0,0,20,20 });
	labelD8->SetHitboxOffsets(labelOffsets);
	labelD8->SetLabelString(&rollAmountsStrings[2]);

	Label* labelD10 = new Label(Gui.GetContext());
	labelD10->SetScreenRect({ 200,160,40,75 });

	labelD10->SetTextureRect({ 0,900,100,100 });
	labelD10->SetDisplayRect({ 0,0,20,20 });
	labelD10->SetHitboxOffsets(labelOffsets);
	labelD10->SetLabelString(&rollAmountsStrings[3]);

	Label* labelD12 = new Label(Gui.GetContext());
	labelD12->SetScreenRect({ 300,80,40,75 });

	labelD12->SetTextureRect({ 0,900,100,100 });
	labelD12->SetDisplayRect({ 0,0,20,20 });
	labelD12->SetHitboxOffsets(labelOffsets);
	labelD12->SetLabelString(&rollAmountsStrings[4]);

	Label* labelD20 = new Label(Gui.GetContext());
	labelD20->SetScreenRect({ 300,160,40,75 });

	labelD20->SetTextureRect({ 0,900,100,100 });
	labelD20->SetDisplayRect({ 0,0,20,20 });
	labelD20->SetHitboxOffsets(labelOffsets);
	labelD20->SetLabelString(&rollAmountsStrings[5]);

	Label* labelD100 = new Label(Gui.GetContext());
	labelD100->SetScreenRect({ 400,120,40,75 });

	labelD100->SetTextureRect({ 0,900,100,100 });
	labelD100->SetDisplayRect({ 0,0,20,20 });
	labelD100->SetHitboxOffsets(labelOffsets);
	labelD100->SetLabelString(&rollAmountsStrings[6]);

	
	Gui.AddControl(labelD4);
	Gui.AddControl(labelD6);
	Gui.AddControl(labelD8);
	Gui.AddControl(labelD10);
	Gui.AddControl(labelD12);
	Gui.AddControl(labelD20);
	Gui.AddControl(labelD100);
	

	std::vector<glm::vec2> rollButtonOffsets =
	{
		glm::vec2(-60,60),
		glm::vec2(-60,-60),
		glm::vec2(60,-60),
		glm::vec2(60,60)
	};

	Button* rollButton = new Button(Gui.GetContext());
	rollButton->SetHitboxOffsets(rollButtonOffsets);
	rollButton->SetScreenRect({ 540,80,100,100 });
	rollButton->SetTextureRect({ 0,1000,100,100 });
	rollButton->SetOnClickCallback(GuiRollEnquedDiceCallback, (void*)this);

	Gui.AddControl(rollButton);

	

	Label* labelSum = new Label(Gui.GetContext());
	labelSum->SetScreenRect({ 700,80,120,120 });

	labelSum->SetTextureRect({ 0,1100,100,100 });
	labelSum->SetDisplayRect({ 0,28,30,30 });
	labelSum->SetHitboxOffsets(rollButtonOffsets);
	labelSum->SetLabelString(&rollSumAmountString);

	Gui.AddControl(labelSum);

	srand(time(0));

	for (int i = 0;i < 7;++i)
	{
		rollAmounts[i] = 0;
		rollAmountsStrings[i] = "0";
	}
	
}

void App::GuiRollEnquedDiceCallback(void* context)
{
	App* app = (App*)context;

	app->Ecs.Clear();

	for (int i = 0;i < app->rollAmounts[0];++i)
	{
		app->Ecs.AddEntity(TETRAHEDRON);
	}
	

	for (int i = 0;i < app->rollAmounts[1];++i)
	{
		app->Ecs.AddEntity(CUBE);
	}
	for (int i = 0;i < app->rollAmounts[2];++i)
	{
		app->Ecs.AddEntity(OCTAHEDRON);
	}
	for (int i = 0;i < app->rollAmounts[3];++i)
	{
		app->Ecs.AddEntity(DECAHEDRON1_10);
	}
	for (int i = 0;i < app->rollAmounts[4];++i)
	{
		app->Ecs.AddEntity(DODECAHEDRON);
	}
	for (int i = 0;i < app->rollAmounts[5];++i)
	{
		app->Ecs.AddEntity(ICOSAHEDRON);
	}
	for (int i = 0;i < app->rollAmounts[6];++i)
	{
		app->Ecs.AddEntity(DECAHEDRON0_9);
		app->Ecs.AddEntity(DECAHEDRON00_90);
	}

	for (int i = 0;i < 7;++i)
	{
		app->rollAmounts[i] = 0;
		app->rollAmountsStrings[i] = "0";
	}
}

void App::GlfwInit(int windowWidth, int windowHeight, const char* windowTitle)
{
	if (!glfwInit())
	{
		std::cout << "[GLFW Error]: Initialization of GLFW failed!" << std::endl;
		return;
	}
	else
	{
		std::cout << "Successfully initialized GLFW!" << std::endl;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (windowWidth < 1000)
	{
		windowWidth = 1000;
	}
	if (windowHeight < 800)
	{
		windowHeight = 800;
	}

	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
	m_window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
	if (!m_window)
	{
		std::cout << "[GLFW Error]: Window creation failed!" << std::endl;
		glfwTerminate();
		return;
	}
	else
	{
		std::cout << "Window creation successful!" << std::endl;
	}

	glfwSetWindowSizeLimits(m_window, 1000, 800, GLFW_DONT_CARE, GLFW_DONT_CARE);

	isWindowMinimized = false;


	glfwMakeContextCurrent(m_window);

	glfwSwapInterval(1);

	//Sets the current App context as the window user pointer
	glfwSetWindowUserPointer(m_window, this);

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Callbacks
	glfwSetScrollCallback(m_window, App::ScrollCallback);
	glfwSetCursorPosCallback(m_window, App::CursorPosCallback);
	glfwSetWindowSizeCallback(m_window, App::WindowSizeCallback);
	glfwSetKeyCallback(m_window, App::KeyCallback);
	glfwSetMouseButtonCallback(m_window, App::MouseButtonCallback);
	glfwSetWindowIconifyCallback(m_window, App::WindowIconifiedCallback);
	glfwSetWindowPosCallback(m_window,App::WindowPosCallback);
}

void App::GlewInit(int windowWidth, int windowHeight)
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << ("[GlEW Error]: Initialization of GLEW failed!") << std::endl;
		return;
	}
	else
	{
		std::cout << ("Initialization of GLEW was successful!") << std::endl;
	}

	//Printing the current OpenGL version on the terminal
	std::cout << "Current OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	//glDisable(GL_CULL_FACE);
	glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

//returns the left bottom corner in UV of the given number

App::~App()
{
	glfwTerminate();
}

void App::Run()
{
	while (!glfwWindowShouldClose(m_window))
	{
		
		Update();
		Render();
		
	}
}

void App::Update()
{
	float current_frame_time = glfwGetTime();

	deltatime = current_frame_time - last_frame_time;
	deltatime = fminf(deltatime, 0.08f); //clamping deltatime so update loop doesnt blow up

	last_frame_time = current_frame_time;

	//lightDirection = glm::vec4(lightDirection, 1.0f) * glm::rotate(glm::mat4(1.0f), glm::radians(0.05f), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
	{
		direction.z += 1.0f;
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		direction.z -= 1.0f;
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		direction.y -= 1.0f;
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		direction.y += 1.0f;
	}
	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		direction.x -= 1.0f;
	}
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		direction.x += 1.0f;
	}
	bool isSpeedy = false;

	if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		isSpeedy = true;
	}


	m_camera.Move(deltatime, direction,isSpeedy);
	

	glfwPollEvents();

	
	viewTransform = m_camera.GetViewMatrix();

	int rollSumNumber;

	Ecs.Update(deltatime,rollSumNumber);

	rollSumAmountString = IntegerToString(rollSumNumber);

	//std::cout << rollSumAmountString << std::endl;

	
	Gui.Update(deltatime);
	
}

void App::Render()
{
	//opengl boiler plate
	

	//toggling the main base shader
	

	//Setting up universal shader unifroms

	//std::cout << isWindowMinimized << std::endl;
	
	if (!isWindowMinimized)
	{
		projectionTransform = m_camera.GetPerspectiveMatrix((float)m_windowWidth / (float)m_windowHeight, 0.1f, 1000.0f);


		Ecs.Render(viewTransform, projectionTransform, m_camera.GetPosition(), (float)m_windowWidth, (float)m_windowHeight);

		if (showGui)
		{
			Gui.Render();
		}
		


		//final backbuffer swap
		glfwSwapBuffers(m_window);
	}

	
	
}




void App::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	App* app = (App*)glfwGetWindowUserPointer(window);

	app->m_camera.SetFovy(app->m_camera.GetFovy() - yoffset*7);
}

void App::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	App* app = (App*)glfwGetWindowUserPointer(window);



	if (app->is_mouse_first_pos)
	{
		app->last_mouse_x = xpos;
		app->last_mouse_y = ypos;
		app->is_mouse_first_pos = false;
	}

	float dx = xpos - app->last_mouse_x;
	float dy = app->last_mouse_y - ypos;

	app->last_mouse_x = xpos;
	app->last_mouse_y = ypos;

	if (app->is_free_cam == true)
	{
		app->m_camera.Rotate(dx, dy);
	}
	else
	{
		app->Gui.MouseMove(xpos, ypos);
	}
	

}

void App::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
	App* app = (App*)glfwGetWindowUserPointer(window);

	glViewport(0, 0, width, height);
	app->m_windowWidth = width;
	app->m_windowHeight = height;

	app->Gui.Resize(width, height);
}

void App::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	App* app = (App*)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		


	}
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		if (app->showGui == false)
		{
			app->showGui = true;
		}
		else
		{
			app->showGui = false;
		}


	}

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
			case GLFW_KEY_C:
				app->Ecs.Clear();
				break;
			case GLFW_KEY_1:
				app->Ecs.AddEntity(TETRAHEDRON);
				break;
			case GLFW_KEY_2:
				app->Ecs.AddEntity(CUBE);
				break;
			case GLFW_KEY_3:
				app->Ecs.AddEntity(OCTAHEDRON);
				break;
			case GLFW_KEY_4:
				app->Ecs.AddEntity(DECAHEDRON1_10);
				break;
			case GLFW_KEY_5:
				app->Ecs.AddEntity(DECAHEDRON0_9);
				break;
			case GLFW_KEY_6:
				app->Ecs.AddEntity(DECAHEDRON00_90);
				break;
			case GLFW_KEY_7:
				app->Ecs.AddEntity(DODECAHEDRON);
				break;
			case GLFW_KEY_8:
				app->Ecs.AddEntity(ICOSAHEDRON);
				break;
			case GLFW_KEY_9:
				
				break;
		}
	}
	
}

void App::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	App* app = (App*)glfwGetWindowUserPointer(window);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//app->Ecs.AddEntity(ICOSAHEDRON);

		//app->Ecs.Update(0.05f);
		if (app->showGui)
		{
			app->Gui.MouseClick();
		}
		
	}

	if(button ==GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		if (app->is_free_cam == false)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			app->is_free_cam = true;

			app->Gui.MouseMove(0, 0);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			app->is_free_cam = false;
		}
	}
}

void App::WindowIconifiedCallback(GLFWwindow* window, int isIconified)
{
	App* app = (App*)glfwGetWindowUserPointer(window);

	if (isIconified)
	{
		app->isWindowMinimized = true;
	}
	else
	{
		app->isWindowMinimized = false;
	}
}

void App::WindowPosCallback(GLFWwindow* window, int xpos, int ypos)
{
	App* app = (App*)glfwGetWindowUserPointer(window);

	//std::cout<<"Dragging"<<std::endl;
}







