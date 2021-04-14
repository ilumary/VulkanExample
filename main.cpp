//
//  app.cpp
//  heartbeat
//
//  Created by Christoph Britsch on 25.02.21.
//

#include "app.hpp"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
    //HelloTriangleApplication app;
    lve::App app{};

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
