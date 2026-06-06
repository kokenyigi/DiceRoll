#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include <GL/glew.h>
#include <iostream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClear_Error();\
    x;\
    ASSERT(GLLogCall(#x,__FILE__,__LINE__))

static void GLClear_Error()
{
	while (glGetError() != GL_NO_ERROR);
}
static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << ")" << function << " " << file << " : " << line << std::endl;
		return false;
	}
	return true;
}


#endif