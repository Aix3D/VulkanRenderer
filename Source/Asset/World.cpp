#include "World.h"
#include "../Math/MathGlobal.h"

namespace Core
{
	World::World()
	{

	}

	void World::OnPause()
	{

	}

	void World::OnResume()
	{

	}

	void World::CreateCamera()
	{
		camera = std::make_unique<Entity>();
		
		camera->transformData = std::make_unique<TransformData>();
		camera->cameraData = std::make_unique<CameraData>();
	}

	void World::CreateRenderableEntity(std::shared_ptr<StaticMesh> staticMesh, std::shared_ptr<Material> material)
	{
		std::unique_ptr<Entity> entity = std::make_unique<Entity>();

		entity->renderData = std::make_unique<RenderableEntityData>();

		entity->renderData->staticMesh = staticMesh;
		entity->renderData->material = material;

		entityDict[entity->GetID()] = std::move(entity);
	}

	World::~World()
	{

	}
}