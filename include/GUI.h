#pragma once

#ifndef GUI_H
#define GUI_H

#include <vector>

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

#include "Utils.h"

struct GuiContext
{
	int windowHeight;
	int windowWidth;

	glm::vec2 mousePos;

	Texture guiTexture;

	Texture alphaBetTexture;

	Mesh<VertexP2T2> quadMesh;
	
	Shader guiShader;
};



class Control
{
protected:
	//We have to have a gui context from which we derive information 
	GuiContext* guiContext;

	//Hitbox - we store the hitbox as a set of hull points
	//Can be concave, and also can be convex, it doesnt matter since we will be using the ray technique
	//These points are basically offsets from the middle of the control in raw screen coordinates
	std::vector<glm::vec2> hitboxOffsets;

	bool isHovered;

	bool hasFocus;

	//Some tweeking values to remember
	//Becauase if we remember these, gui elements will scale *pretty*
	float rawWidth;
	float rawHeight;

	//we need these for rendering
	glm::vec2 screenPosition;
	glm::vec2 screenScale;
	//In degrees!
	float screenRotation;
	
	Rectangle textureRect;
public:
	Control(GuiContext* guiContext);
	virtual ~Control();

	void Update(float deltaTime)
	{
		ControlUpdate(deltaTime);
		SubControlUpdate(deltaTime);
	}

	void Render()
	{
		ControlRender();
		SubControlRender();
	}

	void Resize()
	{
		ControlResize();
		SubControlResize();
	}

	void Click()
	{
		//This if would be the same for all controls and subchildren
		//^ this is only an assumption
		if (isHovered)
		{
			ControlClick();
			SubControlClick();
		}
	}

	//Needs one more virtual event combo
	//Which happens when control hasFocus

	void SetScreenRect(const Rectangle& rawScreenRect);
	void SetTextureRect(const Rectangle& rawTextureRect);
	void SetHitboxOffsets(const std::vector<glm::vec2>& hitboxOffsets);
protected:
	virtual void SubControlUpdate(float deltaTime) {}; //This is virtual, so a subclass can override it
	virtual void SubControlRender() {}; //same shit
	virtual void SubControlResize() {};
	virtual void SubControlClick() {};

	void ControlUpdate(float deltaTime);
	void ControlRender();
	void ControlResize();
	void ControlClick();
};

/*
* All other subcontrols are implemeted and defined in controls.cpp and controls.h files
*/



class GUI
{
private:
	//The main Control container
	//Note: Later on its better for this to be a universal tree, since
	//Controls can have subcontrols implanted inside them, like a layoutpanel or some shit
	//For now, a linear data structure can be enough since no subcontainer is needed
	std::vector<Control*> controls;

	//The main Quad mesh is a rectangle, which will be used to render ALL Controls
	//In 2D cases no more is required Mesh-sense
	//For resource optimization we will only have one quad like this, and we will pass it to render calls
	//It starts out as a NDC coordinated rectangle, with vertex dimensions: (-1,1) x (-1,1)
	//And texture dimensions: (0,0) x (1,1) <---vertically inverted openGL's texture coordinate system
	

	//This texture so far contains all gui control sprites, and to draw the correct ones,
	//The start quad's textureCoords must be transformd to match the dimensions of the
	

	//This shader is used for gui rendering, its important to enable and later disable glalpha blending when used
	//Now i gotta write another shader uuuarogh..
	//Also very important to disable gldepthtest before rendering gui or else shit happens

	//All previous member variables have been compressed into the context below
	//Every single control there is, has to have a guiContext at the time of creation, because then this given control knows
	//How to properly calculate its internal state using the context, this context pointer also enables it to render,
	//without having to be passed render parameters
	
	GuiContext m_guiContext;

public:
	GUI();
	~GUI();

	void Init(int windowWidth, int windowHeight);

	GuiContext* GetContext() { return &m_guiContext; }

	void Update(float deltaTime);
	void Render();
	void Resize(int newWindowWidth, int newWindowHeight);
	void MouseMove(float newMousePosX, float newMousePosY );

	
	void MouseClick();

	//Needs still something like:
	//void KeyPressed(char key);

	void AddControl(Control* control);
};


#endif
