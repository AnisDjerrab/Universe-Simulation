#include <string>
#include "GraphicalLibrary.hpp"

class SelestialObject {
    private:
        Sphere sphere = Sphere(96);
    public:
        double positionX;           // m
        double positionY;           // m
        double positionZ;           // m
        double velocityX;           // m/s
        double velocityY;           // m/s
        double velocityZ;           // m/s
        double mass;                // kg
        double radius;              // m
        SelestialObject(double posX, double posY, double posZ, double velX, double velY, double velZ, double m, double r, string path_of_the_texture) {
            // we first need to setup all the object's properties
            positionX = posX;
            positionY = posY;
            positionZ = posZ;
            velocityX = velX;
            velocityY = velY;
            velocityZ = velZ;
            mass = m;
            radius = r;
            // then, initialize the Sphere object
            vector<int> ind = sphere.getIndices();
            vector<glm::vec3> vert = sphere.getVertices();
            vector<glm::vec2> tex = sphere.getTexCoords();
            vector<glm::vec3> norm = sphere.getNormals();
            // vertex positions
            vector<float> pvalues;
            // texture coordinates
            vector<float> tvalues;
            // normal vectors
            vector<float> nvalues;
            int numIndices = sphere.getNumIndices();
            for (int i = 0; i < numIndices; i++)
            {
                pvalues.push_back((vert[ind[i]]).x);
                pvalues.push_back((vert[ind[i]]).y);
                pvalues.push_back((vert[ind[i]]).z);
                tvalues.push_back((tex[ind[i]]).s);
                tvalues.push_back((tex[ind[i]]).t);
                nvalues.push_back((norm[ind[i]]).x);
                nvalues.push_back((norm[ind[i]]).y);
                nvalues.push_back((norm[ind[i]]).z);
            }
            glGenVertexArrays(2, vao);
            glBindVertexArray(vao[0]);
            glGenBuffers(5, vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
            glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
            float rectanglesPosition[] = {
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f
            };
            float groundTexCoords[] = {
                0.0f,    0.0f,
                1.0f,    0.0f,
                1.0f,    1.0f,
                0.0f,    1.0f
            };
            glBindVertexArray(vao[1]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglesPosition), rectanglesPosition, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(groundTexCoords), groundTexCoords, GL_STATIC_DRAW);
        }
};