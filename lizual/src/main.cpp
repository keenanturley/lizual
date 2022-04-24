#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(0);


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

    std::cout << "Terminating GLFW...";
    glfwTerminate();
    std::cout << "done\n" << "Exiting\n";
    return 0;
}