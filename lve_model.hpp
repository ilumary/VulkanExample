#pragma once

#include "lve_device.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//std
#include <vector>

namespace lve {

class LveModel {
    public:
    
        VkDescriptorSetLayout descriptorSetLayout;
    
        struct Vertex {
            glm::vec3 position;
            glm::vec3 color;
            glm::vec2 texCoord;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };
    
        struct UniformBufferObject {
            glm::mat4 model;
            glm::mat4 view;
            glm::mat4 proj;
        };

        LveModel(LveDevice &device, const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices, const size_t imageSize);
        ~LveModel();

        //prohibit copying
        LveModel(const LveModel &) = delete;
        LveModel &operator=(const LveModel &) = delete;

        void bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t i);
        void draw(VkCommandBuffer commandBuffer);
    
        void updateUniformBuffer(uint32_t currentImage, uint32_t width, uint32_t height);

    private:
        void createDescriptorSetLayout();
        void createDescriptorPool(const size_t imageSize);
        void createDescriptorSets(const size_t imageSize);
        void createTextureImage();
        void createTextureImageView();
        void createTextureSampler();
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint16_t> &indices);
        void createUniformBuffers(const size_t imageSize);
        
        LveDevice &lveDevice;
        
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;
        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;
        uint32_t vertexCount;
        uint32_t indexCount;
        size_t pImageSize;
};

} //namespace lve
