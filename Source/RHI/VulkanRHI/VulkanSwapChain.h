#pragma once
#include "VulkanBase.h"
#include "VulkanImageView.h"

#include VECTOR_INCLUDE_PATH

namespace Core
{
	class VulkanSwapChain
	{
	private:
		VkInstance m_instanceRef;
		VkPhysicalDevice m_physicalDeviceRef;
		VkDevice m_logicalDeviceRef;

		VkSurfaceKHR m_surface;

		uint32 m_queueNodeIndex;
		VkFormat m_colorFormat;
		VkColorSpaceKHR m_colorSpace;

		VkSwapchainKHR m_swapChain;

		ctd::vector<VkImage> m_images;
		ctd::vector<std::unique_ptr<VulkanImageView>> m_imageViews;

		PFN_vkGetPhysicalDeviceSurfaceSupportKHR m_FPGetPhysicalDeviceSurfaceSupportKHR;
		PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR m_FPGetPhysicalDeviceSurfaceCapabilitiesKHR;
		PFN_vkGetPhysicalDeviceSurfaceFormatsKHR m_FPGetPhysicalDeviceSurfaceFormatsKHR;
		PFN_vkGetPhysicalDeviceSurfacePresentModesKHR m_FPGetPhysicalDeviceSurfacePresentModesKHR;
		PFN_vkCreateSwapchainKHR m_FPCreateSwapchainKHR;
		PFN_vkDestroySwapchainKHR m_FPDestroySwapchainKHR;
		PFN_vkGetSwapchainImagesKHR m_FPGetSwapchainImagesKHR;
		PFN_vkAcquireNextImageKHR m_FPAcquireNextImageKHR;
		PFN_vkQueuePresentKHR m_FPQueuePresentKHR;

	public:
		VulkanSwapChain();
		void Connect(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice logicalDevice);
		void InitializeSurface(void* platformHandle, void* platformWindow, ctd::vector<VkQueueFamilyProperties> & m_queueFamilyProperties);
		void Prepare(uint32 * pWidth, uint32 * pHeight, Bool vSync);
		uint32 GetImageCount() const;
		VkFormat GetColorFormat() const;
		VkImageView GetView(int32 index);
		VkResult AcquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t *imageIndex);
		VkResult QueuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);
		~VulkanSwapChain();
	};
}