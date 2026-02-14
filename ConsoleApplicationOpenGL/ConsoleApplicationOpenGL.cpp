#include <iostream>
#include "Shader.h"
#include "Model.h"
#define GLEW_STATIC
#define GLFW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLEW_STATIC
#define GLFW_INCLUDE_NONE

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
bool firstMouse = true;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
const float cameraSpeed = 0.001f;

float const LIGHT_RED = 1.0f;
float const LIGHT_GREEN = 1.0f;
float const LIGHT_BLUE = 1.0f;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

int main()
{
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MainWindow", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW init failed" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        (float)SCR_WIDTH / (float)SCR_HEIGHT,
        0.1f,
        100.0f
    );

    Shader shader;
    shader.loadFromFiles("shaders/vertex.txt", "shaders/fragment.txt");
    shader.use();
    shader.setUniformMatrix4fv("projection", glm::value_ptr(projection));

    shader.setUniform3f("material.ambient", 0.282f, 0.387f, 0.277f);
    shader.setUniform3f("material.diffuse", 0.4f, 0.6f, 0.4f);    
    shader.setUniform3f("material.specular", 0.5f, 0.5f, 0.5f);   
    shader.setUniform1f("material.shininess", 32.0f);

    shader.setUniform3f("light.position", 2.0f, 5.0f, 3.0f);       
    shader.setUniform3f("light.ambient", 0.1f, 0.1f, 0.1f);        
    shader.setUniform3f("light.diffuse", 1.0f, 1.0f, 1.0f);        
    shader.setUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

    Shader shader2;
    shader2.loadFromFiles("shaders/vertex.txt", "shaders/fragment.txt");
    shader2.use();
    shader2.setUniformMatrix4fv("projection", glm::value_ptr(projection));

    shader2.setUniform3f("material.ambient", 0.108f, 0.153f, 0.124f);
    shader2.setUniform3f("material.diffuse", 0.2f, 0.3f, 0.25f);
    shader2.setUniform3f("material.specular", 0.3f, 0.3f, 0.3f);
    shader2.setUniform1f("material.shininess", 16.0f);

    shader2.setUniform3f("light.position", 2.0f, 5.0f, 3.0f);
    shader2.setUniform3f("light.ambient", 0.1f, 0.1f, 0.1f);
    shader2.setUniform3f("light.diffuse", 1.0f, 1.0f, 1.0f);
    shader2.setUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

    Model loadedModel("models/lab3.obj");
    Model fieldModel("models/field.obj");

    glm::mat4 modelMatrixLab = glm::mat4(1.0f);
    glm::mat4 modelMatrixField = glm::mat4(1.0f);
    modelMatrixField = glm::translate(modelMatrixField, glm::vec3(0.0f, -0.03f, 0.0f));


    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwTerminate();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setUniformMatrix4fv("view", glm::value_ptr(view));
        shader.setUniformMatrix4fv("projection", glm::value_ptr(projection));
        shader.setUniformMatrix4fv("model", glm::value_ptr(modelMatrixLab));
        /*shader.setUniform3f("lightColor", LIGHT_RED, LIGHT_GREEN, LIGHT_BLUE);
        shader.setUniform3f("objectColor", 0.282f, 0.387f, 0.277f);*/
        loadedModel.Draw(shader);

        shader2.use();
        shader2.setUniformMatrix4fv("view", glm::value_ptr(view));
        shader2.setUniformMatrix4fv("projection", glm::value_ptr(projection));
        shader2.setUniformMatrix4fv("model", glm::value_ptr(modelMatrixField));
        /*shader2.setUniform3f("lightColor", LIGHT_RED, LIGHT_GREEN, LIGHT_BLUE);
        shader2.setUniform3f("objectColor", 0.108f, 0.153f, 0.124f);*/
        fieldModel.Draw(shader2);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    pitch = pitch > 89.0f ? 89.0f : pitch;
    pitch = pitch < -89.0f ? -89.0f : pitch;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}