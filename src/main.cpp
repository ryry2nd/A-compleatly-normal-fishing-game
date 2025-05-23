#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/RenderObject.h"
#include "engine/HelperFunctions.hpp"
#include "engine/Camera.hpp"
#include "engine/opengl/OpenGlBackend.hpp"
#include "engine/opengl/HelperFunctionsOpengl.hpp"
#include <string>
#include <memory>

class Sun : public RenderObject
{
public:
    Sun(Shader *shader, Image *image, Camera *camera)
        : RenderObject(new OpenGlBackend(), shader, image, camera, glm::vec3(1.0f), Bigint("384600000000000000000000000"))
    {
        scale *= Bigint("150000000000");
        near = 100000;
        far = 1000000000000;
    }
};

int main(int argc, char *argv[])
{
    // it initialises sdl
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    // this makes the window
    const glm::vec2 RES{800, 600};
    SDL_Window *window = SDL_CreateWindow("Game", 100, 100, RES.x, RES.y, SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }
    HelperFunctions *renderingEngine = new HelperFunctionsOpenGl(window);

    SDL_SetRelativeMouseMode(SDL_TRUE); // hides the mouse

    // this is the constants
    const float MOUSE_SENSITIVITY = 2;
    const float WALK_SPEED = 10;
    const float RUN_SPEED = 100;

    // put objects in here to render them
    std::vector<RenderObject *> renderObjects;

    // uuhhh, this is for fun, in case i want to make things a googl meters apart, put whatever number here, see what happens, its pritty cool
    std::string pos = "0";

    for (int i = 0; i < 0; i++)
    {
        pos += "0";
    }

    // this is the camera, cameras are neat
    Camera *camera = new Camera(RES, Bigint(pos), 0.0f, -2.0f);
    float speed = 10;

    // this sets up the shader and texture
    Shader *shader = new ShaderOpenGl("assets/shaders/nearVertex.glsl", "assets/shaders/nearFragment.glsl");
    Image *image = new ImageOpenGl("assets/textures/FISH.png");

    // makes the cubes
    RenderObject cube(new OpenGlBackend(), shader, image, camera);
    // cube.velocity.z = 5;
    renderObjects.push_back(&cube);

    cube.disableBrightness = true;

    RenderObject cube2(new OpenGlBackend(), shader, image, camera);
    cube2.position.x -= Bigint(10);
    renderObjects.push_back(&cube2);

    RenderObject cube3(new OpenGlBackend(), shader, image, camera, glm::vec3(1.0f), 10.0f);
    renderObjects.push_back(&cube3);
    cube3.position.x += Bigint("10");

    Sun sun(shader, image, camera);
    renderObjects.push_back(&sun);
    sun.position.x -= Bigint("150000000000"); // Bigint("21392000000");

    // starts running the game loop
    bool running = true;
    Uint32 lastTicks = SDL_GetTicks();
    Uint32 currentTicks;
    SDL_Event event;
    float deltaTime;

    glm::mat4 cameraMatrixTmp;

    int i;

    while (running)
    {
        currentTicks = SDL_GetTicks();
        deltaTime = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;

        // gets events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;

            // rotates camera
            if (event.type == SDL_MOUSEMOTION)
            {
                camera->yaw -= event.motion.xrel * deltaTime * MOUSE_SENSITIVITY;
                camera->pitch -= event.motion.yrel * deltaTime * MOUSE_SENSITIVITY;
            }

            // when you press escape, leave
            if (event.type == SDL_KEYDOWN)
            {

                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                }
            }
        }

        // gets all keystates
        const Uint8 *keystates = SDL_GetKeyboardState(NULL);

        // if your running, run, otherwise dont
        speed = keystates[SDL_SCANCODE_LSHIFT] ? RUN_SPEED : WALK_SPEED;

        // movement
        if (keystates[SDL_SCANCODE_W])
        {
            glm::vec3 forward = camera->getForwardVector();
            camera->position += (forward * deltaTime * speed);
        }
        if (keystates[SDL_SCANCODE_S])
        {
            glm::vec3 forward = camera->getForwardVector();
            camera->position -= (forward * deltaTime * speed);
        }

        if (keystates[SDL_SCANCODE_D])
        {
            glm::vec3 right = camera->getRightVector();
            camera->position += (right * deltaTime * speed);
        }
        if (keystates[SDL_SCANCODE_A])
        {
            glm::vec3 right = camera->getRightVector();
            camera->position -= (right * deltaTime * speed);
        }

        if (keystates[SDL_SCANCODE_SPACE])
        {
            glm::vec3 down = camera->getDownVector();
            camera->position -= (down * deltaTime * speed);
        }
        if (keystates[SDL_SCANCODE_LCTRL])
        {
            glm::vec3 down = camera->getDownVector();
            camera->position += (down * deltaTime * speed);
        }

        // update all objects
        for (i = 0; i < renderObjects.size(); i++)
        {
            renderObjects[i]->Update(deltaTime);
        }

        // clear background
        renderingEngine->clearBackground();

        // draw all objects
        for (i = 0; i < renderObjects.size(); i++)
        {
            renderObjects[i]->Draw();
        }

        // swap buffer
        renderingEngine->swapBuffer();
    }

    // delete everything
    delete shader;
    delete image;
    delete renderingEngine;
    delete camera;
    return 0;
}