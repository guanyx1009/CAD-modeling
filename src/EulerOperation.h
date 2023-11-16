#ifndef CAD_MODELING_EULEROPERATION_H
#define CAD_MODELING_EULEROPERATION_H

#include "HalfEdge.h"
#include <tuple>

std::tuple<Vertex*, Face*, Solid*> MakeVertexFaceSolid(Vec3d position)
{
    Vertex* newVertex = new Vertex();
    Loop* newLoop = new Loop();
    Face* newFace = new Face();
    Solid* newSolid = new Solid();

    newVertex->coordinate = position;
    newLoop->parentFace = newFace;

    newFace->outerLoop = newLoop;
    newFace->parentSolid = newSolid;

    newSolid->vertices.push_back(newVertex);
    newSolid->faces.push_back(newFace);

    return std::make_tuple(newVertex, newFace, newSolid);
}

std::tuple<Edge*, Vertex*> MakeEdgeVertex(Vertex* oldVertex, Vec3d newPositon, Loop* oldLoop)
{
    Vertex* newVertex = new Vertex();
    HalfEdge* halfEdge0 = new HalfEdge();
    HalfEdge* halfEdge1 = new HalfEdge();
    Edge* newEdge = new Edge();

    newVertex->coordinate = newPositon;

    halfEdge0->startVertex = oldVertex;
    halfEdge1->startVertex = newVertex;

    halfEdge0->next = halfEdge1;
    halfEdge1->pre = halfEdge0;

    halfEdge0->twins = halfEdge1;
    halfEdge1->twins = halfEdge0;

    halfEdge0->parentEdge = newEdge;
    halfEdge1->parentEdge = newEdge;
    halfEdge0->parentLoop = oldLoop;
    halfEdge1->parentLoop = oldLoop;

    newEdge->halfEdge0 = halfEdge0;
    newEdge->halfEdge1 = halfEdge1;

    if (oldLoop->startHalfEdge == nullptr)
    {
        oldLoop->startHalfEdge = halfEdge0;
        halfEdge0->pre = halfEdge1;
        halfEdge1->next = halfEdge0;
    }
    else
    {
        HalfEdge* tempHalfEdge;
        for (tempHalfEdge = oldLoop->startHalfEdge;
        tempHalfEdge->next->startVertex != oldVertex; tempHalfEdge = tempHalfEdge->next);

        halfEdge0->pre = tempHalfEdge;
        halfEdge1->next = tempHalfEdge->next;

        tempHalfEdge->next->pre = halfEdge1;
        tempHalfEdge->next = halfEdge0;
    }

    Solid* oldSolid = oldLoop->parentFace->parentSolid;
    oldSolid->vertices.push_back(newVertex);
    oldSolid->edges.push_back(newEdge);

    return std::make_tuple(newEdge, newVertex);
}

std::tuple<Edge*, Face*> MakeEdgeFace(Loop* oldLoop, Vertex* oldVertex0, Vertex* oldVertex1)
{
    // /|\        /|\
    //  |          |
    //  |---hf1--->|
    //  1<---hf0---0
    //
    Face* newFace = new Face();
    Loop* newLoop = new Loop();
    Edge* newEdge = new Edge();
    HalfEdge* newHalfEdge0 = new HalfEdge();
    HalfEdge* newHalfEdge1 = new HalfEdge();
    newHalfEdge0->twins = newHalfEdge1;
    newHalfEdge1->twins = newHalfEdge0;

    Solid* oldSolid = oldLoop->parentFace->parentSolid;

    oldSolid->edges.push_back(newEdge);
    oldSolid->faces.push_back(newFace);

    newFace->outerLoop = newLoop;
    newFace->parentSolid = oldSolid;

    newLoop->parentFace = newFace;

    newEdge->halfEdge0 = newHalfEdge0;
    newEdge->halfEdge1 = newHalfEdge1;


    HalfEdge* tempHalfEdge0 = oldLoop->findHalfEdgeByStartV(oldVertex0);
    HalfEdge* tempHalfEdge1 = oldLoop->findHalfEdgeByStartV(oldVertex1);

    newHalfEdge0->next = tempHalfEdge1;
    newHalfEdge1->next = tempHalfEdge0;
    newHalfEdge0->pre = tempHalfEdge0->pre;
    newHalfEdge1->pre = tempHalfEdge1->pre;

    tempHalfEdge0->pre->next = newHalfEdge0;
    tempHalfEdge1->pre->next = newHalfEdge1;
    tempHalfEdge0->pre = newHalfEdge1;
    tempHalfEdge1->pre = newHalfEdge0;

    newHalfEdge0->parentLoop = oldLoop;
    newHalfEdge1->parentLoop = newLoop;
    oldLoop->startHalfEdge = newHalfEdge0;
    newLoop->startHalfEdge = newHalfEdge1;

    newHalfEdge0->parentEdge = newEdge;
    newHalfEdge1->parentEdge = newEdge;
    newHalfEdge0->startVertex = oldVertex0;
    newHalfEdge1->startVertex = oldVertex1;

    return std::make_tuple(newEdge, newFace);
}

std::tuple<Loop*> KillEdgeMakeRing(Vertex* oldVertex0, Vertex* oldVertex1, Loop* oldLoop)
{
    Loop* newLoop = new Loop();

    HalfEdge* oldHalfEdge0 = oldLoop->startHalfEdge;
    HalfEdge* oldHalfEdge1 = oldHalfEdge0->next;
    while (oldHalfEdge0->twins != oldHalfEdge1 || oldHalfEdge0->startVertex != oldVertex0 || oldHalfEdge1->startVertex != oldVertex1)
    {
        oldHalfEdge0 = oldHalfEdge1;
        oldHalfEdge1 = oldHalfEdge1->next;
    }

    oldLoop->startHalfEdge = oldHalfEdge0->pre;
    oldHalfEdge0->pre->next = oldHalfEdge1->next;
    oldHalfEdge1->next->pre = oldHalfEdge0->pre;

    newLoop->parentFace = oldLoop->parentFace;
    newLoop->parentFace->innerLoops.push_back(newLoop);

    Edge* oldEdge = oldHalfEdge0->parentEdge;
    Solid* oldSolid = oldLoop->parentFace->parentSolid;
    oldSolid->edges.remove(oldEdge);
    delete oldEdge;
    delete oldHalfEdge0;
    delete oldHalfEdge1;

    return std::make_tuple(newLoop);
}

void KillFaceMakeRingHole(Loop* outerloop, Loop* innerLoop)
{
    Solid* oldSolid = outerloop->parentFace->parentSolid;
    outerloop->parentFace->innerLoops.push_back(innerLoop);
    delete innerLoop->parentFace;
    oldSolid->faces.remove(innerLoop->parentFace);
    innerLoop->parentFace = outerloop->parentFace;
}

void PartialSweep(Loop* loop, Vec3d direction)
{
    HalfEdge* nowHalfEdge = loop->startHalfEdge;
    Vertex* startVertex = nowHalfEdge->startVertex;

    auto startEdgeVertex = MakeEdgeVertex(startVertex, startVertex->coordinate + direction, loop);
    nowHalfEdge = nowHalfEdge->next;
    auto lastEdgeVertex = startEdgeVertex;
    while (nowHalfEdge->startVertex != startVertex)
    {
        auto nowEdgeVertex = MakeEdgeVertex(nowHalfEdge->startVertex, nowHalfEdge->startVertex->coordinate + direction, loop);
        MakeEdgeFace(loop, std::get<1>(lastEdgeVertex), std::get<1>(nowEdgeVertex));
        lastEdgeVertex = nowEdgeVertex;
        nowHalfEdge = nowHalfEdge->next;
    }
    MakeEdgeFace(loop, std::get<1>(lastEdgeVertex), std::get<1>(startEdgeVertex));
}

void Sweep(Face* face, Vec3d direction)
{
    PartialSweep(face->outerLoop, direction);
    for (auto innerLoop : face->innerLoops)
    {
        PartialSweep(innerLoop, direction);
    }
}

#endif //CAD_MODELING_EULEROPERATION_H
