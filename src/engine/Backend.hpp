#pragma once

#include <vector>
#include <string>
#include "HelperFunctions.hpp"

class Backend
{
public:
    virtual ~Backend() = default;

    virtual void setupObject(const std::vector<float> &verts) = 0;
    virtual void updateVerts(const std::vector<float> &verts) = 0;
    virtual void includeShader(Shader *shader) = 0;
    virtual void includeMvp(glm::mat4 &mvp) = 0;
    virtual void includeTexture(Image *image) = 0;
    virtual void includeFloat(std::string location, float f) = 0;
    virtual void finalizeShaders(const std::vector<float> &vertices) = 0;

protected:
    Shader *shader = nullptr;
};