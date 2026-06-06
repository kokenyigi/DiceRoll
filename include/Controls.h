#pragma once

#ifndef CONTROLS_H
#define CONTROLS_H

#include "GUI.h"

class Button : public Control
{
protected:
	//A very lot of comments will be needed here
	//So, in C this is a very general way of writing callbacks
	//Each callback in C has a void* context, which acts as the data, the callback is working on
	//The button then calls the callback whenever its clicked, without knowing a single shit what happens inside it
	void* callbackDataContext;
	void (*onClickCallback)(void*);

	float maxFocusTime;
	float focusTimeSpent;
public:
	Button(GuiContext* guiContext);
	virtual ~Button();

	void SetOnClickCallback(void (*OnClickCallback)(void*), void* callBackContext);
protected:
	void SubControlUpdate(float deltaTime); //Actual implementation of the subfunctions that the super class
	void SubControlRender();
	void SubControlResize();
	void SubControlClick();

	//Potential Button-subclass virtual functions that button subclasses implement
	//for example: 
	// 
	// virtual void SubButtonUpdate()	and 
	// void ButtonUpdate()		calls similiarly in Control class
	//If such a subclass exists, then subControlUpdate(The button's override over the super control's functions has to contain them)
};

class Label : public Control
{
protected:
	std::string* labelString;

	glm::vec2 rawDisplaySize;

	Rectangle labelScreenRect;
public:
	Label(GuiContext* guiContext);
	virtual ~Label();

	void SetLabelString(std::string* string);
	void SetDisplayRect(const Rectangle& displayRect);
protected:
	void SubControlUpdate(float deltaTime) {};
	void SubControlRender();
	void SubControlResize();
	void SubControlClick() {};
};



#endif
