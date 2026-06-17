#include "SimulationEngine.hpp"
#include <random>

SimulationEngine::SimulationEngine(const VulkanCore& coreRef, uint32_t boidCount)
    : core(coreRef), count(boidCount) {

    vk::Device device = core.getDevice();

    vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, core.getComputeQueueFamily());
    commandPool = device.createCommandPool(poolInfo);

    vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);
    commandBuffer = device.allocateCommandBuffers(allocInfo)[0];

    vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);
    computeFence = device.createFence(fenceInfo);

    vk::SemaphoreCreateInfo semaphoreInfo;
    computeSemaphore = device.createSemaphore(semaphoreInfo);

    initializeBuffers();

    computePipeline = std::make_unique<ComputePipeline>(core, "boids.comp.spv", *ubo, *ssboIn, *ssboOut);

    recordComputeCommands();
}

SimulationEngine::~SimulationEngine() {
    vk::Device device = core.getDevice();
    device.waitForFences(1, &computeFence, VK_TRUE, UINT64_MAX);
    device.destroySemaphore(computeSemaphore);
    device.destroyFence(computeFence);
    device.destroyCommandPool(commandPool);
}

void SimulationEngine::initializeBuffers() {
    vk::DeviceSize uboSize = sizeof(SimParams);
    vk::DeviceSize ssboSize = count * sizeof(Boid);

    ubo = std::make_unique<VulkanBuffer>(core, uboSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    ssboIn = std::make_unique<VulkanBuffer>(core, ssboSize, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eDeviceLocal);
    ssboOut = std::make_unique<VulkanBuffer>(core, ssboSize, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

    SimParams params = {0.016f, count, 0.1f, 0.05f, 0.1f, 0.02f, 0.05f, 0.03f, 0.5f};
    void* data = ubo->map();
    memcpy(data, &params, sizeof(params));
    ubo->unmap();

    VulkanBuffer stagingBuffer(core, ssboSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    Boid* boids = static_cast<Boid*>(stagingBuffer.map());

    std::mt19937 rnd(42);
    std::uniform_real_distribution<float> distPos(-1.0f, 1.0f);
    std::uniform_real_distribution<float> distVel(-0.1f, 0.1f);

    for (uint32_t i = 0; i < count; i++) {
        boids[i].position = glm::vec2(distPos(rnd), distPos(rnd));
        boids[i].velocity = glm::vec2(distVel(rnd), distVel(rnd));
    }
    stagingBuffer.unmap();

    vk::CommandBufferAllocateInfo cmdAllocInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);
    vk::CommandBuffer stagingCmd = core.getDevice().allocateCommandBuffers(cmdAllocInfo)[0];

    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    stagingCmd.begin(beginInfo);
    vk::BufferCopy copyRegion(0, 0, ssboSize);
    stagingCmd.copyBuffer(stagingBuffer.getBuffer(), ssboIn->getBuffer(), 1, &copyRegion);
    stagingCmd.end();

    vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &stagingCmd, 0, nullptr);
    core.getComputeQueue().submit(1, &submitInfo, nullptr);
    core.getComputeQueue().waitIdle();
    core.getDevice().freeCommandBuffers(commandPool, 1, &stagingCmd);
}

void SimulationEngine::recordComputeCommands() {
    vk::CommandBufferBeginInfo beginInfo;
    commandBuffer.begin(beginInfo);

    computePipeline->dispatch(commandBuffer, count);

    vk::BufferCopy copyRegion(0, 0, ssboOut->getSize());
    commandBuffer.copyBuffer(ssboOut->getBuffer(), ssboIn->getBuffer(), 1, &copyRegion);

    vk::BufferMemoryBarrier barrier(
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eShaderRead,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        ssboIn->getBuffer(),
        0,
        ssboIn->getSize()
    );

    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eVertexShader,
        {}, 0, nullptr, 1, &barrier, 0, nullptr
    );

    commandBuffer.end();
}

void SimulationEngine::step() {
    vk::Device device = core.getDevice();
    device.waitForFences(1, &computeFence, VK_TRUE, UINT64_MAX);
    device.resetFences(1, &computeFence);

    vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &commandBuffer, 0, nullptr);
    core.getComputeQueue().submit(1, &submitInfo, computeFence);
}

const VulkanBuffer& SimulationEngine::getParticleBuffer() const {
    return *ssboIn;
}

std::vector<Boid> SimulationEngine::getBoidsData() {
    vk::DeviceSize ssboSize = count * sizeof(Boid);
    
    VulkanBuffer readbackBuffer(core, ssboSize, vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);
    vk::CommandBuffer copyCmd = core.getDevice().allocateCommandBuffers(allocInfo)[0];

    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    copyCmd.begin(beginInfo);
    vk::BufferCopy copyRegion(0, 0, ssboSize);
    copyCmd.copyBuffer(ssboIn->getBuffer(), readbackBuffer.getBuffer(), 1, &copyRegion);
    copyCmd.end();

    vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &copyCmd, 0, nullptr);
    core.getComputeQueue().submit(1, &submitInfo, nullptr);
    core.getComputeQueue().waitIdle(); 

    std::vector<Boid> boids(count);
    void* data = readbackBuffer.map();
    memcpy(boids.data(), data, ssboSize);
    readbackBuffer.unmap();

    core.getDevice().freeCommandBuffers(commandPool, 1, &copyCmd);

    return boids;
}