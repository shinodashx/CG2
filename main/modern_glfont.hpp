// #ifdef MORDERN_GLFONT_HPP
// #define MORDERN_GLFONT_HPP

#include <glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdio.h>  // malloc, free, fopen, fclose, ftell, fseek, fread
#include <string.h> // memset
#include <stdlib.h>
#define FONTSTASH_IMPLEMENTATION // Expands implementation
#include "fontstash.h"

FONScontext *glfonsCreate(int width, int height, int flags);
void glfonsDelete(FONScontext *ctx);

unsigned int glfonsRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

struct GLFONScontext
{
    GLuint tex;
    int width, height;
};

static int glfons__renderCreate(void *userPtr, int width, int height)
{
    GLFONScontext *gl = (GLFONScontext *)userPtr;
    // Create may be called multiple times, delete existing texture.
    if (gl->tex != 0)
    {
        glDeleteTextures(1, &gl->tex);
        gl->tex = 0;
    }
    glGenTextures(1, &gl->tex);
    if (!gl->tex)
        return 0;
    gl->width = width;
    gl->height = height;
    glBindTexture(GL_TEXTURE_2D, gl->tex);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, gl->width, gl->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, gl->width, gl->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return 1;
}

static int glfons__renderResize(void *userPtr, int width, int height)
{

    return glfons__renderCreate(userPtr, width, height);
}

static void glfons__renderUpdate(void *userPtr, int *rect, const unsigned char *data)
{
    GLFONScontext *gl = (GLFONScontext *)userPtr;
    int w = rect[2] - rect[0];
    int h = rect[3] - rect[1];

    glBindTexture(GL_TEXTURE_2D, gl->tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    // glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

static void 
glfons__renderDraw(void *userPtr, const float *verts, const float *tcoords, const unsigned int *colors, int nverts)
{
	// ShaderProgram prog({{GL_VERTEX_SHADER, "assets/default.vert"},
	// 					{GL_FRAGMENT_SHADER, "assets/default.frag"}});
    // glUseProgram(prog.programId());
    GLFONScontext *gl = (GLFONScontext *)userPtr;
    if (gl->tex == 0)
        return;
    std::cout << "gl->tex: " << gl->tex << std::endl;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gl->tex);
    // glEnable(gl->tex);


    // display verts
    for (int i = 0; i < nverts * 3; i++)
    {
        printf("%f ", verts[i]);
    }
    printf("\n");


    // construct vec3f position 
    printf("%d\n", nverts);

    std::vector<Vec2f> positions;
    for (int i = 0; i < nverts; i++)
    {
        positions.push_back(Vec2f{verts[i * 2] / 1280.f, verts[i * 2 + 1] / 720.f});
    }
    printf("%d\n", positions.size());
    GLuint positionVbo;
    glGenBuffers(1, &positionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(Vec2f), positions.data(), GL_STATIC_DRAW);

    // construct vec2f texcoord
    std::vector<Vec2f> texcoords;
    for (int i = 0; i < nverts; i++)
    {
        texcoords.push_back(Vec2f{tcoords[i * 2]*255.f, tcoords[i * 2 + 1] * 255.f});
        std::cout << "texcoords: " << texcoords[i].x << " " << texcoords[i].y << std::endl;
    }
    printf("%d\n", texcoords.size());

    GLuint texcoordVbo;
    glGenBuffers(1, &texcoordVbo);
    glBindBuffer(GL_ARRAY_BUFFER, texcoordVbo);
    glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(Vec2f), texcoords.data(), GL_STATIC_DRAW);


    // construct vec3f color
    // std::vector<Vec3f> color;
    // for (int i = 0; i < nverts; i++)
    // {
    //     color.push_back(Vec3f{static_cast<float>(colors[i * 3]), static_cast<float>(colors[i * 3 + 1]), static_cast<float>(colors[i * 3 + 2])});
    // }
    // printf("%d\n", color.size());

    GLuint colorVbo;
    glGenBuffers(1, &colorVbo);
    glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
    glBufferData(GL_ARRAY_BUFFER, nverts * 3 * sizeof(float), colors, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, texcoordVbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // glUniform3f(3, 0.9f, 0.9f, 0.6f);
    // glUniform3f(4, 0.0f, 0.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, nverts);
}

static void glfons__renderDelete(void *userPtr)
{
    GLFONScontext *gl = (GLFONScontext *)userPtr;
    if (gl->tex != 0)
    {
        glDeleteTextures(1, &gl->tex);
    }
    free(gl);
}

FONScontext *glfonsCreate(int width, int height, int flags)
{
    FONSparams params;
    GLFONScontext *gl;

    gl = (GLFONScontext *)malloc(sizeof(GLFONScontext));
    if (gl == NULL)
        goto error;
    memset(gl, 0, sizeof(GLFONScontext));

    memset(&params, 0, sizeof(params));
    params.width = width;
    params.height = height;
    params.flags = (unsigned char)flags;
    params.renderCreate = glfons__renderCreate;
    params.renderResize = glfons__renderResize;
    params.renderUpdate = glfons__renderUpdate;
    params.renderDraw = glfons__renderDraw;
    params.renderDelete = glfons__renderDelete;
    params.userPtr = gl;

    return fonsCreateInternal(&params);

error:
    if (gl != NULL)
        free(gl);
    return NULL;
}

unsigned int glfonsRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    return (r) | (g << 8) | (b << 16) | (a << 24);
}

// #endif

// #ifdef GLFONTSTASH_IMPLEMENTATION

// #endif