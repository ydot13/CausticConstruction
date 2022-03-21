// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

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

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void Do_Movement();
unsigned int loadTexture(char const* path);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400 , lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat Last = 0.0f;

glm::vec3 lightPos(0.f, 0.0f, 5.0f);
std::vector<glm::vec3> cubes;
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
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
   // glfwSetMouseButtonCallback(window, mouse_button_callback);

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
    Shader objectShader("vertex_shader.glsl", "fragment_shader.glsl");
    Shader lampShader("lamp_vertex_shader.glsl", "lamp_fragment_shader.glsl");

    // Set up our vertex data (and buffer(s)) and attribute pointers
    float vertices[] = {
        // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // нижн€€-лева€
           
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // верхн€€-права€   
          0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // нижн€€-права€ 
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // верхн€€-лева€
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // верхн€€-права€
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // нижн€€-лева€                
         // передн€€ грань
        
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // верхн€€-права€
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // нижн€€-лева€
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // нижн€€-права€        
         
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // нижн€€-лева€
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // верхн€€-права€
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // верхн€€-лева€        
         // грань слева
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // нижн€€-лева€
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // верхн€€-права€
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // верхн€€-лева€       
      
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // верхн€€-права€
          -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // нижн€€-лева€
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // нижн€€-права€
         // грань справа
         
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // верхн€€-права€  
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // верхн€€-лева€
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // нижн€€-права€          
         
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // нижн€€-лева€
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // нижн€€-права€
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // верхн€€-лева€
         // нижн€€ грань          
       
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // нижн€€-лева€
          -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // верхн€€-права€
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // верхн€€-лева€        
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // верхн€€-права€
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // нижн€€-лева€
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // нижн€€-права€
         // верхн€€ грань
        
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // верхн€€-права€
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // верхн€€-лева€
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // нижн€€-права€                 
       
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // нижн€€-лева€  
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // нижн€€-права€
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f  // верхн€€-лева€     
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    GLuint VBO, VAO, lightVAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind our Vertex Array Object first, then bind and set our buffers and pointers.
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));  
    glEnableVertexAttribArray(1);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    //glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    unsigned int diffuseMap = loadTexture("container2.jpg");
    unsigned int specularMap = loadTexture("container2_specular.png");
    
    Shader waterShader("water_vertex.glsl", "water_fragment.glsl");
    Water water =  Water(128, 128);

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
        "right.jpg",
            "left.jpg",
            "top.jpg",
            "bottom.jpg",
            "front.jpg",
            "back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    Shader skyboxShader("skybox_shader_vt.glsl","skybox_shader_fr.glsl");

    skyboxShader.Use();
    skyboxShader.SetInt("skybox", 0);

    std::vector<Vertex> gr_vr = {
        {glm::vec3(-1.f, 0.f, -1.f), glm::vec3(), glm::vec2(0.f, 0.f)},
        {glm::vec3(1.f, 0.f, -1.f), glm::vec3(), glm::vec2(1.f, 0.f)},
        {glm::vec3(-1.f, 0.f, 1.f), glm::vec3(), glm::vec2(0.f, 1.f)},
        {glm::vec3(1.f, 0.f, 1.f), glm::vec3(), glm::vec2(1.f, 1.f)},
    };
    Mesh ground(gr_vr, { 0, 1, 2, 1, 2, 3 }, {});

    EnvironmentMap envMap({ ground });
    
    Shader envMapShader("env_map_vertex.glsl", "env_map_fragment.glsl");
    
    Caustics caustics(water.waterGrid);

    Shader causticsShader("caustic_vertex.glsl", "caustic_fragment.glsl");

   

    GLuint FBO, RBO, Frame;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    Frame = CreateTexture(800, 600, GL_RGBA, GL_UNSIGNED_BYTE, GL_MIRRORED_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Frame, 0); 
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        
        
        deltaTime = currentFrame - lastFrame;

        glfwSetWindowTitle(window, std::to_string(1.f / (deltaTime)).c_str());
        lastFrame = currentFrame;

        Last += deltaTime;

        if (Last > 0.032) {
            Last -= 0.032;

            water.Update();

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

            caustics.Draw(causticsShader, water.current_frame, envMap.Frame);

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

        water.Draw(waterShader);
        
        // Swap the buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &lightVAO);
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);

    water->Update();
}



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glm::vec3 tmp = camera.Position + camera.Front;
        cubes.push_back(glm::vec3((int)tmp.x, (int)tmp.y, (int)tmp.z));
    }
}