#include "cylinder.hpp"

#include "../vmlib/mat33.hpp"

SimpleMeshData make_cylinder(bool aCapped, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform)
{
	// TODO: implement me
	std::vector<Vec3f> pos;
	std::vector<Vec3f> normals;
	std::vector<Vec2f> texCoords;

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
			pos.emplace_back(Vec3f{0.f, 0.f, 0.f});
			pos.emplace_back(Vec3f{0.f, prevY, prevZ});
			pos.emplace_back(Vec3f{0.f, y, z});

			// top cap
			pos.emplace_back(Vec3f{1.f, 0.f, 0.f});
			pos.emplace_back(Vec3f{1.f, prevY, prevZ});
			pos.emplace_back(Vec3f{1.f, y, z});

			Vec3f norm1 = Vec3f{1.f, 0.f, 0.f};
			norm1 = N * norm1;
			norm1 = normalize(norm1);
			normals.emplace_back(norm1);
			normals.emplace_back(norm1);
			normals.emplace_back(norm1);

			Vec3f norm2 = Vec3f{-1.f, 0.f, 0.f};
			norm2 = N * norm2;
			norm2 = normalize(norm2);
			normals.emplace_back(norm2);
			normals.emplace_back(norm2);
			normals.emplace_back(norm2);
		}
		pos.emplace_back(Vec3f{0.f, prevY, prevZ});
		pos.emplace_back(Vec3f{0.f, y, z});
		pos.emplace_back(Vec3f{1.f, prevY, prevZ});

		pos.emplace_back(Vec3f{0.f, y, z});
		pos.emplace_back(Vec3f{1.f, y, z});
		pos.emplace_back(Vec3f{1.f, prevY, prevZ});

		Vec3f norm1 = Vec3f{0.f, prevY, prevZ};
		Vec3f norm2 = Vec3f{0.f, y, z};

		norm1 = N * norm1;
		norm2 = N * norm2;

		norm1 = normalize(norm1);
		norm2 = normalize(norm2);

		normals.emplace_back(norm1);
		normals.emplace_back(norm2);
		normals.emplace_back(norm1);

		normals.emplace_back(norm2);
		normals.emplace_back(norm2);
		normals.emplace_back(norm1);

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
	// process normals
	// for (auto &n : normals)
	// {
	// 	Vec4f n4{n.x, n.y, n.z, 0.f};
	// 	Vec4f t = aPreTransform * n4;
	// 	n = Vec3f{t.x, t.y, t.z};
	// }
	std::vector col(pos.size(), aColor);

	return SimpleMeshData{std::move(pos), {}, std::move(col), std::move(normals)};
}
