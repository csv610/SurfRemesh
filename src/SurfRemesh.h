#include <array>
#include <vector>
#include <memory>
#include <string>
#include <map>

#include "veclib.hpp"
#include "trilib.hpp"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

class SurfRemesh
{
public:
    struct Node;
    using NodePtr = std::shared_ptr<Node>;

    struct Edge;
    using EdgePtr = std::shared_ptr<Edge>;

    struct Face;
    using FacePtr = std::shared_ptr<Face>;

    struct Mesh;
    using MeshPtr = std::shared_ptr<Mesh>;

    struct Node
    {
        explicit Node() : id(0), xyz({0,0,0}) {}
        size_t  id;
        Array3D xyz;
        std::vector<EdgePtr> edges;
    };

    struct Edge
    {
        explicit Edge() = default;
        int getOrientation( const NodePtr &v0, const NodePtr &v1) const
        {
            if( nodes[0] == v0  && nodes[1] == v1) return 1;
            if( nodes[0] == v1  && nodes[1] == v0) return -1;
            return 0;
        }

        bool   isSame( const NodePtr &v0, const NodePtr &v1) const
        {
            if( nodes[0] == v0  && nodes[1] == v1) return true;
            if( nodes[0] == v1  && nodes[1] == v0) return true;
            return false;
        }

        int refine( double len);

        std::array<NodePtr,2> nodes;
        std::vector<NodePtr>  newNodes;
    };

    struct Face
    {
        explicit Face() : area(0.0) {}
        void refine(double len, double minAngle = 30.0);
        void delaunay(std::vector<int> &segments, std::vector<double> &uvCoords, std::vector<int> &connect, double minAngle = 30.0);

        EdgePtr getEdgeAt(int i);

        std::vector<double>   project();
        void unproject( const std::vector<double> &uv);

        MeshPtr mesh;
        std::array<Array2D,3> uvCorners;
        std::array<NodePtr,3> nodes;
        double area;
    };

    struct Mesh
    {
        void clear() {
            nodes.clear();
            edges.clear();
            faces.clear();
        }
        std::vector<NodePtr> nodes;
        std::vector<EdgePtr> edges;
        std::vector<FacePtr> faces;
    };

public:
    SurfRemesh();

    /**
     * @brief Set the input mesh file path
     * @param f Path to mesh file (OFF, OBJ, PLY, STL, etc.)
     */
    void setMesh( const std::string &f) {
        infilename = f;
    }

    /**
     * @brief Set number of refinement steps for automatic edge length calculation
     * @param n Number of steps (divides longest edge n times)
     *
     * If maxEdgeLength is specified via setMaxEdgeLength(), this parameter is ignored.
     */
    void setMaxSteps(int n) {
	    maxSteps = n;
    }

    /**
     * @brief Set the desired maximum edge length
     * @param l Maximum edge length in mesh units
     */
    void setMaxEdgeLength( double l) {
        maxEdgeLength = l;
    }

    /**
     * @brief Set the desired minimum edge length
     * @param l Minimum edge length in mesh units (optional constraint)
     */
    void setMinEdgeLength( double l) {
        minEdgeLength = l;
    }

    /**
     * @brief Set the crease angle for feature detection
     * @param a Angle in degrees for identifying sharp edges
     */
    void setCreaseAngle( double a) {
        creaseAngle = a;
    }

    /**
     * @brief Set the minimum angle for quality mesh generation
     * @param a Angle in degrees (20-34 recommended, default 30)
     */
    void setQualityMinAngle( double a) {
        qualityMinAngle = a;
    }

    /**
     * @brief Get edge length statistics (min, mean, max)
     * @return Array of [min, mean, max] edge lengths
     */
    std::array<double,3>  getEdgeLengths() const;

    /**
     * @brief Execute mesh refinement algorithm
     */
    void refine();

    /**
     * @brief Get refined mesh nodes as 3D points
     * @return Vector of Array3D (x,y,z) coordinates
     */
    std::vector<Array3D> getNodes() const;

    /**
     * @brief Get refined mesh triangle connectivity
     * @return Vector of Array3I (i0,i1,i2) vertex indices
     */
    std::vector<Array3I> getTriangles() const;

    /**
     * @brief Save refined mesh to file
     * @param s Output file path (OFF format)
     * @return true on success, false on failure
     */
    bool saveAs( const std::string &s) const;

private:
    mutable Mesh mesh;
    std::string infilename;
    double maxEdgeLength   = 0.0;
    double minEdgeLength   = 0.0;
    double creaseAngle     = 0.0;
    int    maxSteps        = 0;
    double qualityMinAngle = 30.0;

    bool   readOFFMesh( const std::string &f) const;
    bool   loadMesh( const std::string &f) const;
    void   buildEdges() const;
};
