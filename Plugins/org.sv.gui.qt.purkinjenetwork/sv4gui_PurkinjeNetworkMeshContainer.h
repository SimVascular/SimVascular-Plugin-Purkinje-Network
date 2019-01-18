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

#ifndef SV4GUI_PURKINJENETWORK_MESH_CONTAINER_H
#define SV4GUI_PURKINJENETWORK_MESH_CONTAINER_H

#include <iostream>
#include <vector>
#include "mitkBaseData.h"
#include "sv4gui_Mesh.h"
#include <itkEventObject.h>

class sv4guiPurkinjeNetworkMeshContainer : public mitk::BaseData {

  public:

    mitkClassMacro(sv4guiPurkinjeNetworkMeshContainer, mitk::BaseData);
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)

    //virtual methods, that need to be implemented due to mitk::BaseData inheriting
    //from itk::DataObject
    //however if we dont intend to use this object with an itk filter we can leave them
    //empty
    virtual void UpdateOutputInformation() {};
    virtual void SetRequestedRegionToLargestPossibleRegion() {};
    virtual bool RequestedRegionIsOutsideOfTheBufferedRegion() { return false;};
    virtual bool VerifyRequestedRegion() { return true;};
    virtual void SetRequestedRegion(const itk::DataObject *data) {};

    std::vector<double> hoverPoint = std::vector<double>();

    void SetSurfaceMesh(sv4guiMesh* surfaceMesh);
    sv4guiMesh* GetSurfaceMesh();

    void SetModelFaces(std::vector<sv4guiModelElement::svFace*>& faces);
    std::vector<sv4guiModelElement::svFace*> GetModelFaces();

    void SetSurfaceNetwork(sv4guiMesh* surfaceNetwork);
    sv4guiMesh* GetSurfaceNetwork();

    mitk::Point3D GetPickedPoint();
    void SetPickedPoint(mitk::Point3D& point);
    bool HaveNewPickedPoint(bool reset = false);

    void SetModelElement(sv4guiModelElement* modelElement) { m_ModelElement = modelElement; }
    sv4guiModelElement* GetModelElement() { return m_ModelElement; }

    void SetSelectedFaceIndex(int index);
    int GetSelectedFaceIndex();

    void SetSelectedFaceName(const std::string name);
    std::string GetSelectedFaceName();

    void SetNetworkPoints(const double first[3], const double second[3]);
    void GetNetworkPoints(double first[3], double second[3]);

protected:

  mitkCloneMacro(Self);
  sv4guiPurkinjeNetworkMeshContainer();
  sv4guiPurkinjeNetworkMeshContainer(const sv4guiPurkinjeNetworkMeshContainer& other);
  virtual ~sv4guiPurkinjeNetworkMeshContainer();

private:

  //std::vector< std::vector<double> > m_startSeeds;
  //std::vector< std::vector< std::vector<double> > > m_endSeeds;
  sv4guiMesh* m_SurfaceMesh;
  sv4guiMesh* m_SurfaceNetwork;
  mitk::Point3D m_currentPickedPoint;
  std::vector<sv4guiModelElement::svFace*> m_ModelFaces;
  sv4guiModelElement* m_ModelElement;
  int m_SelectedFaceIndex;
  std::string  m_SelectedFaceName;
  bool m_NewPickedPoint;

  bool m_FirstPointSelected;
  double m_FirstPoint[3];
  double m_SecondPoint[3];


};

itkEventMacro( sv4guiPurkinjeNetworkMeshEvent, itk::AnyEvent);
itkEventMacro( sv4guiPurkinjeNetworkMeshSetEvent, sv4guiPurkinjeNetworkMeshEvent);

#endif //SV4GUI_PURKINJENETWORK_MESH_CONTAINER_H
