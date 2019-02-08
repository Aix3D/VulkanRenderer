#include "VulkanSwapChain.h"

namespace Core
{
	using namespace ctd;

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                        \
{                                                                       \
	m_FP##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetInstanceProcAddr(inst, "vk"#entrypoint));                                                              \
}

#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                           \
{                                                                       \
	m_FP##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetDeviceProcAddr(dev, "vk"#entrypoint));                                                           \
}

	VulkanSwapChain::VulkanSwapChain()
	{

	}

	void VulkanSwapChain::Connect(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice logicalDevice)
	{
		m_instanceRef = instance;
		m_physicalDeviceRef = physicalDevice;
		m_logicalDeviceRef = logicalDevice;

		GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceSupportKHR);
		GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
		GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
		GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfacePresentModesKHR);
		GET_DEVICE_PROC_ADDR(logicalDevice, CreateSwapchainKHR);
		GET_DEVICE_PROC_ADDR(logicalDevice, DestroySwapchainKHR);
		GET_DEVICE_PROC_ADDR(logicalDevice, GetSwapchainImagesKHR);
		GET_DEVICE_PROC_ADDR(logicalDevice, AcquireNextImageKHR);
		GET_DEVICE_PROC_ADDR(logicalDevice, QueuePresentKHR);
	}

	void VulkanSwapChain::InitializeSurface(void* platformHandle, void* platformWindow, ctd::vector<VkQueueFamilyProperties> & m_queueFamilyProperties)
	{
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.hinstance = (HINSTANCE)platformHandle;
		surfaceCreateInfo.hwnd = (HWND)platformWindow;
		VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(m_instanceRef, &surfaceCreateInfo, nullptr, &m_surface));
	
		vector<VkBool32> supportsPresent(m_queueFamilyProperties.size());
		
		for (size_t i = 0; i < supportsPresent.size(); ++i)
			m_FPGetPhysicalDeviceSurfaceSupportKHR(m_physicalDeviceRef, static_cast<uint32>(i), m_surface, &supportsPresent[i]);

		uint32 graphicsQueueNodeIndex = MaxUint32;
		uint32 presentQueueNodeIndex = MaxUint32;

		for (size_t i = 0; i < supportsPresent.size(); ++i)
		{
			if ((m_queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
			{
				if (graphicsQueueNodeIndex == MaxUint32)
					graphicsQueueNodeIndex = static_cast<uint32>(i);

				if (supportsPresent[i] == VK_TRUE)
				{
					graphicsQueueNodeIndex = static_cast<uint32>(i);
					presentQueueNodeIndex = static_cast<uint32>(i);
					break;
				}
			}
		}

		if (presentQueueNodeIndex == MaxUint32)
		{
			for (size_t i = 0; i < supportsPresent.size(); ++i)
			{
				if (supportsPresent[i] == VK_TRUE)
				{
					presentQueueNodeIndex = static_cast<uint32>(i);
					break;
				}
			}
		}

		//	XXX:	
		//		process later
		assert(graphicsQueueNodeIndex == presentQueueNodeIndex);

		m_queueNodeIndex = graphicsQueueNodeIndex;

		uint32 formatCount;
		m_FPGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDeviceRef, m_surface, &formatCount, NULL);

		vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		m_FPGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDeviceRef, m_surface, &formatCount, surfaceFormats.data());

		if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			m_colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
			m_colorSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			Bool found_B8G8R8A8_UNORM = False;

			for (auto&& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
				{
					m_colorFormat = surfaceFormat.format;
					m_colorSpace = surfaceFormat.colorSpace;
					found_B8G8R8A8_UNORM = true;
					break;
				}
			}

			if (!found_B8G8R8A8_UNORM)
			{
				m_colorFormat = surfaceFormats[0].format;
				m_colorSpace = surfaceFormats[0].colorSpace;
			}
		}
	}

	void VulkanSwapChain::Prepare(uint32 * pWidth, uint32 * pHeight, Bool vSync)
	{
		VkSurfaceCapabilitiesKHR surfCaps;
		m_FPGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDeviceRef, m_surface, &surfCaps);
		
		uint32 presentModeCount;
		m_FPGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDeviceRef, m_surface, &presentModeCount, NULL);

		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		m_FPGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDeviceRef, m_surface, &presentModeCount, presentModes.data());
		
		VkExtent2D swapchainExtent = {};
		// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
		if (surfCaps.currentExtent.width == (uint32_t)-1)
		{
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			surfCaps.currentExtent.width = *pWidth;
			surfCaps.currentExtent.height = *pHeight;
		}
		else
		{
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = surfCaps.currentExtent;
			*pWidth = surfCaps.currentExtent.width;
			*pHeight = surfCaps.currentExtent.height;
		}

		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

		if (!vSync)
		{
			for (uint32 i = 0; i < presentModeCount; ++i)
			{
				if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				}

				if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
					(presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
				{
					swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
			}
		}

		uint32 desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
		if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
			desiredNumberOfSwapchainImages = surfCaps.maxImageCount;

		VkSurfaceTransformFlagsKHR preTransform;
		if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		else
			preTransform = surfCaps.currentTransform;

		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags =
		{
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};

		for (auto& compositeAlphaFlag : compositeAlphaFlags)
		{
			if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag)
			{
				compositeAlpha = compositeAlphaFlag;
				break;
			};
		}

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.pNext = NULL;
		swapchainCreateInfo.surface = m_surface;
		swapchainCreateInfo.minImageCount = desiredNumberOfSwapchainImages;
		swapchainCreateInfo.imageFormat = m_colorFormat;
		swapchainCreateInfo.imageColorSpace = m_colorSpace;
		swapchainCreateInfo.imageExtent = { swapchainExtent.width, swapchainExtent.height };
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = NULL;
		swapchainCreateInfo.presentMode = swapchainPresentMode;
		swapchainCreateInfo.oldSwapchain = Null;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.compositeAlpha = compositeAlpha;

		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(m_physicalDeviceRef, m_colorFormat, &formatProps);
		
		if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT)
			swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

		m_FPCreateSwapchainKHR(m_logicalDeviceRef, &swapchainCreateInfo, nullptr, &m_swapChain);

		//	TODO:
		//		old swap chain processed later
		//if (oldSwapchain != VK_NULL_HANDLE)
		//{
		//	for (uint32_t i = 0; i < imageCount; i++)
		//	{
		//		vkDestroyImageView(device, buffers[i].view, nullptr);
		//	}
		//	fpDestroySwapchainKHR(device, oldSwapchain, nullptr);
		//}

		uint32 imageCount;
		m_FPGetSwapchainImagesKHR(m_logicalDeviceRef, m_swapChain, &imageCount, NULL);

		m_images.resize(imageCount);
		m_FPGetSwapchainImagesKHR(m_logicalDeviceRef, m_swapChain, &imageCount, m_images.data());

		for (uint32 i = 0; i < imageCount; ++i)
		{
			std::unique_ptr<VulkanImageView> imageView = std::make_unique<VulkanImageView>(
				m_logicalDeviceRef,
				m_colorFormat,
				VK_IMAGE_ASPECT_COLOR_BIT,
				m_images[i]);

			m_imageViews.push_back(std::move(imageView));
		}
	}

	Core::uint32 VulkanSwapChain::GetImageCount() const
	{
		return static_cast<uint32>(m_images.size());
	}

	VkFormat VulkanSwapChain::GetColorFormat() const
	{
		return m_colorFormat;
	}

	VkImageView VulkanSwapChain::GetView(int32 index)
	{
		return m_imageViews[index]->GetHandle();
	}

	VkResult VulkanSwapChain::AcquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t *imageIndex)
	{
		return m_FPAcquireNextImageKHR(m_logicalDeviceRef, m_swapChain, UINT64_MAX, presentCompleteSemaphore, (VkFence)nullptr, imageIndex);
	}

	VkResult VulkanSwapChain::QueuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore /*= VK_NULL_HANDLE*/)
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = NULL;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_swapChain;
		presentInfo.pImageIndices = &imageIndex;

		// Check if a wait semaphore has been specified to wait for before presenting the image
		if (waitSemaphore != VK_NULL_HANDLE)
		{
			presentInfo.pWaitSemaphores = &waitSemaphore;
			presentInfo.waitSemaphoreCount = 1;
		}

		return m_FPQueuePresentKHR(queue, &presentInfo);
	}

	VulkanSwapChain::~VulkanSwapChain()
	{

	}

}