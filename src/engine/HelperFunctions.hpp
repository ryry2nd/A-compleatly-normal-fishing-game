#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class HelperFunctions
{
public:
    virtual void clearBackground() = 0;

    virtual void swapBuffer() = 0;

    virtual ~HelperFunctions() = default;

protected:
    SDL_Window *window;
};

class Image
{
public:
    virtual ~Image() = default;

    virtual unsigned int getID() const = 0;
};

class Shader
{
public:
    virtual unsigned int getShader() const = 0;

    // deletes the thing
    virtual ~Shader() = default;
};