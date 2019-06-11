#pragma once
#include <BaseInclude.h>
#include "IAsset.h"
#include "StaticMesh.h"
#include "Material.h"
#include "Entity.h"
#include "../RHI/VulkanRHI/VulkanDevice.h"
#include "../RHI/VulkanRHI/VulkanDescriptorSetLayout.h"
#include "../RHI/VulkanRHI/VulkanVertexInputState.h"

#include MAP_INCLUDE_PATH

namespace Core
{
	class World : public IAsset
	{
	public:
		ctd::map<int32, std::unique_ptr<Entity>> entityDict;
		std::unique_ptr<Entity> camera;

	public:
		World();
		virtual void OnPause() override;
		virtual void OnResume() override;
		void CreateCamera();
		void CreateRenderableEntity(std::shared_ptr<StaticMesh> staticMesh, std::shared_ptr<Material> material);
		virtual ~World();
	};
}