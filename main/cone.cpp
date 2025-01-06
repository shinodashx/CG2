#include "cone.hpp"

SimpleMeshData make_cone(bool aCapped, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform)
{
    // TODO: implement me
    std::vector<Vec3f> pos;
    std::vector<Vec3f> normals;
    std::vector<Vec3f> capNormals; // Store normals for the bottom cap

    Mat33f const N = mat44_to_mat33(transpose(invert(aPreTransform)));

    float prevY = std::cos(0.f);
    float prevZ = std::sin(0.f);

    for (std::size_t i = 0; i < aSubdivs; ++i)
    {
        float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f; // calculate angle for current point
        float y = std::cos(angle);
        float z = std::sin(angle);

        if (aCapped)
        {
            // bottom cap
            pos.emplace_back(Vec3f{0.f, 0.f, 0.f}); // the bottom of the cone is centered at x=1
            pos.emplace_back(Vec3f{0.f, prevY, prevZ});
            pos.emplace_back(Vec3f{0.f, y, z});

            Vec3f norm1 = Vec3f{-1.f, 0.f, 0.f};
            norm1 = N * norm1;
            norm1 = normalize(norm1);
            capNormals.emplace_back(norm1);
            capNormals.emplace_back(norm1);
            capNormals.emplace_back(norm1);
        }

        pos.emplace_back(Vec3f{1.f, 0.f, 0.f}); // all vertices on the bottom end meet at the apex
        pos.emplace_back(Vec3f{0.f, prevY, prevZ});
        pos.emplace_back(Vec3f{0.f, y, z});

        Vec3f norm2 = Vec3f{0.f, y, z};
        norm2 = N * norm2;
        norm2 = normalize(norm2);
        normals.emplace_back(norm2);
        normals.emplace_back(norm2);
        normals.emplace_back(norm2);

        norm2 = Vec3f{0.f, prevY, prevZ};
        norm2 = N * norm2;
        norm2 = normalize(norm2);
        normals.emplace_back(norm2);
        normals.emplace_back(norm2);
        normals.emplace_back(norm2);

        prevY = y;
        prevZ = z;
    }

    for (auto &p : pos)
    {
        Vec4f p4{p.x, p.y, p.z, 1.f};
        Vec4f t = aPreTransform * p4;
        t /= t.w;
        p = Vec3f{t.x, t.y, t.z};
    }

    // Add cap normals to the normals vector
    normals.insert(normals.end(), capNormals.begin(), capNormals.end());

    std::vector col(pos.size(), aColor);
    return SimpleMeshData{std::move(pos), {}, std::move(col), std::move(normals)};
}
