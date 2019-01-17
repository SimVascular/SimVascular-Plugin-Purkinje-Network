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

#include "sv_polydatasolid_utils.h"

sv4guiPurkinjeNetworkMeshMapper::sv4guiPurkinjeNetworkMeshMapper()
{
  //m_SphereActor = nullptr;
}

sv4guiPurkinjeNetworkMeshMapper::~sv4guiPurkinjeNetworkMeshMapper()
{
}


//-------------------------
// GenerateDataForRenderer
//-------------------------
// Generate the data needed for rendering into renderer.
//
void sv4guiPurkinjeNetworkMeshMapper::GenerateDataForRenderer(mitk::BaseRenderer* renderer)
{
  std::string msgPrefix = "[sv4guiPurkinjeNetworkMeshMapper::GenerateDataForRenderer] ";
  //MITK_INFO << msgPrefix; 

  //make ls propassembly
  mitk::DataNode* node = GetDataNode();
  if (node == NULL) {
    //MITK_INFO << "[sv4guiPurkinjeNetworkMeshMapper::GenerateDataForRenderer] Data node is null";
    return;
  }

  float edgeColor[3]= { 0.0f, 1.0f, 1.0f };
  node->GetColor(edgeColor, renderer, "edge color");

  LocalStorage* local_storage = m_LSH.GetLocalStorage(renderer);

  bool visible = true;
  GetDataNode()->GetVisibility(visible, renderer, "visible");
  if (!visible) {
    local_storage->m_PropAssembly->VisibilityOff();
    return;
  }

  sv4guiPurkinjeNetworkMeshContainer* meshContainer = 
    static_cast< sv4guiPurkinjeNetworkMeshContainer* >( node->GetData() );
  if (meshContainer == NULL) {
    local_storage->m_PropAssembly->VisibilityOff();
    return;
  }

  // [DaveP] Do we need to remove?
  // local_storage->m_PropAssembly->GetParts()->RemoveAllItems();

  //local_storage->m_PropAssembly->VisibilityOn();

  // Show surface mesh.
  //
  auto surfaceMesh = meshContainer->GetSurfaceMesh();
  auto modelFaces = meshContainer->GetModelFaces();
  sv4guiModelElement* modelElement = meshContainer->GetModelElement();
  int selectedFaceIndex = meshContainer->GetSelectedFaceIndex();

  if (surfaceMesh != NULL && m_newMesh) {
    //MITK_INFO << msgPrefix << "Have surface mesh data";
    auto polyMesh = surfaceMesh->GetSurfaceMesh();
/*
    vtkSmartPointer<vtkPolyDataMapper> meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    meshMapper->SetInputData(polyMesh);
    meshMapper->ScalarVisibilityOff();  // Turn off mapping of scalar data, otherwise can't set color.
    vtkSmartPointer<vtkActor> polyMeshActor = vtkSmartPointer<vtkActor>::New();
    polyMeshActor->SetMapper(meshMapper);
    polyMeshActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
    polyMeshActor->GetProperty()->SetEdgeColor(0, 0, 0);
    polyMeshActor->GetProperty()->EdgeVisibilityOn();
    local_storage->m_PropAssembly->AddPart(polyMeshActor);
*/

    double bounds[6];
    polyMesh->GetBounds(bounds);
    double xmin = bounds[0]; double xmax = bounds[1]; double ymin = bounds[2];
    double ymax = bounds[3]; double zmin = bounds[4]; double zmax = bounds[5];
    double dx = xmax - xmin; double dy = ymax - ymin; double dz = zmax - zmin;
    //MITK_INFO << "[sv4guiPurkinjeNetworkMeshMapper::GenerateDataForRenderer] dx "<< dx << "  dy "<<dy<< "  dz "<< dz;

    m_pickRadius = (dx > dy) ? dx : dy;
    m_pickRadius = (m_pickRadius > dz) ? m_pickRadius : dz;
    m_newMesh = false;

    // Show mesh faces.
    //
    vtkPolyData* geom = polyMesh.GetPointer();
    if (geom == nullptr) {
      MITK_WARN << msgPrefix << "No model faces associated with mesh.";
      return;
    }

    //MITK_INFO << msgPrefix << "Number of model faces " << modelFaces.size();
    for (const auto& face : modelFaces) {
      //MITK_INFO << msgPrefix << "Face id " << face->id << " name '" << face->name << "'";
      int faceID = modelElement->GetFaceIdentifierFromInnerSolid(face->id);
      vtkSmartPointer<vtkPolyData> facePolyData = vtkSmartPointer<vtkPolyData>::New();
      PlyDtaUtils_GetFacePolyData(geom, &faceID, facePolyData);
      //MITK_INFO << msgPrefix << "   Num tri  " << facePolyData->GetNumberOfCells();
      //MITK_INFO << msgPrefix << "   Face ptr " << facePolyData;

      vtkSmartPointer<vtkOpenGLPolyDataMapper> faceMapper = vtkSmartPointer<vtkOpenGLPolyDataMapper>::New();
      faceMapper->SetInputData(facePolyData);
      faceMapper->ScalarVisibilityOff();  // Turn off mapping of scalar data, otherwise can't set color.

      vtkSmartPointer<vtkActor> faceActor= vtkSmartPointer<vtkActor>::New();
      faceActor->SetMapper(faceMapper);
      faceActor->GetProperty()->SetEdgeColor(0.0, 0.0, 0.0);
      faceActor->GetProperty()->SetEdgeVisibility(1);
      faceActor->GetProperty()->SetLineWidth(0.5);

      local_storage->m_PropAssembly->AddPart(faceActor);
      local_storage->m_FaceActors.push_back(faceActor);
      local_storage->m_FacePolyData.push_back(facePolyData);
    }

  } else {
    std::vector<vtkSmartPointer<vtkActor>> faceActors = GetFaceActors(renderer);
    for (int i = 0; i < faceActors.size(); ++i) { 
      if (selectedFaceIndex == i) { 
        //MITK_INFO << msgPrefix << "Selected face index " << selectedFaceIndex; 
        faceActors[i]->GetProperty()->SetColor(1.0, 1.0, 0.0);
      } else {
        faceActors[i]->GetProperty()->SetColor(1.0, 1.0, 1.0);
      }
    }
  }

  // If a point has been picked on the mesh then show it and 
  // determine the face / vertex that it is closest to.
  if (meshContainer->HaveNewPickedPoint(true)) {
    local_storage->m_PropAssembly->GetParts()->RemoveItem(m_SphereActor);
    auto point = meshContainer->GetPickedPoint();

    // Find closest face.
    this->findClosestFace(meshContainer, point);

    m_SphereActor = createSphereActor(point);
    local_storage->m_PropAssembly->AddPart(m_SphereActor);

  }

  local_storage->m_PropAssembly->VisibilityOn();
}

std::vector<vtkSmartPointer<vtkActor>> sv4guiPurkinjeNetworkMeshMapper::GetFaceActors(mitk::BaseRenderer* renderer)
{
  LocalStorage *ls = m_LSH.GetLocalStorage(renderer);
  return ls->m_FaceActors;
}

std::vector<vtkSmartPointer<vtkPolyData>> sv4guiPurkinjeNetworkMeshMapper::GetFacePolyData(mitk::BaseRenderer* renderer)
{
  LocalStorage *ls = m_LSH.GetLocalStorage(renderer);
  return ls->m_FacePolyData;
}

void sv4guiPurkinjeNetworkMeshMapper::ApplyAllProperties(mitk::DataNode *node, mitk::BaseRenderer* renderer, 
    vtkSmartPointer<vtkOpenGLPolyDataMapper> mapper, vtkSmartPointer<vtkActor> actor, 
    mitk::LocalStorageHandler<LocalStorage>* handler, bool clipping)
{
  //ApplyMitkPropertiesToVtkProperty( node, actor->GetProperty(), renderer );

}

//-----------------
// findClosestFace
//-----------------
// Find the face in the mesh closest to the picked point.
//
void sv4guiPurkinjeNetworkMeshMapper::findClosestFace(sv4guiPurkinjeNetworkMeshContainer* mesh, 
    mitk::Point3D& point)
{
  std::string msgPrefix = "[sv4guiPurkinjeNetworkMeshMapper::findClosestFace] ";
  //MITK_INFO <<  msgPrefix;
  auto surfaceMesh = mesh->GetSurfaceMesh();
  if (surfaceMesh == NULL) {
    return;
  }

  //MITK_INFO << msgPrefix << "Query point: " << point[0] << " " << point[1] << " " << point[2];
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
  MITK_INFO << msgPrefix + "CellId: " << cellId;
  MITK_INFO << msgPrefix + "subId: " << subId;
  */

  vtkCell* cell = polyMesh->GetCell(cellId);
  if (cell == nullptr) {
    return;
  }

  // Get the points on the face.
  vtkTriangle* triangle = dynamic_cast<vtkTriangle*>(cell);
  double pt[3], p0[3], p1[3], p2[3], minPt[3];
  triangle->GetPoints()->GetPoint(0, p0);
  triangle->GetPoints()->GetPoint(1, p1);
  triangle->GetPoints()->GetPoint(2, p2);
  vtkSmartPointer<vtkPoints> points = triangle->GetPoints();

  // Find the closest face vertex.
  double d, minDist = 1e9;
  int minIndex = -1;
  for (int i = 0; i < 3; i++) {
    triangle->GetPoints()->GetPoint(i, pt);
    d = (pt[0]-point[0])*(pt[0]-point[0]) + (pt[1]-point[1])*(pt[1]-point[1]) + (pt[2]-point[2])*(pt[2]-point[2]);
    if (d < minDist) {
      minDist = d;
      minIndex = i;
    }
  }

  // Set the picked point to the closest face vertex.
  triangle->GetPoints()->GetPoint(minIndex, minPt);
  for (int i = 0; i < 3; i++) {
    point[i] = minPt[i];
  }

  // Calculate the second point as the midpoint of the
  // edge oposite the closest face vertex.
  for (int i = 0; i < 3; i++) {
    m_point1[i] = point[i];
    if (minIndex == 0) {
      m_point2[i] = (p1[i] + p2[i]) / 2.0;
    } else if (minIndex == 1) {
      m_point2[i] = (p0[i] + p2[i]) / 2.0;
    } else {
      m_point2[i] = (p0[i] + p1[i]) / 2.0;
    }
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
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

  if (true) { 
  //if (!m_sphereActor) { 
    vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
    double r = m_pickRadius / 100.0;
    sphere->SetRadius(r);
    sphere->SetCenter(point[0], point[1], point[2]);
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(sphere->GetOutputPort());

    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1,0,0);
    //actor->GetProperty()->SetRepresentationToWireframe();
  }

  return actor;
}

