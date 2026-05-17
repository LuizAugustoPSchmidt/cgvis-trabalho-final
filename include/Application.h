#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Application {
public:
    Application();
    ~Application();

    bool Init();
    void LoadAssets(int argc, char* argv[]);
    void Run();
    void Shutdown();

private:
    GLFWwindow* m_Window;
};

#endif // APPLICATION_H
