#include <freetype2/ft2build.h>
#include "GraphicalLibrary.hpp"
#include FT_FREETYPE_H
#include <glm/vec2.hpp>
#include <iostream>

using namespace std;

// this struct is provided by the learnopengl.com site (https://learnopengl.com/In-Practice/Text-Rendering)
struct Character {
    unsigned int textureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

class font {
    private:
        FT_Library ft;
        FT_Face face;
        vector<Character> ASCII_mapping;
        GLuint VAO, VBO;
        GLuint ProgramID;
    public:
        font(const char* fontPath) {
            // init the freetype library
            if (FT_Init_FreeType(&ft)) {
                cout << "error : can't find freetype library. This is a critical error : there will be no font when displaying the menu." << endl;
            }
            if (FT_New_Face(ft, fontPath, 0, &face)) {
                cout << "error : can't find the font file. This is a critical error : there will be no font when displaying the menu." << endl;
            }
            // get width and height in pixel, to adapt to the screen size.
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            int width  = viewport[2];   // width in pixels
            int height = viewport[3];   // height in pixels
            // set the size of the font to a certain fraction of the width of the GL_View, in pixels
            FT_Set_Pixel_Sizes(face, 0, width / 50);
            // now, we need to initialize this font for all the ASCII characters
            // this code is provided the learnopengl.com site (https://learnopengl.com/In-Practice/Text-Rendering)
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
            for (unsigned char c = 0; c < 128; c++)
            {
                // load character glyph
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    cout << "ERROR::FREETYTPE: Failed to load Glyph" << endl;
                    Character dummy;
                    ASCII_mapping.push_back(dummy);
                    continue;
                }
                // generate texture
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
                );
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // now store character for later use
                unsigned int advance = face->glyph->advance.x;
                Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    advance
                };
                ASCII_mapping.push_back(character);
            }
            FT_Done_Face(face);
            FT_Done_FreeType(ft);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            ProgramID = createShaderProgram("shaders/vertFont.glsl", "shaders/fragFont.glsl");
            // lit le fichier et affiche son contenu
            glUseProgram(ProgramID);
            glUniform2f(glGetUniformLocation(ProgramID, "screenSize"), (float)width, (float)height);
        }
        // this function is coming from the learnopengl.com site (https://learnopengl.com/In-Practice/Text-Rendering)
        void RenderText(string text, float x, float y, float scale, glm::vec3 color)
        {
            glUseProgram(ProgramID);
            glUniform3f(glGetUniformLocation(ProgramID, "textColor"), color.x, color.y, color.z);
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(VAO);

            // iterate through all characters
            for (auto c : text)
            {
                Character ch = ASCII_mapping[static_cast<uint8_t>(c)];

                float xpos = x + ch.Bearing.x * scale;
                float ypos = y + (ch.Size.y - ch.Bearing.y) * scale;

                float w = ch.Size.x * scale;
                float h = ch.Size.y * scale;
                // update VBO for each character
                float vertices[6][4] = {
                    { xpos,     ypos,       0.0f, 0.0f },
                    { xpos,     ypos + h,   0.0f, 1.0f },
                    { xpos + w, ypos + h,   1.0f, 1.0f },

                    { xpos,     ypos,       0.0f, 0.0f },
                    { xpos + w, ypos + h,   1.0f, 1.0f },
                    { xpos + w, ypos,       1.0f, 0.0f }
                };
                // render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, ch.textureID);
                // update content of VBO memory
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                // render quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
                // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
            }
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
};
