#ifndef FRAMEBUFF_H
#define FRAMEBUFF_H

#include <vector>
#include <map>

class Framebuffer
{
public:
	unsigned int ID;
	unsigned int width;
	unsigned int height;
	std::map<unsigned int, unsigned int> colorAttachments;

	Framebuffer(unsigned int width, unsigned int height);
	void use();
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

#endif
