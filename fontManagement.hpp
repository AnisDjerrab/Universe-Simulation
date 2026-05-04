#include <algorithm>
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

struct PreCalculatedString {
    GLuint VAO, VBO;
    GLuint textureID;
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
        PreCalculatedString* PreCalculateTextBitmap(string text, float pos_x, float pos_y, float scale, glm::vec3 color) {
            // the goal of this function is to combine all the individual generated bitmaps of each character into a single bitmap for the hole sentence
            // this is done in order to have blazing fast text rendering
            // first : we need to calculate the total width & height of the combined bitmap
            int totalWidth = 0;
            int maxHeight = 0;
            for (char c : text) {
                Character ch = ASCII_mapping[static_cast<uint8_t>(c)];
                totalWidth += (ch.Advance >> 6) * scale;
                maxHeight = max(maxHeight, (int)(ch.Size.y * scale));
            }
            // calculate the portion a pixel represents on the screen
            // we need to get the screen size in order to do that
            int viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            int width = viewport[2];
            int height = viewport[3];
            float PIXEL_PROPORTION = 2.0f / (float)width;
            float vertices[6][5] = {
                // first triangle
                {pos_x / PIXEL_PROPORTION,                                 pos_y / PIXEL_PROPORTION + maxHeight / PIXEL_PROPORTION, 1.0f, 0.0f, 0.0f},
                {pos_x / PIXEL_PROPORTION,                                 pos_y / PIXEL_PROPORTION                               , 1.0f, 0.0f, 1.0f},
                {pos_x / PIXEL_PROPORTION + totalWidth / PIXEL_PROPORTION, pos_y / PIXEL_PROPORTION                               , 1.0f, 1.0f, 1.0f},
                // second triangle
                {pos_x / PIXEL_PROPORTION,                                 pos_y / PIXEL_PROPORTION + maxHeight / PIXEL_PROPORTION, 1.0f, 0.0f, 0.0f},
                {pos_x / PIXEL_PROPORTION + totalWidth / PIXEL_PROPORTION, pos_y / PIXEL_PROPORTION                               , 1.0f, 1.0f, 1.0f},
                {pos_x / PIXEL_PROPORTION + totalWidth / PIXEL_PROPORTION, pos_y / PIXEL_PROPORTION + maxHeight / PIXEL_PROPORTION, 1.0f, 1.0f, 0.0f}
            };
            // initialize the object
            PreCalculatedString* output = new PreCalculatedString;
            glGenVertexArrays(1, &output->VAO);
            glGenBuffers(1, &output->VBO);
            glBindVertexArray(output->VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

        }
        void RenderText(const PreCalculatedString& ps, glm::vec3 color) {
        }
};
