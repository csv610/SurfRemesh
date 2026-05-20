#include <algorithm>
#include <set>
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>
#include <limits>
#include <array>

#include "SurfRemesh.h"
#include "trilib.hpp"

#define REAL double
#define VOID void

#define ANSI_DECLARATORS
extern "C" {
#include <triangle.h>
}

SurfRemesh :: SurfRemesh()
{
    maxEdgeLength = std::numeric_limits<double>::max();
}

////////////////////////////////////////////////////////////////////////////////
bool SurfRemesh :: loadMesh( const std::string &filename) const
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename,
        aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    if( !scene ) {
        std::cerr << "Error: Can't load model " << filename << std::endl;
        return false;
    }

    std::string ext = filename.substr(filename.find_last_of('.'));
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if( ext == ".off" ) {
        return readOFFMesh(filename);
    }

    size_t countnodes = 0;
    size_t countfaces = 0;
    for( unsigned int m = 0; m < scene->mNumMeshes; m++ ) {
        aiMesh* srcMesh = scene->mMeshes[m];
        countnodes += srcMesh->mNumVertices;
        countfaces += srcMesh->mNumFaces;
    }

    mesh.nodes.resize(countnodes);
    mesh.faces.resize(countfaces);

    size_t nodeOffset = 0;
    size_t faceOffset = 0;
    for( unsigned int m = 0; m < scene->mNumMeshes; m++ ) {
        aiMesh* srcMesh = scene->mMeshes[m];

        for( unsigned int v = 0; v < srcMesh->mNumVertices; v++ ) {
            auto node = std::make_shared<Node>();
            node->id = nodeOffset + v;
            node->xyz = {srcMesh->mVertices[v].x,
                      srcMesh->mVertices[v].y,
                      srcMesh->mVertices[v].z};
            mesh.nodes[nodeOffset + v] = node;
        }

        for( unsigned int f = 0; f < srcMesh->mNumFaces; f++ ) {
            aiFace& face = srcMesh->mFaces[f];
            if( face.mNumIndices != 3 ) continue;

            auto dstFace = std::make_shared<Face>();
            dstFace->nodes = {mesh.nodes[nodeOffset + face.mIndices[0]],
                            mesh.nodes[nodeOffset + face.mIndices[1]],
                            mesh.nodes[nodeOffset + face.mIndices[2]]};
            mesh.faces[faceOffset + f] = dstFace;
        }

        nodeOffset += srcMesh->mNumVertices;
        faceOffset += srcMesh->mNumFaces;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool SurfRemesh :: readOFFMesh( const std::string &filename) const
{
    std::ifstream ifile(filename.c_str(), std::ios::in);
    if( ifile.fail() ) {
        std::cerr << "Error: Can't open input file " << filename << std::endl;
        return false;
    }

    std::string str;
    ifile >> str;
    if( str != "OFF") {
        std::cerr << "Error: input file not in the off format" << std::endl;
        return false;
    }

    int numPoints, numFaces, numEdges;
    ifile >> numPoints >> numFaces >> numEdges;

    if( numPoints < 1) {
        std::cerr << "Warning: Input file has no points " << std::endl;
        return false;
    }

    double x, y, z;
    mesh.nodes.resize(numPoints);

    for( int i = 0; i < numPoints; i++) {
        ifile >> x >> y >> z;
        auto v = std::make_shared<Node>();
        v->id  = i;
        v->xyz = {x,y,z};
        mesh.nodes[i] = v;
    }

    mesh.faces.resize(numFaces);

    int  nn, i0, i1, i2;
    for( int i = 0; i < numFaces; i++) {
        ifile >> nn; assert(nn == 3);
        ifile >> i0 >> i1 >> i2;
        auto n0 = mesh.nodes[i0];
        auto n1 = mesh.nodes[i1];
        auto n2 = mesh.nodes[i2];
        auto face = std::make_shared<Face>();
        face->nodes = {n0, n1, n2};
        mesh.faces[i] = face;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

int SurfRemesh::Edge::refine( double required_length)
{
    newNodes.clear();

    double currlen = JMath::length( nodes[0]->xyz, nodes[1]->xyz);

    if( currlen < required_length) return 0;

    int nsegments = std::max(2, static_cast<int>(currlen/required_length));
    int npoints   = nsegments-1;

    newNodes.resize(npoints);

    double dt = 1.0/(double)nsegments;
    for( int i = 0; i < npoints; i++) {
        double t = (i+1)*dt;
        double x = (1-t)*nodes[0]->xyz[0] + t*nodes[1]->xyz[0];
        double y = (1-t)*nodes[0]->xyz[1] + t*nodes[1]->xyz[1];
        double z = (1-t)*nodes[0]->xyz[2] + t*nodes[1]->xyz[2];
        newNodes[i] = std::make_shared<Node>();
        newNodes[i]->xyz = {x,y,z};
    }
    return 1;
}

////////////////////////////////////////////////////////////////////////////////

SurfRemesh::EdgePtr SurfRemesh::Face::getEdgeAt( int i)
{
    auto v0 = nodes[i];
    auto v1 = nodes[(i+1)%3];
    auto vm = std::min(v0,v1);
    for( auto e : vm->edges) {
        if( e->isSame(v0,v1) ) return e;
    }

    assert(v0 != v1);
    auto newedge = std::make_shared<Edge>();
    newedge->nodes[0] = v0;
    newedge->nodes[1] = v1;
    vm->edges.push_back(newedge);
    return newedge;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<double> SurfRemesh::Face::project()
{
    uvCorners[0] = {0.0, 0.0};

    double len0  = JMath::length(nodes[0]->xyz, nodes[1]->xyz);
    uvCorners[1] = {len0, 0.0};

    double angle  = angleAt(nodes[0]->xyz, nodes[1]->xyz, nodes[2]->xyz, ANGLE_IN_RADIANS);
    double len1   = JMath::length(nodes[0]->xyz, nodes[2]->xyz);
    uvCorners[2]  = {len1*std::cos(angle), len1*std::sin(angle)};

    std::vector<double> uvCoords;

    for( int i = 0; i < 3; i++) {
        uvCoords.push_back( uvCorners[i][0] );
        uvCoords.push_back( uvCorners[i][1] );
        auto edge = getEdgeAt(i);
        int  npoints =  static_cast<int>(edge->newNodes.size());
        if( npoints ) {
            int nsegments = npoints + 1;
            double dt = 1.0/(double)nsegments;
            for( int j = 0; j < npoints; j++) {
                double t = (j+1)*dt;
                double u = (1-t)*uvCorners[i][0] + t*uvCorners[(i+1)%3][0];
                double v = (1-t)*uvCorners[i][1] + t*uvCorners[(i+1)%3][1];
                uvCoords.push_back(u);
                uvCoords.push_back(v);
            }
        }
    }
    return uvCoords;
}

////////////////////////////////////////////////////////////////////////////////
void SurfRemesh::Face::unproject( const std::vector<double> &uvCoords)
{
    std::array<double,3> bCoords;
    std::array<Array3D,3> triCoords;
    triCoords[0] = nodes[0]->xyz;
    triCoords[1] = nodes[1]->xyz;
    triCoords[2] = nodes[2]->xyz;

    auto getXYZCoords = [] ( const std::array<Array3D,3> &tCoords, const Array3D &bCoord) {
        Array3D xyz;
        for( size_t i = 0; i < 3; i++)
            xyz[i] = bCoord[0]*tCoords[0][i] + bCoord[1]*tCoords[1][i] + bCoord[2]*tCoords[2][i];
        return  xyz;
    };

    int numNodes = static_cast<int>(uvCoords.size()/2);

    mesh = std::make_shared<Mesh>();
    mesh->nodes.reserve( static_cast<size_t>(numNodes) );

    int istart =  3;
    for( int i = 0; i < 3; i++) {
        auto edge = getEdgeAt(i);
        int  np   = static_cast<int>(edge->newNodes.size());
        istart   += np;
        mesh->nodes.push_back( nodes[i] );
        int ori = edge->getOrientation(nodes[i], nodes[(i+1)%3] );
        assert( ori );
        if( ori > 0 ) {
            for( int j = 0; j < np; j++)
                mesh->nodes.push_back( edge->newNodes[j] );
        } else {
            for( int j = 0; j < np; j++)
                mesh->nodes.push_back( edge->newNodes[np-j-1] );
        }
    }

    Array2D qPoint;
    for( int i = istart; i < numNodes; i++) {
        qPoint[0]  = uvCoords[2*i];
        qPoint[1]  = uvCoords[2*i+1];
        bCoords    = barycoordinates(uvCorners[0], uvCorners[1], uvCorners[2], qPoint);
        auto vtx   = std::make_shared<Node>();
        vtx->xyz   = getXYZCoords( triCoords, bCoords);
        mesh->nodes.push_back(vtx);
    }
}

////////////////////////////////////////////////////////////////////////////////
void SurfRemesh::Face::delaunay(std::vector<int> &segments, std::vector<double> &uvCoords, std::vector<int> &triConnect, double minAngle)
{
    struct triangulateio in, out;

    in.numberofpoints = static_cast<int>(uvCoords.size()/2);
    in.pointlist = &uvCoords[0];

    double triarea[] = {area};

    in.numberofsegments = static_cast<int>(segments.size()/2);
    in.segmentlist  = &segments[0];

    in.numberofholes = 0;
    in.holelist = nullptr;

    in.numberofpointattributes = 0;
    in.pointattributelist = nullptr;
    in.pointmarkerlist = nullptr;
    in.segmentmarkerlist = nullptr;
    in.triangleattributelist = nullptr;
    in.numberofregions = 0;
    in.regionlist = nullptr;
    in.trianglearealist = triarea;

    out.pointlist = nullptr;
    out.pointmarkerlist = nullptr;
    out.trianglelist = nullptr;
    out.neighborlist = nullptr;
    out.segmentlist = nullptr;
    out.segmentmarkerlist = nullptr;
    out.edgelist = nullptr;
    out.edgemarkerlist = nullptr;

    std::ostringstream oss;
    oss << "BYCPpzQ" << minAngle;
    std::string options = oss.str();

    char *opt = const_cast<char*>( options.c_str() );
    triangulate(opt, &in, &out, static_cast<struct triangulateio *>(nullptr));

    uvCoords.resize(2*out.numberofpoints);
    for (int i = 0; i < out.numberofpoints; i++) {
        uvCoords[2*i]   = out.pointlist[2*i + 0];
        uvCoords[2*i+1] = out.pointlist[2*i + 1];
    }

    triConnect.resize(3*out.numberoftriangles);
    for (int i = 0; i < out.numberoftriangles; i++) {
        triConnect[3*i+0] = out.trianglelist[3*i + 0];
        triConnect[3*i+1] = out.trianglelist[3*i + 1];
        triConnect[3*i+2] = out.trianglelist[3*i + 2];
    }

    if (out.pointlist) free(out.pointlist);
    if (out.trianglelist) free(out.trianglelist);
}

////////////////////////////////////////////////////////////////////////////////

void SurfRemesh::Face::refine(double maxarea, double minAngle)
{
    area = maxarea;
    std::vector<double> uvCoords = project();

    int nPoints = static_cast<int>(uvCoords.size()/2);

    std::vector<int> segments(2*nPoints);

    int index = 0;
    for( int i = 0; i < nPoints; i++) {
        segments[2*i]   = index;
        segments[2*i+1] = (index+1)%nPoints;
        index++;
    }

    std::vector<int> trifaces;
    delaunay(segments, uvCoords, trifaces, minAngle);
    unproject( uvCoords);

    int numTris = static_cast<int>(trifaces.size()/3);
    mesh->faces.resize(numTris);
    for( int i = 0; i < numTris; i++) {
        auto n0 = mesh->nodes[trifaces[3*i+0]];
        auto n1 = mesh->nodes[trifaces[3*i+1]];
        auto n2 = mesh->nodes[trifaces[3*i+2]];
        auto face = std::make_shared<Face>();
        face->nodes = {n0,n1,n2};
        mesh->faces[i] = face;
    }
}

////////////////////////////////////////////////////////////////////////////////

void SurfRemesh::buildEdges() const
{
    for(auto& vtx : mesh.nodes) vtx->edges.clear();

    for( auto& face : mesh.faces) {
        for( int i = 0; i < 3; i++) face->getEdgeAt(i);
    }

    mesh.edges.clear();
    std::set<std::shared_ptr<Edge>> uniqueEdges;
    for( auto& vtx : mesh.nodes) {
        for( auto& e : vtx->edges) uniqueEdges.insert(e);
    }
    for( auto& e : uniqueEdges) mesh.edges.push_back(e);
}

////////////////////////////////////////////////////////////////////////////////

void SurfRemesh::refine()
{
    if( mesh.nodes.empty() ) {
        if (!loadMesh( infilename)) return;
    }

    while(true) {

        buildEdges();

        size_t ncount = 0;
        for( auto& edge : mesh.edges) ncount += edge->refine(maxEdgeLength);

        std::cout << "#Edges refined " << ncount << std::endl;

        if( ncount == 0) break;

        double maxarea = std::sqrt(3)*0.25*maxEdgeLength*maxEdgeLength;
        for( size_t i = 0; i < mesh.faces.size(); i++)
             mesh.faces[i]->refine(maxarea, qualityMinAngle);

        size_t countnodes = mesh.nodes.size();
        for(auto& edge : mesh.edges) countnodes += edge->newNodes.size();

        for(auto& face : mesh.faces) countnodes += face->mesh->nodes.size();

        mesh.nodes.reserve( countnodes);

        size_t offset = mesh.nodes.size();
        for( auto& edge : mesh.edges) {
            for( auto& vtx : edge->newNodes) {
                vtx->id = offset++;
                mesh.nodes.push_back(vtx);
            }
        }
        mesh.edges.clear();

        std::vector<FacePtr> allfaces;
        for( auto& face : mesh.faces) {
            assert( face->mesh );
            for( auto& vtx : face->mesh->nodes) {
                vtx->id = offset++;
                mesh.nodes.push_back(vtx);
            }
            for( auto& f : face->mesh->faces)
                allfaces.push_back(f);
            face->mesh = nullptr;
        }
        mesh.faces = allfaces;
    }
}

////////////////////////////////////////////////////////////////////////////////

std::vector<Array3D> SurfRemesh :: getNodes() const
{
    std::vector<Array3D> nodes;
    nodes.reserve(mesh.nodes.size());
    for( const auto& v : mesh.nodes)
        nodes.push_back(v->xyz);
    return nodes;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<Array3I> SurfRemesh :: getTriangles() const
{
    std::vector<Array3I> tris;
    tris.reserve(mesh.faces.size());
    for( const auto& face : mesh.faces) {
        tris.push_back({static_cast<int>(face->nodes[0]->id),
                     static_cast<int>(face->nodes[1]->id),
                     static_cast<int>(face->nodes[2]->id)});
    }
    return tris;
}

////////////////////////////////////////////////////////////////////////////////

std::array<double,3> SurfRemesh :: getEdgeLengths() const
{
    std::vector<double> lengths;
    for( const auto& face : mesh.faces) {
        auto p0 = face->nodes[0]->xyz;
        auto p1 = face->nodes[1]->xyz;
        auto p2 = face->nodes[2]->xyz;
        lengths.push_back(JMath::length(p0, p1));
        lengths.push_back(JMath::length(p1, p2));
        lengths.push_back(JMath::length(p2, p0));
    }

    double minLen = lengths[0], maxLen = lengths[0], sum = 0;
    for( double len : lengths ) {
        minLen = std::min(minLen, len);
        maxLen = std::max(maxLen, len);
        sum += len;
    }
    double mean = sum / lengths.size();

    return {minLen, mean, maxLen};
}

////////////////////////////////////////////////////////////////////////////////

bool SurfRemesh::saveAs( const std::string &filename) const
{
    std::ofstream ofile( filename.c_str(), std::ios::out);
    if( ofile.fail() ) {
        std::cerr << "Error: Can't open output file " << filename << std::endl;
        return false;
    }

    ofile << "OFF" << std::endl;
    ofile << mesh.nodes.size() << " " << mesh.faces.size() << " 0 " << std::endl;

    for( const auto& v : mesh.nodes) {
        ofile << v->xyz[0] << " " << v->xyz[1] << " " << v->xyz[2] << std::endl;
    }

    for( const auto& face : mesh.faces) {
        ofile << "3 " << face->nodes[0]->id << " "
              << face->nodes[1]->id << " "
              << face->nodes[2]->id << std::endl;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////