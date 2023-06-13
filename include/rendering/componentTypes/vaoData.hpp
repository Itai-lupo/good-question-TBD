#pragma once
#include "core.hpp"
#include <vulkan/vulkan.hpp>

struct vaoAttribute
{
    int binding;
    int location;
    int offset;
    vk::Format format;
    int a = 5;
};

struct vaoBinding
{
    uint32_t stride;
    bool isInstancedRendering = false;
};

struct vao
{
    vaoId id;
    std::vector<vaoAttribute> *attributes = new std::vector<vaoAttribute>();
    std::vector<vaoBinding> *bindings = new std::vector<vaoBinding>();
    int a = 5;
};