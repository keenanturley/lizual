#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

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
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    int compileStatus;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> logText(length);

        glGetShaderInfoLog(id, length, NULL, logText.data());
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader:" << std::endl;
        if (logText.data() != nullptr) {
            std::cout << logText.data() << std::endl;
        }

        glDeleteShader(id);
    }

    int validateStatus;
    glGetShaderiv(id, GL_VALIDATE_STATUS, &validateStatus);
    if (validateStatus == GL_FALSE) {
        int logLength;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> logText(logLength);

        glGetProgramInfoLog(id, logLength, NULL, logText.data());
        std::cout << "Failed to validate " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader:" << std::endl;
        if (logText.data() != nullptr) {
            std::cout << logText.data() << std::endl;
        }
    }
    
    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    int validateStatus;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &validateStatus);
    if (validateStatus == GL_FALSE) {
        int logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength == 0) {
            std::cout << "Program is invalid. No log info was reported. You're on your own on this one!\n";
        }
        else {
            std::vector<char> logText(logLength);

            glGetProgramInfoLog(program, logLength, NULL, logText.data());
            std::cout << "Program is invalid: " << std::endl;
            if (logText.data() != nullptr) {
                std::cout << logText.data() << std::endl;
            }
        }
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

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

    // Initialize GLEW
    std::cout << "Initializing GLEW... ";
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW\n";
        return -1;
    }
    std::cout << "done\n";

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // Create vertex buffer 
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    
    ShaderProgramSource sources = ParseShader("res/shaders/Basic.shader");

    unsigned int shader = CreateShader(sources.vertexSource, sources.fragmentSource);
    glUseProgram(shader);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    std::cout << "Terminating GLFW...";
    glfwTerminate();
    std::cout << "done\n" << "Exiting\n";
    return 0;
}