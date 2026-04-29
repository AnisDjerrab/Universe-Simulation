#include <vector>
#include <string>
#include <variant>
#include "GraphicalLibrary.hpp"
#include <ft2build.h>
#include <variant>
#include FT_FREETYPE_H

using namespace std;

struct MenuItem {
    GLuint VAO[1];
    GLuint VBO[1];
    vector<float> vertices;
    vector<MenuItem*> children;
    string text;
    bool printChildren = false;
};

struct tree {
    string text;
    MenuItem* item;
    variant<void*, vector<tree*>> children_orFuncToCall;
};

void convertTreeInMenuItem(tree* item, int depthX, int depthY) {
    MenuItem* outputItem = new MenuItem;
    outputItem->vertices = {
        -(float)(depthX / 10),  (float)(depthY / 10), 1.0f,                 // top-left
        -(float)(depthX / 10 + 0.2), (float)(depthY / 10), 1.0f,            // top-right
        -(float)(depthX / 10), (float)(depthY / 10 - 0.08), 1.0f,           // bottom-right
        -(float)(depthX / 10),  (float)(depthY / 10), 1.0f,                 // top-left
        -(float)(depthX / 10 + 0.2), (float)(depthY / 10 - 0.08), 1.0f,     // bottom-right
        -(float)(depthX / 10), (float)(depthY / 10 - 0.08), 1.0f            // bottom-left
    };
    glGenVertexArrays(1, outputItem->VAO);
    glBindVertexArray(outputItem->VAO[0]);
    glGenBuffers(1, outputItem->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, outputItem->VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, outputItem->vertices.size() * sizeof(float), outputItem->vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    item->item = outputItem;
    auto children = get(item->children_orFuncToCall);
    
}


class UpperBar {
    private:
        vector<int> appearingMenus;
        vector<MenuItem*> MenuItemsTree;
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
                -1.0f,  1.0f, 1.0f,   // top-left
                1.0f,  1.0f, 1.0f,    // top-right
                1.0f, 0.92f, 1.0f,     // bottom-right
                -1.0f,  1.0f, 1.0f,   // top-left
                1.0f, 0.92f, 1.0f,     // bottom-right
                -1.0f, 0.92f, 1.0f     // bottom-left
            };
            glGenVertexArrays(1, head->item->VAO);
            glBindVertexArray(head->item->VAO[0]);
            glGenBuffers(1, head->item->VBO);
            glBindBuffer(GL_ARRAY_BUFFER, head->item->VBO[0]);
            glBufferData(GL_ARRAY_BUFFER, head->item->vertices.size() * sizeof(float), head->item->vertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

        }
        void diplay() {
            glUseProgram(renderingProgram);
            glBindVertexArray(head->item->VAO[0]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
};
