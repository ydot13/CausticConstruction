// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include<Windows.h>

// GLFW
#include <GLFW/glfw3.h>

// My includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Water.h"
#include "Utilitis.h"
#include "EnvironmentMap.h"
#include"Caustics.h"
#include"TBar.h"
#include"Drawable.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL.h>

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void check_size(GLFWwindow* window, GLuint&, GLuint&, GLuint&);
void genFBuffer(GLuint& FBO, GLuint& RBO, GLuint& Frame);
void Do_Movement();
void setFullscreen(bool fullscreen, GLFWwindow* window);
void DrawScene(glm::mat4& view, glm::mat4& projection, GLuint& skyboxVAO, GLuint& cubemapTexture);
void processCursorWaterIntersection(double x, double y);

// Properties
GLuint screenWidth = 800, screenHeight = 600;
GLuint screenWidthBeforeFSc, screenHeightBeforeFSc;
int screenPosXBeforeFSc, screenPosYBeforeFSc;
bool keys[1024];
bool firstMouse = true;
bool visibleMouse = false;
bool isFullScreen = false;
GLfloat lastX = 400, lastY = 300;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat waterUpdTimer = 0.0f;

// Interface
std::vector<TBar> bars;

// Camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
glm::mat4 light_view = glm::lookAt(glm::vec3(0.0f, 2.f, 0.0f), glm::vec3(0.0f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f));
glm::mat4 light_projection = glm::ortho(-1.f, 1.f, -1.f, 1.f, 0.1f, 4.f);

// Objects
std::shared_ptr<Water> water;
std::shared_ptr<EnvironmentMap> envMap;
std::shared_ptr<Caustics> caustics;
std::shared_ptr<Mesh> ground;

// Shaders
std::shared_ptr<Shader> objectShader;
std::shared_ptr<Shader> waterShader;
std::shared_ptr<Shader> skyboxShader;
std::shared_ptr<Shader> envMapShader;
std::shared_ptr<Shader> causticsShader;
std::shared_ptr<Model> rock;
std::shared_ptr<Model> submarine;

// Model matrices
glm::mat4 model_ground, model_rock, model_sub;
glm::mat4 model_ground_n, model_rock_n, model_sub_n;
// Proj matrix
glm::mat4 projection;

int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create window
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // App without console
    FreeConsole();

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Setup cursore
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) // for smooth movement of cursor
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);

    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Setup and compile our shaders
    objectShader   = std::make_shared<Shader>("shader\\object\\vertex_shader.glsl", "shader\\object\\fragment_shader.glsl");    
    waterShader    = std::make_shared<Shader>("shader\\water\\water_vertex.glsl", "shader\\water\\water_fragment.glsl");
    skyboxShader   = std::make_shared<Shader>("shader\\skybox\\skybox_shader_vt.glsl", "shader\\skybox\\skybox_shader_fr.glsl");
    envMapShader   = std::make_shared<Shader>("shader\\env_map\\env_map_vertex.glsl", "shader\\env_map\\env_map_fragment.glsl");
    causticsShader = std::make_shared<Shader>("shader\\caustics\\caustic_vertex.glsl", "shader\\caustics\\caustic_fragment.glsl");

    // SkyBox setup
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> faces =
    {
        "res\\skybox\\right.jpg",
            "res\\skybox\\left.jpg",
            "res\\skybox\\top.jpg",
            "res\\skybox\\bottom.jpg",
            "res\\skybox\\front.jpg",
            "res\\skybox\\back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader->Use();
    skyboxShader->SetInt("skybox", 0);

    // Ground setup
    std::vector<Vertex> gr_vr = {
        {glm::vec3(-1.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(0.f, 0.f)},
        {glm::vec3(1.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 0.f)},
        {glm::vec3(-1.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(0.f, 1.f)},
        {glm::vec3(1.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(1.f, 1.f)},
    };
    std::vector<unsigned int> ind = { 0, 1, 2, 1, 2, 3 };
    Texture sand;
    sand.id = loadTexture("res\\sand.jpg");
    sand.path = "res\\sand.jpg";
    sand.type = DIFFUSE;
    std::vector<Texture> t = {sand};
    ground = std::make_shared<Mesh>(gr_vr, ind, t);

    // Rock load
    rock = std::make_shared<Model>("res/rock/rock.obj");

    // Submarine load
    submarine = std::make_shared<Model>("res/submarine/Odyssey_OBJ.obj");

    // EnvironmentMap setup
    std::vector<std::shared_ptr<Drawable>> objects = { ground, rock, submarine};
    envMap = std::make_shared<EnvironmentMap>(objects, &screenWidth, &screenWidth, 1024);
    
    // Water setup
    water = std::make_shared<Water>(&screenWidth, &screenHeight, 256);

    // Caustic setup
    caustics = std::make_shared<Caustics>(&screenWidth, &screenHeight, 2048);

    // Setup FrameBuffer
    GLuint FBO, RBO, Frame;
    genFBuffer(FBO, RBO, Frame);

    // Setup Interface
    bars.push_back(TBar(-1, 0.8, 0.5f, 0.03f));

    // Matrices setup
    model_ground = glm::mat4(1.f);
    model_ground = glm::translate(model_ground, glm::vec3(0.f, -0.7f, 0.f));

    model_rock = glm::mat4(1.f);
    model_rock = glm::translate(model_rock, glm::vec3(-0.3, -0.7f, 0.f));
    model_rock = glm::rotate(model_rock, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    float scaleFactor = 1.f / 20;
    model_rock = glm::scale(model_rock, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

    model_sub = glm::mat4(1.f);
    model_sub = glm::translate(model_sub, glm::vec3(0.4f, -0.5f, 0.f));
    scaleFactor = 1.f / 1000;
    model_sub = glm::scale(model_sub, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

    envMap->models.push_back(model_ground);
    envMap->models.push_back(model_rock);
    envMap->models.push_back(model_sub);
    envMap->view = light_view;
    envMap->projection = light_projection;

    caustics->model = glm::mat4(1.f);
    caustics->projection = light_projection;
    caustics->view = light_view;

    glm::mat4 view;

    model_ground_n = glm::transpose(glm::inverse(glm::mat3(model_ground)));
    model_rock_n = glm::transpose(glm::inverse(glm::mat3(model_rock)));
    model_sub_n = glm::transpose(glm::inverse(glm::mat3(model_sub)));

    objectShader->Use();
    objectShader->SetMat4("lightProjection", light_projection);
    objectShader->SetMat4("lightView", light_view);

    waterShader->Use();
    waterShader->SetMat4("model", glm::mat4(1.f));

    // Game loop
    while (!glfwWindowShouldClose(window))
    {

        // Check window resizing
        check_size(window, FBO, RBO, Frame);

        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;

        //glfwSetWindowTitle(window, std::to_string(1.f / (deltaTime)).c_str()); //fps in title

        lastFrame = currentFrame;

        waterUpdTimer += deltaTime;

        // Update water + env_map + caustics
        if (waterUpdTimer > 0.032) {
            waterUpdTimer -= 0.032;
            // Upd water
            water->Update();           
            // Env upd
            envMap->Draw(*envMapShader);
            // Upd caustics
            caustics->Draw(*causticsShader, water->current_frame, envMap->Frame);
        }


        // Check and call events
        glfwPollEvents();
        Do_Movement();

        // Render no water geometry to framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        DrawScene(view, projection, skyboxVAO, cubemapTexture);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindTexture(GL_TEXTURE_2D, Frame);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Render no water geometry
        DrawScene(view, projection, skyboxVAO, cubemapTexture);
        
        // Render using frame buffer (Screen Space Refraction)
        view = camera.GetViewMatrix();
        waterShader->Use();
        waterShader->SetInt("skybox", 0);
        waterShader->SetInt("env", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Frame);
        
        waterShader->SetMat4("view", view);
        waterShader->SetMat4("projection", projection);
        waterShader->SetVec3("cameraPos", camera.Position);
        waterShader->SetFloat("turb", bars[0].getValue());

        water->Draw(*waterShader);

        // Render interface
        for(auto& bar : bars)
            bar.Show();
        
        // Swap the buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteTextures(1, &Frame);
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);
    glDeleteTextures(1, &cubemapTexture);

    glfwTerminate();
    return 0;
}


// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // ESC - close window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Setup key state
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }

    // Full screen state
    if (action == GLFW_PRESS && key == GLFW_KEY_F11) {
        isFullScreen = !isFullScreen;
        setFullscreen(isFullScreen, window);
    }
}

// Mouse movement callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Camera process
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    if(!visibleMouse)
        camera.ProcessMouseMovement(xoffset, yoffset);

    // Bars process
    if (keys[GLFW_MOUSE_BUTTON_LEFT]) {
        xpos = (xpos / screenWidth) * 2 - 1;
        ypos = -((ypos / screenHeight) * 2 - 1);
        for (auto& bar : bars) {
            bar.OnMove(xpos, ypos);
        }
    }
}

// Zoom
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// Mouse buttons callback
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Set button state
    if (action == GLFW_PRESS)
        keys[button] = true;
    if(action == GLFW_RELEASE)
        keys[button] = false;
    

    // Cacl cursor x, y and rescale [-1, 1]
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    x = (x / screenWidth) * 2 - 1;
    y = -((y / screenHeight) * 2 - 1);

    // Press left btn
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
        // Bars process        
        for (auto& bar : bars) {
            bar.OnMouseDown(x, y);
        }

        // Water add  drop
        processCursorWaterIntersection(x, y);
    }
    else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT){
        // Bars process
        for (auto& bar : bars) {
            bar.OnMouseUp(x, y);
        }
    }
    else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
        
        // Change control mode
        visibleMouse = !visibleMouse;
        if (visibleMouse) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

// Checking resizing of screen
void check_size(GLFWwindow* window, GLuint& FBO, GLuint& RBO, GLuint& Frame) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (screenHeight != height || screenWidth != width) {
        screenHeight = height;
        screenWidth = width;

        // Resize FBO for SSR of water
        glDeleteTextures(1, &Frame);
        glDeleteRenderbuffers(1, &RBO);
        glDeleteFramebuffers(1, &FBO);
        genFBuffer(FBO, RBO, Frame);

        // Set new viewport
        glViewport(0, 0, width, height);
    }
}

// Generate framebuffer, renderbuffer and frame textures for SSR
void genFBuffer(GLuint& FBO, GLuint& RBO, GLuint& Frame) {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    Frame = CreateTexture(screenWidth, screenHeight, GL_RGBA, GL_UNSIGNED_BYTE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Frame, 0);
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Render no water geometry
void DrawScene(glm::mat4& view, glm::mat4& projection, GLuint& skyboxVAO, GLuint& cubemapTexture) {
    // Clear the colorbuffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set view + proj matrices
    view = camera.GetViewMatrix();
    projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);

    // Draw ground
    objectShader->Use();
    objectShader->SetMat4("model", model_ground);
    objectShader->SetMat3("modelNormal", model_ground_n);
    objectShader->SetMat4("view", view);
    objectShader->SetMat4("projection", projection);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, caustics->Frame);
    objectShader->SetInt("caustics", 1);
    ground->Draw(*objectShader);

    // Draw rock
    objectShader->Use();
    objectShader->SetMat4("model", model_rock);
    objectShader->SetMat3("modelNormal", model_rock_n);
    objectShader->SetMat4("view", view);
    objectShader->SetMat4("projection", projection);
    rock->Draw(*objectShader);

    // Draw submarine
    objectShader->Use();
    objectShader->SetMat4("model", model_sub);
    objectShader->SetMat3("modelNormal", model_sub_n);
    objectShader->SetMat4("view", view);
    objectShader->SetMat4("projection", projection);
    submarine->Draw(*objectShader);

    //Draw skyBox
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    skyboxShader->Use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    skyboxShader->SetMat4("view", view);
    skyboxShader->SetMat4("projection", projection);

    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}

// Set full screen app
void setFullscreen(bool fullscreen, GLFWwindow* window)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    
    if (fullscreen) {
        screenHeightBeforeFSc = screenHeight;
        screenWidthBeforeFSc = screenWidth;
        glfwGetWindowPos(window, &screenPosXBeforeFSc, &screenPosYBeforeFSc);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }

    if (!fullscreen) {
        glfwSetWindowMonitor(window, nullptr, screenPosXBeforeFSc, screenPosYBeforeFSc, screenWidthBeforeFSc, screenHeightBeforeFSc, GLFW_DONT_CARE);
    }
}

// Check cursor water inersection and add drop
void processCursorWaterIntersection(double x, double y) {
    glm::vec4 sc2, sc1;
    if (!visibleMouse) {
        sc2 = glm::vec4(0, 0, 1.f, 1.f);
        sc1 = glm::vec4(0, 0, -1.f, 1.f);
    }
    else {
        sc2 = glm::vec4(x, y, 1.f, 1.f);
        sc1 = glm::vec4(x, y, -1.f, 1.f);
    }

    glm::mat4 invrs = glm::inverse(camera.GetViewMatrix()) * glm::inverse(glm::perspective(glm::radians(camera.Zoom), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f));
    glm::vec4 world1 = invrs * sc1; // point on nearest camera plane
    glm::vec4 world2 = invrs * sc2; // point on far camera plane
    world1 /= world1.w;
    world2 /= world2.w;

    glm::vec3 pos = world1; // start of ray
    glm::vec3 dir = world2 - world1; // direction of ray

    // water plane eq:0*x + 1*y + 0*z + 0 = 0
    // points on ray - start + alpha * dir
    // intersection with plane - start.y + alpha*dir.y = 0 => alpha = -start.y / dir.y 
    float alpha = -pos.y * 1.f / dir.y; 
    if (dir.y == 0.)
        alpha = -1;

    // x of intersection
    x = pos.x + alpha * dir.x;
    // y (z in our system) of intersection
    float z = pos.z + alpha * dir.z;

    // check intersection with water surface + check that water surface infornt of us NOT behiend (alpha > 0)
    if (alpha > 0 && x >= -1 && x <= 1 && z >= -1 && z <= 1) {
        water->AddDrop((x + 1) / 2, (z + 1) / 2);
    }
}