#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <memory>
#include "compatibility/Backend.hpp"
#include "compatibility/opengl/OpenGlBackend.hpp"
#include "compatibility/HelperFunctions.hpp"
#include "Camera.hpp"
#include "customMath/BigVec.hpp"

class RenderObject
{
public:
    RenderObject(Backend *backend, Shader *shady, Image *im, Camera *cam, BigVec3 pos = BigVec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 scl = glm::vec3(1.0f));
    ~RenderObject();

    void Update(float deltaTime);
    void Draw();

    BigVec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    // change near and far values if you want to have big objects, if you change them to the right value it could be as big as the floating points will allow
    float near = 0.1f;
    float far = 10000.0f;

    RenderObject *parent = nullptr;

private:
    Backend *backend;
    Shader *shader;
    Image *image;
    Camera *camera;
    void setupObject();
    float nearCullFunction() const;
    glm::mat4 getModelMatrix() const;
    std::vector<float> vertices;
};
