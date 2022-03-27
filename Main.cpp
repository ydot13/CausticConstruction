// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include<Windows.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Water.h"
#include "Utilitis.h"
#include "EnvironmentMap.h"
#include"Caustics.h"
#include"TBar.h"


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

// Properties
GLuint screenWidth = 800, screenHeight = 600;

std::vector<TBar> bars;


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void check_size(GLFWwindow* window, EnvironmentMap&, GLuint&, GLuint&, GLuint&);

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
void Do_Movement();
unsigned int loadTexture(char const* path);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400 , lastY = 300;
bool firstMouse = true;
bool visibleMouse = false;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat Last = 0.0f;

glm::mat4 light_view = glm::lookAt(glm::vec3(0.0f, 2.f, 0.0f), glm::vec3(0.0f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f));
glm::mat4 light_projection = glm::ortho(-1.f, 1.f, 1.f, -1.f, -10.f, 10.f);

Water* water;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);
    //FreeConsole();

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);

    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Setup and compile our shaders
    Shader objectShader("shader\\object\\vertex_shader.glsl", "shader\\object\\fragment_shader.glsl");
    
    Shader waterShader("shader\\water\\water_vertex.glsl", "shader\\water\\water_fragment.glsl");

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

    Shader skyboxShader("shader\\skybox\\skybox_shader_vt.glsl","shader\\skybox\\skybox_shader_fr.glsl");

    skyboxShader.Use();
    skyboxShader.SetInt("skybox", 0);

    std::vector<Vertex> gr_vr = {
        {glm::vec3(-1.f, 0.f, -1.f), glm::vec3(), glm::vec2(0.f, 0.f)},
        {glm::vec3(1.f, 0.f, -1.f), glm::vec3(), glm::vec2(1.f, 0.f)},
        {glm::vec3(-1.f, 0.f, 1.f), glm::vec3(), glm::vec2(0.f, 1.f)},
        {glm::vec3(1.f, 0.f, 1.f), glm::vec3(), glm::vec2(1.f, 1.f)},
    };
    Mesh ground(gr_vr, { 0, 1, 2, 1, 2, 3 }, {});

    EnvironmentMap envMap({ ground }, &screenWidth, &screenWidth, 512);
    
    Shader envMapShader("shader\\env_map\\env_map_vertex.glsl", "shader\\env_map\\env_map_fragment.glsl");
    
    water = new Water(&screenWidth, &screenHeight, 128);

    Caustics caustics(water->waterGrid, &screenWidth, &screenHeight, 512);

    Shader causticsShader("shader\\caustics\\caustic_vertex.glsl", "shader\\caustics\\caustic_fragment.glsl");


    GLuint FBO, RBO, Frame;
    genFBuffer(FBO, RBO, Frame);

    bars.push_back(TBar(-1, 0.8, 0.5f, 0.03f));

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        check_size(window, envMap, FBO, RBO, Frame);

        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        
        
        deltaTime = currentFrame - lastFrame;

        glfwSetWindowTitle(window, std::to_string(1.f / (deltaTime)).c_str());
        lastFrame = currentFrame;

        Last += deltaTime;

        if (Last > 0.032) {
            Last -= 0.032;

            water->Update();

            glm::mat4 model = glm::mat4(1.f);
            model = glm::translate(model, glm::vec3(0.f, -0.7f, 0.f));

            envMap.models = { model };
            envMap.view = light_view;
            envMap.projection = light_projection;

            envMap.Draw(envMapShader);

            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
            glBlendEquation(GL_FUNC_ADD);

            caustics.model = glm::mat4(1.f);
            caustics.projection = light_projection;
            caustics.view = light_view;

            caustics.Draw(causticsShader, water->current_frame, envMap.Frame);

            glDisable(GL_BLEND);
        }


        // Check and call events
        glfwPollEvents();
        Do_Movement();

        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model(1.f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);
       
        
        
        //Ground
        objectShader.Use();
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.f, -0.7f, 0.f));
        objectShader.SetMat4("model", model);
        objectShader.SetMat4("view", view);
        objectShader.SetMat4("projection", projection);
        objectShader.SetMat4("lightProjection", light_projection);
        objectShader.SetMat4("lightView", light_view);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, caustics.Frame);
        objectShader.SetInt("caustics", 0);
        ground.Draw(objectShader);


        //SkyBox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

        
        skyboxShader.Use();

        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.SetMat4("view", view);
        skyboxShader.SetMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindTexture(GL_TEXTURE_2D, Frame);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);


        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        model = glm::mat4(1.f);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);



        //Ground
        objectShader.Use();
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.f, -0.7f, 0.f));
        objectShader.SetMat4("model", model);
        objectShader.SetMat4("view", view);
        objectShader.SetMat4("projection", projection);
        objectShader.SetMat4("lightProjection", light_projection);
        objectShader.SetMat4("lightView", light_view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, caustics.Frame);
        objectShader.SetInt("caustics", 0);

        ground.Draw(objectShader);

        //SkyBox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content


        skyboxShader.Use();

        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.SetMat4("view", view);
        skyboxShader.SetMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        //Water
        view = camera.GetViewMatrix();
        waterShader.Use();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Frame);
        waterShader.SetInt("env", 1);
        waterShader.SetMat4("model", glm::mat4(1.f));
        waterShader.SetMat4("view", view);
        waterShader.SetMat4("projection", projection);
        waterShader.SetVec3("cameraPos", camera.Position);

        waterShader.SetInt("skybox", 0);

        water->Draw(waterShader);

        for(auto& bar : bars)
            bar.Show();
        
        // Swap the buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &skyboxVAO);

    glfwTerminate();
    return 0;
}


// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //cout << key << endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

    if (keys[GLFW_MOUSE_BUTTON_LEFT]) {
        xpos = (xpos / screenWidth) * 2 - 1;
        ypos = -((ypos / screenHeight) * 2 - 1);
        for (auto& bar : bars) {
            bar.OnMove(xpos, ypos);
        }
    }
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);

    water->Update();
}



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
        keys[button] = true;
    if(action == GLFW_RELEASE)
        keys[button] = false;

    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        x = (x / screenWidth) * 2 - 1;
        y = -((y / screenHeight) * 2 - 1);
        for (auto& bar : bars) {
            bar.OnMouseDown(x, y);
        }

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
        glm::vec4 world1 = invrs * sc1;
        glm::vec4 world2 = invrs * sc2;
        world1 /= world1.w;
        world2 /= world2.w;

        glm::vec3 pos = world1;
        glm::vec3 dir = world2 - world1;
        float alpha = -pos.y * 1.f / dir.y;

        x = pos.x + alpha * dir.x;
        float z = pos.z + alpha * dir.z;

        if (alpha > 0 && x >= -1 && x <= 1 && z >= -1 && z <= 1) {
            water->AddDrop((x + 1) / 2, (z + 1) / 2);
        }
    }
    else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT){
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        x = (x / screenWidth) * 2 - 1;
        y = -((y / screenHeight) * -1);
        for (auto& bar : bars) {
            bar.OnMouseUp(x, y);
        }
    }
    else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
        visibleMouse = !visibleMouse;
        if (visibleMouse) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

void check_size(GLFWwindow* window, EnvironmentMap& envMap, GLuint& FBO, GLuint& RBO, GLuint& Frame) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (screenHeight != height || screenWidth != width) {
        screenHeight = height;
        screenWidth = width;
        envMap.Resize();

        glDeleteTextures(1, &Frame);
        glDeleteRenderbuffers(1, &RBO);
        glDeleteFramebuffers(1, &FBO);
        genFBuffer(FBO, RBO, Frame);
        glViewport(0, 0, width, height);
    }
}