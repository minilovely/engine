#include "RenderQueue.h"
#include <algorithm>


void RenderQueue::Clear()
{
	cmds.clear();
}

void RenderQueue::Add(const RenderCommand& cmd)
{
	cmds.push_back(cmd);
}

void RenderQueue::Sort()
{
	std::stable_sort(cmds.begin(), cmds.end(),
		[](const auto& a, const auto& b) {return a.value < b.value; });
}

void RenderQueue::Draw() const
{
    for (const auto& cmd : cmds) {
        cmd.shader->setMat4("MVP", cmd.MVP);
        cmd.shader->setMat4("M", cmd.modelMatrix);
        cmd.shader->setVec3("viewPos", cmd.viewPos);
        cmd.material->bindTextures();
        cmd.mesh->Bind();
        cmd.mesh->Draw();
    }
}


