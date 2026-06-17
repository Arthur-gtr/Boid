#pragma once
#include <vulkan/vulkan.hpp>

class VulkanBuffer {
public:
    VulkanBuffer(vk::Device device, vk::PhysicalDevice physicalDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
    ~VulkanBuffer();

    VulkanBuffer(const VulkanBuffer&) = delete;
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;

    vk::Buffer getBuffer() const;
    vk::DeviceSize getSize() const;
    void* map();
    void unmap();

private:
    vk::Device device;
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    vk::DeviceSize bufferSize;

    uint32_t findMemoryType(vk::PhysicalDevice physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
};