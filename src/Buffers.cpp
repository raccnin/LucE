#include <LucE/Buffers.hpp>
#include <LucE/Texture.hpp>

#include <string>
#include <glm/glm.hpp>
#include <glad.h>
#include <map>
#include <iostream>

UniformMat4Buf::UniformMat4Buf(const std::string &name, const glm::mat4 data[], const unsigned int dataSize, const unsigned int bindIdx)
{
    this->name = name.c_str();
    this->bindIdx = bindIdx;
    glGenBuffers(1, &ID);
    glBindBuffer(GL_UNIFORM_BUFFER, ID);
    glBufferData(GL_UNIFORM_BUFFER, dataSize, NULL, GL_STATIC_DRAW);
    for (unsigned int i = 0; i < dataSize; i++)
    {
        //ensure std130 format (may be unecessary step)
        glBufferSubData(GL_UNIFORM_BUFFER, i*sizeof(glm::mat4), sizeof(glm::mat4), &data[i][0][0]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindIdx, ID);
}   
void UniformMat4Buf::fillIdx(const unsigned int idx, const glm::mat4 &mat)
{
    glBindBuffer(GL_UNIFORM_BUFFER, ID);
    glBufferSubData(GL_UNIFORM_BUFFER, idx*sizeof(glm::mat4), sizeof(glm::mat4), &mat[0][0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// frame buffer
// ------------
Framebuffer::Framebuffer(unsigned int width, unsigned int height)
	: width(width), height(height)
{
    // create buffer
    glGenFramebuffers(1, &ID);
    glBindFramebuffer(GL_FRAMEBUFFER, ID);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::generate(unsigned int internal_format)
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	Texture2D colourAttachment(internal_format);
	colourAttachment.generate(width, height, NULL);
	if (internal_format == GL_DEPTH_COMPONENT)
	{
		attachBuffer(colourAttachment.ID, GL_DEPTH_COMPONENT);
	}
	else 
	{
		attachBuffer(colourAttachment.ID, GL_TEXTURE_2D);
	}

	RenderBufferStorage rbo(width, height);
	attachBuffer(rbo.ID, GL_RENDERBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachBuffer(unsigned int buffID, unsigned int attachmentType, unsigned int number /* default 0 */)
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	switch (attachmentType)
	{
		case GL_TEXTURE_2D:
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + number, GL_TEXTURE_2D, buffID, 0);	
			colorAttachments[GL_COLOR_ATTACHMENT0 + number] = buffID;
			activeAttachments.push_back(GL_COLOR_ATTACHMENT0 + number);
			break;
		case GL_RENDERBUFFER:
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffID);	
			break;
		case GL_DEPTH_COMPONENT:
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffID, 0);
			colorAttachments[GL_COLOR_ATTACHMENT0 + number] = buffID;
			activeAttachments.push_back(GL_COLOR_ATTACHMENT0 + number);
			break;	
		default:
			std::cout << "ERROR::NOT A VALID BUFFER ATTACHMENT TYPE\n";
			break;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// multisampled frame buffer
// -------------------------
msFramebuffer::msFramebuffer(unsigned int width, unsigned int height, unsigned int samples, GLenum internal_format /* default GL_RGB */)
{
    glGenFramebuffers(1, &ID);
    glBindFramebuffer(GL_FRAMEBUFFER, ID);
    // make multisampled color attachment
    glGenTextures(1, &colourBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colourBuffer);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colourBuffer, 0);
    // make multisampled rbo
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER::NOT_COMPLETE" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderBufferStorage::RenderBufferStorage(unsigned int width, unsigned int height, bool multisampled, unsigned int samples)
	: multisampled(multisampled)
{
	glGenRenderbuffers(1, &ID);
	glBindRenderbuffer(GL_RENDERBUFFER, ID);
	if (multisampled)
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	}
	else
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);	
	}
}
