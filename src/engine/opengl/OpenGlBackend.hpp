#pragma once

#include <vector>
#include "../Backend.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class OpenGlBackend : public Backend
{
public:
    OpenGlBackend() : VAO(0), VBO(0) {}

    ~OpenGlBackend()
    {
        if (VAO != 0)
            glDeleteVertexArrays(1, &VAO);
        if (VBO != 0)
            glDeleteBuffers(1, &VBO);
    }

    void setupObject(const std::vector<float> &vertices)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Normal attribute
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    void updateVerts(const std::vector<float> &vertices)
    {
        // updates the vertices (you dont need to run this unless you changed the vertices)
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    }

    void includeShader(Shader *shader)
    {
        this->shader = shader;
        glUseProgram(shader->getShader());
    }

    void includeMat4(const std::string &name, const glm::mat4 &mat)
    {
        glUniformMatrix4fv(glGetUniformLocation(shader->getShader(), name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void includeTexture(Image *image)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, image->getID());

        GLuint texLoc = glGetUniformLocation(shader->getShader(), "texture1");
        if (texLoc != -1)
            glUniform1i(texLoc, 0);
    }

    void includeFloat(const std::string &location, const float f)
    {
        glUniform1f(glGetUniformLocation(shader->getShader(), location.c_str()), f);
    }

    void includeTripleFloat(const std::string &location, const float f1, const float f2, const float f3)
    {
        glUniform3f(glGetUniformLocation(shader->getShader(), location.c_str()), f1, f2, f3);
    }

    void includeInt(const std::string &location, const int i)
    {
        glUniform1i(glGetUniformLocation(shader->getShader(), location.c_str()), i);
    }

    void finalizeShaders(const std::vector<float> &vertices)
    {
        int floatsPerVertex = 5;
        int totalVertices = vertices.size() / floatsPerVertex;
        int triangles = totalVertices / 3;

        glBindVertexArray(VAO); // ngl who knows what this crap means, according to the names it applies and binds stuff
        glDrawArrays(GL_TRIANGLES, 0, totalVertices);
        glBindVertexArray(0);
    }

private:
    GLuint VAO, VBO;
};