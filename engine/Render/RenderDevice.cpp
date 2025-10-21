#include "RenderDevice.h"
#include "glad.h"

#include <string>
#include <iostream>

void RenderDevice::SetViewport(int x, int y, int w, int h)
{
	glViewport(x, y, w, h);
}

void RenderDevice::Clear(const glm::vec3& c)
{
	glClearColor(c.r, c.g, c.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderDevice::SetDepthTest(bool enable)
{
	enable ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void RenderDevice::SetDepthWrite(bool enable)
{
	enable ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
}

void RenderDevice::SetColorWrite(bool enable)
{
	enable ? glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE)
		: glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
}

void RenderDevice::SetCullMode(std::string mode)
{
	if (mode == "Back")
	{
		glCullFace(GL_BACK);
		return;
	}
	if (mode == "Front")
	{
		glCullFace(GL_FRONT);
		return;
	}
	if (mode == "Front_and_Back")
	{
		glCullFace(GL_FRONT_AND_BACK);
		return;
	}
	std::cout << "[RenderDevice] cullMode setting failed, cullMode only accepts 'Back','Front',Front_and_Back'" << std::endl;
}

void RenderDevice::SetCullEnabled(bool on)
{
	on ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}
