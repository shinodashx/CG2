#include "rectangle.hpp"
#include <iostream>

SimpleMeshData make_rectangle_2d(
    bool aCapped,
    Vec3f aColor,
    Mat44f aPreTransform)
{
    float width = 1.f;
    float height = 1.f;

    std::vector<Vec3f> pos = {{-0.5f, -0.5f, 0.f},
                              {0.5f, -0.5f, 0.f},
                              {0.5f, 0.5f, 0.f},
                              {-0.5f, -0.5f, 0.f},
                              {0.5f, 0.5f, 0.f},
                              {-0.5f, 0.5f, 0.f}};
    std::vector<Vec2f> texcoords = {{0.f, 0.f},
                                    {1.f, 0.f},
                                    {1.f, 1.f},
                                    {0.f, 0.f},
                                    {1.f, 1.f},
                                    {0.f, 1.f}};
    std::vector<Vec3f> colors;
    std::vector<Vec3f> normals;

    colors.emplace_back(aColor);
    colors.emplace_back(aColor);
    colors.emplace_back(aColor);
    colors.emplace_back(aColor);
    colors.emplace_back(aColor);
    colors.emplace_back(aColor);

    // texcoords.emplace_back(Vec2f{0.f, 0.f});

    for (auto &p : pos)
    {
        Vec4f p4{p.x, p.y, p.z, 1.f};
        Vec4f t = aPreTransform * p4;
        t /= t.w;
        p = Vec3f{t.x, t.y, t.z};
    }

    return SimpleMeshData{std::move(pos), std::move(texcoords), std::move(colors), std::move(normals)};
}

SimpleMeshData make_cube(
    Vec3f aColor,
    Mat44f aPreTransform)
{
    float width = 1.f;
    float height = 1.f;
    float depth = 1.f;

    std::cout << aColor.x << " " << aColor.y << " " << aColor.z << std::endl;

    std::vector<Vec3f> pos{
        // front
        {-0.5f, -0.5f, 0.5f},
        {0.5f, -0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {-0.5f, 0.5f, 0.5f},

        // back
        {-0.5f, -0.5f, -0.5f},
        {-0.5f, 0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f},
        {0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f},

        // left
        {-0.5f, -0.5f, -0.5f},
        {-0.5f, 0.5f, -0.5f},
        {-0.5f, 0.5f, 0.5f},
        {-0.5f, -0.5f, -0.5f},
        {-0.5f, 0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f},

        // right
        {0.5f, -0.5f, 0.5f},
        {0.5f, -0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f},
        {0.5f, -0.5f, 0.5f},
        {0.5f, 0.5f, -0.5f},
        {0.5f, 0.5f, 0.5f},

        // top
        {-0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, -0.5f},
        {-0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, -0.5f},
        {-0.5f, 0.5f, -0.5f},

        // bottom
        {-0.5f, -0.5f, -0.5f},
        {0.5f, -0.5f, -0.5f},
        {0.5f, -0.5f, 0.5f},
        {-0.5f, -0.5f, -0.5f},
        {0.5f, -0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f},
    };

    std::vector<Vec2f> texcoords{
        // front
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},

        // back
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 1.0f},

        // left
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},

        // right
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},

        // top
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},

        // bottom
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}};
    std::vector<Vec3f> colors(pos.size(), aColor);
    std::vector<Vec3f> normals;

    // Calculate face normals
    Vec3f frontNormal{0, 0, 1};
    Vec3f backNormal{0, 0, -1};
    Vec3f leftNormal{-1, 0, 0};
    Vec3f rightNormal{1, 0, 0};
    Vec3f topNormal{0, 1, 0};
    Vec3f bottomNormal{0, -1, 0};

    // Duplicate face normal per vertex
    for (int i = 0; i < 6; i++)
    {
        normals.push_back(frontNormal);
    }

    for (int i = 0; i < 6; i++)
    {
        normals.push_back(backNormal);
    }

    // ...

    for (int i = 0; i < 6; i++)
    {
        normals.push_back(bottomNormal);
    }

    for (auto &p : pos)
    {
        Vec4f p4{p.x, p.y, p.z, 1.f};
        Vec4f t = aPreTransform * p4;
        t /= t.w;
        p = Vec3f{t.x, t.y, t.z};
    }

    return SimpleMeshData{
        std::move(pos), std::move(texcoords),
        std::move(colors), std::move(normals)};
}