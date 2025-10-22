#include "PassShadow.h"
#include "../Assets/PassAssets.h"
#include "../Scene/Mesh.h"
#include "../Scene/Actor.h"
#include "../Scene/Transform.h"
#include "../System/LightManager.h"

void PassShadow::Init()
{
	std::shared_ptr<PassAssets> asset = std::make_shared<PassAssets>();
	asset->Load("Assets/Passes_json/shadow.json");
	shader = asset->getShader();

}

void PassShadow::Collect(const Camera& cam, Mesh* mesh, RenderQueue& outQueue)
{
	auto* trans = mesh->GetOwner()->GetComponent<Transform>();
	const auto& lights = LightManager::Get().getActiveLight();

}
