#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include "Shader.h"
#include "Image.h"
#include "Camera.hpp"
#include "BigVec.hpp"

class RenderObject
{
public:
    RenderObject(Shader *shady, Image *im, Camera *cam);
    RenderObject(Shader *shady, Image *im, Camera *cam, BigVec3 pos, glm::vec3 rot, glm::vec3 scl);
    ~RenderObject();

    void Update(float deltaTime);
    void Draw();

    BigVec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    // change near and far values if you want to have big objects, if you change them to the right value it could be as big as the floating points will allow
    float near = 0.1f;
    float far = 10000.0f;

    RenderObject *parent;

private:
    GLuint VAO, VBO;
    Shader *shader;
    Image *image;
    Camera *camera;
    void setupObject();
    void updateVerts();
    float nearCullFunction() const;
    glm::mat4 getModelMatrix() const;
    std::vector<float> vertices;
};
