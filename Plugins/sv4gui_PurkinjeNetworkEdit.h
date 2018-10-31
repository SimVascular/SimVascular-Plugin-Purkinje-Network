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

#ifndef sv4guiPurkinjeNetworkEdit_H
#define sv4guiPurkinjeNetworkEdit_H

#include <string>
#include <QString>
#include <QDir>
#include <sv4gui_QmitkFunctionality.h>

#include "sv4gui_DataNodeOperationInterface.h"
#include "sv4gui_DataNodeOperation.h"
#include "sv4gui_ProjectManager.h"

#include "sv4gui_PurkinjeNetworkUtils.h"
#include <sv4gui_PurkinjeSeedContainer.h>
#include <sv4gui_PurkinjeSeedInteractor.h>
#include <sv4gui_PurkinjeSeedMapper.h>

#include <mitkImage.h>

namespace Ui {
class sv4guiPurkinjeNetworkEdit;
}

class sv4guiPurkinjeNetworkEdit : public sv4guiQmitkFunctionality
{
    Q_OBJECT

public:

    sv4guiPurkinjeNetworkEdit();

    virtual ~sv4guiPurkinjeNetworkEdit();

    virtual void CreateQtPartControl(QWidget *parent) override;

    static const QString EXTENSION_ID;

    void UpdateImageList();

    std::string getImageName(int imageIndex);

    mitk::Image::Pointer getImage(std::string image_name);

    virtual void OnSelectionChanged(std::vector<mitk::DataNode*> nodes) override;

    sv4guiPurkinjeNetworkUtils::itkImPoint getItkImage(int index);

    void addNode(mitk::DataNode::Pointer child_node, mitk::DataNode::Pointer parent_node);

    void storeImage(sv4guiPurkinjeNetworkUtils::itkImPoint image);

    void storePolyData(vtkSmartPointer<vtkPolyData> vtkPd);

public slots:

    //display
    void seedSize();
    void displayGuide(bool state);
    void displaySeeds(bool state);

    //displaay buttons
    void imageEditingTabSelected();
    void filteringTabSelected();
    void segmentationTabSelected();
    void pipelinesTabSelected();

    //run buttons
    void runFullCollidingFronts();

    void runThreshold();

    void runBinaryThreshold();

    void runCollidingFronts();

    void runGradientMagnitude();

    void runEditImage();

    void runCropImage();

    void runResampleImage();

    void runZeroLevel();

    void runSmoothing();

    void runAnisotropic();

    void runIsovalue();

    void runGeodesicLevelSet();

public:

protected:

  Ui::sv4guiPurkinjeNetworkEdit *ui;
  QWidget *m_parent;
  QmitkStdMultiWidget* m_DisplayWidget;

  mitk::DataNode::Pointer m_ProjectFolderNode;
  mitk::DataStorage::Pointer m_DataStorage;
  mitk::DataNode::Pointer m_SelecteNode;
  int m_TimeStep;
  QString m_StoreDir;

  sv4guiDataNodeOperationInterface* m_Interface;

  std::string m_selectedAlgorithm;


  sv4guiPurkinjeSeedContainer::Pointer m_SeedContainer;

  bool m_init = true;

  sv4guiPurkinjeSeedInteractor::Pointer m_SeedInteractor;

  sv4guiPurkinjeSeedMapper::Pointer m_SeedMapper;

  sv4guiPurkinjeNetworkUtils::itkImPoint CombinedCollidingFronts(
    sv4guiPurkinjeNetworkUtils::itkImPoint, double lower, double upper);

  mitk::DataNode::Pointer m_SeedNode;

private:

  void Initialize();
  void makeDir();
  QDir getDir();
  mitk::DataNode::Pointer getProjectNode();
  void createDataFolder();
  sv4guiProjectManager svProj;

};

#endif // sv4guiPurkinjeNetworkEdit_H
