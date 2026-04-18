#include "SelestialObjects.hpp"
#include "UpperBar.hpp"

float aspect;
int width;
int height;
fpsCounter* fps;
double timeData;
double* currentTime = &timeData;
SelestialObject* Sphere;
glm::mat4 vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-0.0f, -0.0f, -3.0f));
tree head;
class UpperBar* bar;

void init() {
    head.text = "(placeholder)";
    head.item = new MenuItem;
    head.children_orFuncToCall = vector<tree*>();
    bar = new UpperBar(&head);
    bar->GenUI();
}

void display(GLFWwindow* window) {
    fps->updateFPS();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    glViewport(0, 0, width, height);
    Sphere->refresh(0, aspect, vMat);
    bar->diplay();
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 800, "Program", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, 800, 800);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glewInit();
    init();
    fps = new fpsCounter(window, currentTime, "BlenderTexture");
    Sphere = new SelestialObject(0, 0, 0, 0, 0, 0, 0, 0, "assets/8k_earth_daymap.jpg");
    while (!glfwWindowShouldClose(window)) {
        timeData = glfwGetTime();
        display(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
