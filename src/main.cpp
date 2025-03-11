#include "LucE/Light.hpp"
#include <LucE/Camera.hpp>
#include <LucE/Shader.hpp>
#include <LucE/Mesh.hpp>
#include <LucE/Model.hpp>
#include <LucE/Buffers.hpp>
#include <LucE/Texture.hpp>


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
void drawScene(Model* models[], unsigned int nModels, Light &light, Shader &shader, bool depthPass = false);
void drawQuad(unsigned int quadVAO, Framebuffer &buffer, Shader &shader);
GLFWwindow* setup_window(unsigned const int scr_width, unsigned const int scr_height, std::string &title);
void setShaderUniforms(Shader &shader);
void renderToDepth(Framebuffer &depthBuffer, Shader &depthShader, SpotLight &light, Model* scene[], int nModels);
std::vector<float> getDipoleDistribution(SSSMaterial& material);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

const float PI = 3.141;
const float E = 2.718;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// camera
Camera camera(glm::vec3(-4.0f, 3.0f, 4.0f));


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
    Shader shader = Shader((shaderDir+"/shader3D_base.vs").c_str(), (shaderDir+"/spotlight_sss.fs").c_str());
		Shader depthShader = Shader((shaderDir+"/depthPass.vs").c_str(), (shaderDir+"/depthPass.fs").c_str());
    Shader frameShader = Shader((shaderDir+"/pass_through.vs").c_str(), (shaderDir+"/tonemap.fs").c_str());
    Shader depthVisualiser = Shader((shaderDir+"/pass_through.vs").c_str(), (shaderDir+"/visualise_depth.fs").c_str());
		Shader lightShader = Shader((shaderDir+"/shader3D_base.vs").c_str(), (shaderDir+"/light_frag.fs").c_str());

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
		Model cube((objDir + "/cube/cube.obj"));
    std::cout << "Loaded Models\n"; 
    unsigned int frameQuad = makeQuad();

    Model* scene[] = {&cube};


		// light config
		// ------------
		glm::vec3 lightPos = glm::vec3(0.0f, 0.5f, 0.0f);
		glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
		std::string lightModelPath = (objDir + "/sphere/sphere.obj");
		float spotlightInnerCutoff = cos(glm::radians(20.0f));
		float spotlightOuterCutoff = cos(glm::radians(50.0f));
    SpotLight light(lightPos, lightColour, lightModelPath.c_str(), 
										cube.worldPos, spotlightInnerCutoff, spotlightOuterCutoff);


    // shader config
    // -------------

    camera.lookAt(0.0f, 0.0f, 0.0f);
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

		// image space SSS calc
		// --------------------
	
		// compute dipole approx table
		float mScattering = 0.8;
		float mAbsorption = 0.2;
		float mRri = 2.5;
		float mMeanCosine = 0.7;
		SSSMaterial SSSMat(mScattering, mAbsorption, mRri, mMeanCosine);
		std::vector<float> dipole_lookup = getDipoleDistribution(SSSMat);
		int maxDistance = dipole_lookup.size();
		std::cout << "dipole lookup (length " << dipole_lookup.size() << "): ";
		for (float i : dipole_lookup)
		{
			std::cout << i << ",";
		}
		std::cout <<"\n";
    
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

				// 1. render from light (shadowMap, splatMap)
				// ------------------------------------------


				// 2. render from camera for world positions
				// -----------------------------------------
		

				// 3. distribute SSS contribution on model
				//    1. render splats
				//    2. calculate single and multiple scattering at each splat point
				//    3. alpha blend contribution of each point to scatter texture
				//    4. apply scatter texture to object positions

				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				drawScene(scene, sizeof(scene) / sizeof(*scene), light, shader);


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

void drawScene(Model* models[] /* array of pointers */, unsigned int nModels, Light &light, Shader &shader, bool depthPass)
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

float dipoleDistribution(int r, float pAlpha, float sigma_tr, float z_r, float z_v)
{
		float d_r = sqrt(pow(r, 2) + pow(z_r, 2));
		float d_v = sqrt(pow(r, 2) + pow(z_v, 2));
		
		float sum_coeff = pAlpha / (4 * PI);
		float augend_coeff = pow(E, -sigma_tr * d_r) / (d_r * d_r); 
		float addend_coeff = pow(E, -sigma_tr * d_v) / (d_v * d_v); 

		float augend = (z_r * (sigma_tr + (1/d_r))) * augend_coeff;
		float addend = (z_v * (sigma_tr + (1/d_v))) * addend_coeff;
		
		return (sum_coeff * (augend + addend));
}


std::vector<float> getDipoleDistribution(SSSMaterial& material)
{
		// 1. compute R_d till R_d(d) < certain value that makes it negligible
		// 2. store both this max distance and all values into a vector
		// 3. generate distribution texture from this value
		
		// reduced coeffs
		float pScatter = (1 - material.meanCosine) * material.scattering;
		float pExtinction = material.absorption + pScatter;
		float pAlpha = pScatter / pExtinction;
		float sigma_tr = sqrt(3 * material.absorption * pExtinction);

		// Fresnel and A (?)
		float F_dr = (-1.440 / pow(material.rri, 2)) + (0.710 / material.rri) + 0.0668 + (0.636 * material.rri);
		// may cause divide by 0?
		float A = (1 + F_dr) / (1 - F_dr);	

		// dipole distances
		float z_r = 1 / pExtinction;
		float z_v = z_r * (1 + (4 * A) / 3);

		int r = 0;
		std::vector<float> distribution;

	do {
		distribution.push_back(dipoleDistribution(++r, pAlpha, sigma_tr, z_r, z_v));
	} while (distribution.back() > 0.01);

	return distribution;
}
