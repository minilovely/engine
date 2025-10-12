#include "RenderDevice.h"
#include "glad.h"

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
