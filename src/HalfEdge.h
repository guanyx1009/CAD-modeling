#ifndef CAD_MODELING_HALFEDGE_H
#define CAD_MODELING_HALFEDGE_H

#include "Common.h"
#include <list>

class Vertex;
class Edge;
class HalfEdge;
class Loop;
class Face;
class Solid;


class Vertex
{
public:
    Vec3d coordinate;
    Vertex() {coordinate.x = coordinate.y = coordinate.z = 0;}
};

class Edge
{
public:
    Edge() : halfEdge0(nullptr), halfEdge1(nullptr) {}
    HalfEdge* halfEdge0;
    HalfEdge* halfEdge1;
};

class HalfEdge
{
public:
    HalfEdge() : startVertex(nullptr), parentLoop(nullptr), parentEdge(nullptr),
                next(nullptr), pre(nullptr), twins(nullptr) {}
    Vertex* startVertex;
    Loop* parentLoop;
    Edge* parentEdge;

    HalfEdge* next;
    HalfEdge* pre;
    HalfEdge* twins;
};

class Loop
{
public:
    Loop() : startHalfEdge(nullptr), parentFace(nullptr) {}
    HalfEdge* startHalfEdge;
    Face* parentFace;

    HalfEdge* findHalfEdgeByStartV(Vertex* startVertex)
    {
        auto tempHalfEdge = startHalfEdge;
        while (tempHalfEdge)
        {
            if (tempHalfEdge->startVertex == startVertex)
            {
                return tempHalfEdge;
            }
            tempHalfEdge = tempHalfEdge->next;
            if (tempHalfEdge == startHalfEdge)
                break;
        }
        return nullptr;
    }
};

class Face
{
public:
    Face() : outerLoop(nullptr), parentSolid(nullptr) {}
    Loop* outerLoop;
    std::list<Loop*> innerLoops;
    Solid* parentSolid;
};

class Solid
{
public:
    std::list<Vertex*> vertices;
    std::list<Edge*> edges;
    std::list<Face*> faces;
};

#endif //CAD_MODELING_HALFEDGE_H
