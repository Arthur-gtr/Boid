#pragma once
#include <SFML/Window.hpp>
#include <vulkan/vulkan.hpp>
#include "VulkanBuffer.hpp"
#include "BoidTypes.hpp"
#include <memory>
#include <vector>

struct PushData {
    glm::mat4 viewProj;
    glm::vec4 color;
};

class VulkanEngine {
public:
    VulkanEngine(sf::WindowBase& window, uint32_t boidCount);
    ~VulkanEngine();
    void drawFrame(float bassLevel, float trebleLevel);

private:
    vk::Instance instance;
    vk::SurfaceKHR surface;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::Queue queue;
    uint32_t queueFamily;

    vk::SwapchainKHR swapchain;
    vk::Format swapchainFormat;
    vk::Extent2D swapchainExtent;
    std::vector<vk::Image> swapchainImages;
    std::vector<vk::ImageView> swapchainImageViews;

    vk::Image depthImage;
    vk::DeviceMemory depthImageMemory;
    vk::ImageView depthImageView;

    vk::RenderPass renderPass;
    vk::DescriptorSetLayout computeSetLayout;
    vk::DescriptorSetLayout graphicsSetLayout;
    vk::PipelineLayout computePipelineLayout;
    vk::PipelineLayout graphicsPipelineLayout;
    vk::Pipeline computePipeline;
    vk::Pipeline graphicsPipeline;
    std::vector<vk::Framebuffer> framebuffers;

    vk::CommandPool commandPool;
    vk::CommandBuffer commandBuffer;
    vk::DescriptorPool descriptorPool;
    vk::DescriptorSet computeSet;
    vk::DescriptorSet graphicsSet;

    vk::Semaphore imageAvailableSemaphore;
    vk::Semaphore renderFinishedSemaphore;
    vk::Fence inFlightFence;

    std::unique_ptr<VulkanBuffer> ubo;
    std::unique_ptr<VulkanBuffer> ssboIn;
    std::unique_ptr<VulkanBuffer> ssboOut;

    uint32_t count;

    std::unique_ptr<VulkanBuffer> ssboCellHeads;
    std::unique_ptr<VulkanBuffer> ssboNextBoids;
    vk::Pipeline computeClearPipeline;
    vk::Pipeline computeBuildPipeline;
};