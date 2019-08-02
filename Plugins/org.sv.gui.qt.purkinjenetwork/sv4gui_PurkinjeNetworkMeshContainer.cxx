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

#include "sv4gui_PurkinjeNetworkMeshContainer.h"
#include "math.h"

#include <berryIPreferencesService.h>
#include <berryIPreferences.h>
#include <berryPlatform.h>

//-------------
// Constructor
//-------------

sv4guiPurkinjeNetworkMeshContainer::sv4guiPurkinjeNetworkMeshContainer()
{
  MITK_INFO << "[sv4guiPurkinjeNetworkMeshContainer::sv4guiPurkinjeNetworkMeshContainer] ";
  //m_startSeeds = std::vector<std::vector<double>>();
  //m_endSeeds = std::vector<std::vector<std::vector<double>>>();
  hoverPoint.push_back(0.0);
  hoverPoint.push_back(0.0);
  hoverPoint.push_back(0.0);
  m_SelectedFaceIndex = -1;
  m_NewPickedPoint = false;
  m_FirstPointDefined = false;
  m_SecondPointDefined = false;
  for (int i = 0; i < 3; i++) {
    m_FirstPoint[i] = 0.0;
    m_SecondPoint[i] = 0.0;
  }
}

//------------------
// Copy Constructor
//------------------
sv4guiPurkinjeNetworkMeshContainer::sv4guiPurkinjeNetworkMeshContainer(const sv4guiPurkinjeNetworkMeshContainer& other) :BaseData(other)
{
}

sv4guiPurkinjeNetworkMeshContainer::~sv4guiPurkinjeNetworkMeshContainer(){

}

void sv4guiPurkinjeNetworkMeshContainer::SetSurfaceMesh(sv4guiMesh* surfaceMesh)
{
  m_SurfaceMesh = surfaceMesh;
}

sv4guiMesh* sv4guiPurkinjeNetworkMeshContainer::GetSurfaceMesh()
{
  return m_SurfaceMesh;
}

void sv4guiPurkinjeNetworkMeshContainer::SetModelFaces(std::vector<sv4guiModelElement::svFace*>& faces)
{
  for (const auto& face : faces) {
    m_ModelFaces.emplace_back(face);
  }
}

std::vector<sv4guiModelElement::svFace*> sv4guiPurkinjeNetworkMeshContainer::GetModelFaces()
{
  std::vector<sv4guiModelElement::svFace*> tmp(m_ModelFaces); 
  return tmp;
}

//------------------------
// Get/Set Network Points 
//------------------------
void sv4guiPurkinjeNetworkMeshContainer::SetNetworkPoints(const double first[3], const double second[3])
{
  for (int i = 0; i < 3; i++) {
    m_FirstPoint[i] = first[i];
    m_SecondPoint[i] = second[i]; 
  }
  m_FirstPointDefined = true;
  m_SecondPointDefined = true;
  m_NewNetworkPoints = true;
}

void sv4guiPurkinjeNetworkMeshContainer::SetFirstNetworkPoint(const double point[3])
{
  for (int i = 0; i < 3; i++) {
    m_FirstPoint[i] = point[i];
  }
  m_FirstPointDefined = true;
}

void sv4guiPurkinjeNetworkMeshContainer::SetSecondNetworkPoint(const double point[3])
{
  for (int i = 0; i < 3; i++) {
    m_SecondPoint[i] = point[i];
  }
  m_SecondPointDefined = true;
}


void sv4guiPurkinjeNetworkMeshContainer::SetNetworkPoints(const std::array<double,3>& first, const std::array<double,3>& second)
{
  m_FirstPoint = first;
  m_SecondPoint = second; 
}

void sv4guiPurkinjeNetworkMeshContainer::GetNetworkPoints(std::array<double,3>& first, std::array<double,3>& second)
{
  first = m_FirstPoint;
  second = m_SecondPoint; 
}

void sv4guiPurkinjeNetworkMeshContainer::ResetNetworkPoints()
{
  MITK_INFO << "######## ResetNetworkPoints ";
  m_FirstPointDefined = false;
  m_SecondPointDefined = false;
}

bool sv4guiPurkinjeNetworkMeshContainer::HaveNetworkPoints()
{
  auto msg = "[sv4guiPurkinjeNetworkMeshContainer::HaveNetworkPoints] ";
  MITK_INFO << msg << "m_FirstPointDefined: " << m_FirstPointDefined;
  MITK_INFO << msg << "m_SecondPointDefined: " << m_SecondPointDefined;
  return m_FirstPointDefined & m_SecondPointDefined;
}

//------------------------
// Get/Set SurfaceNetwork 
//------------------------
void sv4guiPurkinjeNetworkMeshContainer::SetSurfaceNetwork(sv4guiMesh* surfaceNetwork)
{
  m_SurfaceNetwork = surfaceNetwork;
}
sv4guiMesh* sv4guiPurkinjeNetworkMeshContainer::GetSurfaceNetwork()
{
  return m_SurfaceNetwork;
}

//------------------------------
// Get/Set SelectedFacePolyData 
//------------------------------
void sv4guiPurkinjeNetworkMeshContainer::SetSelectedFacePolyData(vtkSmartPointer<vtkPolyData> polyData)
{
  m_SelectedFacePolyData = polyData;
}
vtkSmartPointer<vtkPolyData> sv4guiPurkinjeNetworkMeshContainer::GetSelectedFacePolyData()
{
  return m_SelectedFacePolyData;
}

//--------------------------
// Get/Set SelectedFaceName
//--------------------------
void sv4guiPurkinjeNetworkMeshContainer::SetSelectedFaceName(const std::string name)
{
  m_SelectedFaceName = name;
}
std::string sv4guiPurkinjeNetworkMeshContainer::GetSelectedFaceName()
{
  return m_SelectedFaceName;
}

bool sv4guiPurkinjeNetworkMeshContainer::HaveSelectedFace()
{
  return m_SelectedFaceName != "";
}

//---------------------------
// Get/Set SelectedFaceIndex
//---------------------------
void sv4guiPurkinjeNetworkMeshContainer::SetSelectedFaceIndex(int index)
{
  m_SelectedFaceIndex = index;
}
int sv4guiPurkinjeNetworkMeshContainer::GetSelectedFaceIndex()
{
  return m_SelectedFaceIndex;
}

//------------------------
// Get/Set PickedPointdex
//-------------------------
mitk::Point3D sv4guiPurkinjeNetworkMeshContainer::GetPickedPoint() 
{ 
  return m_currentPickedPoint; 
}
void sv4guiPurkinjeNetworkMeshContainer::SetPickedPoint(mitk::Point3D& point) 
{ 
  m_currentPickedPoint = point; 
  m_NewPickedPoint = true;
}

bool sv4guiPurkinjeNetworkMeshContainer::HaveNewNetworkPoints(bool reset)
{
  auto tmp = m_NewNetworkPoints;
  if (reset) {
    m_NewNetworkPoints = false;
  }
  return tmp; 
}


//--------------------
// HaveNewPickedPoint 
//--------------------

bool sv4guiPurkinjeNetworkMeshContainer::HaveNewPickedPoint(bool reset)
{
  auto tmp = m_NewPickedPoint;
  if (reset) {
    m_NewPickedPoint = false;
  }
  return tmp; 
}


