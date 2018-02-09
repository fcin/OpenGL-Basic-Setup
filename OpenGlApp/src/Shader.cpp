#include "Shader.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include "Renderer.h"

Shader::Shader(const std::string & filepath)
	:m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
	
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string & name, int value)
{
	int location = GetUniformLocation(name);
	GLCall(glUniform1i(location, value));
}

void Shader::SetUniform4f(const std::string & name, float v0, float v1, float v2, float v3)
{
	int location = GetUniformLocation(name);
	GLCall(glUniform4f(location, v0, v1, v2, v3));
}

int Shader::GetUniformLocation(const std::string & name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
	{
		std::cout << "Warning: uniform " << name << " doesn't exist";
	}

	m_UniformLocationCache[name] = location;
	return location;
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::ifstream stream(filepath);
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	std::string line;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;


		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return ShaderProgramSource(ss[(int)ShaderType::VERTEX].str(), ss[(int)ShaderType::FRAGMENT].str());
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile shader!" << message;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

int Shader::CreateShader(const std::string vertexShader, const std::string fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

