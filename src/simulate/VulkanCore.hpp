#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <stdexcept>

class VulkanCore {
public:
    VulkanCore();
    ~VulkanCore();

    VulkanCore(const VulkanCore&) = delete;
    VulkanCore& operator=(const VulkanCore&) = delete;

    vk::Instance getInstance() const;
    vk::PhysicalDevice getPhysicalDevice() const;
    vk::Device getDevice() const;
    vk::Queue getGraphicsQueue() const;
    vk::Queue getComputeQueue() const;
    uint32_t getGraphicsQueueFamily() const;
    uint32_t getComputeQueueFamily() const;

private:
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::Queue graphicsQueue;
    vk::Queue computeQueue;
    uint32_t graphicsQueueFamily;
    uint32_t computeQueueFamily;

    void createInstance();
    void pickPhysicalDevice();
    void createLogicalDevice();
};