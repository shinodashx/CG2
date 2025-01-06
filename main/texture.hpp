#ifndef TEXTURE_HPP_D0746DED_C9C6_40CD_B6E0_C6FEF665DD31
#define TEXTURE_HPP_D0746DED_C9C6_40CD_B6E0_C6FEF665DD31

#include <glad.h>
struct atlasPos
{
    int row;
    int col;
};

GLuint load_texture_2d(char const *aPath);
GLuint load_font_atlas_texture_2d(char const *aPath, int aRow, int aCol, int unitWidth, int unitHeight);
atlasPos getAtlasPosByChar(char c);

#endif // TEXTURE_HPP_D0746DED_C9C6_40CD_B6E0_C6FEF665DD31
