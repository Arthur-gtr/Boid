#pragma once
#include "VulkanCore.hpp"
#include "VulkanBuffer.hpp"
#include <string>
#include <vector>

class ComputePipeline {
public:
    ComputePipeline(const VulkanCore& core, const std::string& shaderPath, const VulkanBuffer& ubo, const VulkanBuffer& ssboIn, const VulkanBuffer& ssboOut);
    ~ComputePipeline();

    void dispatch(vk::CommandBuffer cmd, uint32_t boidCount);
    vk::DescriptorSet getDescriptorSet() const;

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