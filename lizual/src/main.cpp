#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

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


static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    if (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" 
            << GL_ERROR_MAP.at(error) << "): "
            << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

struct ShaderProgramSource {
    std::string vertexSource;
    std::string fragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath) {
    std::ifstream stream(filePath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (std::getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    
    GLCall(glShaderSource(id, 1, &src, NULL));
    GLCall(glCompileShader(id));

    int compileStatus;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus));
    if (compileStatus == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        std::vector<char> logText(length);

        GLCall(glGetShaderInfoLog(id, length, NULL, logText.data()));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader:" << std::endl;
        if (logText.data() != nullptr) {
            std::cout << logText.data() << std::endl;
        }

        GLCall(glDeleteShader(id));
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));

    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    int validateStatus;
    GLCall(glGetProgramiv(program, GL_VALIDATE_STATUS, &validateStatus));
    if (validateStatus == GL_FALSE) {
        int logLength;
        GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength));

        if (logLength == 0) {
            std::cout << "Program is invalid. No log info was reported. You're on your own on this one!\n";
        }
        else {
            std::vector<char> logText(logLength);

            GLCall(glGetProgramInfoLog(program, logLength, NULL, logText.data()));
            std::cout << "Program is invalid: " << std::endl;
            if (logText.data() != nullptr) {
                std::cout << logText.data() << std::endl;
            }
        }
    }

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

int main(void)
{
    GLFWwindow* window;

    // Initialize GLFW
    std::cout << "Initializing GLFW... ";
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }
    std::cout << "done\n";

    // Set OpenGL context version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, LIZ_GLFW_CONTEXT_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, LIZ_GLFW_CONTEXT_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    std::cout << "Creating a window with GLFW... ";
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        std::cout << "Failed to create window through GLFW\n";
        return -1;
    }
    std::cout << "done\n";

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // VSync
    glfwSwapInterval(1);

    // Initialize GLEW
    std::cout << "Initializing GLEW... ";
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW\n";
        return -1;
    }
    std::cout << "done\n";

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // Create vertex buffer 
    float positions[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 2 * sizeof(unsigned int), indices, GL_STATIC_DRAW));
    
    ShaderProgramSource sources = ParseShader("res/shaders/Basic.shader");

    unsigned int shader = CreateShader(sources.vertexSource, sources.fragmentSource);
    GLCall(glUseProgram(shader));

    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);

    // Unbind everything before rendering
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    
    // Setup changing red color
    float r = 0.0f;
    float increment = 0.01f;

    /* Loop until the user closes the window */
    std::cout << "Rendering...\n";
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        // Bind OpenGL objects
        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
        GLCall(glBindVertexArray(vao));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if (r > 1.0f)
            increment = -0.01f;
        else if (r < 0.0f)
            increment = 0.01f;
        r += increment;

        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }

    GLCall(glDeleteProgram(shader));

    std::cout << "Terminating GLFW...";
    glfwTerminate();
    std::cout << "done\n" << "Exiting\n";
    return 0;
}