#include "CubeClass.h"

const GLfloat CubeClass::gravity = -9.8f;

CubeClass::CubeClass(glm::vec3 position, glm::vec3 size) : position(position), size(size), velocity(0.0f), deltaTime(0.0f), currentFrame(0.0f), lastFrame(0.0f) {}

glm::vec3 CubeClass::getPosition() const
{
    return position;
}

void CubeClass::update()
{
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Euler integration
    velocity += gravity * deltaTime;
    position.y += velocity * deltaTime;

    // Collision detection with the ground
    if (position.y < -0.5f) {
        position.y = -0.5f;
        velocity = -velocity * 0.8f;  // Bounce back with some energy loss
    }
}

void CubeClass::resolveCollision(const CubeClass& other) {
    // Very simple collision response: just reverse velocities
    velocity = -velocity;
    // You could add more complex collision response here, such as
    // calculating new velocities based on mass and momentum conservation.
}

bool CubeClass::checkCollision(const CubeClass& other) const {
    // AABB collision detection
    return position.x - size.x < other.position.x + other.size.x &&
        position.x + size.x > other.position.x - other.size.x &&
        position.y - size.y < other.position.y + other.size.y &&
        position.y + size.y > other.position.y - other.size.y &&
        position.z - size.z < other.position.z + other.size.z &&
        position.z + size.z > other.position.z - other.size.z;
}