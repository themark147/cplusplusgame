#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>

class CubeClass
{
    public:
        CubeClass(glm::vec3 position, glm::vec3 size);
        glm::vec3 getPosition() const;
        void update();
    private:
        void resolveCollision(const CubeClass& other);
        bool checkCollision(const CubeClass& other) const;

        glm::vec3 position;
        glm::vec3 size;
        GLfloat velocity;
        GLfloat deltaTime;
        GLfloat currentFrame;
        GLfloat lastFrame;

        static const GLfloat gravity;
};

