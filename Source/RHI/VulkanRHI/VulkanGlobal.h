#pragma once
#include "VulkanBase.h"
#include "..\..\DesignPattern\Singleton.h"

namespace Core
{
	class VulkanGlobal : public Singleton<VulkanGlobal>
	{
	public:
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

		uint32 GetMemoryType(uint32 typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound = nullptr);
	};
}