#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/stb_image.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/ext/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <chrono>
#include <reactphysics3d/reactphysics3d.h>

#include "Camera.h"

#include "Mesh/Object.h"
#include "shadeer.h"
#include "Mesh.h"
#include "Model.h"

#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window, PhysicsCommon& common, PhysicsWorld* world);
void createBox(PhysicsCommon& common, PhysicsWorld* world);
void drawDebug(DebugRenderer& debugRenderer, uint vertexPositionLoc, uint vertexColorLoc);

int widthScreen = 1024;
int heightScreen = 768;

Camera camera(glm::vec3(0.0f, 0.0f, 15.0f));
float lastX = widthScreen / 2.0f;
float lastY = heightScreen / 2.0f;
bool firstMouse = true;

std::vector<Object*> boxes;

using chrono_clock = std::chrono::high_resolution_clock;

std::chrono::time_point<chrono_clock> mStartTime;
std::chrono::time_point<std::chrono::high_resolution_clock> mLastUpdateTime;

/// Used to fix the time step and avoid strange time effects
std::chrono::duration<double> mAccumulator;
std::chrono::duration<double> deltaTime;

bool F_pressed = false;
bool TAB_pressed = false;

/// Vertex Buffer Object for the debug info lines vertices data
openglframework::VertexBufferObject mDebugVBOLinesVertices(GL_ARRAY_BUFFER);

/// Vertex Array Object for the lines vertex data
openglframework::VertexArrayObject mDebugLinesVAO;

/// Vertex Buffer Object for the debug info trinangles vertices data
openglframework::VertexBufferObject mDebugVBOTrianglesVertices(GL_ARRAY_BUFFER);

/// Vertex Array Object for the triangles vertex data
openglframework::VertexArrayObject mDebugTrianglesVAO;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(widthScreen, heightScreen, "React physics test", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    mDebugVBOLinesVertices.create();

    // Create the VAO for both VBOs
    mDebugLinesVAO.create();
    mDebugLinesVAO.bind();

    // Bind the VBO of vertices
    mDebugVBOLinesVertices.bind();

    // Unbind the VAO
    mDebugLinesVAO.unbind();

    mDebugVBOLinesVertices.unbind();

    // ----- Triangles ----- //

    // Create the VBO for the vertices data
    mDebugVBOTrianglesVertices.create();

    // Create the VAO for both VBOs
    mDebugTrianglesVAO.create();
    mDebugTrianglesVAO.bind();

    // Bind the VBO of vertices
    mDebugVBOTrianglesVertices.bind();

    // Unbind the VAO
    mDebugTrianglesVAO.unbind();

    mDebugVBOTrianglesVertices.unbind();

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    Model mineModel("resource/backpack.obj", glm::vec3(0.0f, 0.0f, 0.0f));
    Model floorModel("resource/sphere.obj", glm::vec3(0.0f, 0.0f, 0.0f));

    Shader mainShader("Source/6.2.coordinate_systems.vs", "Source/6.2.coordinate_systems.fs");
    Shader lightingShader("Source/light_casters.vs", "Source/light_casters.fs");
    PhysicsCommon physicsCommon;

    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    lightingShader.setInt("material.roughness", 3);

    // Create a physics world
    PhysicsWorld* world = physicsCommon.createPhysicsWorld();
    world->setIsDebugRenderingEnabled(true);

    DebugRenderer& debugRenderer = world->getDebugRenderer();

    // Select the contact points and contact normals to be displayed
    debugRenderer.setIsDebugItemDisplayed(DebugRenderer::DebugItem::COLLISION_SHAPE, true);

    std::chrono::duration<double> timeStep = std::chrono::duration<double>(1.0f / 60.0f);

    mStartTime = std::chrono::high_resolution_clock::now();
    mLastUpdateTime = mStartTime;
    mAccumulator = std::chrono::duration<double>::zero();

    Object* floor = new Object(glm::vec3(0, -10, 0));
    floor->create(physicsCommon, world, BodyType::STATIC, Vector3(10, 1, 10));

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processInput(window, physicsCommon, world);

        rp3d::DebugRenderer& debugRenderer = world->getDebugRenderer();

        // ----- Triangles ---- //
        const uint nbTriangles = debugRenderer.getNbTriangles();

        // std::cout << nbTriangles << std::endl;

        glClearColor(0.15f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (nbTriangles > 0)
        {
            // Vertices
            mDebugVBOTrianglesVertices.bind();
            GLsizei sizeVertices = static_cast<GLsizei>(nbTriangles * sizeof(rp3d::DebugRenderer::DebugTriangle));
            mDebugVBOTrianglesVertices.copyDataIntoVBO(sizeVertices, debugRenderer.getTrianglesArray(), GL_STREAM_DRAW);
            mDebugVBOTrianglesVertices.unbind();
        }

        mainShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)widthScreen / (float)heightScreen, 0.1f, 100.0f);
        mainShader.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        mainShader.setMat4("view", view);

        std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();
        deltaTime = currentTime - mLastUpdateTime;

        // Update the current display time
        mLastUpdateTime = currentTime;
        mAccumulator += deltaTime;

        while (mAccumulator >= timeStep) {
            world->update(timeStep.count());

            mAccumulator -= timeStep;
        }
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, floor->getPosition());
        mainShader.setMat4("model", floor->getRotationMatrix());
        floorModel.Draw(mainShader);

        int vertexPositionLoc = mainShader.getAttribLocation("aPos");
        int vertexColorLoc = mainShader.getAttribLocation("vertexColor");

        // Triangles
        if (nbTriangles > 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            drawDebug(debugRenderer, vertexPositionLoc, vertexColorLoc);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        for (std::vector<Object*>::iterator it = boxes.begin(); it != boxes.end(); ++it)
        {
            lightingShader.use();

            lightingShader.setVec3("light.position", 0.0f, 7.0f, 0.0f);
            lightingShader.setVec3("camPos", camera.Position);

            // light properties
            lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
            lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
            lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

            // material properties
            lightingShader.setFloat("material.shininess", 32.0f);

            lightingShader.setMat4("projection", projection);
            lightingShader.setMat4("view", view);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, (*it)->getPosition());
            mainShader.setMat4("model", model * (*it)->getRotationMatrix());
            lightingShader.setMat4("model", model * (*it)->getRotationMatrix());
            // Get the location of shader attribute variables
            mineModel.Draw(mainShader);
            mineModel.Draw(lightingShader);
        }

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}

void processInput(GLFWwindow* window, PhysicsCommon& common, PhysicsWorld* world)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && F_pressed == false) {
        createBox(common, world);
        F_pressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && F_pressed == true) {
        F_pressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime.count());
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime.count());
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime.count());
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime.count());
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    widthScreen = width;
    heightScreen = height;
    glViewport(0, 0, width, height);
}

void createBox(PhysicsCommon& common, PhysicsWorld* world)
{
    glm::vec3 spawnPosition = (camera.Front * glm::vec3(15)) + camera.Position;
    Object* object = new Object(spawnPosition);

    boxes.push_back(object);
    object->create(common, world, BodyType::DYNAMIC, Vector3(1.5, 1.5, 1.5));
    // object->getRigidBody()->applyLocalForceAtLocalPosition(Vector3(1000, 1000, 1000) * Vector3(camera.Front.x, camera.Front.y, camera.Front.z), Vector3(0.15, 0.7, 1.5));
}

void drawDebug(DebugRenderer& debugRenderer, uint vertexPositionLoc, uint vertexColorLoc)
{
    // Bind the VAO
    mDebugTrianglesVAO.bind();

    mDebugVBOTrianglesVertices.bind();

    glVertexAttribPointer(vertexPositionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(rp3d::Vector3) + sizeof(rp3d::uint32), (char*)nullptr);
    glEnableVertexAttribArray(vertexPositionLoc);

    glVertexAttribIPointer(vertexColorLoc, 3, GL_UNSIGNED_INT, sizeof(rp3d::Vector3) + sizeof(rp3d::uint32), (void*)sizeof(rp3d::Vector3));
    glEnableVertexAttribArray(vertexColorLoc);

    // Draw the triangles geometry
    glDrawArrays(GL_TRIANGLES, 0, debugRenderer.getNbTriangles() * 3);

    glDisableVertexAttribArray(vertexPositionLoc);
    glDisableVertexAttribArray(vertexColorLoc);

    mDebugVBOTrianglesVertices.unbind();

    // Unbind the VAO
    mDebugTrianglesVAO.unbind();
}