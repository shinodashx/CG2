#include "texture.hpp"

#include <cassert>

#include <stb_image.h>
#include <iostream>

#include "../support/error.hpp"

#include <unordered_map>

std::unordered_map<int, GLuint> atlasMap;
std::unordered_map<char, atlasPos> atlasPosMap;

GLuint load_texture_2d(char const *aPath)
{
	assert(aPath);

	stbi_set_flip_vertically_on_load(true);

	int w, h, channels;
	stbi_uc *data = stbi_load(aPath, &w, &h, &channels, 4);
	if (!data)
	{
		throw Error("Failed to load texture '%s'", aPath);
	}

	GLuint tex = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 6.f);

	return tex;
}

GLuint load_font_atlas_texture_2d(char const *aPath, int aRow, int aCol, int unitWidth, int unitHeight)
{
	int pos = aRow * 8 + aCol;
	if (atlasMap.find(pos) != atlasMap.end())
	{
		return atlasMap[pos];
	}
	assert(aPath);

	stbi_set_flip_vertically_on_load(false);
	int w, h, channels;
	stbi_uc *data = stbi_load(aPath, &w, &h, &channels, 4);
	if (!data)
	{
		throw Error("Failed to load texture '%s'", aPath);
	}

	GLuint tex = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	stbi_uc *newData = new stbi_uc[unitWidth * unitHeight * 4];
	for (int i = 0; i < unitHeight; i++)
	{
		for (int j = 0; j < unitWidth; j++)
		{
			int index = (i * unitWidth + j) * 4;
			int oldIndex = ((i + aRow * unitHeight) * w + j + aCol * unitWidth) * 4;
			newData[index] = data[oldIndex];
			newData[index + 1] = data[oldIndex + 1];
			newData[index + 2] = data[oldIndex + 2];
			newData[index + 3] = data[oldIndex + 3];
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, unitWidth, unitHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, newData);

	stbi_image_free(data);
	stbi_image_free(newData);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 6.f);

	atlasMap[pos] = tex;

	return tex;
}


atlasPos getAtlasPosByChar(char c)
{
	if (atlasPosMap.find(c) != atlasPosMap.end())
	{
		return atlasPosMap[c];
	}
	std::string str[8][8] = {{"0", "1", "2", "3", "4", "5", "6", "7"},
							 {"8", "9", "A", "B", "C", "D", "E", "F"},
							 {"G", "H", "I", "J", "K", "L", "M", "N"},
							 {"O", "P", "Q", "R", "S", "T", "U", "V"},
							 {"W", "X", "Y", "Z", "a", "b", "c", "d"},
							 {"e", "f", "g", "h", "i", "j", "k", "l"},
							 {"m", "n", "o", "p", "q", "r", "s", "t"},
							 {"u", "v", "w", "x", "y", "z", ".", ":"}};
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			if(c == str[i][j][0]){
				atlasPos pos;
				pos.row = i;
				pos.col = j;
				atlasPosMap[c] = pos;
				return pos;
			}
		}
		
	}


	return {0, 0};
}
