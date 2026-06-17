#pragma once
#include "VulkanCore.hpp"
#include "VulkanBuffer.hpp"
#include <string>
#include <vector>


class GraphicsPipeline {
public:
    GraphicsPipeline(const VulkanCore& core, vk::RenderPass renderPass, const std::string& vertPath, const std::string& fragPath, const VulkanBuffer& ssbo);
    ~GraphicsPipeline();

    void draw(vk::CommandBuffer cmd, uint32_t boidCount);


private:
    const VulkanCore& core;
    vk::Pipeline pipeline;
    vk::PipelineLayout pipelineLayout;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorPool descriptorPool;
    vk::DescriptorSet descriptorSet;

    vk::ShaderModule createShaderModule(const std::string& path);
    std::vector<char> readFile(const std::string& filename);
};