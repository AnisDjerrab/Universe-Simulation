#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#pragma once

class Sphere
{
private:
    int numVertices;
    int numIndices;
    std::vector<int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
public:
    Sphere(int precision);
    int getNumVertices();
    int getNumIndices();
    std::vector<int> getIndices();
    std::vector<glm::vec3> getVertices();
    std::vector<glm::vec2> getTexCoords();
    std::vector<glm::vec3> getNormals();
};

GLuint createShaderProgram(std::string PathToVert, std::string PathToFrag);

class fpsCounter
{
private:
public:
    fpsCounter(GLFWwindow *win, double *time, std::string title);
    void updateFPS();
};

struct indice {
    unsigned int v;
    unsigned int vt;
    unsigned int vn;
};

class SingleObject {
    private:
    public:
        int numVertices = 0;
        std::vector<indice*> indices;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> texCoords;
        std::vector<glm::vec3> normals;
        glm::mat4 mvMat, vMat, mMat, pMat;
        GLuint vao[1];
        GLuint vbo[3];
        SingleObject(std::string vertPath, std::string fragPath, std::string ObjectPath, float aspect);
        void UpdateAspect(float aspect);
        void initTexture(std::string pathOfTheTexture, int flag1, unsigned int flag2, unsigned int flag3);
        void initTextures(int numberOfTextures, std::vector<std::string> pathOfTextures, std::vector<int> flag1, std::vector<unsigned int> flag2, std::vector<unsigned int> flag3);
        void initVertexGeneric();
        void PrepDisplayObjectGeneric();
        void displayTextures();
        void GenericDraw();
};