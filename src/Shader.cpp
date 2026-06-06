#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>

#include <GL/glew.h>

#include "Debug.h"

Shader::Shader() : m_programId(0)
{
	//std::cout << "Shader" << std::endl;
}

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	Set(vertexShaderPath, fragmentShaderPath);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_programId));
}

void Shader::Set(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	std::string vertexShaderSource;
	ReadShaderSource(vertexShaderPath, vertexShaderSource);

	std::string fragmentShaderSource;
	ReadShaderSource(fragmentShaderPath, fragmentShaderSource);

	LinkShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_programId));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}



void Shader::ReadShaderSource(const std::string& shaderPath, std::string& shaderSource)
{
	shaderSource = "";

	std::ifstream file(shaderPath);

	std::string linebuffer;
	while (std::getline(file, linebuffer))
	{
		shaderSource.append(linebuffer + '\n');
	}

	file.close();
}

unsigned int Shader::CompileShaderSource(unsigned int shaderType, const std::string& shaderSource)
{
	//std::cout << shaderSource << std::endl;
	GLCall(unsigned int shader_program_id = glCreateShader(shaderType));
	const char* src_code = shaderSource.c_str();
	GLCall(glShaderSource(shader_program_id, 1, &src_code, nullptr));
	GLCall(glCompileShader(shader_program_id));

	//Error checking
	int result;
	GLCall(glGetShaderiv(shader_program_id, GL_COMPILE_STATUS, &result));
	if (result == GL_TRUE)
	{
		std::cout << "Shader Compilation Successful!" << std::endl;
	}
	else
	{
		std::cout << (shaderType == GL_VERTEX_SHADER ? "Vertex Shader" : "Fragment Shader")
			<< " Compilation Failed..." << std::endl;
		int length;
		GLCall(glGetShaderiv(shader_program_id, GL_INFO_LOG_LENGTH, &length));

		char* message = new char[length];

		GLCall(glGetShaderInfoLog(shader_program_id, length, &length, message));
		std::cout << message << std::endl;

		delete[] message;
	}

	return shader_program_id;
}

void Shader::LinkShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	if (m_programId != 0)
	{
		GLCall(glDeleteProgram(m_programId));
	}
	GLCall(m_programId = glCreateProgram());

	unsigned int vertex_shader = CompileShaderSource(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fragment_shader = CompileShaderSource(GL_FRAGMENT_SHADER, fragmentShaderSource);

	GLCall(glAttachShader(m_programId, vertex_shader));
	GLCall(glAttachShader(m_programId, fragment_shader));
	GLCall(glLinkProgram(m_programId));
	GLCall(glValidateProgram(m_programId));

	GLCall(glDeleteShader(vertex_shader));
	GLCall(glDeleteShader(fragment_shader));
}

int Shader::GetUniformLocation(const std::string& name) const
{
	int retval = glGetUniformLocation(m_programId, name.c_str());

	return retval;
}