#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>   
#include <GL/glew.h>
#include "Debug.h"

class Shader
{
private:
	unsigned int m_programId;
public:
	Shader();
	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~Shader();

	void Set(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	void Bind() const;
	void Unbind() const;

	//Uniform setter
	//Only works if the given type has been properly defined before in shader.cpp
	template <typename UnwrittenType>
	void SetUniform(const std::string& name, const UnwrittenType& data) const;

	//temp


private:
	void ReadShaderSource(const std::string& shaderPath, std::string& shaderSource);
	unsigned int CompileShaderSource(unsigned int shaderType, const std::string& shaderSource);
	void LinkShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	int GetUniformLocation(const std::string& name) const;
};





//Actual shader Uniform implementations
template<>
inline void Shader::SetUniform<glm::mat4>(const std::string& name, const glm::mat4& data) const
{
	int location = GetUniformLocation(name);
	if (location < 0)
	{
		return;
	}
	else
	{
		GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(data)));
	}
	
}

template<>
inline void Shader::SetUniform<glm::vec3>(const std::string& name, const glm::vec3& data) const
{
	int location = GetUniformLocation(name);
	if (location == -1)return;
	GLCall(glUniform3fv(location, 1, glm::value_ptr(data)));
}

template<>
inline void Shader::SetUniform<glm::vec2>(const std::string& name, const glm::vec2& data) const
{
	int location = GetUniformLocation(name);
	if (location == -1)return;
	GLCall(glUniform2fv(location, 1, glm::value_ptr(data)));
}

template<>
inline void Shader::SetUniform<int>(const std::string& name, const int& value) const
{
	int location = GetUniformLocation(name);
	if (location == -1)return;
	GLCall(glUniform1i(location, value));
}

template<>
inline void Shader::SetUniform<float>(const std::string& name, const float& value) const
{
	int location = GetUniformLocation(name);
	if (location == -1)return;
	GLCall(glUniform1f(location, value));
}






template<typename UnwrittenType>
void Shader::SetUniform(const std::string& name, const UnwrittenType& data) const
{
	static_assert(sizeof(UnwrittenType) == 0, "A uniform setter for this type hasn't been defined before!\n");
}


#endif