#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>


#if NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif