#pragma once

#include "lve_device.hpp"
#include "lve_model.hpp"
#include "lve_pipeline.hpp"
#include "lve_swap_chain.hpp"
#include "lve_window.hpp"

//std
#include <memory>
#include <vector>

namespace lve {

class App {
    public:
        //Window consts
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        App();
        ~App();
        
        //prohibit copying and initliasing with instance
        App(const App &) = delete;
        App &operator=(const App &) = delete;
        
        void run();
    
    private:
    
        void loadModels();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();
        
        LveWindow lveWindow{WIDTH, HEIGHT, "App"};
        LveDevice lveDevice{lveWindow};
        LveSwapChain lveSwapChain{lveDevice, lveWindow.getExtent()};
        std::unique_ptr<LvePipeline> lvePipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<LveModel> lveModel;
};

} // namespace lve
