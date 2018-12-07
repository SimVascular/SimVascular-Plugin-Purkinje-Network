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

#include <Python.h>

#include "sv4gui_PurkinjeNetworkEdit.h"
#include "ui_sv4gui_PurkinjeNetworkEdit.h"
#include "sv4gui_PurkinjeNetworkFolder.h"

#include "sv4gui_VtkPurkinjeNetworkSphereWidget.h"

#include "sv4gui_Model.h"
//[dp] #include "sv4gui_MeshFactory.h"
//[dp] #include "sv4gui_Mesh.h"
//[dp] #include "sv4gui_MitkMesh.h"
//[dp] #include "sv4gui_MitkMeshOperation.h"
//[dp] #include "sv4gui_MitkMeshIO.h"
#include "sv4gui_ProjectManager.h"

#include "sv4gui_ModelElementPolyData.h"
#include "sv4gui_ModelElementAnalytic.h"

#include "sv4gui_DataNodeOperation.h"

#include <berryIPreferencesService.h>
#include <berryIPreferences.h>
#include <berryPlatform.h>

#include <QmitkStdMultiWidgetEditor.h>
#include <mitkNodePredicateDataType.h>
#include <mitkUndoController.h>
#include <mitkSliceNavigationController.h>
#include <mitkProgressBar.h>
#include <mitkStatusBar.h>

#include <usModuleRegistry.h>

#include <vtkProperty.h>
#include <vtkXMLUnstructuredGridReader.h>

#include "sv4gui_PurkinjeNetworkIO.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>

#include <iostream>
using namespace std;

const QString sv4guiPurkinjeNetworkEdit::EXTENSION_ID = "org.sv.views.purkinjenetwork";
static const std::string PurkinjeNetwork_DIR_PATH = "Purkinje-Network";
static const std::string PurkinjeNetwork_NODE_NAME = "Purkinje-Network";

sv4guiPurkinjeNetworkEdit::sv4guiPurkinjeNetworkEdit() : ui(new Ui::sv4guiPurkinjeNetworkEdit)
{
    m_DataInteractor = NULL;
    m_ModelSelectFaceObserverTag = -1;
    m_SphereWidget = NULL;

    // [DaveP] The plugin does not currently references any module code so the module's shared 
    // library won't be loaded on Ubuntu (works ok on MacOS). This causes mitk to not find the 
    // state machine xml files which are stored in the module shared library. Force loading the 
    // module shared library be using a module class here. 
    //
    // This will not be a problem when I rewrite this code and place most of it under the module.
    //
    auto nio = new sv4guiPurkinjeNetworkIO();
}

sv4guiPurkinjeNetworkEdit::~sv4guiPurkinjeNetworkEdit()
{
    delete ui;
}

void sv4guiPurkinjeNetworkEdit::CreateQtPartControl( QWidget *parent )
{
    MITK_INFO << "[sv4guiPurkinjeNetworkEdit::CreateQtPartControl] ";
    m_Parent = parent;
    ui->setupUi(parent);

//    parent->setMaximumWidth(450);

    m_DisplayWidget=GetActiveStdMultiWidget();

    if (m_DisplayWidget==NULL) {
        parent->setEnabled(false);
        MITK_ERROR << "Plugin MeshEdit Init Error: No QmitkStdMultiWidget Available!";
        return;
    }

    // Initialize folder nodes.
    Initialize();

    // Define widget event handlers.
    connect(ui->buttonCreateNetwork, SIGNAL(clicked()), this, SLOT(CreateNetwork()));
    connect(ui->buttonLoadMesh, SIGNAL(clicked()), this, SLOT(LoadMesh()));
    connect(ui->meshCheckBox, SIGNAL(clicked(bool)), this, SLOT(displayMesh(bool)));
    connect(ui->networkCheckBox, SIGNAL(clicked(bool)), this, SLOT(displayNetwork(bool)));

    m_Interface= new sv4guiDataNodeOperationInterface();

    if (m_init){
        MITK_INFO << "[sv4guiPurkinjeNetworkEdit::CreateQtPartControl] Making network node";

        // Create mesh container, node and mapper.
        //
        m_MeshContainer = sv4guiPurkinjeNetworkMeshContainer::New();
        auto meshNode = mitk::DataNode::New();
        meshNode->SetData(m_MeshContainer);
        meshNode->SetVisibility(true);
        meshNode->SetName("mesh");
        m_MeshNode = meshNode;

        m_MeshMapper = sv4guiPurkinjeNetworkMeshMapper::New();
        m_MeshMapper->SetDataNode(meshNode);
        m_MeshMapper->m_box = false;
        meshNode->SetMapper(mitk::BaseRenderer::Standard3D, m_MeshMapper);

        // Create network container, node and mapper.
        //
        m_1DContainer = sv4guiPurkinjeNetwork1DContainer::New();
        auto node = mitk::DataNode::New();
        node->SetData(m_1DContainer);
        node->SetVisibility(true);
        node->SetName("1D network");
        m_1DNode = node;

        m_1DMapper = sv4guiPurkinjeNetwork1DMapper::New();
        m_1DMapper->SetDataNode(node);
        m_1DMapper->m_box = false;
        m_1DNode->SetMapper(mitk::BaseRenderer::Standard3D, m_1DMapper);

        // Create interactor to select mesh point.
        m_MeshInteractor = sv4guiPurkinjeNetworkInteractor::New();
        m_MeshInteractor->LoadStateMachine("meshInteraction.xml", us::ModuleRegistry::GetModule("sv4guiModulePurkinjeNetwork"));
        m_MeshInteractor->SetEventConfig("meshConfig.xml", us::ModuleRegistry::GetModule("sv4guiModulePurkinjeNetwork"));
        m_MeshInteractor->SetDataNode(meshNode);
        /*
        */

        // Set visibility of data node for Model.
        mitk::DataNode::Pointer model_folder_node = GetDataStorage()->GetNamedNode("Models");
        if (model_folder_node) {
            MITK_INFO << "[sv4guiPurkinjeNetworkEdit::CreateQtPartControl] Models folder not null";
            model_folder_node->SetVisibility(true);
        } else {
            MITK_INFO << "[sv4guiPurkinjeNetworkEdit::CreateQtPartControl] Models folder is null";
        }

        mitk::DataNode::Pointer images_folder_node = GetDataStorage()->GetNamedNode("Purkinje-Network");
        //mitk::DataNode::Pointer images_folder_node = GetDataStorage()->GetNamedNode("Images");
        if (images_folder_node) {
            MITK_INFO << "[sv4guiPurkinjeNetworkEdit::CreateQtPartControl] Images folder not null";
            images_folder_node->SetVisibility(false);
        }

        mitk::RenderingManager::GetInstance()->RequestUpdateAll();
        m_init = false;
    }

    // connect(ui->btnMeshInfo, SIGNAL(clicked()), this, SLOT(DisplayMeshInfo()) );
}

void sv4guiPurkinjeNetworkEdit::displayMesh(bool state)
{
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::displayMesh] state " << state;

  if (!state) {
      GetDataStorage()->Remove(m_MeshNode);
  } else {
      auto image_folder_node = GetDataStorage()->GetNamedNode("Purkinje-Network");
      //auto image_folder_node = GetDataStorage()->GetNamedNode("Images");
      // Add m_MeshNode to parent image_folder_node?
      if(image_folder_node) {
          MITK_INFO << "[sv4guiPurkinjeNetworkEdit::displayMesh] Add m_MeshNode to image_folder_node";
          GetDataStorage()->Add(m_MeshNode, image_folder_node);
      }
  }

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void sv4guiPurkinjeNetworkEdit::displayNetwork(bool state)
{
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::displayNetwork] state " << state;

  if (!state) {
      GetDataStorage()->Remove(m_1DNode);
  } else {
      auto image_folder_node = GetDataStorage()->GetNamedNode("Purkinje-Network");
      if(image_folder_node) {
          MITK_INFO << "[sv4guiPurkinjeNetworkEdit::displayMesh] Add m_MeshNode to image_folder_node";
          GetDataStorage()->Add(m_1DNode, image_folder_node);
      }
  }

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void sv4guiPurkinjeNetworkEdit::Initialize()
{
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::Initialize] Initialize";
  m_DataStorage = GetDataStorage();

  mitk::NodePredicateDataType::Pointer isProjFolder = mitk::NodePredicateDataType::New("sv4guiProjectFolder");
  mitk::DataNode::Pointer m_ProjFolderNode = m_DataStorage->GetNode (isProjFolder);

  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::Initialize] Making directory\n";
  mitk::DataNode::Pointer projFolderNode = getProjectNode();
  QDir dir;

  // Get the project path.
  std::string projPath = "";
  if (!projFolderNode){
      MITK_ERROR << "[sv4guiPurkinjeNetworkEdit::Initialize] Project folder node null, cannot get project path\n";
      dir = QDir("not_found");
  } else{
      projFolderNode->GetStringProperty("project path", projPath);
      QString QprojPath = QString(projPath.c_str());
      dir = QDir(QprojPath);
  }

  // Create directory if it doesn't exist.
  if (dir.exists()) {
      QString Qstore_dir = QString(PurkinjeNetwork_DIR_PATH.c_str());
      m_StoreDir = Qstore_dir;

      if (!dir.exists(Qstore_dir)) {
          MITK_INFO <<"Purkinje Network directory doesnt exist, creating\n";
          dir.mkdir(Qstore_dir);
      } else {
          MITK_INFO << "Purkinje Network directory already exists\n";
      }
  }

  // Create the data folder.
  if (projFolderNode) {
      mitk::DataNode::Pointer node = m_DataStorage->GetNamedNode(PurkinjeNetwork_NODE_NAME);

      if (!node) {
          MITK_INFO << "[PurkinjeNetwork] No Purkinje Network node, creating";
          QString folderName = QString(PurkinjeNetwork_NODE_NAME.c_str());
          node = svProj.CreateDataFolder<PurkinjeNetworkFolder>(m_DataStorage, folderName, projFolderNode);
      }
  }
}

mitk::DataNode::Pointer sv4guiPurkinjeNetworkEdit::getProjectNode()
{
  mitk::NodePredicateDataType::Pointer isProjFolder = mitk::NodePredicateDataType::New("sv4guiProjectFolder");
  //mitk::NodePredicateDataType::Pointer isProjFolder = mitk::NodePredicateDataType::New("svProjectFolder");
  mitk::DataNode::Pointer projFolderNode = m_DataStorage->GetNode (isProjFolder);
  return projFolderNode;
}

// Create a Purkinje Network. 
//
void sv4guiPurkinjeNetworkEdit::CreateNetwork()
{
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::CreateNetwork] ";

  // Get the project data node. 
  mitk::DataNode::Pointer projFolderNode = getProjectNode();
  std::string projPath = "";
  projFolderNode->GetStringProperty("project path", projPath);
  QString QprojPath = QString(projPath.c_str());
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::CreateNetwork] projPath " << projPath;

  // Set the input and output files.
  std::string dir = "/Users/parkerda/software/SimVascular/SimVascular-fork/SimCardio/Modules/PurkinjeNetwork/python/fractal-tree/";
  std::string infile = dir + "sphere.vtu";
  std::string outfile = projPath + "/" + m_StoreDir.toStdString() + "/sphere-network";

  //  Execute python script to compute fractal tree network. 
  std::string cmd;
  cmd += "import fractal_tree\n";
  cmd += "fractal_tree.run(";
  cmd += "infile='" + infile + "',";
  cmd += "outfile='" + outfile + "')\n";
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::CreateNetwork] cmd " << cmd;
  //PyRun_SimpleString(cmd.c_str());

  // Load VTK file containing network elements.
  std::string networkFileName = projPath + "/" + m_StoreDir.toStdString() + "/sphere-network.vtu";
  LoadNetwork(networkFileName);
}

// Read a Purkinje network file.
//
// The network is represented as an unstructured mesh of 1D elements.
// The elements are stored in a VTK .vtu format file.
//
sv4guiMesh* sv4guiPurkinjeNetworkEdit::LoadNetwork(std::string fileName)
{
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::LoadNetwork] ";
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::LoadNetwork] Read surface network " << fileName;

  m_SurfaceNetwork = new sv4guiMesh();
  //m_SurfaceNetwork->ReadSurfaceFile(fileName);
  m_SurfaceNetwork->ReadVolumeFile(fileName);
  m_1DContainer->SetSurfaceNetwork(m_SurfaceNetwork);
}

// Load a surface mesh in VTK vtp format.
//
void sv4guiPurkinjeNetworkEdit::LoadMesh()
{
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::LoadMesh] ";

  try {
      berry::IPreferencesService* prefService = berry::Platform::GetPreferencesService();
      berry::IPreferences::Pointer prefs;

      if (prefService) {   
          prefs = prefService->GetSystemPreferences()->Node("/General");
      } else {
          prefs = berry::IPreferences::Pointer(0);
      }

      QString lastFilePath = "";

      if (prefs.IsNotNull()) {
          lastFilePath = prefs->Get("LastFileOpenPath", "");
      }

      if (lastFilePath=="") {
          lastFilePath=QDir::homePath();
      }

      m_MeshFileName = QFileDialog::getOpenFileName(NULL, tr("Import Surfac Mesh (Choose File)"), lastFilePath,
        tr("Mesh file (*.vtp)"));

      m_MeshFileName = m_MeshFileName.trimmed();

      if (m_MeshFileName.isEmpty()) {
          return;
      }

      MITK_INFO << "[sv4guiPurkinjeNetworkEdit::LoadMesh] Read surface mesh " << m_MeshFileName.toStdString();

      m_SurfacMesh = new sv4guiMesh();
      m_SurfacMesh->ReadSurfaceFile(m_MeshFileName.toStdString());
      auto polyMesh = m_SurfacMesh->GetSurfaceMesh();

      auto points = polyMesh->GetPoints();
      auto numPoints = points->GetNumberOfPoints();
      MITK_INFO << "[sv4guiPurkinjeNetworkEdit::LoadMesh] Number of points " << numPoints; 

      auto polygons = polyMesh->GetPolys();
      auto numPolys = polygons->GetNumberOfCells();
      MITK_INFO << "[sv4guiPurkinjeNetworkEdit::LoadMesh] Number of triangles " << numPolys; 

      m_MeshContainer->SetSurfaceMesh(m_SurfacMesh);


      //polyMesh->Print(std::cout);
  }

  catch(...) {
      MITK_ERROR << "Error loading Purkinje surface mesh.!";
  }


}

void sv4guiPurkinjeNetworkEdit::Visible()
{
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::Visible] "; 
  OnSelectionChanged(GetDataManagerSelection());
}

void sv4guiPurkinjeNetworkEdit::OnSelectionChanged(std::vector<mitk::DataNode*> nodes )
{
}


void sv4guiPurkinjeNetworkEdit::NodeAdded(const mitk::DataNode* node)
{
}

void sv4guiPurkinjeNetworkEdit::Hidden()
{
//    ClearAll();
}

void sv4guiPurkinjeNetworkEdit::NodeChanged(const mitk::DataNode* node)
{
/*
    if (m_MeshNode==node) {
        ui->labelMeshName->setText(QString::fromStdString(m_MeshNode->GetName()));
    }
*/
}

void sv4guiPurkinjeNetworkEdit::NodeRemoved(const mitk::DataNode* node)
{
}


void sv4guiPurkinjeNetworkEdit::UpdateSphereData()
{
}





