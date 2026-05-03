#include "SelestialObjects.hpp"
#include "UpperBar.hpp"
#include <initializer_list>
#include <vector>
#include <unordered_map>

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

// syntax of this harcoded menu:
// 1) the root of the menu can have whatever name you want (it doesn't matter)
// 2) a submenu must have the property submenu in it.
// 3) to exit a submenu, since it is a flat list, you must have the key "exit_submenu" (again, the property doesn't matter here)
// 4) a function call here must be known and referenced in the hardcoded function call list.

vector<string> menu = {
    "Root", "submenu",
        "element1", "submenu",
            "action1", "open",
            "action2", "open",
            "action3", "open",
            "sub_element1", "submenu",
                "action1", "open",
                "action2", "open",
                "exit_submenu", "",
            "exit_submenu", "",
        "element2", "submenu",
            "action1", "open",
            "exit_submenu", "",
        "element3", "submenu",
            "exit_submenu", "",
        "exit_submenu", ""
};

void init() {
    head.text = "Root";
    head.item = new MenuItem;
    int index = 0;
    convertArrayIntoTree(menu, index, &head);
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
    glDisable(GL_DEPTH_TEST);
    bar->display();
    glEnable(GL_DEPTH_TEST);
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
