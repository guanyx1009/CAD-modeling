#include <vtkSmartPointer.h>
#include <vtkPolygon.h>
#include <vtkTriangle.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkContourTriangulator.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include "EulerOperation.h"

std::tuple<Vertex*, Face*, Solid*> drawEx1();
std::tuple<Vertex*, Face*, Solid*> drawEx2();

int main() {
    auto mvfs1 = drawEx2();
    vtkSmartPointer<vtkAppendPolyData> appendFilter =
            vtkSmartPointer<vtkAppendPolyData>::New();

    Solid* solid = std::get<2>(mvfs1);
    for(auto i : solid->faces)
    {
        vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkPolyData> outPD = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkCellArray> loops = vtkSmartPointer<vtkCellArray>::New();
        int nowPointIdx = 0;
        int numLines = 0;
        auto startHalfEdge = i->outerLoop->startHalfEdge;
        int originalIdx = nowPointIdx;
        vtkIdType firstLine = -1;
        do
        {
            pts->InsertNextPoint(startHalfEdge->startVertex->coordinate.x, startHalfEdge->startVertex->coordinate.y, startHalfEdge->startVertex->coordinate.z);
            startHalfEdge = startHalfEdge->next;
            if (i->outerLoop->startHalfEdge->startVertex != startHalfEdge->startVertex)
            {
                nowPointIdx++;
                if (firstLine == -1)
                {
                    firstLine = loops->InsertNextCell({nowPointIdx - 1, nowPointIdx});;
                }
                else loops->InsertNextCell({nowPointIdx - 1, nowPointIdx});
                numLines++;
            }
            else
            {
                loops->InsertNextCell({nowPointIdx, originalIdx});
                numLines++;
            }
        }while(i->outerLoop->startHalfEdge->startVertex != startHalfEdge->startVertex);


        for (auto j : i->innerLoops)
        {
            auto startHalfEdge = j->startHalfEdge;
            auto nowHalfEdge = j->startHalfEdge;
            nowPointIdx++;
            int innerOriginalIdx = nowPointIdx;

            do {
                pts->InsertNextPoint(nowHalfEdge->startVertex->coordinate.x, nowHalfEdge->startVertex->coordinate.y, nowHalfEdge->startVertex->coordinate.z);
                nowHalfEdge = nowHalfEdge->next;
                if (nowHalfEdge->startVertex != startHalfEdge->startVertex)
                {
                    nowPointIdx++;
                    loops->InsertNextCell({nowPointIdx - 1, nowPointIdx});
                    numLines++;
                }
                else
                {
                    loops->InsertNextCell({nowPointIdx, innerOriginalIdx});
                    numLines++;
                }
            } while (nowHalfEdge->startVertex != startHalfEdge->startVertex);
        }

        vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
        pd->SetPoints(pts);
        pd->SetLines(loops);

        vtkSmartPointer<vtkCellArray> outputCA = vtkSmartPointer<vtkCellArray>::New();
        vtkSmartPointer<vtkContourTriangulator> ct = vtkSmartPointer<vtkContourTriangulator>::New();
        ct->TriangulateContours(pd, firstLine, numLines, outputCA, nullptr);

        outPD->SetPoints(pts);
        outPD->SetPolys(outputCA);

        appendFilter->AddInputData(outPD);
        appendFilter->Update();
    }

    vtkSmartPointer<vtkCleanPolyData> cleanFilter =
            vtkSmartPointer<vtkCleanPolyData>::New();
    cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
    cleanFilter->Update();

    //Create a mapper and actor
    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(cleanFilter->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
//    actor->GetProperty()->SetRepresentationToWireframe();

    // Graphics objects
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer>::New();
    ren1->SetBackground(0, 0, 0);
    ren1->AddActor(actor);
    ren1->ResetCamera();

    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(ren1);
    renWin->SetSize(640, 480);

    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);

    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

std::tuple<Vertex*, Face*, Solid*> drawEx1()
{
    auto mvfs1 = MakeVertexFaceSolid(Vec3d { 0, 2, 0});
    auto loop = std::get<2>(mvfs1)->faces.front()->outerLoop;
    auto mev1 = MakeEdgeVertex(std::get<0>(mvfs1), Vec3d{ 1, 0, 0 }, loop);
    auto mev2 = MakeEdgeVertex(std::get<1>(mev1), Vec3d{ 2, 0, 0 }, loop);
    auto mev3 = MakeEdgeVertex(std::get<1>(mev2), Vec3d{ 1, -1, 0 }, loop);
    auto mev4 = MakeEdgeVertex(std::get<1>(mev3), Vec3d{ 1.5, -2, 0 }, loop);
    auto mev5 = MakeEdgeVertex(std::get<1>(mev4), Vec3d{ 0, -1.5, 0 }, loop);
    auto mev6 = MakeEdgeVertex(std::get<1>(mev5), Vec3d{ -1.5, -2, 0 }, loop);
    auto mev7 = MakeEdgeVertex(std::get<1>(mev6), Vec3d{ -1, -1, 0 }, loop);
    auto mev8 = MakeEdgeVertex(std::get<1>(mev7), Vec3d{ -2, 0, 0 }, loop);
    auto mev9 = MakeEdgeVertex(std::get<1>(mev8), Vec3d{ -1, 0, 0 }, loop);
    auto mef1 = MakeEdgeFace(loop, std::get<1>(mev9), std::get<0>(mvfs1));

    loop = std::get<1>(mef1)->outerLoop;
    auto mev10 = MakeEdgeVertex(std::get<0>(mvfs1), Vec3d{ 0, 1, 0 }, loop);
    auto kemr1 = KillEdgeMakeRing(std::get<0>(mvfs1), std::get<1>(mev10), loop);
    loop = std::get<0>(kemr1);
    auto mev11 = MakeEdgeVertex(std::get<1>(mev10), Vec3d{ 0.5, 0, 0 }, loop);
    auto mev12 = MakeEdgeVertex(std::get<1>(mev11), Vec3d{ 1, 0, 0 }, loop);
    auto mev13 = MakeEdgeVertex(std::get<1>(mev12), Vec3d{ 0.5, -0.5, 0 }, loop);
    auto mev14 = MakeEdgeVertex(std::get<1>(mev13), Vec3d{ 0.75, -1, 0 }, loop);
    auto mev15 = MakeEdgeVertex(std::get<1>(mev14), Vec3d{ 0, -0.75, 0 }, loop);
    auto mev16 = MakeEdgeVertex(std::get<1>(mev15), Vec3d{ -0.75, -1, 0 }, loop);
    auto mev17 = MakeEdgeVertex(std::get<1>(mev16), Vec3d{ -0.5, -0.5, 0 }, loop);
    auto mev18 = MakeEdgeVertex(std::get<1>(mev17), Vec3d{ -1, 0, 0 }, loop);
    auto mev19 = MakeEdgeVertex(std::get<1>(mev18), Vec3d{ -0.5, 0, 0 }, loop);
    auto mef2 = MakeEdgeFace(loop, std::get<1>(mev19), std::get<1>(mev10));

    Sweep(std::get<1>(mef1),Vec3d{ 0, 0, 1 });
    KillFaceMakeRingHole(std::get<2>(mvfs1)->faces.front()->outerLoop, std::get<1>(mef2)->outerLoop);

    return mvfs1;
}

std::tuple<Vertex*, Face*, Solid*> drawEx2()
{
    auto mvfs1 = MakeVertexFaceSolid(Vec3d { 0, 0, 0});
    auto loop = std::get<2>(mvfs1)->faces.front()->outerLoop;
    auto mev1 = MakeEdgeVertex(std::get<0>(mvfs1), Vec3d{ 5, 0, 0 }, loop);
    auto mev2 = MakeEdgeVertex(std::get<1>(mev1), Vec3d{ 5, 3, 0 }, loop);
    auto mev3 = MakeEdgeVertex(std::get<1>(mev2), Vec3d{ 0, 3, 0 }, loop);
    auto mef1 = MakeEdgeFace(loop, std::get<1>(mev3), std::get<0>(mvfs1));

    loop = std::get<1>(mef1)->outerLoop;
    auto mev4 = MakeEdgeVertex(std::get<0>(mvfs1), Vec3d{ 1, 1, 0 }, loop);
    auto kemr1 = KillEdgeMakeRing(std::get<0>(mvfs1), std::get<1>(mev4), loop);

    loop = std::get<0>(kemr1);
    auto mev5 = MakeEdgeVertex(std::get<1>(mev4), Vec3d{ 2, 1, 0 }, loop);
    auto mev6 = MakeEdgeVertex(std::get<1>(mev5), Vec3d{ 2, 2, 0 }, loop);
    auto mev7 = MakeEdgeVertex(std::get<1>(mev6), Vec3d{ 1, 2, 0 }, loop);
    auto mef2 = MakeEdgeFace(loop, std::get<1>(mev7), std::get<1>(mev4));

    loop = std::get<1>(mef1)->outerLoop;
    auto mev8 = MakeEdgeVertex(std::get<1>(mev1), Vec3d{ 4, 1, 0 }, loop);
    auto kemr2 = KillEdgeMakeRing(std::get<1>(mev1), std::get<1>(mev8), loop);

    loop = std::get<0>(kemr2);
    auto mev9 = MakeEdgeVertex(std::get<1>(mev8), Vec3d{ 4, 2, 0 }, loop);
    auto mev10 = MakeEdgeVertex(std::get<1>(mev9), Vec3d{ 3, 2, 0 }, loop);
    auto mev11 = MakeEdgeVertex(std::get<1>(mev10), Vec3d{ 3, 1, 0 }, loop);
    auto mef3 = MakeEdgeFace(loop, std::get<1>(mev11), std::get<1>(mev8));
    Sweep(std::get<1>(mef1),Vec3d{ 1, 0, 1 });
    KillFaceMakeRingHole(std::get<2>(mvfs1)->faces.front()->outerLoop, std::get<1>(mef2)->outerLoop);
    KillFaceMakeRingHole(std::get<2>(mvfs1)->faces.front()->outerLoop, std::get<1>(mef3)->outerLoop);

    return mvfs1;
}