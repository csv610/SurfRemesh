#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "veclib.hpp"

using namespace JMath;

TEST_CASE("Length 2D", "[veclib]")
{
    Array2D a = {0.0, 0.0};
    Array2D b = {3.0, 4.0};
    REQUIRE(length(a, b) == 5.0);
}

TEST_CASE("Length 3D", "[veclib]")
{
    Array3D a = {0.0, 0.0, 0.0};
    Array3D b = {1.0, 2.0, 2.0};
    REQUIRE(length(a, b) == 3.0);
}

TEST_CASE("Length2", "[veclib]")
{
    Array3D a = {0.0, 0.0, 0.0};
    Array3D b = {3.0, 4.0, 0.0};
    REQUIRE(length2(a, b) == 25.0);
}

TEST_CASE("Magnitude", "[veclib]")
{
    Array3D v = {3.0, 4.0, 0.0};
    REQUIRE(magnitude(v) == 5.0);
}

TEST_CASE("MakeVector", "[veclib]")
{
    Array3D head = {1.0, 2.0, 3.0};
    Array3D tail = {0.0, 0.0, 0.0};
    auto result = make_vector(head, tail);
    REQUIRE(result[0] == 1.0);
    REQUIRE(result[1] == 2.0);
    REQUIRE(result[2] == 3.0);
}

TEST_CASE("CrossProduct", "[veclib]")
{
    Array3D a = {1.0, 0.0, 0.0};
    Array3D b = {0.0, 1.0, 0.0};
    auto c = cross_product(a, b);
    REQUIRE(c[0] == 0.0);
    REQUIRE(c[1] == 0.0);
    REQUIRE(c[2] == 1.0);
}

TEST_CASE("UnitVector", "[veclib]")
{
    Array3D v = {3.0, 4.0, 0.0};
    auto u = unit_vector(v);
    REQUIRE_THAT(u[0], Catch::Matchers::WithinAbs(0.6, 1e-10));
    REQUIRE_THAT(u[1], Catch::Matchers::WithinAbs(0.8, 1e-10));
    REQUIRE(u[2] == 0.0);
}

TEST_CASE("MaxValue", "[veclib]")
{
    REQUIRE(max_value(1.0, 2.0, 3.0) == 3.0);
    REQUIRE(max_value(5.0, 3.0, 4.0) == 5.0);
}

TEST_CASE("MinValue", "[veclib]")
{
    REQUIRE(min_value(1.0, 2.0, 3.0) == 1.0);
    REQUIRE(min_value(5.0, 3.0, 4.0) == 3.0);
}

TEST_CASE("MinValueFour", "[veclib]")
{
    REQUIRE(min_value(1.0, 2.0, 3.0, 4.0) == 1.0);
    REQUIRE(min_value(5.0, 3.0, 7.0, 4.0) == 3.0);
}

TEST_CASE("AverageValue", "[veclib]")
{
    std::vector<double> v = {1.0, 2.0, 3.0, 4.0};
    REQUIRE(average_value(v) == 2.5);
}

TEST_CASE("MeanValue", "[veclib]")
{
    std::vector<double> v = {1.0, 2.0, 3.0, 4.0, 5.0};
    REQUIRE(mean_value(v) == 3.0);
}

TEST_CASE("StandardDeviation", "[veclib]")
{
    std::vector<double> v = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    double sd = standard_deviation(v);
    REQUIRE_THAT(sd, Catch::Matchers::WithinAbs(2.138, 0.01));
}