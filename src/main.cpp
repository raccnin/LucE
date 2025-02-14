#include "LucE/Light.hpp"
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
#include <stb_image.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int makeQuad();
void blitBuffers(msFramebuffer const &readBuf, Framebuffer const &drawBuf);
void drawScene(Model* models[], unsigned int nModels, Light &light, Shader &shader);
void drawQuad(unsigned int quadVAO, Framebuffer &buffer, Shader &shader);
GLFWwindow* setup_window(unsigned const int scr_width, unsigned const int scr_height, std::string &title);
void setShaderUniforms(Shader &shader);
void renderToDepth(Framebuffer &depthBuffer, Shader &depthShader, SpotLight &light, Model* scene[], int nModels);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// camera
Camera camera(glm::vec3(-2.0f, 3.0f, 3.0f));

// input data
bool rotatingLight = false;

int main()
{
		// invert y coordinates for GL to use properly
    stbi_set_flip_vertically_on_load(true);

		std::string title = "OpenGL";
		GLFWwindow* window = setup_window(SCR_WIDTH, SCR_HEIGHT, title);
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
    //std::string shaderDir = "/home/shalash/Repos/Diss24/engine/src/shaders";
    Shader shader = Shader((shaderDir+"/shader3D_base.vs").c_str(), (shaderDir+"/spotlight_shadowed.fs").c_str());
		Shader depthShader = Shader((shaderDir+"/depthPass.vs").c_str(), (shaderDir+"/depthPass.fs").c_str());
    Shader frameShader = Shader((shaderDir+"/pass_through.vs").c_str(), (shaderDir+"/tonemap.fs").c_str());
    Shader depthVisualiser = Shader((shaderDir+"/pass_through.vs").c_str(), (shaderDir+"/visualise_depth.fs").c_str());

    // object config
    // -------------
    std::string objDir = "/home/pailiah/Repos/Diss24/Engine/assets";
    //std::string objDir = "/home/shalash/Repos/Diss24/engine/assets";
    /*
    Material cubeMat = {glm::vec3(0.1f), glm::vec3(0.2f), glm::vec3(0.3f), 1.0f};
    Model cube((objDir + "/cube/cube.obj"), cubeMat);
    Model angel((objDir + "/statue/angel.obj"), cubeMat);
    */
    //Model backpack((objDir + "/backpack/backpack.obj"));
		Model angel((objDir + "/statue/angel.obj"));
    std::cout << "Loaded Models\n"; 
    unsigned int frameQuad = makeQuad();

    Model* scene[] = {&angel};

		// light config
		// ------------
		glm::vec3 lightPos = glm::vec3(2.0f, 2.0f, 3.0f);
		float spotlightInnerCutoff = cos(glm::radians(5.0f));
		float spotlightOuterCutoff = cos(glm::radians(10.0f));
    SpotLight light(lightPos, glm::vec3(0.05f), glm::vec3(0.5f), glm::vec3(1.0f), angel.worldPos + glm::vec3(0.0f, 2.8f, 0.0f), spotlightInnerCutoff, spotlightOuterCutoff);


    // shader config
    // -------------
    glm::mat4 model = glm::mat4(1.0f);

    camera.lookAt(0.0f, 2.5f, 0.0f);
    glm::mat4 view = camera.getViewMatrix();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 mats[] = {view, projection};

    UniformMat4Buf Matrices("Matrices", mats, sizeof(mats), 0);
		//Matrices.fillIdx(VIEW, light.getViewMatrix());
    shader.use();
    shader.setBlockBinding(Matrices.name, Matrices.bindIdx);

    frameShader.use();
    frameShader.setInt("frameTexture", 0);

    // framebuffer (Post-Processing)
    // -----------------------------
		Framebuffer shadowMap(SHADOW_WIDTH, SHADOW_HEIGHT, GL_DEPTH_COMPONENT);
    msFramebuffer msBuffer(SCR_WIDTH, SCR_HEIGHT, 4, GL_RGBA16F);
    Framebuffer screenBuffer(SCR_WIDTH, SCR_HEIGHT);
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glEnable(GL_DEPTH_TEST);
        // input
        // -----
        processInput(window);
        // per frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        float time = glfwGetTime();
				light.lookAt(glm::vec3(0.0f, cos(time)+2.0f, 0.0f));
				//Matrices.fillIdx(VIEW, light.getViewMatrix());
				
				// generate depth map
				// ------------------
				// 1. render to depth map	
				glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
				shadowMap.use();
				glClear(GL_DEPTH_BUFFER_BIT);

				depthShader.use();
				depthShader.setMat4("lightTransform", light.getTransformMatrix());
				drawScene(scene, sizeof(scene) / sizeof(*scene), light, depthShader);

        // render to frame buffer
        // ----------------------
        // 1. bind MSAA framebuffer
        msBuffer.use();

        // 2. clear buffers
				glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.2, 0, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // 3. draw scene
				shader.use();
				shader.setMat4("lightTransform", light.getTransformMatrix());
				glActiveTexture(GL_TEXTURE0);
				shadowMap.colourBuffer.bind();
				shader.setInt("shadowMap", 0);
        drawScene(scene, sizeof(scene) / sizeof(*scene), light, shader);
        // condense MSAA buffer to single buffer
        blitBuffers(msBuffer, screenBuffer);
        
        // draw to framebuffer plane
        // -------------------------
        // 1. bind to default
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        // 2. disable depth test and clear buffers
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

        // 3. render quad with scene data
       	 drawQuad(frameQuad, screenBuffer, frameShader);
				//drawQuad(frameQuad, depthMap, depthVisualiser);

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

unsigned int makeQuad()
{
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

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

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

    return VAO;
}

void blitBuffers(msFramebuffer const &readBuf, Framebuffer const &drawBuf)
{
        glBindFramebuffer(GL_READ_BUFFER, readBuf.ID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawBuf.ID);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_READ_BUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}

void drawScene(Model* models[] /* array of pointers */, unsigned int nModels, Light &light, Shader &shader)
{
    shader.use();
		light.setUniforms(shader);
    for(unsigned int i = 0; i < nModels; i++)
    {
        models[i]->draw(shader);
    }
    glUseProgram(0);
}

void drawQuad(unsigned int quadVAO, Framebuffer &buffer, Shader &shader)
{
        shader.use();
				glActiveTexture(GL_TEXTURE0);
				shader.setInt("frameTexture", 0);
				buffer.colourBuffer.bind();
        glBindVertexArray(quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glUseProgram(0);
}

GLFWwindow* setup_window( unsigned const int scr_width, unsigned const int scr_height, std::string &title)
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title.c_str(), NULL, NULL);
	return window;

}

void setShaderUniforms(Shader &shader)
{
	shader.setVec3("viewPos", camera.worldPos);
}

void renderToDepth(Framebuffer &depthBuffer, Shader &depthShader, SpotLight &light, Model* scene[], int nModels)
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	depthBuffer.use();
	depthShader.use();
	glClear(GL_DEPTH_BUFFER_BIT);
	depthShader.setMat4("lightTransform", light.getTransformMatrix());
	drawScene(scene, nModels, light, depthShader);
	// reset
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}
