#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "SurfRemesh.h"

TEST_CASE("GetOrientation", "[edge]")
{
    auto node0 = std::make_shared<SurfRemesh::Node>();
    auto node1 = std::make_shared<SurfRemesh::Node>();

    SurfRemesh::Edge edge;
    edge.nodes[0] = node0;
    edge.nodes[1] = node1;

    REQUIRE(edge.getOrientation(node0, node1) == 1);
    REQUIRE(edge.getOrientation(node1, node0) == -1);

    auto other = std::make_shared<SurfRemesh::Node>();
    REQUIRE(edge.getOrientation(node0, other) == 0);
}

TEST_CASE("IsSame", "[edge]")
{
    auto node0 = std::make_shared<SurfRemesh::Node>();
    auto node1 = std::make_shared<SurfRemesh::Node>();

    SurfRemesh::Edge edge;
    edge.nodes[0] = node0;
    edge.nodes[1] = node1;

    REQUIRE(edge.isSame(node0, node1) == true);
    REQUIRE(edge.isSame(node1, node0) == true);

    auto other = std::make_shared<SurfRemesh::Node>();
    REQUIRE(edge.isSame(node0, other) == false);
}

TEST_CASE("RefineShortEdge", "[edge]")
{
    auto node0 = std::make_shared<SurfRemesh::Node>();
    auto node1 = std::make_shared<SurfRemesh::Node>();
    node0->xyz = {0.0, 0.0, 0.0};
    node1->xyz = {1.0, 0.0, 0.0};

    SurfRemesh::Edge edge;
    edge.nodes[0] = node0;
    edge.nodes[1] = node1;

    int result = edge.refine(2.0);
    REQUIRE(result == 0);
    REQUIRE(edge.newNodes.empty());
}

TEST_CASE("RefineLongEdge", "[edge]")
{
    auto node0 = std::make_shared<SurfRemesh::Node>();
    auto node1 = std::make_shared<SurfRemesh::Node>();
    node0->xyz = {0.0, 0.0, 0.0};
    node1->xyz = {10.0, 0.0, 0.0};

    SurfRemesh::Edge edge;
    edge.nodes[0] = node0;
    edge.nodes[1] = node1;

    int result = edge.refine(1.0);
    REQUIRE(result == 1);
    REQUIRE_FALSE(edge.newNodes.empty());
}

TEST_CASE("RefineExactLength", "[edge]")
{
    auto node0 = std::make_shared<SurfRemesh::Node>();
    auto node1 = std::make_shared<SurfRemesh::Node>();
    node0->xyz = {0.0, 0.0, 0.0};
    node1->xyz = {5.0, 0.0, 0.0};

    SurfRemesh::Edge edge;
    edge.nodes[0] = node0;
    edge.nodes[1] = node1;

    int result = edge.refine(5.0);
    REQUIRE(result == 1);
}

TEST_CASE("RefineCreatesCorrectNodes", "[edge]")
{
    auto node0 = std::make_shared<SurfRemesh::Node>();
    auto node1 = std::make_shared<SurfRemesh::Node>();
    node0->xyz = {0.0, 0.0, 0.0};
    node1->xyz = {4.0, 0.0, 0.0};

    SurfRemesh::Edge edge;
    edge.nodes[0] = node0;
    edge.nodes[1] = node1;

    edge.refine(1.0);

    REQUIRE(edge.newNodes.size() == 3);
    REQUIRE_THAT(edge.newNodes[0]->xyz[0], Catch::Matchers::WithinAbs(1.0, 1e-10));
    REQUIRE_THAT(edge.newNodes[1]->xyz[0], Catch::Matchers::WithinAbs(2.0, 1e-10));
    REQUIRE_THAT(edge.newNodes[2]->xyz[0], Catch::Matchers::WithinAbs(3.0, 1e-10));
}

TEST_CASE("RefineClearsPreviousNodes", "[edge]")
{
    auto node0 = std::make_shared<SurfRemesh::Node>();
    auto node1 = std::make_shared<SurfRemesh::Node>();
    node0->xyz = {0.0, 0.0, 0.0};
    node1->xyz = {10.0, 0.0, 0.0};

    SurfRemesh::Edge edge;
    edge.nodes[0] = node0;
    edge.nodes[1] = node1;

    edge.refine(2.0);
    REQUIRE_FALSE(edge.newNodes.empty());
    size_t firstCount = edge.newNodes.size();

    edge.refine(1.0);
    REQUIRE(edge.newNodes.size() >= firstCount);
}