#include "RenderObject.h"

void addFace(std::vector<float> &verts,
             glm::vec3 vert0,
             glm::vec3 vert1,
             glm::vec3 vert2,
             glm::vec3 vert3,
             glm::vec2 uvMin,
             glm::vec2 uvMax)
{
    glm::vec2 uv0 = {uvMin.x, uvMax.y}; // bottom-left
    glm::vec2 uv1 = {uvMax.x, uvMax.y}; // bottom-right
    glm::vec2 uv2 = {uvMax.x, uvMin.y}; // top-right
    glm::vec2 uv3 = {uvMin.x, uvMin.y}; // top-left

    // Triangle 1
    verts.insert(verts.end(), {vert0.x, vert0.y, vert0.z, uv0.x, uv0.y});
    verts.insert(verts.end(), {vert1.x, vert1.y, vert1.z, uv1.x, uv1.y});
    verts.insert(verts.end(), {vert2.x, vert2.y, vert2.z, uv2.x, uv2.y});

    // Triangle 2
    verts.insert(verts.end(), {vert2.x, vert2.y, vert2.z, uv2.x, uv2.y});
    verts.insert(verts.end(), {vert3.x, vert3.y, vert3.z, uv3.x, uv3.y});
    verts.insert(verts.end(), {vert0.x, vert0.y, vert0.z, uv0.x, uv0.y});
}

std::vector<float> makeTexturedCube(float size = 1.0f)
{
    std::vector<float> verts;
    float s = size / 2.0f;

    // Front face (+Z)
    addFace(verts, {-s, -s, s}, {s, -s, s}, {s, s, s}, {-s, s, s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    // Back face (-Z)
    addFace(verts, {s, -s, -s}, {-s, -s, -s}, {-s, s, -s}, {s, s, -s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    // Left face (-X)
    addFace(verts, {-s, -s, -s}, {-s, -s, s}, {-s, s, s}, {-s, s, -s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    // Right face (+X)
    addFace(verts, {s, -s, s}, {s, -s, -s}, {s, s, -s}, {s, s, s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    // Top face (+Y)
    addFace(verts, {-s, s, s}, {s, s, s}, {s, s, -s}, {-s, s, -s}, {0.0f, 0.0f}, {1.0f, 1.0f});
    // Bottom face (-Y)
    addFace(verts, {-s, -s, -s}, {s, -s, -s}, {s, -s, s}, {-s, -s, s}, {0.0f, 0.0f}, {1.0f, 1.0f});

    return verts;
}

RenderObject::RenderObject(Backend *backend, Shader *shady, Image *im, Camera *cam, BigVec3 pos, glm::vec3 rot, glm::vec3 scl)
    : position(pos), rotation(rot), scale(scl), shader(shady), image(im), camera(cam)
{
    this->backend = backend;

    vertices = makeTexturedCube();
    setupObject();
}

RenderObject::~RenderObject()
{
    delete backend;
}

void RenderObject::setupObject()
{
    backend->setupObject(vertices);
}

glm::mat4 RenderObject::getModelMatrix() const
{
    glm::mat4 model = glm::mat4(1.0f);

    // converts the position to be local to the camera
    glm::vec3 relativePos = camera->convertToLocal(position);
    model = glm::translate(model, relativePos);

    // rotates the model
    model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));

    // scales the model
    model = glm::scale(model, scale);
    return model;
}

// I just made the default update to rotate all around
void RenderObject::Update(float deltaTime)
{
    rotation.y -= deltaTime;
    rotation.x -= deltaTime;
    rotation.z -= deltaTime;
}

// it culls everything close and its different depending on the near value
float RenderObject::nearCullFunction() const
{
    return near <= 0.1f ? 0.0f : 100.0f;
}

void RenderObject::Draw()
{
    // it gets those camera values and then the mvp
    glm::mat4 viewProj = camera->getProjectionMatrix(near, far);
    glm::mat4 viewMatrix = camera->getViewMatrix();
    glm::mat4 mvp = viewProj * viewMatrix * getModelMatrix();

    backend->includeShader(shader);
    backend->includeMvp(mvp);
    backend->includeFloat("u_CullRadius", nearCullFunction());
    backend->includeTexture(image);
    backend->finalizeShaders(vertices);
}