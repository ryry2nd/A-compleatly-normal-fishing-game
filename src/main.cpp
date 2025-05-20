#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/RenderObject.h"
#include "engine/Shader.h"
#include "engine/Image.h"
#include "engine/Camera.hpp"
#include <string>

int main(int argc, char *argv[])
{
    // it initialises sdl
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    // this does what it says
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // this makes the window
    const glm::vec2 RES{800, 600};
    SDL_Window *window = SDL_CreateWindow("Game", 100, 100, RES.x, RES.y, SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    // this makes the opengl content
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW\n";
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // this turns on all the cool opengl settings that probably should be the default
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_STENCIL_TEST);

    SDL_SetRelativeMouseMode(SDL_TRUE); // hides the mouse

    // this is the constants
    const float MOUSE_SENSITIVITY = 2;
    const float WALK_SPEED = 10;
    const float RUN_SPEED = 100;

    // put objects in here to render them
    std::vector<RenderObject *> renderObjects;

    // uuhhh, this is for fun, in case i want to make things a googl meters apart, put whatever number here, see what happens, its pritty cool
    std::string pos = "1";

    for (int i = 0; i < 1; i++)
    {
        pos += "0";
    }

    // this is the camera, cameras are neat
    Camera camera(RES, Bigint(pos), 0.0f, -2.0f);
    float speed = 10;

    // this sets up the shader and texture
    Shader *shader = new Shader("assets/shaders/cubeVertex.glsl", "assets/shaders/cubeFragment.glsl");
    Image *image = new Image("assets/textures/FISH.png");

    // makes the cubes
    RenderObject cube(shader, image, &camera);
    cube.position.x += Bigint(pos);

    renderObjects.push_back(&cube);

    RenderObject cube2(shader, image, &camera);

    renderObjects.push_back(&cube2);

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
                camera.yaw -= event.motion.xrel * deltaTime * MOUSE_SENSITIVITY;
                camera.pitch -= event.motion.yrel * deltaTime * MOUSE_SENSITIVITY;
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
            glm::vec3 forward = camera.getForwardVector();
            camera.position += (forward * deltaTime * speed);
        }
        if (keystates[SDL_SCANCODE_S])
        {
            glm::vec3 forward = camera.getForwardVector();
            camera.position -= (forward * deltaTime * speed);
        }

        if (keystates[SDL_SCANCODE_D])
        {
            glm::vec3 right = camera.getRightVector();
            camera.position += (right * deltaTime * speed);
        }
        if (keystates[SDL_SCANCODE_A])
        {
            glm::vec3 right = camera.getRightVector();
            camera.position -= (right * deltaTime * speed);
        }

        if (keystates[SDL_SCANCODE_SPACE])
        {
            glm::vec3 down = camera.getDownVector();
            camera.position -= (down * deltaTime * speed);
        }
        if (keystates[SDL_SCANCODE_LCTRL])
        {
            glm::vec3 down = camera.getDownVector();
            camera.position += (down * deltaTime * speed);
        }

        // update all objects
        for (i = 0; i < renderObjects.size(); i++)
        {
            renderObjects[i]->Update(deltaTime);
        }

        // clear background
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw all objects
        for (i = 0; i < renderObjects.size(); i++)
        {
            renderObjects[i]->Draw();
        }

        // swap buffer
        SDL_GL_SwapWindow(window);
    }

    // delete everything
    delete shader;
    delete image;
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}