#include "GraphicsPipeline.hpp"
#include <fstream>

GraphicsPipeline::GraphicsPipeline(const VulkanCore& coreRef, vk::RenderPass renderPass, const std::string& vertPath, const std::string& fragPath, const VulkanBuffer& ssbo)
    : core(coreRef) {

    vk::Device device = core.getDevice();

    vk::DescriptorSetLayoutBinding ssboBinding(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex);
    vk::DescriptorSetLayoutCreateInfo layoutInfo({}, 1, &ssboBinding);
    descriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);

    vk::DescriptorPoolSize poolSize(vk::DescriptorType::eStorageBuffer, 1);
    vk::DescriptorPoolCreateInfo poolInfo({}, 1, 1, &poolSize);
    descriptorPool = device.createDescriptorPool(poolInfo);

    vk::DescriptorSetAllocateInfo allocInfo(descriptorPool, 1, &descriptorSetLayout);
    descriptorSet = device.allocateDescriptorSets(allocInfo)[0];

    vk::DescriptorBufferInfo ssboInfo(ssbo.getBuffer(), 0, ssbo.getSize());
    vk::WriteDescriptorSet descriptorWrite(descriptorSet, 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &ssboInfo, nullptr);
    device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 1, &descriptorSetLayout);
    pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

    vk::ShaderModule vertShaderModule = createShaderModule(vertPath);
    vk::ShaderModule fragShaderModule = createShaderModule(fragPath);

    vk::PipelineShaderStageCreateInfo shaderStages[] = {
        vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertShaderModule, "main"),
        vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragShaderModule, "main")
    };

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo({}, 0, nullptr, 0, nullptr);
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly({}, vk::PrimitiveTopology::eTriangleList, VK_FALSE);
    vk::PipelineViewportStateCreateInfo viewportState({}, 1, nullptr, 1, nullptr);
    vk::PipelineRasterizationStateCreateInfo rasterizer({}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f);
    vk::PipelineMultisampleStateCreateInfo multisampling({}, vk::SampleCountFlagBits::e1, VK_FALSE);
    vk::PipelineColorBlendAttachmentState colorBlendAttachment(VK_FALSE, vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    vk::PipelineColorBlendStateCreateInfo colorBlending({}, VK_FALSE, vk::LogicOp::eCopy, 1, &colorBlendAttachment);

    std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo dynamicState({}, dynamicStates);

    vk::GraphicsPipelineCreateInfo pipelineInfo({}, 2, shaderStages, &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling, nullptr, &colorBlending, &dynamicState, pipelineLayout, renderPass, 0);

    pipeline = device.createGraphicsPipeline(nullptr, pipelineInfo).value;

    device.destroyShaderModule(vertShaderModule);
    device.destroyShaderModule(fragShaderModule);
}

GraphicsPipeline::~GraphicsPipeline() {
    vk::Device device = core.getDevice();
    device.destroyPipeline(pipeline);
    device.destroyPipelineLayout(pipelineLayout);
    device.destroyDescriptorPool(descriptorPool);
    device.destroyDescriptorSetLayout(descriptorSetLayout);
}

void GraphicsPipeline::draw(vk::CommandBuffer cmd, uint32_t boidCount) {
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    cmd.draw(3, boidCount, 0, 0);
}

vk::ShaderModule GraphicsPipeline::createShaderModule(const std::string& path) {
    std::vector<char> code = readFile(path);
    vk::ShaderModuleCreateInfo createInfo({}, code.size(), reinterpret_cast<const uint32_t*>(code.data()));
    return core.getDevice().createShaderModule(createInfo);
}

std::vector<char> GraphicsPipeline::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
    throw std::runtime_error("Can't load the shader: " + filename);
}
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}