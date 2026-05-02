#include <string>
#include <variant>
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
                index+=2;
                break;
            }
            if (arr[index + 1] == "submenu") {
                auto tmp = new tree;
                tmp->text = arr[index];
                convertArrayIntoTree(arr, index, tmp);
                get<vector<tree*>>(children_orFuncToCall).push_back(tmp);
            }
        }
        node->children_orFuncToCall = children_orFuncToCall;
    } else {
        if (known_symbols.find(arr[index + 1]) != known_symbols.end()) {
            children_orFuncToCall = known_symbols[arr[index + 1]];
        } else {
            children_orFuncToCall = undeclared;
        }
        node->children_orFuncToCall = children_orFuncToCall;
        index+=2;
    }
}

void convertTreeInMenuItem(tree* item, int& depthX, int& depthY) {
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
    if (holds_alternative<vector<tree*>>(item->children_orFuncToCall)) {
        auto children = get<vector<tree*>>(item->children_orFuncToCall);
        depthX++;
        depthY = 2;
        for (tree* child : children) {
            convertTreeInMenuItem(child, depthX, depthY);
            depthY++;
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
            int depthX = 0;
            int depthY = 2;
            if (holds_alternative<vector<tree*>>(head->children_orFuncToCall)) {
                auto children = get<vector<tree*>>(head->children_orFuncToCall);
                depthX++;
                depthY = 2;
                for (tree* child : children) {
                    convertTreeInMenuItem(child, depthX, depthY);
                    depthY++;
                }
            }
        }
        void diplay() {
            glUseProgram(renderingProgram);
            // now, we call a recursive function to display each and every menu item
            displayMenuItems(head);
        }
};
