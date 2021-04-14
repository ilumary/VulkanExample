//
//  app.cpp
//  heartbeat
//
//  Created by Christoph Britsch on 04.03.21.
//

#include "app.hpp"

//std
#include <array>
#include <stdexcept>

namespace lve {
    
App::App(){
    loadModels();
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
}

App::~App(){
    vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
}

void App::run() {
    while (!lveWindow.shouldClose()) {
        glfwPollEvents();
        drawFrame();
    }
    vkDeviceWaitIdle(lveDevice.device());
}

void App::loadModels() {
    std::vector<LveModel::Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };
    std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    };
    lveModel = std::make_unique<LveModel>(lveDevice, vertices, indices, lveSwapChain.imageCount());
}

void App::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &lveModel->descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void App::createPipeline() {
    PipelineConfigInfo pipelineConfig{};
    LvePipeline::defaultPipelineConfigInfo(pipelineConfig, lveSwapChain.width(), lveSwapChain.height());
    pipelineConfig.renderPass = lveSwapChain.getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    lvePipeline = std::make_unique<LvePipeline>(
        lveDevice,
        "/Users/cb18mfw04/Development/Engine/heartbeat/shaders/vert.spv",
        "/Users/cb18mfw04/Development/Engine/heartbeat/shaders/frag.spv",
        pipelineConfig
    );
}

void App::createCommandBuffers() {
    commandBuffers.resize(lveSwapChain.imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = lveDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (int i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lveSwapChain.getRenderPass();
        renderPassInfo.framebuffer = lveSwapChain.getFrameBuffer(i);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = lveSwapChain.getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        lvePipeline->bind(commandBuffers[i]);
        lveModel->bind(commandBuffers[i], pipelineLayout, i);
        lveModel->draw(commandBuffers[i]);

        vkCmdEndRenderPass(commandBuffers[i]);
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
          throw std::runtime_error("failed to record command buffer!");
        }
    }
}

void App::drawFrame() {
    uint32_t imageIndex;
    auto result = lveSwapChain.acquireNextImage(&imageIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }
    lveModel->updateUniformBuffer(imageIndex, lveSwapChain.width(), lveSwapChain.height());
    result = lveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
}

} // namespace lve
