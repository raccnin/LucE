#include <LucE/Texture.hpp>
#include <LucE/RenderBufferStorage.hpp>
#include <LucE/Framebuffer.hpp>

#include <glad.h>
#include <string>
#include <iostream>
#include <map>

Framebuffer::Framebuffer(unsigned int width, unsigned int height)
	: width(width), height(height)
{
    // create buffer
    glGenFramebuffers(1, &ID);
    glBindFramebuffer(GL_FRAMEBUFFER, ID);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::use(){
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glDrawBuffers(activeAttachments.size(), &activeAttachments[0]);
	unsigned int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::string err = "ERROR::FRAMEBUFFER" + std::to_string(ID) + "::";
		switch (status)
		{
			case GL_FRAMEBUFFER_UNDEFINED:
				err += "UNDEFINED";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				err += "INCOMPLETE_ATTACHMENT";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				err += "INCOMPLETE_MISSING_ATTACHMENT";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				err += "INCOMPLETE_DRAW_BUFFER";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				err += "INCOMPLETE_READ_BUFFER";
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				err += "UNSUPPORTED";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				err += "INCOMPLETE_LAYER_TARGETS";
				break;
			default:
				std::cout << "OTHER";
				break;
		}
		std::cout << err << std::endl;
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
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


