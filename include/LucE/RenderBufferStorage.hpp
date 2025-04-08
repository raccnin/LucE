#ifndef RENDERBUFFERSTORAGE_H
#define RENDERBUFFERSTORAGE_H

class RenderBufferStorage
{
public: 
	unsigned int ID;
	bool multisampled;

	RenderBufferStorage(unsigned int width, unsigned int height, bool multisampled = false, unsigned int samples = 0);
};

#endif
