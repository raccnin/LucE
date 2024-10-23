#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <LucE/Shader.hpp>
#include <LucE/Mesh.hpp>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

    // compile shader programs
    // -----------------------
    Shader shader = Shader("/home/shalash/Repos/Diss24/engine/src/shaders/example.vs", "/home/shalash/Repos/Diss24/engine/src/shaders/example.fs");

    // object vertices
    // ---------------
    Vertex triangleVertices[]{
        Vertex{glm::vec3(-0.5f, -0.5f, 1.0f), glm::vec3(1.0f)},
        Vertex{glm::vec3(0.0f, 0.5f, 1.0f), glm::vec3(1.0f)},
        Vertex{glm::vec3(0.5f, -0.5f, 1.0f), glm::vec3(1.0f)}
    };
    // object indices
    // --------------
    unsigned int triangleIndices[]{
        0, 1, 2
    };

    // object config
    // -------------
    Mesh triangle = Mesh(triangleVertices, sizeof(triangleVertices), triangleIndices, sizeof(triangleIndices));

    glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // clear buffers
        // -------------
        glClear(GL_COLOR_BUFFER_BIT);

        triangle.draw(shader);
        
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