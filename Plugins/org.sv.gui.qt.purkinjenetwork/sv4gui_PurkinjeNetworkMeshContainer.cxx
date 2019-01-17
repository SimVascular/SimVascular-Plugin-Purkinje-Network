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

void sv4guiPurkinjeNetworkMeshContainer::SetSurfaceNetwork(sv4guiMesh* surfaceNetwork)
{
  m_SurfaceNetwork = surfaceNetwork;
}

sv4guiMesh* sv4guiPurkinjeNetworkMeshContainer::GetSurfaceNetwork()
{
  return m_SurfaceNetwork;
}

void sv4guiPurkinjeNetworkMeshContainer::SetSelectedFaceIndex(int index)
{
  m_SelectedFaceIndex = index;
}

int sv4guiPurkinjeNetworkMeshContainer::GetSelectedFaceIndex()
{
  return m_SelectedFaceIndex;
}

mitk::Point3D sv4guiPurkinjeNetworkMeshContainer::GetPickedPoint() 
{ 
  return m_currentPickedPoint; 
}

void sv4guiPurkinjeNetworkMeshContainer::SetPickedPoint(mitk::Point3D& point) 
{ 
  m_currentPickedPoint = point; 
  m_NewPickedPoint = true;
}

bool sv4guiPurkinjeNetworkMeshContainer::HaveNewPickedPoint(bool reset)
{
  auto tmp = m_NewPickedPoint;
  if (reset) {
    m_NewPickedPoint = false;
  }
  return tmp; 
}


