#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "SurfRemesh.h"

TEST_CASE("GetEdgeAt", "[face]")
{
    auto node0 = std::make_shared<SurfRemesh::Node>();
    auto node1 = std::make_shared<SurfRemesh::Node>();
    auto node2 = std::make_shared<SurfRemesh::Node>();
    node0->id = 0;
    node1->id = 1;
    node2->id = 2;
    node0->xyz = {0.0, 0.0, 0.0};
    node1->xyz = {1.0, 0.0, 0.0};
    node2->xyz = {0.0, 1.0, 0.0};

    SurfRemesh::Face face;
    face.nodes = {node0, node1, node2};

    auto edge0 = face.getEdgeAt(0);
    REQUIRE(edge0 != nullptr);
    REQUIRE(edge0->isSame(node0, node1) == true);

    auto edge1 = face.getEdgeAt(1);
    REQUIRE(edge1 != nullptr);
    REQUIRE(edge1->isSame(node1, node2) == true);

    auto edge2 = face.getEdgeAt(2);
    REQUIRE(edge2 != nullptr);
    REQUIRE(edge2->isSame(node2, node0) == true);
}

TEST_CASE("Project", "[face]")
{
    auto node0 = std::make_shared<SurfRemesh::Node>();
    auto node1 = std::make_shared<SurfRemesh::Node>();
    auto node2 = std::make_shared<SurfRemesh::Node>();
    node0->id = 0;
    node1->id = 1;
    node2->id = 2;
    node0->xyz = {0.0, 0.0, 0.0};
    node1->xyz = {1.0, 0.0, 0.0};
    node2->xyz = {0.0, 1.0, 0.0};

    SurfRemesh::Face face;
    face.nodes = {node0, node1, node2};

    auto uv = face.project();

    REQUIRE(uv.size() >= 6);

    REQUIRE_THAT(face.uvCorners[0][0], Catch::Matchers::WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(face.uvCorners[0][1], Catch::Matchers::WithinAbs(0.0, 1e-10));

    REQUIRE_THAT(face.uvCorners[1][0], Catch::Matchers::WithinAbs(1.0, 1e-10));
    REQUIRE_THAT(face.uvCorners[1][1], Catch::Matchers::WithinAbs(0.0, 1e-10));
}

TEST_CASE("ProjectRightAngle", "[face]")
{
    auto node0 = std::make_shared<SurfRemesh::Node>();
    auto node1 = std::make_shared<SurfRemesh::Node>();
    auto node2 = std::make_shared<SurfRemesh::Node>();
    node0->id = 0;
    node1->id = 1;
    node2->id = 2;
    node0->xyz = {0.0, 0.0, 0.0};
    node1->xyz = {3.0, 0.0, 0.0};
    node2->xyz = {0.0, 4.0, 0.0};

    SurfRemesh::Face face;
    face.nodes = {node0, node1, node2};

    auto uv = face.project();

    REQUIRE_THAT(face.uvCorners[0][0], Catch::Matchers::WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(face.uvCorners[0][1], Catch::Matchers::WithinAbs(0.0, 1e-10));

    REQUIRE_THAT(face.uvCorners[1][0], Catch::Matchers::WithinAbs(3.0, 1e-10));
    REQUIRE_THAT(face.uvCorners[1][1], Catch::Matchers::WithinAbs(0.0, 1e-10));

    REQUIRE_THAT(face.uvCorners[2][0], Catch::Matchers::WithinAbs(0.0, 1e-10));
    REQUIRE_THAT(face.uvCorners[2][1], Catch::Matchers::WithinAbs(4.0, 1e-10));
}

TEST_CASE("ProjectWithRefinedEdges", "[face]")
{
    auto node0 = std::make_shared<SurfRemesh::Node>();
    auto node1 = std::make_shared<SurfRemesh::Node>();
    auto node2 = std::make_shared<SurfRemesh::Node>();
    node0->id = 0;
    node1->id = 1;
    node2->id = 2;
    node0->xyz = {0.0, 0.0, 0.0};
    node1->xyz = {2.0, 0.0, 0.0};
    node2->xyz = {0.0, 2.0, 0.0};

    SurfRemesh::Face face;
    face.nodes = {node0, node1, node2};

    auto edge01 = face.getEdgeAt(0);
    edge01->newNodes.resize(1);
    edge01->newNodes[0] = std::make_shared<SurfRemesh::Node>();
    edge01->newNodes[0]->xyz = {1.0, 0.0, 0.0};

    auto uv = face.project();
    REQUIRE(uv.size() > 6);
}

TEST_CASE("Unproject", "[face]")
{
    auto node0 = std::make_shared<SurfRemesh::Node>();
    auto node1 = std::make_shared<SurfRemesh::Node>();
    auto node2 = std::make_shared<SurfRemesh::Node>();
    node0->id = 0;
    node1->id = 1;
    node2->id = 2;
    node0->xyz = {0.0, 0.0, 0.0};
    node1->xyz = {1.0, 0.0, 0.0};
    node2->xyz = {0.0, 1.0, 0.0};

    SurfRemesh::Face face;
    face.nodes = {node0, node1, node2};

    auto uv = face.project();

    face.unproject(uv);

    REQUIRE(face.mesh != nullptr);
    REQUIRE(face.mesh->nodes.size() >= 3);
}