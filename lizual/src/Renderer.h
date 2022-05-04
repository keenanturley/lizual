#pragma once

#include <unordered_map>
#include <GL/glew.h>

#define ENUM_MAP_ITEM(x) \
    {x, #x}

#define ASSERT(x) \
    if (!(x)) __debugbreak();

#ifdef DEBUG
#define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#endif

static const std::unordered_map<GLenum, std::string> GL_ERROR_MAP{
    ENUM_MAP_ITEM(GL_NO_ERROR),
    ENUM_MAP_ITEM(GL_INVALID_ENUM),
    ENUM_MAP_ITEM(GL_INVALID_VALUE),
    ENUM_MAP_ITEM(GL_INVALID_OPERATION),
    ENUM_MAP_ITEM(GL_INVALID_FRAMEBUFFER_OPERATION),
    ENUM_MAP_ITEM(GL_OUT_OF_MEMORY),
    ENUM_MAP_ITEM(GL_STACK_UNDERFLOW),
    ENUM_MAP_ITEM(GL_STACK_OVERFLOW)
};

static constexpr int LIZ_GLFW_CONTEXT_VERSION_MAJOR = 3;
static constexpr int LIZ_GLFW_CONTEXT_VERSION_MINOR = 3;

void GLClearError() noexcept;

bool GLLogCall(const char* function, const char* file, int line);