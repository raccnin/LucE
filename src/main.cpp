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

void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void makeQuad(unsigned int &quadVAO, unsigned int &quadVBO);
void blitBuffers(msFramebuffer const &readBuf, Framebuffer const &drawBuf);
void drawScene(Model* models[], unsigned int nModels, Light &light, Shader &shader, bool depthPass = false);
void drawQuad(unsigned int quadVAO, unsigned int textureID, Shader &shader);
GLFWwindow* setup_window(unsigned const int scr_width, unsigned const int scr_height, std::string &title);
void setShaderUniforms(Shader &shader);
void renderToDepth(Framebuffer &depthBuffer, Shader &depthShader, SpotLight &light, Model* scene[], int nModels);
std::vector<float> getDipoleDistribution(SSSMaterial& material);
void orientQuad(Camera& camera, unsigned int quadVBO, float size);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;
const unsigned int SPLAT_RES = 128;

const float PI = 3.141;
const float E = 2.718;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

int main()
{
		// invert y coordinates for GL to use properly
		// aware of inverting UV's in assimp! 
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
    glfwSetCursorPosCallback(window, mouseCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
		// TODO: must change this resource manager it's so bad good lord
    //std::string shaderDir = "/home/pailiah/Repos/Diss24/Engine/src/shaders";
    std::string shaderDir = "/home/shalash/Repos/Diss24/engine/src/shaders";
    Shader shader = Shader((shaderDir+"/shader3D_base.vs").c_str(), (shaderDir+"/spotlight_sss.fs").c_str());
    Shader frameShader = Shader((shaderDir+"/pass_through.vs").c_str(), (shaderDir+"/tonemap.fs").c_str());
		Shader lightShader = Shader((shaderDir+"/shader3D_base.vs").c_str(), (shaderDir+"/light_frag.fs").c_str());
		Shader gBufShader = Shader((shaderDir+"/shader3D_base.vs").c_str(), (shaderDir+"/gBufMap.fs").c_str());
		Shader splatShader = Shader((shaderDir+"/splatGen.vs").c_str(), (shaderDir+"/splatGen.fs").c_str());
		Shader scatterShader = Shader((shaderDir+"/shader3D_base.vs").c_str(), (shaderDir+"/draw_scatter.fs").c_str());
		Shader dipoleDebug = Shader((shaderDir+"/shader3D_base.vs").c_str(), (shaderDir+"/draw_dipole.fs").c_str());

    // object config
    // -------------
    //std::string objDir = "/home/pailiah/Repos/Diss24/Engine/assets";
    std::string objDir = "/home/shalash/Repos/Diss24/engine/assets";
    /*
    Material cubeMat = {glm::vec3(0.1f), glm::vec3(0.2f), glm::vec3(0.3f), 1.0f};
    Model cube((objDir + "/cube/cube.obj"), cubeMat);
    Model angel((objDir + "/statue/angel.obj"), cubeMat);
    */
    //Model backpack((objDir + "/backpack/backpack.obj"));
		//Model angel((objDir + "/statue/statue.obj"));
		Model sphere((objDir + "/sphere/sphere.obj"));
		Model cube((objDir + "/cube/cube.obj"));
    std::cout << "Loaded Models\n"; 
		unsigned int quadVAO, quadVBO;
		makeQuad(quadVAO, quadVBO);
		


    Model* scene[] = {&sphere};


		// light config
		// ------------
		glm::vec3 lightPos = glm::vec3(1.5f, 0.5f, 1.8f);
		glm::vec3 lightColour = glm::vec3(1.0f);
		const float radius = 3.5f;
		std::string lightModelPath = (objDir + "/lightball/sphere.obj");
		float spotlightInnerCutoff = cos(glm::radians(10.0f));
		float spotlightOuterCutoff = cos(glm::radians(10.0f));
    SpotLight light(lightPos, lightColour, lightModelPath.c_str(), 
										scene[0]->worldPos + glm::vec3(0.0, 0.0, 0.0), spotlightInnerCutoff, spotlightOuterCutoff);


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

    // framebuffers (Post-Processing)
    // -----------------------------
		Framebuffer splatBuffer(SPLAT_RES, SPLAT_RES);
		Texture2D splatPositions(GL_RGBA16F, GL_FLOAT);
		Texture2D splatNormals(GL_RGBA16F, GL_FLOAT);
		Texture2D* splatAttachments[] = {&splatPositions, &splatNormals};
		char* description;
		for (unsigned int i = 0; i < 2; i++)
		{
			splatAttachments[i]->setFilters(GL_NEAREST, GL_NEAREST);
			splatAttachments[i]->generate(SPLAT_RES, SPLAT_RES, NULL);
			splatBuffer.attachBuffer(splatAttachments[i]->ID, GL_TEXTURE_2D, i);
		}
		RenderBufferStorage splatRBO(SPLAT_RES, SPLAT_RES);
		splatBuffer.attachBuffer(splatRBO.ID, GL_RENDERBUFFER);

		Framebuffer gBuffer(SCR_WIDTH, SCR_HEIGHT);
		Texture2D gWorldPositions(GL_RGBA16F, GL_FLOAT);
		gWorldPositions.generate(SCR_WIDTH, SCR_HEIGHT, NULL);
		gBuffer.attachBuffer(gWorldPositions.ID, GL_TEXTURE_2D);
		RenderBufferStorage gRBO(SCR_WIDTH, SCR_HEIGHT);
		gBuffer.attachBuffer(gRBO.ID, GL_RENDERBUFFER);

		Framebuffer scatterBuffer(SCR_WIDTH, SCR_HEIGHT);
		Texture2D scatterTexture(GL_RGBA16F, GL_FLOAT);
		scatterTexture.generate(SCR_WIDTH, SCR_HEIGHT, NULL);
		scatterBuffer.attachBuffer(scatterTexture.ID, GL_TEXTURE_2D);
		RenderBufferStorage scatterRBO(SCR_WIDTH, SCR_HEIGHT);
		scatterBuffer.attachBuffer(scatterRBO.ID, GL_RENDERBUFFER);

		// image space SSS calc
		// --------------------
	
		// compute dipole approx table
		float mScattering = 1.0;
		float mAbsorption = 0.1;
		float mRri = 2.5;
		float mMeanCosine = -0.5;
		glm::vec3 mAlbedo(0.1, 0.8, 0.3);
		SSSMaterial SSSMat(mAlbedo, mScattering, mAbsorption, mRri, mMeanCosine);
		std::vector<float> dipoleLookup = getDipoleDistribution(SSSMat);
		// dipole[i] = R_d(i / 10)	
		float maxDistance = dipoleLookup.size() / 100.0f;
		std::cout << "maxDistance: " << maxDistance << std::endl;
		// store in 1D texture
		
		std::cout << "dipole lookup (length " << dipoleLookup.size() << "): ";
		for (float i : dipoleLookup)
		{
			std::cout << i << ",";
		}
		std::cout <<"\n";
		Texture2D dipoleLookupTex(GL_R32F, GL_FLOAT);
		dipoleLookupTex.setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
		dipoleLookupTex.generate(dipoleLookup.size(), 1, &dipoleLookup[0]);

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
				light.setPos(glm::vec3(radius * cos(time), light.position.y, radius* sin(time)));
				//camera.setPos(glm::vec3(3.0  + 2 * cos(time), camera.worldPos.y, camera.worldPos.z));

				glEnable(GL_DEPTH_TEST);
				glClearColor(0.0, 0.0, 0.0, 1.0);
				// 1. render from light (splatmap)
				// world positions and normals (single scatter) 
				// rbo also holds shadowmap information
				// --------------------------------------------
				splatBuffer.use();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glViewport(0, 0, SPLAT_RES, SPLAT_RES);
				Matrices.fillIdx(VIEW, light.getViewMatrix());
				drawScene(scene, sizeof(scene) / sizeof(*scene), light, gBufShader);

				// 2. render from camera for world positions
				// -----------------------------------------
				gBuffer.use();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
				Matrices.fillIdx(VIEW, camera.getViewMatrix());
				drawScene(scene, sizeof(scene) / sizeof(*scene), light, gBufShader);

				// 3. distribute SSS contribution on model
				//    1. render splats
				//     render nSplat quads, using render ID to index splat positions
				//    2. calculate single and multiple scattering at each splat point
				//			using, dipole lookup, world positions, and normals
				//    3. alpha blend contribution of each point to scatter texture
				//    4. apply scatter texture to object positions

				scatterBuffer.use();
				glEnable(GL_BLEND);

				// additive alpha blending
				glBlendFunc(GL_ONE, GL_ONE);
				glDisable(GL_DEPTH_TEST);
				glClear(GL_COLOR_BUFFER_BIT);

				// orient quad to be perependicular to camera
				
				orientQuad(camera, quadVBO, 0.5);
				
				// setting splatShader uniforms 
				splatShader.use();
				glActiveTexture(GL_TEXTURE0);
				splatPositions.bind();
				glActiveTexture(GL_TEXTURE1);
				splatNormals.bind();
				glActiveTexture(GL_TEXTURE2);
				gWorldPositions.bind();
				glActiveTexture(GL_TEXTURE3);
				dipoleLookupTex.bind();

				splatShader.setVec2("windowSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
				splatShader.setVec3("viewPos", camera.worldPos);
				splatShader.setInt("splatPositions", 0);	
				splatShader.setInt("splatNormals", 1);
				splatShader.setInt("viewPositions", 2);
				splatShader.setInt("dipoleLookup", 3);
				splatShader.setInt("splatResolution", SPLAT_RES);	
				splatShader.setInt("maxDistance", dipoleLookup.size());

				SSSMat.setUniforms(splatShader);
				light.setUniforms(splatShader);

				// drawing splats
				glBindVertexArray(quadVAO);
				glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (SPLAT_RES*SPLAT_RES));


				// render scattering to object + specular
				// -------------------------------------- 
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glEnable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				scatterShader.use();
				
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, scatterBuffer.getAttachment(GL_COLOR_ATTACHMENT0));
				scatterShader.setInt("scatterTexture", 0);
				scatterShader.setVec2("windowSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
				SSSMat.setUniforms(scatterShader);
				light.setUniforms(scatterShader);
				scatterShader.setVec3("viewPos", camera.worldPos);
				scene[0]->draw(scatterShader);

				lightShader.use();
				light.setUniforms(lightShader);
				light.draw(lightShader);


				//TODO: make neat visualisation? all passes below final image?
				// render 3 quads (splatMap, cameraPositions, scatterTexture)

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
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, deltaTime);
}


void makeQuad(unsigned int &quadVAO, unsigned int &quadVBO)
{
    const float quadVertices[]
    {   // position  // texcoord
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f // top right
    };
    const unsigned int quadIndices[]
    {
        0, 1, 2, 2, 3, 0
    };

    unsigned int EBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), &quadIndices, GL_STATIC_DRAW);
    // attribute config
    // ----------------
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) 0);
    // texCoord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) (sizeof(float) * 3));

    glBindVertexArray(0);
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

void drawQuad(unsigned int quadVAO, unsigned int textureID, Shader &shader)
{
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	shader.setInt("frameTexture", 0);
	glBindTexture(GL_TEXTURE_2D, textureID);
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

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.processMouse(xoffset, yoffset);
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

void orientQuad(Camera& camera, unsigned int quadVBO, float size)
{
	float halfDistance = size/2;
	glm::vec3 topRight = halfDistance * glm::normalize(camera.up + camera.right);		
	glm::vec3 topLeft =  halfDistance * glm::normalize(camera.up + -camera.right);
	glm::vec3 bottomRight =  halfDistance * glm::normalize(-camera.up + camera.right);
	glm::vec3 bottomLeft = halfDistance *  glm::normalize(-camera.up + -camera.right);

	float quadVertices[]
		{   // position  // texcoord
			topLeft.x, topLeft.y, topLeft.z, 0.0f, 1.0f, // top left
			bottomLeft.x, bottomLeft.y, bottomLeft.z, 0.0f, 0.0f, // bottom left
			bottomRight.x, bottomRight.y, bottomRight.z, 1.0f, 0.0f, // bottom right
			topRight.x, topRight.y, topRight.z, 1.0f, 1.0f // top right
		};
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

}

float dipoleDistribution(float r, float pAlpha, float sigma_tr, float z_r, float z_v)
{
		r /= 100.0;
		
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
		float pAlpha = pScatter / pExtinction; // proportion of extinction that is scatter
		float sigma_tr = sqrt(3 * material.absorption * pExtinction);

		// Fresnel and A (?)
		float F_dr = (-1.440 / pow(material.rri, 2)) + (0.710 / material.rri) + 0.0668 + (0.0636 * material.rri);
		// may cause divide by 0?
		float A = (1 + F_dr) / (1 - F_dr);	

		// dipole distances
		float z_r = 1 / pExtinction;
		float z_v = z_r * (1 + ((4 * A) / 3));

		int r = 0;
		std::vector<float> distribution;

	do {
		distribution.push_back(dipoleDistribution(++r, pAlpha, sigma_tr, z_r, z_v));
	} while (distribution.back() > 0.001);

	return distribution;
}
