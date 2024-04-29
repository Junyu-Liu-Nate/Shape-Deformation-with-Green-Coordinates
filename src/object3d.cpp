#include "object3d.h"

Object3D::Object3D()
{

}

void Object3D::updateVertices(const HalfEdgeMesh& heMesh) {
    const float LARGE_NUMBER_THRESHOLD = 5;

    //----- Green Coordinates
    #pragma omp parallel for
    for (ObjectVertex& objectVertex : vertexList) {
        Vector3f term1 = Vector3f(0,0,0);
        for (int i = 0; i < objectVertex.greenCord.phiCoords.size(); i++) {
            term1 += objectVertex.greenCord.phiCoords.at(i) * heMesh.vertices.at(i).position.cast<float>();
        }

        Vector3f term2 = Vector3f(0,0,0);
        for (int i = 0; i < objectVertex.greenCord.psiCoords.size(); i++) {
            // TODO: s can be toggled between 1 and the calculation
            float s = calculateS(heMesh.faces.at(i));
            term2 += objectVertex.greenCord.psiCoords.at(i) * heMesh.faces.at(i).calculateNormal().cast<float>() * s;
        }

        objectVertex.position = term1 + term2;
    }

    //----- MVC Coordinates
//    #pragma omp parallel for
//    for (ObjectVertex& objectVertex : vertexList) {
//        Vector3f newPos = Vector3f(0,0,0);
//        float wTotal = 0;
//        for (int i = 0; i < objectVertex.mvcCoord.wCoords.size(); i++) {
//            newPos += objectVertex.mvcCoord.wCoords.at(i) * heMesh.vertices.at(i).position.cast<float>();
//            wTotal += objectVertex.mvcCoord.wCoords.at(i);
//        }
//        objectVertex.position = newPos / wTotal;
//    }
}

vector<Vector3f> Object3D::getVertices() {
    vector<Vector3f> returnVertices;
    returnVertices.resize(vertexList.size());

    for (int i = 0; i < vertexList.size(); i++) {
        returnVertices.at(i) = vertexList.at(i).position;
    }

    return returnVertices;
}

double Object3D::calculateS(const Face face) {
    Vector3d p0 = face.halfEdges[0]->vertex->initialPosition;
    Vector3d p1 = face.halfEdges[1]->vertex->initialPosition;
    Vector3d p2 = face.halfEdges[2]->vertex->initialPosition;

    Vector3d u = p1 - p0;
    Vector3d v = p2 - p0;

    Vector3d p0_prime = face.halfEdges[0]->vertex->position;
    Vector3d p1_prime = face.halfEdges[1]->vertex->position;
    Vector3d p2_prime = face.halfEdges[2]->vertex->position;

    Vector3d u_prime = p1_prime - p0_prime;
    Vector3d v_prime = p2_prime - p0_prime;

    double nominator = sqrt(pow(u_prime.norm(),2) * pow(v.norm(),2) - 2*u_prime.dot(v_prime)*u.dot(v) + pow(v_prime.norm(),2) * pow(u.norm(),2));
    double denomonator = sqrt(8) * face.calculateArea();

    double s = nominator / denomonator;

    return s;
}
