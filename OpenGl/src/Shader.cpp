#include "Shader.h"




Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader)
{
    m_RendererID = CreateShader(vertexShader, fragmentShader);
    glUseProgram(m_RendererID);
}
Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
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
        int lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        char* message = (char*)alloca(lenght * sizeof(char));
        glGetShaderInfoLog(id, lenght, &lenght, message);
        std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return(id);
}
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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

    return(program);
}

unsigned int Shader::GetUniformLocation(const std::string& name)
{
    unsigned int location = glGetUniformLocation(m_RendererID, name.c_str());
    return(location);
}
void Shader::SetUniformMat4fv(const std::string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::SetUniformVec3(const std::string& name, const glm::vec3& value)
{
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}
void Shader::SetUniformfloat(const std::string& name, const float& value)
{
    glUniform1f(GetUniformLocation(name), value);
}
void Shader::SetUniformInt(const std::string& name, const int& value)
{
    glUniform1i(GetUniformLocation(name), value);
}
void Shader::Bind()
{
    glUseProgram(m_RendererID);
}
void Shader::UnBind()
{
    glUseProgram(0);
}