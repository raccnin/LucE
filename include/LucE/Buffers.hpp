#ifndef BUFFERS_H
#define BUFFERS_H

#include <string>
#include <glm/glm.hpp>
#include "Texture.hpp"
#include <iostream>
#include <map>
#include <vector>

class UniformMat4Buf
{
    public:
        unsigned int ID;
        unsigned int bindIdx;
        const char* name;
        UniformMat4Buf(const std::string &name, const glm::mat4 data[], const unsigned int dataSize, const unsigned int bindIdx);
        void fillIdx(const unsigned int idx, const glm::mat4 &mat);
};

enum TransformIdx
{
    VIEW,
    PROJECTION
};


class Framebuffer
{
public:
	unsigned int ID;
	unsigned int width;
	unsigned int height;
	std::map<unsigned int, unsigned int> colorAttachments;

	Framebuffer(unsigned int width, unsigned int height);
	void use(){
		glBindFramebuffer(GL_FRAMEBUFFER, ID);
		glDrawBuffers(activeAttachments.size(), &activeAttachments[0]);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR:FRAMEBUFFER::NOT_COMPLETE" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
	void attachBuffer(unsigned int buffID, unsigned int attachmentType, unsigned int number = 0);
	/*
				 * generate FBO with a single colour buffer with associated internal format
				 * and depth stencil attachment
				*/
	void generate(unsigned int internal_format);
	unsigned int getAttachment(unsigned int colorAttachment)
	{
		return colorAttachments[colorAttachment];
	}
private:
	// duplicate information
	// TODO: get list of keys of colorAttachments instead
	std::vector<unsigned int> activeAttachments;
};

class msFramebuffer
{
    public:
        unsigned int ID;
        unsigned int colourBuffer;

        msFramebuffer(unsigned int width, unsigned int height, unsigned int samples, GLenum internal_format = GL_RGB);
        void use(){
            glBindFramebuffer(GL_FRAMEBUFFER, ID);
						if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
						{
							std::cout << "ERROR:FRAMEBUFFER::NOT_COMPLETE" << std::endl;
							glBindFramebuffer(GL_FRAMEBUFFER, 0);
						}
        }
};

class RenderBufferStorage
{
public: 
	unsigned int ID;
	bool multisampled;

	RenderBufferStorage(unsigned int width, unsigned int height, bool multisampled = false, unsigned int samples = 0);
};
#endif
