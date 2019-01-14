/* Copyright (c) Stanford University, The Regents of the University of
 *               California, and others.
 *
 * All Rights Reserved.
 *
 * See Copyright-SimVascular.txt for additional details.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sv4gui_PurkinjeNetworkMeshMapper.h"
#include "sv4gui_PurkinjeNetworkMeshContainer.h"
#include "vtkPolyDataMapper.h"
#include "vtkSphereSource.h"
#include "vtkCubeSource.h"
#include <vtkDataSetMapper.h>
#include <vtkCellLocator.h>

sv4guiPurkinjeNetworkMeshMapper::sv4guiPurkinjeNetworkMeshMapper()
{
}

sv4guiPurkinjeNetworkMeshMapper::~sv4guiPurkinjeNetworkMeshMapper()
{
}

// Generate the data needed for rendering into renderer.
//
void sv4guiPurkinjeNetworkMeshMapper::GenerateDataForRenderer(mitk::BaseRenderer* renderer)
{
  //MITK_INFO << "[sv4guiPurkinjeNetworkMeshMapper::GenerateDataForRenderer] ";

  //make ls propassembly
  mitk::DataNode* node = GetDataNode();

  if (node == NULL) {
      //MITK_INFO << "[sv4guiPurkinjeNetworkMeshMapper::GenerateDataForRenderer] Data node is null";
      return;
  }

  LocalStorage* local_storage = m_LSH.GetLocalStorage(renderer);

  bool visible = true;
  GetDataNode()->GetVisibility(visible, renderer, "visible");
  //MITK_INFO << "[sv4guiPurkinjeNetworkMeshMapper::GenerateDataForRenderer] visible " << visible;

  if (!visible) {
      local_storage->m_PropAssembly->VisibilityOff();
      return;
  }

  sv4guiPurkinjeNetworkMeshContainer* mesh = 
    static_cast< sv4guiPurkinjeNetworkMeshContainer* >( node->GetData() );

  if (mesh == NULL) {
      //MITK_INFO << "[sv4guiPurkinjeNetworkMeshMapper::GenerateDataForRenderer] No data ";
      local_storage->m_PropAssembly->VisibilityOff();
      return;
  } else {
      //MITK_INFO << "[sv4guiPurkinjeNetworkMeshMapper::GenerateDataForRenderer] Have data ";
  }

  // [DaveP] Do we need to remove?
  // local_storage->m_PropAssembly->GetParts()->RemoveAllItems();

  //local_storage->m_PropAssembly->VisibilityOn();

  // Show surface mesh.
  //
  auto surfaceMesh = mesh->GetSurfaceMesh();
  auto modelFaces = mesh->GetModelFaces();

  if (surfaceMesh != NULL && m_newMesh) {
    //MITK_INFO << "[sv4guiPurkinjeNetworkMeshMapper::GenerateDataForRenderer] Have surface mesh data ";
    auto polyMesh = surfaceMesh->GetSurfaceMesh();
    vtkSmartPointer<vtkPolyDataMapper> meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    meshMapper->SetInputData(polyMesh);
    meshMapper->ScalarVisibilityOff();  // Turn off mapping of scalar data, otherwise can't set color.
    vtkSmartPointer<vtkActor> polyMeshActor = vtkSmartPointer<vtkActor>::New();
    polyMeshActor->SetMapper(meshMapper);
    polyMeshActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
    polyMeshActor->GetProperty()->SetEdgeColor(0, 0, 0);
    polyMeshActor->GetProperty()->EdgeVisibilityOn();
    local_storage->m_PropAssembly->AddPart(polyMeshActor);

    double bounds[6];
    polyMesh->GetBounds(bounds);
    double xmin = bounds[0]; double xmax = bounds[1]; double ymin = bounds[2];
    double ymax = bounds[3]; double zmin = bounds[4]; double zmax = bounds[5];
    double dx = xmax - xmin; double dy = ymax - ymin; double dz = zmax - zmin;
    MITK_INFO << "[sv4guiPurkinjeNetworkMeshMapper::GenerateDataForRenderer] dx "<< dx 
        << "  dy "<<dy<< "  dz "<< dz;

    m_pickRadius = (dx > dy) ? dx : dy;
    m_pickRadius = (m_pickRadius > dz) ? m_pickRadius : dz;
    m_newMesh = false;

  } else {
      //MITK_INFO << "[sv4guiPurkinjeNetworkMeshMapper::GenerateDataForRenderer] No surface mesh data ";
  }

  // Set Face color?
/*
  float selectedColor[3]= { 1.0f, 1.0f, 0.0f };
  node->GetColor(selectedColor, renderer, "face selected color");

  for (const auto& face : modelFaces) {
    vtkSmartPointer<vtkPolyData> facePolyData = face->vpd;
    vtkSmartPointer<vtkOpenGLPolyDataMapper> faceMapper = vtkSmartPointer<vtkOpenGLPolyDataMapper>::New();
    vtkSmartPointer<vtkActor> faceActor= vtkSmartPointer<vtkActor>::New();
    faceActor->SetMapper(faceMapper);

    //ApplyAllProperties(renderer, faceMapper, faceActor);

    Superclass::ApplyColorAndOpacityProperties(renderer, faceActor);

    ApplyAllProperties(node, renderer, faceMapper, faceActor, &m_LSH, true);

    faceActor->GetProperty()->SetColor(selectedColor[0], selectedColor[1], selectedColor[2]);
    faceActor->GetProperty()->SetOpacity(face->opacity);
  }
*/

  // Show picked point.
  local_storage->m_PropAssembly->GetParts()->RemoveItem(m_sphereActor);
  auto point = mesh->getPickedPoint();
  auto pointActor = createSphereActor(point);
  local_storage->m_PropAssembly->AddPart(pointActor);

  // Find closest face.
  this->findClosestFace(mesh, point);

  local_storage->m_PropAssembly->VisibilityOn();
}

void sv4guiPurkinjeNetworkMeshMapper::ApplyAllProperties(mitk::DataNode *node, mitk::BaseRenderer* renderer, 
    vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper, vtkSmartPointer<vtkActor> actor, 
    mitk::LocalStorageHandler<LocalStorage>* handler, bool clipping)
{
  //ApplyMitkPropertiesToVtkProperty( node, actor->GetProperty(), renderer );

}

// Find the face in the mesh closest to the picked point.
//
void sv4guiPurkinjeNetworkMeshMapper::findClosestFace(sv4guiPurkinjeNetworkMeshContainer* mesh, 
    mitk::Point3D& point)
{
  //MITK_INFO << "[sv4guiPurkinjeNetworkMeshMapper::findClosestFace] ";
  auto surfaceMesh = mesh->GetSurfaceMesh();
  if (surfaceMesh == NULL) {
    return;
  }

  //MITK_INFO << "Query point: " << point[0] << " " << point[1] << " " << point[2];
  auto polyMesh = surfaceMesh->GetSurfaceMesh();

  // Build the cell locator.
  vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
  cellLocator->SetDataSet(polyMesh);
  cellLocator->BuildLocator();

  // Find the closest point.
  double testPoint[3] = {point[0], point[1], point[2]};
  double closestPoint[3];
  double closestPointDist2; 
  vtkIdType cellId; 
  int subId; 
  cellLocator->FindClosestPoint(testPoint, closestPoint, cellId, subId, closestPointDist2);
  
  /*
  MITK_INFO << "Coordinates of closest point: " << closestPoint[0] << " " << closestPoint[1] << " " << closestPoint[2];
  MITK_INFO << "Squared distance to closest point: " << closestPointDist2;
  MITK_INFO << "CellId: " << cellId;
  */

  vtkCell* cell = polyMesh->GetCell(cellId);
  vtkTriangle* triangle = dynamic_cast<vtkTriangle*>(cell);
  double p0[3];
  double p1[3];
  double p2[3];
  triangle->GetPoints()->GetPoint(0, p0);
  //std::cout << "p0: " << p0[0] << " " << p0[1] << " " << p0[2] << std::endl;
  triangle->GetPoints()->GetPoint(1, p1);
  //std::cout << "p1: " << p1[0] << " " << p1[1] << " " << p1[2] << std::endl;
  triangle->GetPoints()->GetPoint(2, p2);
  //std::cout << "p2: " << p2[0] << " " << p2[1] << " " << p2[2] << std::endl;

  for (int i = 0; i < 3; i++) {
    m_point1[i] = point[i];
    m_point2[i] = (p0[i] + p1[i]) / 2.0;
  }

 
  // Find neighbor cells.
/*
  vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
  polyMesh->GetCellPoints(cellId, cellPointIds);
  std::set<vtkIdType> neighbors;
  for(vtkIdType i = 0; i < cellPointIds->GetNumberOfIds(); i++) {
    vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
    idList->InsertNextId(cellPointIds->GetId(i));
    //get the neighbors of the cell
    vtkSmartPointer<vtkIdList> neighborCellIds = vtkSmartPointer<vtkIdList>::New();
    polyMesh->GetCellNeighbors(cellId, idList, neighborCellIds);

    for(vtkIdType j = 0; j < neighborCellIds->GetNumberOfIds(); j++) {
      neighbors.insert(neighborCellIds->GetId(j));
    }
  }

  MITK_INFO << "Point neighbor ids are: ";
  for(std::set<vtkIdType>::iterator it1 = neighbors.begin(); it1 != neighbors.end(); it1++) {
    MITK_INFO << " " << *it1;
  }
*/

}

void sv4guiPurkinjeNetworkMeshMapper::ResetMapper(mitk::BaseRenderer* renderer)
{
  LocalStorage *ls = m_LSH.GetLocalStorage(renderer);
  ls->m_PropAssembly->VisibilityOff();
}

vtkProp* sv4guiPurkinjeNetworkMeshMapper::GetVtkProp(mitk::BaseRenderer* renderer)
{
  //MITK_INFO << "[sv4guiPurkinjeNetworkMeshMapper::GetVtkProp] ";
  ResetMapper(renderer);
  GenerateDataForRenderer(renderer);
  LocalStorage *ls = m_LSH.GetLocalStorage(renderer);
  return ls->m_PropAssembly;
}

vtkSmartPointer<vtkActor> sv4guiPurkinjeNetworkMeshMapper::createSphereActor(mitk::Point3D& point)
{
  if (true) { 
  //if (!m_sphereActor) { 
    vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
    double r = m_pickRadius / 100.0;
    sphere->SetRadius(r);
    sphere->SetCenter(point[0], point[1], point[2]);
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(sphere->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1,0,0);
    //actor->GetProperty()->SetRepresentationToWireframe();
    m_sphereActor = actor;
  }

  return m_sphereActor;
}

