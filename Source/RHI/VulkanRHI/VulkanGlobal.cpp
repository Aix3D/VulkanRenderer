#include "VulkanGlobal.h"

namespace Core
{
	Core::uint32 VulkanGlobal::GetMemoryType(uint32 typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound /*= nullptr*/)
	{
		for (uint32 i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i)
		{
			if ((typeBits & 1) == 1)
			{
				if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					if (memTypeFound)
					{
						*memTypeFound = true;
					}

					return i;
				}
			}

			typeBits >>= 1;
		}

		if (memTypeFound)
		{
			*memTypeFound = false;

			return 0;
		}
		else
		{
			assert(False);
			return 0;
		}
	}

}