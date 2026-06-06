#include "GUI.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp> // transzformációkhoz

/*
* Functions Related To the Graphical Use Interface class
* This class basically acts as a container for our Controls
* 
*/
GUI::GUI()
{
	//Nothing yet
}

GUI::~GUI()
{
	//Nothing yet
}

void GUI::Init(int windowWidth, int windowHeight)
{
	m_guiContext.windowWidth = windowWidth;
	m_guiContext.windowHeight = windowHeight;

	m_guiContext.mousePos = glm::vec2(0, 0);

	std::vector<VertexP2T2> quadVertices =
	{
		{glm::vec2(-1,1),glm::vec2(0,1)},
		{glm::vec2(-1,-1),glm::vec2(0,0)},
		{glm::vec2(1,-1),glm::vec2(1,0)},
		{glm::vec2(1,1),glm::vec2(1,1)}
 	};

	std::vector<unsigned int> quadIndices =
	{
		0,1,2,
		2,3,0
	};

	m_guiContext.quadMesh.Set(quadVertices, quadIndices);

	m_guiContext.guiTexture.Set("res/textures/gui_alpha.png");

	m_guiContext.alphaBetTexture.Set("res/textures/gui_alphabet.png");

	m_guiContext.guiShader.Set("res/shaders/gui_vertex_shader.vert", "res/shaders/gui_fragment_shader.frag");
}

void GUI::Update(float deltaTime)
{
	for (int i = 0;i < controls.size();++i)
	{
		controls[i]->Update(deltaTime);
	}
}

void GUI::Render()
{
	m_guiContext.guiShader.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);


	m_guiContext.guiTexture.Bind(0);
	m_guiContext.alphaBetTexture.Bind(1);
	m_guiContext.guiShader.SetUniform<int>("uGuiTexture", 0);
	m_guiContext.guiShader.SetUniform<int>("uAlphabetTexture", 1);

	m_guiContext.guiShader.SetUniform<int>("uObjectTypeId", 0);


	for (int i = 0;i < controls.size();++i)
	{
		controls[i]->Render(); //Needs more parameters!
	}


	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	m_guiContext.guiShader.Unbind();
}

void GUI::Resize(int newWindowWidth, int newWindowHeight)
{
	m_guiContext.windowWidth = newWindowWidth;
	m_guiContext.windowHeight = newWindowHeight;

	for (int i = 0;i < controls.size();++i)
	{
		controls[i]->Resize(); 
	}
}

void GUI::MouseMove(float newMousePosX, float newMousePosY)
{
	m_guiContext.mousePos.x = newMousePosX;
	m_guiContext.mousePos.y = newMousePosY;
}

void GUI::MouseClick()
{
	for (int i = 0;i < controls.size();++i)
	{
		controls[i]->Click();
	}
}

void GUI::AddControl(Control* control)
{
	controls.push_back(control);
}


/*
* Here, below lie the super class', the Control class' functions
* These are the same exact functions inside every SubControl class
*/

Control::Control(GuiContext* guiContext)
{
	this->guiContext = guiContext;

	screenPosition = glm::vec2(0, 0);
	screenScale = glm::vec2(0, 0);
	screenRotation = 0.0f;

	textureRect = { 0,0,0,0 };
	
	rawHeight = 0.0f;
	rawWidth = 0.0f;

	isHovered = false;

	hasFocus = false;
}

Control::~Control()
{
	//Nothing yet
}

void Control::SetScreenRect(const Rectangle& rawScreenRect)
{
	float normalizedDeviceX = (rawScreenRect.x / (float)guiContext->windowWidth) * 2 - 1;
	float normalizedDeviceY = ((rawScreenRect.y / (float)guiContext->windowHeight) * 2 -1) * -1; //has to be vertically flipped

	float normalizedDeviceWidth = rawScreenRect.width / (float)guiContext->windowWidth;
	float normalizedDeviceHeight = rawScreenRect.height/ (float)guiContext->windowHeight;

	rawWidth = rawScreenRect.width;
	rawHeight = rawScreenRect.height;
	

	//screenRect = { normalizedDeviceX,normalizedDeviceY,normalizedDeviceWidth,normalizedDeviceHeight };
	screenPosition = glm::vec2(normalizedDeviceX, normalizedDeviceY);
	screenScale = glm::vec2(normalizedDeviceWidth, normalizedDeviceHeight);
}

void Control::SetTextureRect(const Rectangle& rawTextureRect)
{
	float textureSpaceX = rawTextureRect.x / (float)guiContext->guiTexture.GetWidth();
	float textureSpaceY = 1.0f - (rawTextureRect.y + rawTextureRect.height)/ (float)guiContext->guiTexture.GetHeight() ;

	float textureSpaceWidth = rawTextureRect.width / (float)guiContext->guiTexture.GetWidth();
	float textureSpaceHeight = rawTextureRect.height/ (float)guiContext->guiTexture.GetHeight();

	textureRect = { textureSpaceX,textureSpaceY,textureSpaceWidth,textureSpaceHeight };
}

void Control::SetHitboxOffsets(const std::vector<glm::vec2>& hitboxOffsets)
{
	this->hitboxOffsets.reserve(hitboxOffsets.size());
	for (int i = 0;i < hitboxOffsets.size();++i)
	{
		this->hitboxOffsets.push_back(hitboxOffsets[i]);
	}
}

void Control::ControlUpdate(float deltaTime)
{
	//here we update stuff, 
	//mainly for now, we just update if the control is hovered or not
	int validIntersectCount = 0;

	glm::vec2 controlScreenMid = NDCToScreen(screenPosition, guiContext->windowWidth, guiContext->windowHeight);
	for (int i = 0;i < hitboxOffsets.size();++i)
	{
		glm::vec2 p1 = controlScreenMid + hitboxOffsets[i];
		glm::vec2 p2 = controlScreenMid + hitboxOffsets[(i + 1) % hitboxOffsets.size()];

		float t = HorizontalRayToLineIntersect(guiContext->mousePos, p1, p2);

		//std::cout << t << std::endl;
		if (t>=0)
		{
			++validIntersectCount;
		}
	}

	//if we got an odd number of intersections, then the mousPos must be inside the control's hitbox
	if (validIntersectCount % 2 == 1)
	{
		isHovered = true;
		//std::cout << "I am being hovered!\n";
	}
	else
	{
		isHovered = false;
	}
}

void Control::ControlRender()
{
	
	

	glm::mat4 screenTransform = glm::translate(glm::mat4(1.0f), glm::vec3(screenPosition.x, screenPosition.y, 0)) *
								glm::scale(glm::mat4(1.0f), glm::vec3(screenScale.x, screenScale.y, 1))*
								glm::rotate(glm::mat4(1.0f), glm::radians(screenRotation), glm::vec3(0, 0, 1));

    glm::vec2 textureScale = glm::vec2(textureRect.width, textureRect.height);
	glm::vec2 textureOffset = glm::vec2(textureRect.x, textureRect.y);
	if (isHovered)
	{
		textureOffset.x = textureRect.x + textureScale.x;
	}
	if (hasFocus)
	{
		textureOffset.x = textureRect.x + 2*textureScale.x;
	}

	guiContext->guiShader.SetUniform<glm::mat4>("uScreenTransform", screenTransform);
	guiContext->guiShader.SetUniform<glm::vec2>("uTextureScale", textureScale);
	guiContext->guiShader.SetUniform<glm::vec2>("uTextureOffset", textureOffset);

	guiContext->guiShader.SetUniform<int>("uObjectTypeId", 0);

	guiContext->quadMesh.Draw();
}

void Control::ControlResize()
{
	screenScale.x = rawWidth / (float)guiContext->windowWidth;
	screenScale.y = rawHeight / (float)guiContext->windowHeight;
}

void Control::ControlClick()
{
	//hasFocus = true; //maybe not the best place for this
	//This function can be empty, because idk what a bare control should do when clicked
	
}
