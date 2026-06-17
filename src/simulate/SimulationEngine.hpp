#pragma once
#include "VulkanCore.hpp"
#include "VulkanBuffer.hpp"
#include "ComputePipeline.hpp"
#include "BoidTypes.hpp"
#include <memory>

class SimulationEngine {
public:
    SimulationEngine(const VulkanCore& core, uint32_t boidCount);
    ~SimulationEngine();

    void step();
    const VulkanBuffer& getParticleBuffer() const;
    std::vector<Boid> getBoidsData();


private:
    const VulkanCore& core;
    uint32_t count;

    std::unique_ptr<VulkanBuffer> ubo;
    std::unique_ptr<VulkanBuffer> ssboIn;
    std::unique_ptr<VulkanBuffer> ssboOut;
    std::unique_ptr<ComputePipeline> computePipeline;

    vk::CommandPool commandPool;
    vk::CommandBuffer commandBuffer;
    vk::Fence computeFence;
    vk::Semaphore computeSemaphore;

    void initializeBuffers();
    void recordComputeCommands();
};