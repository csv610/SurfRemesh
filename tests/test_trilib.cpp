#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "trilib.hpp"

TEST_CASE("Area Triangle", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {1.0, 0.0, 0.0};
    Array3D p2 = {0.0, 1.0, 0.0};

    double a = area(p0, p1, p2);
    REQUIRE_THAT(a, Catch::Matchers::WithinAbs(0.5, 1e-10));
}

TEST_CASE("Area Equilateral", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {1.0, 0.0, 0.0};
    Array3D p2 = {0.5, sqrt(3.0)/2.0, 0.0};

    double a = area(p0, p1, p2);
    REQUIRE_THAT(a, Catch::Matchers::WithinAbs(sqrt(3.0)/4.0, 1e-10));
}

TEST_CASE("Angles", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {1.0, 0.0, 0.0};
    Array3D p2 = {0.0, 1.0, 0.0};

    auto ang = angles(p0, p1, p2, ANGLE_IN_DEGREES);
    REQUIRE_THAT(ang[0], Catch::Matchers::WithinAbs(90.0, 1e-5));
    REQUIRE_THAT(ang[1], Catch::Matchers::WithinAbs(45.0, 1e-5));
    REQUIRE_THAT(ang[2], Catch::Matchers::WithinAbs(45.0, 1e-5));
}

TEST_CASE("AngleAt", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {1.0, 0.0, 0.0};
    Array3D p2 = {0.0, 1.0, 0.0};

    double a = angleAt(p0, p1, p2, ANGLE_IN_DEGREES);
    REQUIRE_THAT(a, Catch::Matchers::WithinAbs(90.0, 1e-5));
}

TEST_CASE("MinLength", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {3.0, 0.0, 0.0};
    Array3D p2 = {0.0, 4.0, 0.0};

    REQUIRE(minlength(p0, p1, p2) == 3.0);
}

TEST_CASE("MaxLength", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {3.0, 0.0, 0.0};
    Array3D p2 = {0.0, 4.0, 0.0};

    REQUIRE(maxlength(p0, p1, p2) == 5.0);
}

TEST_CASE("IsAcute", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {1.0, 0.0, 0.0};
    Array3D p2 = {0.0, 1.0, 0.0};

    REQUIRE(isAcute(p0, p1, p2) == true);
}

TEST_CASE("IsObtuse", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {1.0, 0.0, 0.0};
    Array3D p2 = {-1.0, 0.01, 0.0};

    REQUIRE(isObtuse(p0, p1, p2) == true);
    REQUIRE(isAcute(p0, p1, p2) == false);
}

TEST_CASE("IsDegenerate", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {1.0, 0.0, 0.0};
    Array3D p2 = {2.0, 0.0, 0.0};

    REQUIRE(isDegenerate(p0, p1, p2) == true);
}

TEST_CASE("Normal", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {1.0, 0.0, 0.0};
    Array3D p2 = {0.0, 1.0, 0.0};

    auto n = normal(p0, p1, p2);
    REQUIRE_THAT(n[0], Catch::Matchers::WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(n[1], Catch::Matchers::WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(n[2], Catch::Matchers::WithinAbs(1.0, 1e-10));
}

TEST_CASE("Centroid", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {3.0, 0.0, 0.0};
    Array3D p2 = {0.0, 3.0, 0.0};

    auto c = centroid(p0, p1, p2);
    REQUIRE(c[0] == 1.0);
    REQUIRE_THAT(c[1], Catch::Matchers::WithinAbs(1.0, 1e-10));
    REQUIRE(c[2] == 0.0);
}

TEST_CASE("BarycentricCoordinates", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {1.0, 0.0, 0.0};
    Array3D p2 = {0.0, 1.0, 0.0};
    Array3D q = {0.25, 0.25, 0.0};

    auto bc = barycoordinates(p0, p1, p2, q);
    REQUIRE_THAT(bc[0], Catch::Matchers::WithinAbs(0.5, 1e-10));
    REQUIRE_THAT(bc[1], Catch::Matchers::WithinAbs(0.25, 1e-10));
    REQUIRE_THAT(bc[2], Catch::Matchers::WithinAbs(0.25, 1e-10));
}

TEST_CASE("Circumcenter", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {1.0, 0.0, 0.0};
    Array3D p2 = {0.5, sqrt(3.0)/2.0, 0.0};

    auto cc = circumcenter(p0, p1, p2);
    REQUIRE_THAT(cc[0], Catch::Matchers::WithinAbs(0.5, 1e-10));
    REQUIRE_THAT(cc[1], Catch::Matchers::WithinAbs(sqrt(3.0)/6.0, 1e-10));
}

TEST_CASE("Circumradius", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {1.0, 0.0, 0.0};
    Array3D p2 = {0.5, sqrt(3.0)/2.0, 0.0};

    double r = circumradius(p0, p1, p2);
    REQUIRE_THAT(r, Catch::Matchers::WithinAbs(1.0/sqrt(3.0), 1e-10));
}

TEST_CASE("Incenter", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {3.0, 0.0, 0.0};
    Array3D p2 = {0.0, 4.0, 0.0};

    auto ic = incenter(p0, p1, p2);
    double a = 5.0, b = 4.0, c = 3.0;
    double t = a + b + c;
    REQUIRE_THAT(ic[0], Catch::Matchers::WithinAbs((a*p0[0] + b*p1[0] + c*p2[0])/t, 1e-10));
    REQUIRE_THAT(ic[1], Catch::Matchers::WithinAbs((a*p0[1] + b*p1[1] + c*p2[1])/t, 1e-10));
}

TEST_CASE("Inradius", "[trilib]")
{
    Array3D p0 = {0.0, 0.0, 0.0};
    Array3D p1 = {3.0, 0.0, 0.0};
    Array3D p2 = {0.0, 4.0, 0.0};

    double r = inradius(p0, p1, p2);
    REQUIRE_THAT(r, Catch::Matchers::WithinAbs(1.0, 1e-10));
}