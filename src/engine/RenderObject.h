#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <memory>
#include "Backend.hpp"
#include "HelperFunctions.hpp"
#include "Camera.hpp"
#include "customMath/BigVec.hpp"

struct Light
{
    BigVec3 &position;
    glm::vec3 color;
    Bigint intensity;
};

class RenderObject
{
public:
    RenderObject(Backend *backend, Shader *shady, Image *im, Camera *cam, glm::vec3 emissionColor = glm::vec3(0, 0, 0), Bigint emissionIntensity = Bigint(), BigVec3 pos = BigVec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 scl = glm::vec3(1.0f));
    ~RenderObject();

    void Update(float deltaTime);
    void Draw();

    BigVec3 position;
    glm::vec3 rotation;
    BigVec3 scale;

    BigVec3 velocity;
    BigVec3 acceleration;

    // change near and far values if you want to have big objects, if you change them to the right value it could be as big as the floating points will allow
    float near = 0.1f;
    float far = 10000.0f;

    static float gamma;
    static bool disableBrightness;

protected:
    void
    addVarsToShader();
    RenderObject *parent = nullptr;
    void setupObject();
    float nearCullFunction() const;
    glm::mat4 getModelMatrix() const;
    BigVec3 tempLocalPosition;

private:
    Backend *backend;
    Shader *shader;
    Image *image;
    Camera *camera;
    Light *thisLight = nullptr;
    std::vector<float> vertices;
    Bigint calculateInverseSquareLaw(const BigVec3 &subtractedPos, const Bigint &intensity) const;
    Bigint calculateDistanceSquared(const BigVec3 &subtractedPos) const;

    static std::vector<Light *> allLights;
};
