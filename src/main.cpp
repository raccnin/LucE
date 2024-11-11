#include <LucE/Camera.hpp>
#include <LucE/Shader.hpp>
#include <LucE/Mesh.hpp>
#include <LucE/Model.hpp>
#include <LucE/Buffers.hpp>

#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// camera
Camera camera(glm::vec3(-2.0f, 3.0, 3.0f));

// input data
bool rotatingLight = false;


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    // OpenGL global states
    // --------------------
    glEnable(GL_DEPTH_TEST);

    // compile shader programs
    // -----------------------
    std::string shaderDir = "/home/pailiah/Repos/Diss24/Engine/src/shaders";
    Shader shader = Shader((shaderDir+"/shader3D_base.vs").c_str(), (shaderDir+"/shader3D_base.fs").c_str());
    Shader frameShader = Shader((shaderDir+"/framebuffer.vs").c_str(), (shaderDir+"/post_processing.fs").c_str());

    // object config
    // -------------

    // framebuffer quad
        // quad
    const float quadVertices[]
    {   // position  // texcoord
        -1.0f, 1.0f, 0.0f, 1.0f, // top left
        -1.0f, -1.0f, 0.0f, 0.0f, // bottom left
        1.0f, -1.0f, 1.0f, 0.0f, // bottom right
        1.0f, 1.0f, 1.0f, 1.0f // top right
    };
    const unsigned int quadIndices[]
    {
        0, 1, 2, 2, 3, 0
    };

    unsigned int quadVAO, quadVBO, quadEBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), &quadIndices, GL_STATIC_DRAW);
    // attribute config
    // ----------------
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*) 0);
    // texCoord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*) (sizeof(float) * 2));

    glBindVertexArray(0);

    std::string objDir = "/home/pailiah/Repos/Diss24/Engine/assets";
    Material cubeMat = {glm::vec3(0.1f), glm::vec3(0.5f), glm::vec3(0.3f), 1.0f};
    Model cube((objDir + "/cube/cube.obj"), cubeMat);
    Model angel((objDir + "/statue/angel.obj"), cubeMat);
    Light light{glm::vec3(5.0f), glm::vec3(0.5f), glm::vec3(0.1f), glm::vec3(1.0f)};

    // shader config
    // -------------
    glm::mat4 model = glm::mat4(1.0f);

    camera.lookAt(0.0f, 2.0f, 0.0f);
    glm::mat4 view = camera.getViewMatrix();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 mats[] = {view, projection};

    UniformMat4Buf Matrices("Matrices", mats, sizeof(mats), 0);
    shader.use();
    shader.setBlockBinding(Matrices.name, Matrices.bindIdx);

    frameShader.use();
    frameShader.setInt("frameTexture", 0);

    
    // framebuffer (Post-Processing)
    // -----------------------------
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    // color attachment
    unsigned int colourBuffer;
    glGenTextures(1, &colourBuffer);
    glBindTexture(GL_TEXTURE_2D, colourBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer, 0);
    
    // rbo for depth/stencil
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        // per frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        float time = glfwGetTime();

        // render to frame buffer
        // ----------------------
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // clear buffers
        // -------------
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setVec3("viewPos", camera.worldPos);
        angel.draw(shader, light);
 
        // draw to framebuffer plane
        // -------------------------
        // 1. bind to default
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. disable depth test and clear buffers
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 3. render quad with scene data
        frameShader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, colourBuffer);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        rotatingLight = !rotatingLight;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}