#pragma once
#include "VulkanCore.hpp"

class VulkanBuffer {
public:
    VulkanBuffer(const VulkanCore& core, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
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