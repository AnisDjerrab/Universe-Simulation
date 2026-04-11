#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

using namespace std;

class Sphere
{
private:
    int numVertices;
    int numIndices;
    vector<int> indices;
    vector<glm::vec3> vertices;
    vector<glm::vec2> texCoords;
    vector<glm::vec3> normals;

public:
    Sphere(int precision)
    {
        numVertices = (precision + 1) * (precision + 1);
        numIndices = precision * precision * 6;
        for (int i = 0; i < numVertices; i++)
        {
            vertices.push_back(glm::vec3());
            texCoords.push_back(glm::vec2());
            normals.push_back(glm::vec3());
        }
        for (int i = 0; i < numIndices; i++)
        {
            indices.push_back(0);
        }
        // calculate triangle vertices
        for (int i = 0; i <= precision; i++)
        {
            for (int j = 0; j <= precision; j++)
            {
                float y = (float)cos(glm::radians(180.0f - i * 180.0f / precision));
                float x = -(float)cos(glm::radians(j * 360.0f / precision)) * (float)abs(cos(asin(y)));
                float z = (float)sin(glm::radians(j * 360.0f / precision)) * (float)abs(cos(asin(y)));
                vertices[i * (precision + 1) + j] = glm::vec3(x, y, z);
                texCoords[i * (precision + 1) + j] = glm::vec2(((float)j / precision), ((float)i / precision));
                normals[i * (precision + 1) + j] = glm::vec3(x, y, z);
            }
        }
        // calculate triangle indices
        for (int i = 0; i < precision; i++)
        {
            for (int j = 0; j < precision; j++)
            {
                indices[6 * (i * precision + j) + 0] = i * (precision + 1) + j;
                indices[6 * (i * precision + j) + 1] = i * (precision + 1) + j + 1;
                indices[6 * (i * precision + j) + 2] = (i + 1) * (precision + 1) + j;
                indices[6 * (i * precision + j) + 3] = i * (precision + 1) + j + 1;
                indices[6 * (i * precision + j) + 4] = (i + 1) * (precision + 1) + j + 1;
                indices[6 * (i * precision + j) + 5] = (i + 1) * (precision + 1) + j;
            }
        }
    }
    int getNumVertices()
    {
        return numVertices;
    }
    int getNumIndices()
    {
        return numIndices;
    }
    vector<int> getIndices()
    {
        return indices;
    }
    vector<glm::vec3> getVertices()
    {
        return vertices;
    }
    vector<glm::vec2> getTexCoords()
    {
        return texCoords;
    }
    vector<glm::vec3> getNormals()
    {
        return normals;
    }
};

GLuint createShaderProgram(string PathToVert, string PathToFrag)
{
    // read the shader files
    string vShaderSource = "";
    string fShaderSource = "";
    ifstream vertFile(PathToVert, ios::in);
    string line = "";
    while (!vertFile.eof())
    {
        getline(vertFile, line);
        vShaderSource.append(line + "\n");
    }
    vertFile.close();
    ifstream fragFile(PathToFrag, ios::in);
    while (!fragFile.eof())
    {
        getline(fragFile, line);
        fShaderSource.append(line + "\n");
    }
    fragFile.close();
    // compile it, link it, and return it
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *vSourcePtr = vShaderSource.c_str();
    const char *fSourcePtr = fShaderSource.c_str();
    glShaderSource(vShader, 1, &vSourcePtr, nullptr);
    glShaderSource(fShader, 1, &fSourcePtr, nullptr);
    glCompileShader(vShader);
    glCompileShader(fShader);
    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);
    glLinkProgram(vfProgram);
    if (vfProgram < 0)
    {
        cout << "critical error : compilation failed with exit code : " << vfProgram << endl;
    }
    return vfProgram;
}

class fpsCounter
{
private:
    GLFWwindow *window;
    double lastTime;
    double framesPassed;
    double *currentTime;
    string DefaultTitle;
public:
    fpsCounter(GLFWwindow *win, double *time, string title)
    {
        window = win;
        lastTime = glfwGetTime();
        currentTime = time;
        DefaultTitle = title;
    }
    void updateFPS()
    {
        framesPassed++;
        if ((*currentTime - lastTime) > 1.0f)
        {
            double fps = framesPassed / (*currentTime - lastTime);
            string title = DefaultTitle + " (FPS : " + to_string(fps) + ")";
            glfwSetWindowTitle(window, title.c_str());
            lastTime = *currentTime;
            framesPassed = 0;
        }
    }
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
        vector<indice*> indices;
        vector<glm::vec3> vertices;
        vector<glm::vec2> texCoords;
        vector<glm::vec3> normals;
        glm::mat4 mvMat, vMat, mMat, pMat;
        GLuint vao[1];
        GLuint vbo[3];
        SingleObject(string vertPath, string fragPath, string ObjectPath, float aspect);
        void UpdateAspect(float aspect);
        void initTexture(string pathOfTheTexture, int flag1, unsigned int flag2, unsigned int flag3);
        void initTextures(int numberOfTextures, vector<string> pathOfTextures, vector<int> flag1, vector<unsigned int> flag2, vector<unsigned int> flag3);
        void initVertexGeneric();
        void PrepDisplayObjectGeneric();
        void displayTextures();
        void GenericDraw();
};