#include "Controls.h"

Button::Button(GuiContext* guiContext) : Control(guiContext)
{
	callbackDataContext = nullptr;
	onClickCallback = nullptr;

	maxFocusTime = 0.18f;

	focusTimeSpent = 0.0f;
}

Button::~Button()
{
	//Nthing needed
}

void Button::SetOnClickCallback(void(*OnClickCallback)(void*), void* callBackContext)
{
	this->callbackDataContext = callBackContext;
	this->onClickCallback = OnClickCallback;
}

void Button::SubControlUpdate(float deltaTime)
{
	if (hasFocus)
	{
		focusTimeSpent += deltaTime;

		if (focusTimeSpent >= maxFocusTime)
		{
			hasFocus = false;
			focusTimeSpent = 0.0f;
		}
	}
	
}

void Button::SubControlRender()
{
	//Maybe, if its clicked, then we change the render a bit or smth
}

void Button::SubControlResize()
{
	//Nothing really
}

void Button::SubControlClick()
{
	//When a button is clicked, here we call it's
	this->onClickCallback(callbackDataContext);

	hasFocus = true;
}



//Label stuff

Label::Label(GuiContext* guiContext) : Control(guiContext)
{
	labelString = nullptr;

	rawDisplaySize = glm::vec2(0, 0);

	labelScreenRect = { 0,0,0,0 };
}

Label::~Label()
{
	//nothing needed
}

void Label::SetLabelString(std::string* string)
{
	this->labelString = string;
}

void Label::SetDisplayRect(const Rectangle& displayRect)
{
	rawDisplaySize = glm::vec2(displayRect.width, displayRect.height);

	glm::vec2 screenDisplayMid = ScreenToNDC(NDCToScreen(screenPosition, guiContext->windowWidth, guiContext->windowHeight) + 
		glm::vec2(displayRect.x,displayRect.y), guiContext->windowWidth, guiContext->windowHeight);

	float ndcDisplayWidth = rawDisplaySize.x / (float)guiContext->windowWidth;
	float ndcDisplayHeight = rawDisplaySize.y / (float)guiContext->windowHeight;

	labelScreenRect = { screenDisplayMid.x,screenDisplayMid.y,ndcDisplayWidth,ndcDisplayHeight };
}

void Label::SubControlRender()
{
	Rectangle alphabetRect = { 0,0,50.0f / (float)guiContext->alphaBetTexture.GetWidth() ,1.0f};

	float xOffset;

	if (labelString->size() % 2 == 0)
	{
		xOffset = -((int)labelString->size()/2 - 0.5f) * labelScreenRect.width;
	}
	else
	{
		xOffset = -((int)labelString->size()/2) * labelScreenRect.width;
	}

	glm::vec2 ndcRenderStart = glm::vec2(labelScreenRect.x + xOffset, labelScreenRect.y);

	

	char zeroAscii = '0';
	guiContext->guiShader.SetUniform<int>("uObjectTypeId", 1);
	for (int i = 0;i < labelString->size();++i)
	{
		int currentAlphabetOffset = labelString->at(i) - zeroAscii;

		glm::vec2 currentRenderPosition = ndcRenderStart + glm::vec2(i * labelScreenRect.width, 0);

		//std::cout << currentRenderPosition.x << " " << currentRenderPosition.y << std::endl;

		glm::mat4 screenTransform = glm::translate(glm::mat4(1.0f), glm::vec3(currentRenderPosition.x, currentRenderPosition.y, 0)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(labelScreenRect.width, labelScreenRect.height, 1));
			
		guiContext->guiShader.SetUniform<glm::mat4>("uScreenTransform", screenTransform);

		glm::vec2 alphabetPosition = glm::vec2(alphabetRect.x + currentAlphabetOffset * alphabetRect.width, alphabetRect.y);

		

		glm::vec2 alphabetScale = glm::vec2(alphabetRect.width, alphabetRect.height);

		

		guiContext->guiShader.SetUniform<glm::vec2>("uTextureScale", alphabetScale);
		guiContext->guiShader.SetUniform<glm::vec2>("uTextureOffset", alphabetPosition);

		guiContext->quadMesh.Draw();
	}
}

void Label::SubControlResize()
{
	labelScreenRect.width = rawDisplaySize.x / (float)guiContext->windowWidth;
	labelScreenRect.height = rawDisplaySize.y / (float)guiContext->windowHeight;
}




