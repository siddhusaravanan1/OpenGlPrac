#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 lightPos(0.75f, 0.0f, 1.0f);
std::string vertexShader =
"#version 330 core\n"
"\n"
"layout(location = 0) in vec3 position;\n"
"layout(location = 1) in vec3 aNormal;\n"
"layout(location = 2) in vec2 aTexture;\n"

"out vec3 normal;\n"
"out vec3 fragPos;\n"
"out vec2 texCoord;\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"

"\n"
"void main()\n"
"{\n"
"   fragPos = vec3(model * vec4(position, 1.0f));\n"
"   normal = mat3(transpose(inverse(model))) * aNormal;\n"
"   gl_Position = projection * view * model * vec4(position, 1.0);\n"
"   texCoord = aTexture;\n"
"}\n";

std::string fragmentShader =
"#version 330 core\n"
"\n"
"in vec2 texCoord;"
"struct Material {\n"
"   sampler2D texture_diffuse1;\n"
"   sampler2D texture_specular1;\n"
"   sampler2D texture_normal1;\n"
"   sampler2D texture_ao1;\n"
"   sampler2D texture_roughness1;\n"
"   float Shininess;\n"
"};\n"

"struct DirLight {\n"
"   vec3 ambient;\n"
"   vec3 diffuse;\n"
"   vec3 specular;\n"
"   vec3 direction;\n"
"};\n"

"struct PointLight {\n"
"   vec3 ambient;\n"
"   vec3 diffuse;\n"
"   vec3 specular;\n"
"   vec3 position;\n"

"   float constant;\n"
"   float linear;\n"
"   float quadratic;\n"
"};\n"
"struct SpotLight {\n"
"   vec3 ambient;\n"
"   vec3 diffuse;\n"
"   vec3 specular;\n"
"   vec3 position;\n"
"   vec3 direction;\n"
"   float inCutOff;\n"
"   float outCutOff;\n"

"   float constant;\n"
"   float linear;\n"
"   float quadratic;\n"
"};\n"

"uniform Material material;\n"
"uniform DirLight dirLight;\n"
"uniform PointLight pointLight;\n"
"uniform SpotLight spotLight;\n"

"out vec4 FragColor;\n"

"in vec3 normal;\n"
"in vec3 fragPos;\n"

"uniform vec3 cameraPos;\n"

"vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);\n"
"vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos);\n"
"vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 viewDir, vec3 fragPos);\n"

"\n"
"void main()\n"
"{\n"
"   vec3 viewDir = normalize(cameraPos - fragPos);\n"
"   vec3 norm = normalize(normal);\n"

"   vec3 result = calcDirLight(dirLight, norm, viewDir);\n"
"   //result += calcPointLight(pointLight, norm, viewDir, fragPos);\n"
"   result += calcSpotLight(spotLight, norm, viewDir, fragPos);\n"
"   FragColor = vec4(result, 1.0f);\n"
"}\n"

"vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)\n"
"{\n"
"   vec3 normMap = texture(material.texture_normal1, texCoord).rgb;\n"
"   vec3 aoMap = texture(material.texture_ao1, texCoord).rgb;\n"
"   vec3 roughMap = texture(material.texture_roughness1, texCoord).rgb;\n"
"   vec3 ambient = light.ambient * texture(material.texture_diffuse1, texCoord).rgb;\n"

"   vec3 lightDir = normalize(-light.direction);\n"
"   float diff = max(dot(lightDir, normal), 0.0f);\n"
"   vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, texCoord).rgb;\n"

"   vec3 reflectDir = reflect(-lightDir, normal);\n"
"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);\n"
"   vec3 specular = light.specular * spec * texture(material.texture_specular1, texCoord).rgb;\n"

"   ambient *= normMap;\n"
"   diffuse *= normMap;\n"
"   specular *= normMap;\n"

"   ambient *= aoMap;\n"
"   diffuse *= aoMap;\n"
"   specular *= aoMap;\n"

"   ambient *= roughMap;\n"
"   diffuse *= roughMap;\n"
"   specular *= roughMap;\n"

"   return (ambient + diffuse + specular);\n"
"}\n"
"vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos)\n"
"{\n"
"   vec3 normMap = texture(material.texture_normal1, texCoord).rgb;\n"
"   vec3 aoMap = texture(material.texture_ao1, texCoord).rgb;\n"
"   vec3 roughMap = texture(material.texture_roughness1, texCoord).rgb;\n"

"   vec3 ambient = light.ambient * texture(material.texture_diffuse1, texCoord).rgb;\n"

"   vec3 lightDir = normalize(light.position - fragPos);\n"
"   float diff = max(dot(lightDir, normal), 0.0f);\n"
"   vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, texCoord).rgb;\n"

"   vec3 reflectDir = reflect(-lightDir, normal);\n"
"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);\n"
"   vec3 specular = light.specular * spec * texture(material.texture_specular1, texCoord).rgb;\n"

"   float distance = length(light.position - fragPos);\n"
"   float attenuation = 1.0f/(light.constant + light.linear * distance + light.quadratic * (distance * distance));\n"\

"   ambient *= attenuation;\n"
"   diffuse *= attenuation;\n"
"   specular *= attenuation;\n"

"   ambient *= normMap;\n"
"   diffuse *= normMap;\n"
"   specular *= normMap;\n"

"   ambient *= aoMap;\n"
"   diffuse *= aoMap;\n"
"   specular *= aoMap;\n"

"   ambient *= roughMap;\n"
"   diffuse *= roughMap;\n"
"   specular *= roughMap;\n"

"   return (ambient + diffuse + specular);\n"
"}\n"
"vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos)\n"
"{\n"
"   vec3 normMap = texture(material.texture_normal1, texCoord).rgb;\n"
"   vec3 aoMap = texture(material.texture_ao1, texCoord).rgb;\n"
"   vec3 roughMap = texture(material.texture_roughness1, texCoord).rgb;\n"

"   vec3 lightDir = normalize(light.position - fragPos);\n"
"   float theta = dot(lightDir, normalize(-light.direction));\n"
"   float epsilon = (light.inCutOff - light.outCutOff);\n"
"   float intensity = clamp((theta - light.outCutOff) / epsilon, 0.0, 1.0);\n"

"   vec3 ambient = light.ambient * texture(material.texture_diffuse1, texCoord).rgb;\n"

"   float diff = max(dot(lightDir, normal),0.0f);\n"
"   vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, texCoord).rgb;\n"

"   vec3 reflectDir = reflect(-lightDir, normal);\n"
"   float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.Shininess);\n"
"   vec3 specular = light.specular * spec * texture(material.texture_specular1, texCoord).rgb;\n"
"   diffuse  *= intensity;\n"
"   specular *= intensity;\n"

"float distance = length(light.position - fragPos);\n"
"float attenuation = 1.0f/(light.constant + light.linear * distance + light.quadratic * (distance * distance));\n"

"   ambient *= attenuation;\n"
"   diffuse *= attenuation;\n"
"   specular *= attenuation;\n"

"   ambient *= normMap;\n"
"   diffuse *= normMap;\n"
"   specular *= normMap;\n"

"   ambient *= aoMap;\n"
"   diffuse *= aoMap;\n"
"   specular *= aoMap;\n"

"   ambient *= roughMap;\n"
"   diffuse *= roughMap;\n"
"   specular *= roughMap;\n"
"   return(ambient + diffuse + specular);\n"
"}\n"
;


std::string lightVertexShader =
"#version 330 core\n"
"\n"
"layout(location = 0) in vec3 position;\n"
"layout(location = 1) in vec3 aNormals;\n"
"layout(location = 2) in vec2 aTexCoords;\n"

"out vec2 TexCoords;\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"

"\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(position, 1.0);\n"
"   TexCoords = aTexCoords;\n"
"}\n";

std::string lightFragmentShader =
"#version 330 core\n"
"\n"

"in vec2 TexCoords;\n"
"out vec4 FragColor;\n"

"uniform sampler2D texture_diffuse1;\n"

"\n"
"void main()\n"
"{\n"
"   FragColor =  texture(texture_diffuse1, TexCoords);\n"
"}\n";

void processInput(GLFWwindow* window);
int main(void)
{
    if (!glfwInit())
        return -1;

    GLFWwindow* window;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glewInit();
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error" << std::endl;
    }
    stbi_set_flip_vertically_on_load(true);
    //depth test
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    Shader boxShader(vertexShader, fragmentShader);
    Model ourModel("C:/Users/ss/Documents/OpenGL/Basics-Opengl/OpenGlPrac/resource/Model/backpack.obj");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInput(window);
        boxShader.Bind();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, 0.5f * (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);

        glm::vec3 dirAmbient = glm::vec3(0.1f);
        glm::vec3 dirDiffuse = glm::vec3(0.8f);
        glm::vec3 dirSpecular = glm::vec3(1.0f);

        glm::vec3 pointAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
        glm::vec3 pointDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        glm::vec3 pointSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

        glm::vec3 spotAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 spotDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 spotSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

        boxShader.SetUniformMat4fv("model", model);
        boxShader.SetUniformMat4fv("view", view);
        boxShader.SetUniformMat4fv("projection", projection);

        boxShader.SetUniformVec3("cameraPos", cameraPos);
        boxShader.SetUniformfloat("material.Shininess", 32.0f);

        boxShader.SetUniformVec3("dirLight.ambient", glm::vec3(dirAmbient));
        boxShader.SetUniformVec3("dirLight.diffuse", glm::vec3(dirDiffuse));
        boxShader.SetUniformVec3("dirLight.specular", glm::vec3(dirSpecular));
        boxShader.SetUniformVec3("dirLight.direction", glm::vec3(-1.0f, -1.0f, -0.3f));

        boxShader.SetUniformVec3("pointLight.ambient", glm::vec3(pointAmbient));
        boxShader.SetUniformVec3("pointLight.diffuse", glm::vec3(pointDiffuse));
        boxShader.SetUniformVec3("pointLight.specular", glm::vec3(pointSpecular));
        boxShader.SetUniformVec3("pointLight.position", glm::vec3(lightPos));

        boxShader.SetUniformfloat("pointLight.constant", 1.0f);
        boxShader.SetUniformfloat("pointLight.linear", 0.09f);
        boxShader.SetUniformfloat("pointLight.quadratic", 0.032f);

        boxShader.SetUniformVec3("spotLight.ambient", glm::vec3(spotAmbient));
        boxShader.SetUniformVec3("spotLight.diffuse", glm::vec3(spotDiffuse));
        boxShader.SetUniformVec3("spotLight.specular", glm::vec3(spotSpecular));
        boxShader.SetUniformVec3("spotLight.position", glm::vec3(cameraPos));
        boxShader.SetUniformVec3("spotLight.direction", glm::vec3(cameraFront));


        boxShader.SetUniformfloat("spotLight.constant", 1.0f);
        boxShader.SetUniformfloat("spotLight.linear", 0.09f);
        boxShader.SetUniformfloat("spotLight.quadratic", 0.032f);

        boxShader.SetUniformfloat("spotLight.inCutOff", glm::cos(glm::radians(12.5f)));
        boxShader.SetUniformfloat("spotLight.outCutOff", glm::cos(glm::radians(15.5f)));


        boxShader.Bind();
        ourModel.Draw(boxShader);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}