#include <LucE/RenderBufferStorage.hpp>
#include <glad.h>

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
