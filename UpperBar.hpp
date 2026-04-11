#include <vector>
#include <string>
#include <variant>
#include "GraphicalLibrary.hpp"

using namespace std;

struct MenuItem {
    GLuint VAO[1];
    GLuint VBO[1];
    vector<float> vertices;
};

struct tree {
    string text;
    MenuItem* item;
    variant<void*, vector<tree*>> children_orFuncToCall;
};

class UpperBar {
    private:
        vector<int> appearingMenus;
        tree* head;
        GLuint renderingProgram;
    public:
        UpperBar(tree* m) {
            head = m;
            appearingMenus.push_back(0);
            // create the rendering program
            renderingProgram = createShaderProgram("shaders/vertMenu.glsl", "shaders/fragMenu.glsl");
        }
        void GenUI() {
            // special treatment for the root node
            // create a rectangle that uses the entire top of the screen
            head->item->vertices = {
                -1.0f,  1.0f, // top-left
                1.0f,  1.0f,  // top-right
                1.0f, 0.9f,   // bottom-right
                -1.0f,  1.0f,  // top-left
                1.0f, 0.9f,   // bottom-right
                -1.0f, 0.9f   // bottom-left
            };
            glGenVertexArrays(1, head->item->VAO);
            glBindVertexArray(head->item->VAO[0]);
            glGenBuffers(1, head->item->VBO);
            glBindBuffer(GL_ARRAY_BUFFER, head->item->VBO[0]);
            glBufferData(GL_ARRAY_BUFFER, head->item->vertices.size() * sizeof(float), head->item->vertices.data(), GL_STATIC_DRAW);
        }
        void diplay() {
            glUseProgram(renderingProgram);
            glBindVertexArray(head->item->VAO[0]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
};
