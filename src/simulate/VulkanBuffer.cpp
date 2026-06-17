#include "VulkanBuffer.hpp"
#include <stdexcept>

VulkanBuffer::VulkanBuffer(vk::Device dev, vk::PhysicalDevice pDev, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
    : device(dev), bufferSize(size) {
    vk::BufferCreateInfo bufferInfo({}, size, usage, vk::SharingMode::eExclusive);
    buffer = device.createBuffer(bufferInfo);
    vk::MemoryRequirements memRequirements = device.getBufferMemoryRequirements(buffer);
    vk::MemoryAllocateInfo allocInfo(memRequirements.size, findMemoryType(pDev, memRequirements.memoryTypeBits, properties));
    memory = device.allocateMemory(allocInfo);
    device.bindBufferMemory(buffer, memory, 0);
}

VulkanBuffer::~VulkanBuffer() {
    device.destroyBuffer(buffer);
    device.freeMemory(memory);
}

vk::Buffer VulkanBuffer::getBuffer() const { return buffer; }
vk::DeviceSize VulkanBuffer::getSize() const { return bufferSize; }

void* VulkanBuffer::map() { return device.mapMemory(memory, 0, bufferSize, {}); }
void VulkanBuffer::unmap() { device.unmapMemory(memory); }

uint32_t VulkanBuffer::findMemoryType(vk::PhysicalDevice pDev, uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties = pDev.getMemoryProperties();
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) return i;
    }
    throw std::runtime_error("Vulkan Error");
}