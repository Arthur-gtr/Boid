#include "VulkanEngine.hpp"
#include <SFML/Window/Vulkan.hpp>
#include <fstream>
#include <stdexcept>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>
#include <random>

static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("file not found: " + filename);
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0); file.read(buffer.data(), fileSize); file.close();
    return buffer;
}

VulkanEngine::VulkanEngine(sf::WindowBase& window, uint32_t boidCount) : count(boidCount) {
    auto exts = sf::Vulkan::getGraphicsRequiredInstanceExtensions();
    vk::InstanceCreateInfo instInfo({}, nullptr, 0, nullptr, exts.size(), exts.data());
    instance = vk::createInstance(instInfo);

    VkSurfaceKHR c_surface;
    if (!window.createVulkanSurface(instance, c_surface))
        throw std::runtime_error("Surface SFML failed");
    surface = c_surface;

    std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
    physicalDevice = devices.front();

    std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();
    queueFamily = 0;
    for (uint32_t i = 0; i < queueFamilies.size(); i++) {
        if ((queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) && (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) && physicalDevice.getSurfaceSupportKHR(i, surface)) {
            queueFamily = i;
            break;
        }
    }

    float priority = 1.0f;
    vk::DeviceQueueCreateInfo qInfo({}, queueFamily, 1, &priority);
    const char* deviceExts[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    vk::PhysicalDeviceFeatures features;
    vk::DeviceCreateInfo devInfo({}, 1, &qInfo, 0, nullptr, 1, deviceExts, &features);
    device = physicalDevice.createDevice(devInfo);
    queue = device.getQueue(queueFamily, 0);

    vk::SurfaceCapabilitiesKHR caps = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    swapchainFormat = vk::Format::eB8G8R8A8Unorm;
    swapchainExtent = caps.currentExtent;
    vk::SwapchainCreateInfoKHR swpInfo({}, surface, caps.minImageCount + 1, swapchainFormat, vk::ColorSpaceKHR::eSrgbNonlinear, swapchainExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 0, nullptr, caps.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, vk::PresentModeKHR::eFifo, VK_TRUE, nullptr);
    swapchain = device.createSwapchainKHR(swpInfo);
    swapchainImages = device.getSwapchainImagesKHR(swapchain);

    for (auto img : swapchainImages) {
        swapchainImageViews.push_back(device.createImageView({{}, img, vk::ImageViewType::e2D, swapchainFormat, {}, {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}}));
    }

    vk::Format depthFmt = vk::Format::eD32Sfloat;
    vk::ImageCreateInfo dImgInfo({}, vk::ImageType::e2D, depthFmt, {swapchainExtent.width, swapchainExtent.height, 1}, 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment);
    depthImage = device.createImage(dImgInfo);
    vk::MemoryRequirements memReq = device.getImageMemoryRequirements(depthImage);
    vk::PhysicalDeviceMemoryProperties memProps = physicalDevice.getMemoryProperties();
    uint32_t memType = 0;

    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
        if ((memReq.memoryTypeBits & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal) {
            memType = i; 
            break; 
        }
    }
    depthImageMemory = device.allocateMemory({memReq.size, memType});
    device.bindImageMemory(depthImage, depthImageMemory, 0);
    depthImageView = device.createImageView({{}, depthImage, vk::ImageViewType::e2D, depthFmt, {}, {vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1}});

    vk::AttachmentDescription colorAtt({}, swapchainFormat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
    vk::AttachmentDescription depthAtt({}, depthFmt, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::AttachmentReference colorRef(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depthRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorRef, nullptr, &depthRef);
    vk::AttachmentDescription attachments[] = {colorAtt, depthAtt};
    vk::RenderPassCreateInfo rpInfo({}, 2, attachments, 1, &subpass);
    renderPass = device.createRenderPass(rpInfo);

    for (auto iv : swapchainImageViews) {
        vk::ImageView views[] = {iv, depthImageView};
        framebuffers.push_back(device.createFramebuffer({{}, renderPass, 2, views, swapchainExtent.width, swapchainExtent.height, 1}));
    }

    std::vector<vk::DescriptorSetLayoutBinding> cBinds = {
        {0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eCompute},
        {1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute},
        {2, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute}};
    computeSetLayout = device.createDescriptorSetLayout({{}, cBinds});
    
    std::vector<vk::DescriptorSetLayoutBinding> gBinds = {{0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex}};
    graphicsSetLayout = device.createDescriptorSetLayout({{}, gBinds});

    vk::PushConstantRange pcRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushData));
    computePipelineLayout = device.createPipelineLayout({{}, 1, &computeSetLayout});
    graphicsPipelineLayout = device.createPipelineLayout({{}, 1, &graphicsSetLayout, 1, &pcRange});

    auto compCode = readFile("boids.comp.spv");
    vk::ShaderModule compMod = device.createShaderModule({{}, compCode.size(), reinterpret_cast<const uint32_t*>(compCode.data())});
    vk::ComputePipelineCreateInfo cpInfo({}, {{}, vk::ShaderStageFlagBits::eCompute, compMod, "main"}, computePipelineLayout);
    computePipeline = device.createComputePipeline(nullptr, cpInfo).value;
    device.destroyShaderModule(compMod);

    auto vertCode = readFile("boids.vert.spv");
    auto fragCode = readFile("boids.frag.spv");
    vk::ShaderModule vertMod = device.createShaderModule({{}, vertCode.size(), reinterpret_cast<const uint32_t*>(vertCode.data())});
    vk::ShaderModule fragMod = device.createShaderModule({{}, fragCode.size(), reinterpret_cast<const uint32_t*>(fragCode.data())});
    vk::PipelineShaderStageCreateInfo sStages[] = {{{}, vk::ShaderStageFlagBits::eVertex, vertMod, "main"}, {{}, vk::ShaderStageFlagBits::eFragment, fragMod, "main"}};
    vk::PipelineVertexInputStateCreateInfo vInput;
    vk::PipelineInputAssemblyStateCreateInfo iAsm({}, vk::PrimitiveTopology::eTriangleList, VK_FALSE);
    vk::Viewport vp(0, 0, (float)swapchainExtent.width, (float)swapchainExtent.height, 0.0f, 1.0f);
    vk::Rect2D sc({0, 0}, swapchainExtent);
    vk::PipelineViewportStateCreateInfo vpState({}, 1, &vp, 1, &sc);
    vk::PipelineRasterizationStateCreateInfo rast({}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, VK_FALSE, 0, 0, 0, 1.0f);
    vk::PipelineMultisampleStateCreateInfo ms({}, vk::SampleCountFlagBits::e1);
    vk::PipelineDepthStencilStateCreateInfo ds({}, VK_TRUE, VK_TRUE, vk::CompareOp::eLess);
    vk::PipelineColorBlendAttachmentState cBlendAtt(VK_FALSE, vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    vk::PipelineColorBlendStateCreateInfo cBlend({}, VK_FALSE, vk::LogicOp::eCopy, 1, &cBlendAtt);
    vk::GraphicsPipelineCreateInfo gpInfo({}, 2, sStages, &vInput, &iAsm, nullptr, &vpState, &rast, &ms, &ds, &cBlend, nullptr, graphicsPipelineLayout, renderPass, 0);
    graphicsPipeline = device.createGraphicsPipeline(nullptr, gpInfo).value;
    device.destroyShaderModule(vertMod); device.destroyShaderModule(fragMod);

    commandPool = device.createCommandPool({vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queueFamily});
    commandBuffer = device.allocateCommandBuffers({commandPool, vk::CommandBufferLevel::ePrimary, 1})[0];
    imageAvailableSemaphore = device.createSemaphore({});
    renderFinishedSemaphore = device.createSemaphore({});
    inFlightFence = device.createFence({vk::FenceCreateFlagBits::eSignaled});

    vk::DeviceSize ssboSize = count * sizeof(Boid);
    ubo = std::make_unique<VulkanBuffer>(device, physicalDevice, sizeof(SimParams), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    ssboIn = std::make_unique<VulkanBuffer>(device, physicalDevice, ssboSize, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eDeviceLocal);
    ssboOut = std::make_unique<VulkanBuffer>(device, physicalDevice, ssboSize, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eDeviceLocal);

    SimParams params = {0.016f, count, 0.4f, 0.2f, 0.4f, 0.005f, 0.02f, 0.01f, 0.5f, 0.0f, 0.0f, 0.0f};
    void* uData = ubo->map(); memcpy(uData, &params, sizeof(params)); ubo->unmap();

    VulkanBuffer stage(device, physicalDevice, ssboSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    Boid* boids = static_cast<Boid*>(stage.map());
    std::mt19937 rnd(42); std::uniform_real_distribution<float> dPos(-4.0f, 4.0f), dVel(-0.5f, 0.5f);
    for (uint32_t i = 0; i < count; i++) {
        boids[i].position = glm::vec4(dPos(rnd), dPos(rnd), dPos(rnd), 1.0f);
        boids[i].velocity = glm::vec4(dVel(rnd), dVel(rnd), dVel(rnd), 0.0f);
    }
    stage.unmap();

    vk::CommandBuffer sCmd = device.allocateCommandBuffers({commandPool, vk::CommandBufferLevel::ePrimary, 1})[0];
    sCmd.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    vk::BufferCopy cReg(0, 0, ssboSize);
    sCmd.copyBuffer(stage.getBuffer(), ssboIn->getBuffer(), 1, &cReg);
    sCmd.end();
    vk::SubmitInfo sInfo(0, nullptr, nullptr, 1, &sCmd, 0, nullptr);
    (void)queue.submit(1, &sInfo, nullptr); queue.waitIdle();
    device.freeCommandBuffers(commandPool, 1, &sCmd);

    std::vector<vk::DescriptorPoolSize> pSizes = {{vk::DescriptorType::eUniformBuffer, 1}, {vk::DescriptorType::eStorageBuffer, 3}};
    descriptorPool = device.createDescriptorPool({{}, 2, pSizes});
    std::vector<vk::DescriptorSetLayout> layouts = {computeSetLayout, graphicsSetLayout};
    auto sets = device.allocateDescriptorSets({descriptorPool, 2, layouts.data()});
    computeSet = sets[0]; graphicsSet = sets[1];

    vk::DescriptorBufferInfo uInfo(ubo->getBuffer(), 0, sizeof(SimParams));
    vk::DescriptorBufferInfo iInfo(ssboIn->getBuffer(), 0, ssboSize);
    vk::DescriptorBufferInfo oInfo(ssboOut->getBuffer(), 0, ssboSize);
    std::vector<vk::WriteDescriptorSet> writes = {
        {computeSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &uInfo, nullptr},
        {computeSet, 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &iInfo, nullptr},
        {computeSet, 2, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &oInfo, nullptr},
        {graphicsSet, 0, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &oInfo, nullptr}};
    device.updateDescriptorSets(writes, {});
}

VulkanEngine::~VulkanEngine() {
    device.waitIdle();
    ubo.reset();
    ssboIn.reset();
    ssboOut.reset();

    device.destroySemaphore(imageAvailableSemaphore);
    device.destroySemaphore(renderFinishedSemaphore);
    device.destroyFence(inFlightFence);
    device.destroyCommandPool(commandPool);
    device.destroyPipeline(computePipeline);
    device.destroyPipelineLayout(computePipelineLayout);
    device.destroyDescriptorSetLayout(computeSetLayout);
    device.destroyPipeline(graphicsPipeline);
    device.destroyPipelineLayout(graphicsPipelineLayout);
    device.destroyRenderPass(renderPass);
    device.destroyDescriptorSetLayout(graphicsSetLayout);
    device.destroyDescriptorPool(descriptorPool);

    for (auto fb : framebuffers) device.destroyFramebuffer(fb);

    device.destroyImageView(depthImageView);
    device.destroyImage(depthImage);
    device.freeMemory(depthImageMemory);

    for (auto iv : swapchainImageViews) device.destroyImageView(iv);

    device.destroySwapchainKHR(swapchain);
    device.destroy();

    instance.destroySurfaceKHR(surface);
    instance.destroy();
}

void VulkanEngine::drawFrame(float bassLevel, float trebleLevel) {
    (void)device.waitForFences(1, &inFlightFence, VK_TRUE, UINT64_MAX);
    uint32_t imgIndex;
    (void)device.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailableSemaphore, nullptr, &imgIndex);
    (void)device.resetFences(1, &inFlightFence);

    float audioLevel = (bassLevel + trebleLevel) * 0.5f;
    SimParams currentParams = {0.016f, count, 0.4f, 0.2f, 0.4f, 0.005f, 0.02f, 0.01f, 0.5f, bassLevel, trebleLevel, 0.0f};
    void* uData = ubo->map();
    memcpy(uData, &currentParams, sizeof(SimParams));
    ubo->unmap();

    commandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, computePipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, computePipelineLayout, 0, 1, &computeSet, 0, nullptr);
    commandBuffer.dispatch((count + 255) / 256, 1, 1);

    vk::BufferMemoryBarrier barrier(vk::AccessFlagBits::eShaderWrite, vk::AccessFlagBits::eShaderRead, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, ssboOut->getBuffer(), 0, VK_WHOLE_SIZE);
    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eVertexShader, {}, 0, nullptr, 1, &barrier, 0, nullptr);

    std::array<vk::ClearValue, 2> cv;
    cv[0].color = vk::ClearColorValue(std::array<float, 4>{0.05f, 0.05f, 0.08f, 1.0f});
    cv[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
    commandBuffer.beginRenderPass({renderPass, framebuffers[imgIndex], {{0, 0}, swapchainExtent}, cv}, vk::SubpassContents::eInline);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 2.0f, 6.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)swapchainExtent.width / swapchainExtent.height, 0.1f, 20.0f);
    proj[1][1] *= -1;
    
    PushData pushData;
    pushData.viewProj = proj * view;

    float targetRed   = std::min(1.0f, 0.1f + bassLevel * 1.5f);
    float targetGreen = std::min(1.0f, 0.3f + trebleLevel * 2.0f);
    float targetBlue  = std::max(0.2f, 1.0f - (bassLevel * 0.6f));

    static float smoothRed   = 0.1f;
    static float smoothGreen = 0.8f;
    static float smoothBlue  = 1.0f;

    float lerpSpeed = 0.05f;
    smoothRed   += (targetRed   - smoothRed)   * lerpSpeed;
    smoothGreen += (targetGreen - smoothGreen) * lerpSpeed;
    smoothBlue  += (targetBlue  - smoothBlue)  * lerpSpeed;

    pushData.color = glm::vec4(smoothRed, smoothGreen, smoothBlue, 1.0f);


    commandBuffer.pushConstants(graphicsPipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushData), &pushData);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphicsPipelineLayout, 0, 1, &graphicsSet, 0, nullptr);
    commandBuffer.draw(12, count, 0, 0);
    commandBuffer.endRenderPass();

    vk::BufferCopy copyReg(0, 0, count * sizeof(Boid));
    commandBuffer.copyBuffer(ssboOut->getBuffer(), ssboIn->getBuffer(), 1, &copyReg);
    vk::BufferMemoryBarrier barrierCopy(vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, ssboIn->getBuffer(), 0, VK_WHOLE_SIZE);
    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eComputeShader, {}, 0, nullptr, 1, &barrierCopy, 0, nullptr);

    commandBuffer.end();

    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::SubmitInfo submit(1, &imageAvailableSemaphore, waitStages, 1, &commandBuffer, 1, &renderFinishedSemaphore);
    (void)queue.submit(1, &submit, inFlightFence);
    vk::PresentInfoKHR presentInfo(1, &renderFinishedSemaphore, 1, &swapchain, &imgIndex);
    (void)queue.presentKHR(&presentInfo);
}