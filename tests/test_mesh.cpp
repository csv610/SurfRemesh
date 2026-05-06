#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "SurfRemesh.h"
#include <fstream>

TEST_CASE("DefaultConstructor", "[mesh]")
{
    SurfRemesh remesh;
    auto nodes = remesh.getNodes();
    auto triangles = remesh.getTriangles();
    REQUIRE(nodes.empty());
    REQUIRE(triangles.empty());
}

TEST_CASE("SetMaxEdgeLength", "[mesh]")
{
    SurfRemesh remesh;
    remesh.setMaxEdgeLength(0.5);
}

TEST_CASE("SetMinEdgeLength", "[mesh]")
{
    SurfRemesh remesh;
    remesh.setMinEdgeLength(0.1);
}

TEST_CASE("SetCreaseAngle", "[mesh]")
{
    SurfRemesh remesh;
    remesh.setCreaseAngle(45.0);
}

TEST_CASE("SetMaxSteps", "[mesh]")
{
    SurfRemesh remesh;
    remesh.setMaxSteps(5);
}

TEST_CASE("EmptyMeshRefine", "[mesh]")
{
    SurfRemesh remesh;
    remesh.refine();

    auto nodes = remesh.getNodes();
    auto triangles = remesh.getTriangles();
    REQUIRE(nodes.empty());
    REQUIRE(triangles.empty());
}