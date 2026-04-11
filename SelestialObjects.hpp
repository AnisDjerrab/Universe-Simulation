#include <string>
#include "GraphicalLibrary.hpp"

class SelestialObject {
    private:
        Sphere sphere = Sphere(96);
        GLuint vao[1];
        GLuint vbo[3];
        GLuint textureID;
        GLuint renderingProgram;
        GLuint startsProgram;
        glm::mat4 mvMat;
        glm::mat4 pMat;
        glm::mat4 mMat;
        GLuint projLoc;
        GLuint mvLoc;
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
            // create the shader programs
            renderingProgram = createShaderProgram("shaders/vertSphere.glsl", "shaders/fragSphere.glsl");
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
            glGenVertexArrays(1, vao);
            glBindVertexArray(vao[0]);
            glGenBuffers(3, vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
            glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
            // finally, we load the picture.
            textureID = SOIL_load_OGL_texture(path_of_the_texture.c_str(), SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
        }
        void refresh(int scaling, float aspect, glm::mat4 vMat) {
            // draw the Sphere
            glUseProgram(renderingProgram);
            projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
            mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
            auto pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
            mMat = glm::translate(glm::mat4(1.0f), glm::vec3(positionX, positionY, positionZ));
            mvMat = vMat * mMat;
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
            glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
            glBindVertexArray(vao[0]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(2);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
        }
};
