// You will need to define your own tests. Refer to CW1 or Exercise G.3 for
// examples.
#define CATCH_CONFIG_MAIN
#include "../third_party/catch2/include/catch2/catch_amalgamated.hpp"

#include "../vmlib/mat44.hpp"
#include <cmath>
#include <iostream>

float M_PI = 3.14159265358979323846;
// Utility function to compare two Mat44f matrices
bool areMatricesEqual(const Mat44f& a, const Mat44f& b, double epsilon = 0.001f) {
    for (int i = 0; i < 16; ++i) {
        if (std::abs(a.v[i] - b.v[i]) > epsilon) return 0;
    }
    return 1;
}



TEST_CASE("Matrix Multiplication") {
    Mat44f a = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    Mat44f b = {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    Mat44f expected = {80, 70, 60, 50, 240, 214, 188, 162, 400, 358, 316, 274, 560, 502, 444, 386};

    REQUIRE( areMatricesEqual(a * b, expected) );
}

bool areVectorsEqual(const Vec4f& a, const Vec4f& b, float epsilon = 0.001f) {
    for (int i = 0; i < 4; ++i) {
        if (std::abs(a[i] - b[i]) > epsilon) return false;
    }
    return true;
}

TEST_CASE("Matrix-Vector Multiplication") {
    Mat44f a = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };
    Vec4f v = {1, 2, 3, 1};
    Vec4f expected = {18, 46, 74, 102};

    Vec4f result = a * v;
    REQUIRE( areVectorsEqual(result, expected) );
}


TEST_CASE("Rotation Matrix X") {
    float angle = M_PI / 2; // 90 degrees
    Mat44f expected = {1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1};

    REQUIRE( areMatricesEqual(make_rotation_x(angle), expected) );
}

TEST_CASE("Rotation Matrix Y") {
    float angle = M_PI / 2; // 90 degrees
    Mat44f expected = {0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1};

    REQUIRE( areMatricesEqual(make_rotation_y(angle), expected) );
}

TEST_CASE("Rotation Matrix Z") {
    float angle = M_PI / 2; // 90 degrees
    Mat44f expected = {0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    REQUIRE( areMatricesEqual(make_rotation_z(angle), expected) );
}

TEST_CASE("Translation Matrix") {
    Vec3f translation = {1, 2, 3};
    Mat44f expected = {1, 0, 0, 1, 0, 1, 0, 2, 0, 0, 1, 3, 0, 0, 0, 1};

    REQUIRE( areMatricesEqual(make_translation(translation), expected) );
}

TEST_CASE("Perspective Projection Matrix") {
    float fov = M_PI / 2; // 90 degrees
    float aspect = 16.0f / 9.0f; // widescreen
    float near = 0.1f;
    float far = 100.0f;
    float f = 1.0f / tan(fov / 2); // This is the scale factor

    Mat44f expected = {
        0.5625,
0,
0,
0,
0,
1,
0,
0,
0,
0,
-1.002,
-0.4004,
0,
0,
-1,
1,
    };

    REQUIRE( areMatricesEqual(make_perspective_projection(fov, aspect, near, far), expected) );
}


int main(int argc, char* argv[]) {
    // Global setup can go here

    int result = Catch::Session().run(argc, argv);



    // Global cleanup can go here

    return result;
}