//STD
#include <iostream>

//GLAD
#include <glad/glad.h>

//GLM
#include "glm/ext/vector_float3.hpp"
#include <glm/gtc/type_ptr.hpp> //Access to the value_ptr

//ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//SDL
#include <SDL.h>
//#include <SDL_ttf.h>

//LEARNOPENGL
#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

//GENERAL
#include "main.h"

using namespace std;
using namespace glm;

//Window
int windowWidth;
int windowHeight;

//VAO vertex attribute positions in correspondence to vertex attribute type
enum VAO_IDs { Triangles, Indices, Colours, Textures, NumVAOs = 2 };
//VAOs
GLuint VAOs[NumVAOs];

//Buffer types
enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };
//Buffer objects
GLuint Buffers[NumBuffers];

//Transformations
//Relative position within world space
vec3 cameraPosition = vec3(0.0f, 0.0f, 3.0f);
//The direction of travel
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
//Up position within world space
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

//Camera sidways rotation
float cameraYaw = -90.0f;
//Camera vertical rotation
float cameraPitch = 0.0f;
//Determines if first entry of mouse into window
bool mouseFirstEntry = true;
//Positions of camera from given last frame
float cameraLastXPos = 800.0f / 2.0f;
float cameraLastYPos = 600.0f / 2.0f;

//Time
//Time change
float deltaTime = 0.0f;
//Last value of time change
float lastFrame = 0.0f;

int main(int argc, char* argv[])
{
    //Initialisation of GLFW
    //glfwInit();
    //Initialisation of 'GLFWwindow' object
    windowWidth = 1280;
    windowHeight = 720;
    
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        cout << "SDL failed to initialise.\n";
        return -1;
    }

    /*SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);*/

    SDL_Window* window = SDL_CreateWindow("window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window)
    {
        cout << "SDL window failed to initialise.\n";
        return -1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == NULL)
    {
        cout << "SDL context failed to initialise.\n" << SDL_GetError() << endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        cout << "SDL renderer failed to initialise.\n";
        return -1;
    }

    /*SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);*/

    /*GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Lab5", NULL, NULL);

    //Checks if window has been successfully instantiated
    if (window == NULL)
    {
        cout << "GLFW Window did not instantiate\n";
        glfwTerminate();
        return -1;
    }

    //Sets cursor to automatically bind to window & hides cursor pointer
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Binds OpenGL to window
    glfwMakeContextCurrent(window);*/

    //Initialisation of GLAD
    //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        cout << "GLAD failed to initialise.\n";
        return -1;
    }

    //Loading of shaders
    Shader Shaders("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
    Model Rock("media/rock/Rock07-Base.obj");
    Shaders.use();

    //Sets the viewport size within the window to match the window size of 1280x720
    glViewport(0, 0, 1280, 720);

    //Sets the framebuffer_size_callback() function as the callback for the window resizing event
    /*glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Sets the mouse_callback() function as the callback for the mouse movement event
    glfwSetCursorPosCallback(window, mouse_callback);*/

    //Model matrix
    mat4 model = mat4(1.0f);
    //Scaling to zoom in
    model = scale(model, vec3(0.025f, 0.025f, 0.025f));
    //Looking straight forward
    model = rotate(model, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
    //Elevation to look upon terrain
    model = translate(model, vec3(0.0f, -2.f, -1.5f));

    //Projection matrix
    mat4 projection = perspective(radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

    //Render loop
    //while (glfwWindowShouldClose(window) == false)

    bool quit = false;
    SDL_Event Event;

    while (quit == false)
    {
        while (SDL_PollEvent(&Event) != 0)
        {
            //Time
            //float currentFrame = static_cast<float>(glfwGetTime());
            float currentFrame = static_cast<float>(SDL_GetTicks());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            //Input
            ProcessUserInput(Event, quit);
            //ProcessUserInput(window); //Takes user input

            //Rendering
            //SDL_SetRenderDrawColor(renderer, 64, 0, 255, 255);
            //SDL_RenderClear(renderer);

            //Rendering (old)
            glClearColor(0.25f, 0.0f, 1.0f, 1.0f); //Colour to display on cleared window
            glClear(GL_COLOR_BUFFER_BIT); //Clears the colour buffer
            glClear(GL_DEPTH_BUFFER_BIT); //Might need

            glEnable(GL_CULL_FACE); //Discards all back-facing triangles

            //Transformations
            mat4 view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp); //Sets the position of the viewer, the movement direction in relation to it & the world up direction
            mat4 mvp = projection * view * model;
            Shaders.setMat4("mvpIn", mvp); //Setting of uniform with Shader class

            //Drawing
            Rock.Draw(Shaders);

            //Refreshing
            //glfwSwapBuffers(window); //Swaps the colour buffer
            //glfwPollEvents(); //Queries all GLFW events

            //SDL_RenderPresent(renderer);
            SDL_GL_SwapWindow(window);

            /*if (Event.type == SDL_QUIT)
            {
                quit = true;
            }*/
        }
    }

    SDL_DestroyWindow(window);

    //Safely terminates GLFW
    //glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //Resizes window based on contemporary width & height values
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    //Initially no last positions, so sets last positions to current positions
    if (mouseFirstEntry)
    {
        cameraLastXPos = (float)xpos;
        cameraLastYPos = (float)ypos;
        mouseFirstEntry = false;
    }

    //Sets values for change in position since last frame to current frame
    float xOffset = (float)xpos - cameraLastXPos;
    float yOffset = cameraLastYPos - (float)ypos;

    //Sets last positions to current positions for next frame
    cameraLastXPos = (float)xpos;
    cameraLastYPos = (float)ypos;

    //Moderates the change in position based on sensitivity value
    const float sensitivity = 0.025f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    //Adjusts yaw & pitch values against changes in positions
    cameraYaw += xOffset;
    cameraPitch += yOffset;

    //Prevents turning up & down beyond 90 degrees to look backwards
    if (cameraPitch > 89.0f)
    {
        cameraPitch = 89.0f;
    }
    else if (cameraPitch < -89.0f)
    {
        cameraPitch = -89.0f;
    }

    //Modification of direction vector based on mouse turning
    vec3 direction;
    direction.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
    direction.y = sin(radians(cameraPitch));
    direction.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));
    cameraFront = normalize(direction);
}

void ProcessUserInput(SDL_Event& EventIn, bool& quitIn)
{
    //SDL_PollEvent(&EventIn);

    //SDL_PollEvent()
    //SDL_GetKeyName(EventIn->key.keysym.sym);

    /*if (Event.type == SDL_QUIT)
    {
        quitIn = true;
    }*/

    const Uint8* keyStates = SDL_GetKeyboardState(NULL);

    if (EventIn.type == SDL_KEYDOWN)
    {
        const float movementSpeed = 0.001f * deltaTime;

        /*if (keyStates[SDL_SCANCODE_ESCAPE]) { quitIn = true; }
        if (keyStates[SDL_SCANCODE_W])
        {
            cameraPosition += movementSpeed * cameraFront;
        }
        if (keyStates[SDL_SCANCODE_S]) { cameraPosition -= movementSpeed * cameraFront; }
        if (keyStates[SDL_SCANCODE_A]) { cameraPosition -= normalize(cross(cameraFront, cameraUp)) * movementSpeed; }
        if (keyStates[SDL_SCANCODE_D]) { cameraPosition += normalize(cross(cameraFront, cameraUp)) * movementSpeed; }*/

        if (EventIn.key.keysym.sym == SDLK_ESCAPE) { quitIn = true; }
        if (EventIn.key.keysym.sym == SDLK_w) { cameraPosition += movementSpeed * cameraFront; }
        if (EventIn.key.keysym.sym == SDLK_s) { cameraPosition -= movementSpeed * cameraFront; }
        if (EventIn.key.keysym.sym == SDLK_a) { cameraPosition -= normalize(cross(cameraFront, cameraUp)) * movementSpeed; }
        if (EventIn.key.keysym.sym == SDLK_d) { cameraPosition += normalize(cross(cameraFront, cameraUp)) * movementSpeed; }
    }
}

void ProcessUserInputGLFW(GLFWwindow* WindowIn)
{
    //Closes window on 'exit' key press
    if (glfwGetKey(WindowIn, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(WindowIn, true);
    }

    //Extent to which to move in one instance
    const float movementSpeed = 1.0f * deltaTime;
    //WASD controls
    if (glfwGetKey(WindowIn, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPosition += movementSpeed * cameraFront;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPosition -= movementSpeed * cameraFront;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPosition -= normalize(cross(cameraFront, cameraUp)) * movementSpeed;
    }
    if (glfwGetKey(WindowIn, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPosition += normalize(cross(cameraFront, cameraUp)) * movementSpeed;
    }
}