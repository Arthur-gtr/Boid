#include "VulkanBuffer.hpp"

VulkanBuffer::VulkanBuffer(const VulkanCore& core, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
    : device(core.getDevice()), bufferSize(size) {

    vk::BufferCreateInfo bufferInfo({}, size, usage, vk::SharingMode::eExclusive);
    buffer = device.createBuffer(bufferInfo);

    vk::MemoryRequirements memRequirements = device.getBufferMemoryRequirements(buffer);
    vk::MemoryAllocateInfo allocInfo(memRequirements.size, findMemoryType(core.getPhysicalDevice(), memRequirements.memoryTypeBits, properties));

    memory = device.allocateMemory(allocInfo);
    device.bindBufferMemory(buffer, memory, 0);
}

VulkanBuffer::~VulkanBuffer() {
    device.destroyBuffer(buffer);
    device.freeMemory(memory);
}

vk::Buffer VulkanBuffer::getBuffer() const { return buffer; }
vk::DeviceSize VulkanBuffer::getSize() const { return bufferSize; }

void* VulkanBuffer::map() {
    return device.mapMemory(memory, 0, bufferSize, {});
}

void VulkanBuffer::unmap() {
    device.unmapMemory(memory);
}

uint32_t VulkanBuffer::findMemoryType(vk::PhysicalDevice physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("");
}