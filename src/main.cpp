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
Camera camera(glm::vec3(5.0f));

// mouse data
float lastX = 0.0f;
float lastY = 0.0f;
bool firstMouse = true;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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

    // object vertices
    // ---------------
    std::vector<Vertex> triangleVertices = {
        Vertex{glm::vec3(-0.5f, -0.5f, 0.0f),glm::vec3(1.0f)},
        Vertex{glm::vec3(0.0f, 0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1.0f)}
    };


    // object indices
    // --------------
    std::vector<unsigned int> triangleIndices = {
        0, 1, 2
    };

    // object textures
    std::vector<Texture> triangleTextures= {
        Texture{0 , "texture_diffuse"}
    };

    // object config
    // -------------
    std::string objDir = "/home/pailiah/Repos/Diss24/Engine/assets";
    Material cubeMat = {glm::vec3(0.0f, 0.5f, 0.5f)};
    Model cube((objDir + "/cube/cube.obj"), cubeMat);



    // shader config
    // -------------
    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 view = camera.getViewMatrix();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 mats[] = {view, projection};

    UniformMat4Buf Matrices("Matrices", mats, sizeof(mats), 0);
    shader.setBlockBinding(Matrices.name, Matrices.bindIdx);

    glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
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

        // clear buffers
        // -------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube.draw(shader);
        /*
        model = glm::mat4(1.0f);
        model = glm::scale(model, aVec);
        shader.setuMat4("model", model);
        shader.setuVec3("aColor", glm::vec3(0.0f));
        unitVector.draw(shader);
        */

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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}