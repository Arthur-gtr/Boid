#include "ComputePipeline.hpp"
#include <fstream>

ComputePipeline::ComputePipeline(const VulkanCore& coreRef, const std::string& shaderPath, const VulkanBuffer& ubo, const VulkanBuffer& ssboIn, const VulkanBuffer& ssboOut)
    : core(coreRef) {

    vk::Device device = core.getDevice();

    std::vector<vk::DescriptorSetLayoutBinding> bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eCompute),
        vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute),
        vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute)
    };

    vk::DescriptorSetLayoutCreateInfo layoutInfo({}, bindings);
    descriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);

    std::vector<vk::DescriptorPoolSize> poolSizes = {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1),
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 2)
    };

    vk::DescriptorPoolCreateInfo poolInfo({}, 1, poolSizes);
    descriptorPool = device.createDescriptorPool(poolInfo);

    vk::DescriptorSetAllocateInfo allocInfo(descriptorPool, 1, &descriptorSetLayout);
    descriptorSet = device.allocateDescriptorSets(allocInfo)[0];

    vk::DescriptorBufferInfo uboInfo(ubo.getBuffer(), 0, ubo.getSize());
    vk::DescriptorBufferInfo inInfo(ssboIn.getBuffer(), 0, ssboIn.getSize());
    vk::DescriptorBufferInfo outInfo(ssboOut.getBuffer(), 0, ssboOut.getSize());

    std::vector<vk::WriteDescriptorSet> descriptorWrites = {
        vk::WriteDescriptorSet(descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &uboInfo, nullptr),
        vk::WriteDescriptorSet(descriptorSet, 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &inInfo, nullptr),
        vk::WriteDescriptorSet(descriptorSet, 2, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &outInfo, nullptr)
    };

    device.updateDescriptorSets(descriptorWrites, {});

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 1, &descriptorSetLayout);
    pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

    vk::ShaderModule compShaderModule = createShaderModule(shaderPath);
    vk::PipelineShaderStageCreateInfo shaderStageInfo({}, vk::ShaderStageFlagBits::eCompute, compShaderModule, "main");

    vk::ComputePipelineCreateInfo pipelineInfo({}, shaderStageInfo, pipelineLayout);
    pipeline = device.createComputePipeline(nullptr, pipelineInfo).value;

    device.destroyShaderModule(compShaderModule);
}

ComputePipeline::~ComputePipeline() {
    vk::Device device = core.getDevice();
    device.destroyPipeline(pipeline);
    device.destroyPipelineLayout(pipelineLayout);
    device.destroyDescriptorPool(descriptorPool);
    device.destroyDescriptorSetLayout(descriptorSetLayout);
}

void ComputePipeline::dispatch(vk::CommandBuffer cmd, uint32_t boidCount) {
    cmd.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    cmd.dispatch((boidCount + 255) / 256, 1, 1);
}

vk::DescriptorSet ComputePipeline::getDescriptorSet() const { return descriptorSet; }

vk::ShaderModule ComputePipeline::createShaderModule(const std::string& path) {
    std::vector<char> code = readFile(path);
    vk::ShaderModuleCreateInfo createInfo({}, code.size(), reinterpret_cast<const uint32_t*>(code.data()));
    return core.getDevice().createShaderModule(createInfo);
}

std::vector<char> ComputePipeline::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Can't load the shader : " + filename);
    }
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}