#include <LucE/Camera.hpp>
#include <LucE/Shader.hpp>
#include <LucE/Mesh.hpp>

#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// camera
Camera camera(glm::vec3(2.0f));

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
    Vertex triangleVertices[]{
        Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1.0f)},
        Vertex{glm::vec3(0.0f, 0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1.0f)}
    };

    Vertex cubeVertices[] = {
        // positions       
        Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f)}
    };

    Vertex vecVertices[] = {
        Vertex{glm::vec3(0.0f), glm::vec3(1.0f)},
        Vertex{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f)}
    };

    // object indices
    // --------------
    unsigned int triangleIndices[]{
        0, 1, 2
    };
    unsigned int vecIndices[]{
        0, 1
    };
    // rudimentary implementation, TODO: remove
    unsigned int cubeIndices[36];
    for (unsigned int i = 0; i < 36; i++)
    {
        cubeIndices[i] = i;
    }

    // define uniform buffers
    // ----------------------
    
    // object config
    // -------------
    Mesh triangle(triangleVertices, sizeof(triangleVertices), triangleIndices, sizeof(triangleIndices));
    Mesh cube(cubeVertices, sizeof(cubeVertices), cubeIndices, sizeof(cubeIndices));
    Mesh unitVector(vecVertices, sizeof(vecVertices), vecIndices, sizeof(vecIndices), GL_LINE_STRIP);


    // shader config
    // -------------
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    shader.setuMat4("model", model);

    glm::mat4 view = camera.getViewMatrix();
    shader.setuMat4("view", view);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 1000.0f);
    shader.setuMat4("projection", projection);

    glm::vec3 aVec(3.0f);

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

        model = glm::mat4(1.0f);
        shader.setuMat4("model", model);
        shader.setuVec3("aColor", glm::vec3(0.4, 0.0, 0.7));
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