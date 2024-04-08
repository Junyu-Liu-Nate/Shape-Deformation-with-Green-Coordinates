#ifndef HALFEDGE_H
#define HALFEDGE_H

#include <list>
#include <Eigen/Core>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <Eigen/SVD>

using namespace std;
using namespace Eigen;

struct HalfEdge;
struct Vertex;
struct Face;

struct HalfEdge {
    HalfEdge* twin;
    HalfEdge* next;
    Vertex* vertex;

    // Constructor for convenience
    HalfEdge() : twin(nullptr), next(nullptr), vertex(nullptr) {}
};

struct Vertex {
    int vertexIdx;

    Eigen::Vector3f position; // Actual position
    Eigen::Vector3f nextPosition;

    HalfEdge* halfEdge;

    // Variables used for calculating Green Coordinates (For Cages)
    Eigen::Vector3f calculatePosition;
    float s;
    float I;
    float II;
    Vector3f q;
    Vector3f N;

    // Constructor for convenience
    Vertex(const Eigen::Vector3f& pos) : position(pos), halfEdge(nullptr) {}
};

struct Face {
    HalfEdge* halfEdges[3];  // Array to store pointers to the face's half-edges

    // Constructor for convenience
    Face() : halfEdges{nullptr, nullptr, nullptr} {}

    Eigen::Vector3f getNormal() const {
        if (halfEdges[0] == nullptr || halfEdges[1] == nullptr || halfEdges[2] == nullptr) {
            // Invalid face, return zero vector as placeholder
            return Eigen::Vector3f(0.0f, 0.0f, 0.0f);
        }
        // Obtain the position of the vertices
        Eigen::Vector3f p0 = halfEdges[0]->vertex->position;
        Eigen::Vector3f p1 = halfEdges[1]->vertex->position;
        Eigen::Vector3f p2 = halfEdges[2]->vertex->position;

        // Compute two vectors on the face
        Eigen::Vector3f v1 = p1 - p0;
        Eigen::Vector3f v2 = p2 - p0;

        // Compute the cross product to get the face normal
        Eigen::Vector3f normal = v1.cross(v2);

        // Normalize the normal to ensure it's a unit vector
        normal.normalize();

        return normal;
    }
};

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

class HalfEdgeMesh {
public:
    HalfEdgeMesh();

    std::vector<Vertex> vertices;
    std::list<HalfEdge> halfEdges;
    std::vector<Face> faces;

    void buildHalfEdgeStructure(const std::vector<Eigen::Vector3f>& _vertices,
                                const std::vector<Eigen::Vector3i>& _faces);

    void updateVertexPos(std::vector<Eigen::Vector3f>& outVertices);

    int vertexDegree(Vertex* vertex);

private:

};

#endif // HALFEDGE_H
