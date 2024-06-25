#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include<iostream>

class Shader
{
private:
	unsigned int m_RendererID;

public:
	Shader(const std::string& vertexShader, const std::string& fragmentShader);
	~Shader();

	void Bind();
	void UnBind();
	void SetUniformMat4fv(const std::string& name, const glm::mat4& matrix);
	void SetUniformVec3(const std::string& name, const glm::vec3& value);
	void SetUniformfloat(const std::string& name, const float& value);
	void SetUniformInt(const std::string& name, const int& value);

	static unsigned int CompileShader(unsigned int type, const std::string& source);
	static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

private:
	unsigned int GetUniformLocation(const std::string& name);

};