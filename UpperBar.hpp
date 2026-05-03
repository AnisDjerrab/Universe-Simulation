#include <string>
#include <variant>
#include <iostream>
#include "GraphicalLibrary.hpp"
#include <vector>
#include <unordered_map>

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
    variant<void(*)(), vector<tree*>> children_orFuncToCall;
};

// here, we'll declare all our menu options
void undeclared() {
    cout << "error : function not implemented." << endl;
}


// This is the hardcoded known function call list
unordered_map<string, void(*)()> known_symbols = {
};


void convertArrayIntoTree(vector<string>& arr, int& index, tree* node) {
    variant<void(*)(), vector<tree*>> children_orFuncToCall;
    if (arr[index + 1] == "submenu") {
        index+=2;
        children_orFuncToCall = vector<tree*>();
        for (index; index < arr.size(); index+=2) {
            if (arr[index] == "exit_submenu") {
                break;
            }
            auto tmp = new tree;
            tmp->text = arr[index];
            convertArrayIntoTree(arr, index, tmp);
            get<vector<tree*>>(children_orFuncToCall).push_back(tmp);
        }
        node->children_orFuncToCall = children_orFuncToCall;
    } else {
        if (known_symbols.find(arr[index + 1]) != known_symbols.end()) {
            children_orFuncToCall = known_symbols[arr[index + 1]];
        } else {
            children_orFuncToCall = undeclared;
        }
        node->children_orFuncToCall = children_orFuncToCall;
    }
}

void convertTreeInMenuItem(tree* item, float& depthX, float& depthY, int& level) {
    MenuItem* outputItem = new MenuItem;
    float x = -1.0f + depthX * 0.2f;
    float y =  1.0f - depthY * 0.08f;
    float w = 0.4f;
    float h = 0.08f;
    outputItem->vertices = {
        x,     y,     1.0f,
        x + w, y,     1.0f,
        x + w, y - h, 1.0f,
        x,     y,     1.0f,
        x + w, y - h, 1.0f,
        x,     y - h, 1.0f
    };
    glGenVertexArrays(1, outputItem->VAO);
    glBindVertexArray(outputItem->VAO[0]);
    glGenBuffers(1, outputItem->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, outputItem->VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, outputItem->vertices.size() * sizeof(float), outputItem->vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    item->item = outputItem;
    // now, generate the characters

    // finally, recursively process the following menus
    if (holds_alternative<vector<tree*>>(item->children_orFuncToCall)) {
        auto children = get<vector<tree*>>(item->children_orFuncToCall);
        float depthX_tmp = depthX + 2, depthY_tmp;
        level++;
        for (int i = 0; i < children.size(); i++) {
            cout << level << endl;
            if (level < 2) {
                depthY_tmp = depthY + i + 1;
            } else {
                depthY_tmp = depthY + i;
            }
            convertTreeInMenuItem(children[i], depthX_tmp, depthY_tmp, level);
        }
    }
}

void displayMenuItems(tree* item) {
    glBindVertexArray(item->item->VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    if (holds_alternative<vector<tree*>>(item->children_orFuncToCall)) {
        auto children = get<vector<tree*>>(item->children_orFuncToCall);
        for (tree* child : children) {
            displayMenuItems(child);
        }
    }
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
            // now, generate the menu items tree
            // we call the recursive function to convert the tree into a menu items tree
            // depthX and depthY are use to position the menu items on the screen => max value is 10
            float depthX = -2;
            float depthY = 0;
            int level = 0;
            if (holds_alternative<vector<tree*>>(head->children_orFuncToCall)) {
                auto children = get<vector<tree*>>(head->children_orFuncToCall);
                for (tree* child : children) {
                    convertTreeInMenuItem(child, depthX, depthY, level);
                    level = 0;
                    depthY = 0;
                    depthX += 2;
                }
            }
            std::cout << "Root has "
                      << (holds_alternative<vector<tree*>>(head->children_orFuncToCall) ?
                          get<vector<tree*>>(head->children_orFuncToCall).size() : 0)
                      << " direct children\n";
        }
        void display() {
            glUseProgram(renderingProgram);
            // now, we call a recursive function to display each and every menu item
            displayMenuItems(head);
        }
};
