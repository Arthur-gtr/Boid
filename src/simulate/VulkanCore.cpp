#include "VulkanCore.hpp"

VulkanCore::VulkanCore() {
    createInstance();
    pickPhysicalDevice();
    createLogicalDevice();
}

VulkanCore::~VulkanCore() {
    device.destroy();
    instance.destroy();
}

vk::Instance VulkanCore::getInstance() const { return instance; }
vk::PhysicalDevice VulkanCore::getPhysicalDevice() const { return physicalDevice; }
vk::Device VulkanCore::getDevice() const { return device; }
vk::Queue VulkanCore::getGraphicsQueue() const { return graphicsQueue; }
vk::Queue VulkanCore::getComputeQueue() const { return computeQueue; }
uint32_t VulkanCore::getGraphicsQueueFamily() const { return graphicsQueueFamily; }
uint32_t VulkanCore::getComputeQueueFamily() const { return computeQueueFamily; }

void VulkanCore::createInstance() {
    vk::ApplicationInfo appInfo("BoidsCompute", 1, "BoidsEngine", 1, VK_API_VERSION_1_3);
    vk::InstanceCreateInfo createInfo({}, &appInfo);
    instance = vk::createInstance(createInfo);
}

void VulkanCore::pickPhysicalDevice() {
    std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
    if (devices.empty()) throw std::runtime_error("");
    physicalDevice = devices.front();
}

void VulkanCore::createLogicalDevice() {
    std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();
    bool foundGraphics = false;
    bool foundCompute = false;

    for (uint32_t i = 0; i < queueFamilies.size(); i++) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            graphicsQueueFamily = i;
            foundGraphics = true;
        } else if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) {
            computeQueueFamily = i;
            foundCompute = true;
        }
    }

    if (!foundCompute && foundGraphics) {
        computeQueueFamily = graphicsQueueFamily;
    }

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;
    queueCreateInfos.push_back(vk::DeviceQueueCreateInfo({}, graphicsQueueFamily, 1, &queuePriority));
    if (graphicsQueueFamily != computeQueueFamily) {
        queueCreateInfos.push_back(vk::DeviceQueueCreateInfo({}, computeQueueFamily, 1, &queuePriority));
    }

    vk::PhysicalDeviceFeatures deviceFeatures;
    vk::DeviceCreateInfo createInfo({}, queueCreateInfos, {}, {}, &deviceFeatures);
    device = physicalDevice.createDevice(createInfo);

    graphicsQueue = device.getQueue(graphicsQueueFamily, 0);
    computeQueue = device.getQueue(computeQueueFamily, 0);
}