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
glm::vec3 cameraPos = glm::vec3(2.0f, 1.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
const float cameraSpeed = 0.001f;

float const LIGHT_RED = 1.0f;
float const LIGHT_GREEN = 1.0f;
float const LIGHT_BLUE = 1.0f;

int currentArm = 1;
const float detailsSpeed = 0.0005f;

float minPosRollerZ = 0.0f;
float maxPosRollerZ = 4.02f;
float currentPosRollerZ = 0.0f;

float maxPosArmsY = 0.0f;
float minPosArmsY = -0.48f;
float currentPosArm1Y = 0.0f;
float currentPosArm2Y = 0.0f;

float minAngleRotate = 0.0f;
float maxAngleRotate = 45.0f;
float xPivot = -0.461076f;
float yPivot = -1.35491f;
float rotateSpeed = 0.01f;

glm::vec3 vecRotate = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 pivot = glm::vec3(xPivot, yPivot, 0.0f);

float yArm1Pivot = yPivot;
float yArm2Pivot = yPivot;
glm::vec3 arm1pivot = glm::vec3(xPivot, yArm1Pivot, 0.0f);
glm::vec3 arm2pivot = glm::vec3(xPivot, yArm2Pivot, 0.0f);

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
Shader getArm1Shader(glm::mat4 projection);
Shader getArm2Shader(glm::mat4 projection);
Shader getRollerShader(glm::mat4 projection);
Shader getMainColumnShader(glm::mat4 projection);
Shader getRotateDetailShader(glm::mat4 projection);
Shader getFieldShader(glm::mat4 projection);
glm::vec3 getPositionFromMatrix(const glm::mat4& matrix);

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

    Shader arm1Shader = getArm1Shader(projection);
    Shader arm2Shader = getArm2Shader(projection);
    Shader rollerShader = getRollerShader(projection);
    Shader mainColumnShader = getMainColumnShader(projection);
    Shader rotateDetailShader = getRotateDetailShader(projection);
    Shader fieldShader = getFieldShader(projection);

    Model arm1Model("models/arm1.obj");
    Model arm2Model("models/arm2.obj");
    Model rollerModel("models/roller.obj");
    Model mainColumnModel("models/main_column.obj");
    Model rotateDetailModel("models/rotate_detail.obj");
    Model fieldModel("models/field.obj");

    glm::mat4 modelMatrixArm1 = glm::mat4(1.0f);
    glm::mat4 modelMatrixArm2 = glm::mat4(1.0f);
    glm::mat4 modelMatrixRoller = glm::mat4(1.0f);
    glm::mat4 modelMatrixMainColumn = glm::mat4(1.0f);
    glm::mat4 modelMatrixRotateDetail = glm::mat4(1.0f);
    glm::mat4 modelMatrixField = glm::mat4(1.0f);
    modelMatrixField = glm::translate(modelMatrixField, glm::vec3(0.0f, -0.03f, 0.0f));

    bool armChangedThisFrame = false;
    float currentAngleRotate = 0.0f;

    ////debug
    //glm::vec3 debugPivot = glm::vec3(0.0f, 0.0f, 0.0f);
    //glm::mat4 initialMatrixArm1;  // сохраняем начальное положение
    //glm::mat4 initialMatrixArm2;
    //glm::mat4 initialMatrixRotateDetail;
    //bool isRotating = false;


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

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            if (!armChangedThisFrame)
                currentArm == 1 ? currentArm = 2 : currentArm = 1;
                armChangedThisFrame = true;

        } else { armChangedThisFrame = false; }

        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
            glm::vec3 vecTranslate = glm::vec3(0.0f, detailsSpeed, 0.0f);
            if (currentArm == 1) {
                if (currentPosArm1Y < maxPosArmsY) {
                    modelMatrixArm1 = glm::translate(modelMatrixArm1, vecTranslate);
                    yArm1Pivot += detailsSpeed;
                    currentPosArm1Y += detailsSpeed;
                    arm1pivot = glm::vec3(xPivot, yArm1Pivot, 0.0f);
                }
            
            }
            else if (currentArm == 2) {
                if (currentPosArm2Y < maxPosArmsY) {
                    modelMatrixArm2 = glm::translate(modelMatrixArm2, vecTranslate);
                    yArm2Pivot += detailsSpeed;
                    currentPosArm2Y += detailsSpeed;
                    arm2pivot = glm::vec3(xPivot, yArm2Pivot, 0.0f);
                }
            }
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
            glm::vec3 vecTranslate = glm::vec3(0.0f, -detailsSpeed, 0.0f);
            if (currentArm == 1) {
                if (currentPosArm1Y > minPosArmsY) {
                    modelMatrixArm1 = glm::translate(modelMatrixArm1, vecTranslate);
                    yArm1Pivot -= detailsSpeed;
                    currentPosArm1Y -= detailsSpeed;
                    arm1pivot = glm::vec3(xPivot, yArm1Pivot, 0.0f);
                }
            } else if (currentArm == 2) {
                if (currentPosArm2Y > minPosArmsY) {
                    modelMatrixArm2 = glm::translate(modelMatrixArm2, vecTranslate);
                    yArm2Pivot -= detailsSpeed;
                    currentPosArm2Y -= detailsSpeed;
                    arm2pivot = glm::vec3(xPivot, yArm2Pivot, 0.0f);
                }
            }
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            if (currentPosRollerZ < maxPosRollerZ) {
                glm::vec3 vecTranslate = glm::vec3(0.0f, 0.0f, detailsSpeed);
                modelMatrixRoller = glm::translate(modelMatrixRoller, vecTranslate);
                modelMatrixArm1 = glm::translate(modelMatrixArm1, vecTranslate);
                modelMatrixArm2 = glm::translate(modelMatrixArm2, vecTranslate);
                modelMatrixRotateDetail = glm::translate(modelMatrixRotateDetail, vecTranslate);

                currentPosRollerZ += detailsSpeed;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            if (currentPosRollerZ > minPosRollerZ) {
                glm::vec3 vecTranslate = glm::vec3(0.0f, 0.0f, -detailsSpeed);
                modelMatrixRoller = glm::translate(modelMatrixRoller, vecTranslate);
                modelMatrixArm1 = glm::translate(modelMatrixArm1, vecTranslate);
                modelMatrixArm2 = glm::translate(modelMatrixArm2, vecTranslate);
                modelMatrixRotateDetail = glm::translate(modelMatrixRotateDetail, vecTranslate);

                currentPosRollerZ -= detailsSpeed;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            if (currentAngleRotate > minAngleRotate) {
                modelMatrixArm1 = glm::translate(modelMatrixArm1, -arm1pivot);
                modelMatrixArm1 = glm::rotate(modelMatrixArm1, glm::radians(rotateSpeed), vecRotate);
                modelMatrixArm1 = glm::translate(modelMatrixArm1, arm1pivot);

                modelMatrixArm2 = glm::translate(modelMatrixArm2, -arm2pivot);
                modelMatrixArm2 = glm::rotate(modelMatrixArm2, glm::radians(rotateSpeed), vecRotate);
                modelMatrixArm2 = glm::translate(modelMatrixArm2, arm2pivot);

                modelMatrixRotateDetail = glm::translate(modelMatrixRotateDetail, -pivot);
                modelMatrixRotateDetail = glm::rotate(modelMatrixRotateDetail, glm::radians(rotateSpeed), vecRotate);
                modelMatrixRotateDetail = glm::translate(modelMatrixRotateDetail, pivot);

                currentAngleRotate -= rotateSpeed;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            if (currentAngleRotate < maxAngleRotate) {
                modelMatrixArm1 = glm::translate(modelMatrixArm1, -arm1pivot);
                modelMatrixArm1 = glm::rotate(modelMatrixArm1, glm::radians(-rotateSpeed), vecRotate);
                modelMatrixArm1 = glm::translate(modelMatrixArm1, arm1pivot);
                
                modelMatrixArm2 = glm::translate(modelMatrixArm2, -arm2pivot);
                modelMatrixArm2 = glm::rotate(modelMatrixArm2, glm::radians(-rotateSpeed), vecRotate);
                modelMatrixArm2 = glm::translate(modelMatrixArm2, arm2pivot);

                modelMatrixRotateDetail = glm::translate(modelMatrixRotateDetail, -pivot);
                modelMatrixRotateDetail = glm::rotate(modelMatrixRotateDetail, glm::radians(-rotateSpeed), vecRotate);
                modelMatrixRotateDetail = glm::translate(modelMatrixRotateDetail, pivot);

                currentAngleRotate += rotateSpeed;
            }
        }

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        arm1Shader.use();
        arm1Shader.setUniformMatrix4fv("view", glm::value_ptr(view));
        arm1Shader.setUniformMatrix4fv("projection", glm::value_ptr(projection));
        arm1Shader.setUniformMatrix4fv("model", glm::value_ptr(modelMatrixArm1));
        arm1Model.Draw(arm1Shader);

        arm2Shader.use();
        arm2Shader.setUniformMatrix4fv("view", glm::value_ptr(view));
        arm2Shader.setUniformMatrix4fv("projection", glm::value_ptr(projection));
        arm2Shader.setUniformMatrix4fv("model", glm::value_ptr(modelMatrixArm2));
        arm2Model.Draw(arm2Shader);

        rollerShader.use();
        rollerShader.setUniformMatrix4fv("view", glm::value_ptr(view));
        rollerShader.setUniformMatrix4fv("projection", glm::value_ptr(projection));
        rollerShader.setUniformMatrix4fv("model", glm::value_ptr(modelMatrixRoller));
        rollerModel.Draw(rollerShader);

        mainColumnShader.use();
        mainColumnShader.setUniformMatrix4fv("view", glm::value_ptr(view));
        mainColumnShader.setUniformMatrix4fv("projection", glm::value_ptr(projection));
        mainColumnShader.setUniformMatrix4fv("model", glm::value_ptr(modelMatrixMainColumn));
        mainColumnModel.Draw(mainColumnShader);

        rotateDetailShader.use();
        rotateDetailShader.setUniformMatrix4fv("view", glm::value_ptr(view));
        rotateDetailShader.setUniformMatrix4fv("projection", glm::value_ptr(projection));
        rotateDetailShader.setUniformMatrix4fv("model", glm::value_ptr(modelMatrixRotateDetail));
        rotateDetailModel.Draw(rotateDetailShader);

        fieldShader.use();
        fieldShader.setUniformMatrix4fv("view", glm::value_ptr(view));
        fieldShader.setUniformMatrix4fv("projection", glm::value_ptr(projection));
        fieldShader.setUniformMatrix4fv("model", glm::value_ptr(modelMatrixField));
        fieldModel.Draw(fieldShader);

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

Shader getArm1Shader(glm::mat4 projection) {
    Shader shader;
    shader.loadFromFiles("shaders/vertex.txt", "shaders/fragment.txt");
    shader.use();
    shader.setUniformMatrix4fv("projection", glm::value_ptr(projection));

    shader.setUniform3f("material.ambient", 0.282f * 0.3f, 0.387f * 0.3f, 0.277f * 0.3f);
    shader.setUniform3f("material.diffuse", 0.282f, 0.387f, 0.277f);
    shader.setUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
    shader.setUniform1f("material.shininess", 32.0f);

    shader.setUniform3f("light.position", 2.0f, 5.0f, 3.0f);
    shader.setUniform3f("light.ambient", 0.1f, 0.1f, 0.1f);
    shader.setUniform3f("light.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

    return shader;
}

Shader getArm2Shader(glm::mat4 projection) {
    Shader shader;
    shader.loadFromFiles("shaders/vertex.txt", "shaders/fragment.txt");
    shader.use();
    shader.setUniformMatrix4fv("projection", glm::value_ptr(projection));

    shader.setUniform3f("material.ambient", 0.282f * 0.3f, 0.387f * 0.3f, 0.277f * 0.3f);
    shader.setUniform3f("material.diffuse", 0.282f, 0.387f, 0.277f);
    shader.setUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
    shader.setUniform1f("material.shininess", 32.0f);

    shader.setUniform3f("light.position", 2.0f, 5.0f, 3.0f);
    shader.setUniform3f("light.ambient", 0.1f, 0.1f, 0.1f);
    shader.setUniform3f("light.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

    return shader;
}

Shader getRollerShader(glm::mat4 projection) {
    Shader shader;
    shader.loadFromFiles("shaders/vertex.txt", "shaders/fragment.txt");
    shader.use();
    shader.setUniformMatrix4fv("projection", glm::value_ptr(projection));

    shader.setUniform3f("material.ambient", 0.474f * 0.3f, 0.474f * 0.3f, 0.474f * 0.3f);
    shader.setUniform3f("material.diffuse", 0.474f, 0.474f, 0.474f);
    shader.setUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
    shader.setUniform1f("material.shininess", 32.0f);

    shader.setUniform3f("light.position", 2.0f, 5.0f, 3.0f);
    shader.setUniform3f("light.ambient", 0.1f, 0.1f, 0.1f);
    shader.setUniform3f("light.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

    return shader;
}

Shader getMainColumnShader(glm::mat4 projection) {
    Shader shader;
    shader.loadFromFiles("shaders/vertex.txt", "shaders/fragment.txt");
    shader.use();
    shader.setUniformMatrix4fv("projection", glm::value_ptr(projection));

    shader.setUniform3f("material.ambient", 0.410f * 0.3f, 0.403f * 0.3f, 0.439f * 0.3f);
    shader.setUniform3f("material.diffuse", 0.410f, 0.403f, 0.439f);
    shader.setUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
    shader.setUniform1f("material.shininess", 32.0f);

    shader.setUniform3f("light.position", 2.0f, 5.0f, 3.0f);
    shader.setUniform3f("light.ambient", 0.1f, 0.1f, 0.1f);
    shader.setUniform3f("light.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

    return shader;
}

Shader getRotateDetailShader(glm::mat4 projection) {
    Shader shader;
    shader.loadFromFiles("shaders/vertex.txt", "shaders/fragment.txt");
    shader.use();
    shader.setUniformMatrix4fv("projection", glm::value_ptr(projection));

    shader.setUniform3f("material.ambient", 0.292f * 0.3f, 0.292f * 0.3f, 0.292f * 0.3f);
    shader.setUniform3f("material.diffuse", 0.292f, 0.292f, 0.292f);
    shader.setUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
    shader.setUniform1f("material.shininess", 32.0f);

    shader.setUniform3f("light.position", 2.0f, 5.0f, 3.0f);
    shader.setUniform3f("light.ambient", 0.1f, 0.1f, 0.1f);
    shader.setUniform3f("light.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

    return shader;
}

Shader getFieldShader(glm::mat4 projection) {
    Shader shader;
    shader.loadFromFiles("shaders/vertex.txt", "shaders/fragment.txt");
    shader.use();
    shader.setUniformMatrix4fv("projection", glm::value_ptr(projection));

    shader.setUniform3f("material.ambient", 0.108f * 0.3f, 0.153f * 0.3f, 0.124f * 0.3f);
    shader.setUniform3f("material.diffuse", 0.108f, 0.153f, 0.124f);
    shader.setUniform3f("material.specular", 0.3f, 0.3f, 0.3f);
    shader.setUniform1f("material.shininess", 8.0f);

    shader.setUniform3f("light.position", 2.0f, 5.0f, 3.0f);
    shader.setUniform3f("light.ambient", 0.1f, 0.1f, 0.1f);
    shader.setUniform3f("light.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

    return shader;
}

glm::vec3 getPositionFromMatrix(const glm::mat4& matrix) {
    return glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
}