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

#include "sv4gui_MitkMesh.h"
#include "sv_polydatasolid_utils.h"

#include "sv4gui_Model.h"
#include "sv4gui_ModelElement.h"
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
    m_SphereWidget = NULL;
    m_PurkinjeNetworkNode = NULL;
    m_MeshSelectFaceObserverTag = -1;
    m_MeshSelectStartPointObserverTag = -1;

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
    connect(ui->buttonLoadMesh, SIGNAL(clicked()), this, SLOT(LoadMesh()));
    connect(ui->selectMeshComboBox, SIGNAL(clicked()), this, SLOT(SelectMesh()));
    connect(ui->buttonCreateNetwork, SIGNAL(clicked()), this, SLOT(CreateNetwork()));
    connect(ui->meshCheckBox, SIGNAL(clicked(bool)), this, SLOT(showMesh(bool)));
    connect(ui->networkCheckBox, SIGNAL(clicked(bool)), this, SLOT(showNetwork(bool)));
    connect(ui->meshSurfaceNameLineEdit, SIGNAL(returnPressed()), this, SLOT(MeshSurfaceName()));

    connect(ui->startPointXLineEdit, SIGNAL(returnPressed()), this, SLOT(MeshSurfaceStartPoint()));
    connect(ui->startPointYLineEdit, SIGNAL(returnPressed()), this, SLOT(MeshSurfaceStartPoint()));
    connect(ui->startPointZLineEdit, SIGNAL(returnPressed()), this, SLOT(MeshSurfaceStartPoint()));

    m_Interface = new sv4guiDataNodeOperationInterface();

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

        // Get the surface meshes on which to generate networks.
        SetMeshInformation();

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

        // Set visibility of Model data node.
        mitk::DataNode::Pointer model_folder_node = GetDataStorage()->GetNamedNode("Models");
        if (model_folder_node) {
            MITK_INFO << "[sv4guiPurkinjeNetworkEdit::CreateQtPartControl] Models folder not null";
            model_folder_node->SetVisibility(false);
        } else {
            MITK_INFO << "[sv4guiPurkinjeNetworkEdit::CreateQtPartControl] Models folder is null";
        }

        // Set visibility for Meshes data node.
        mitk::DataNode::Pointer mesh_folder_node = GetDataStorage()->GetNamedNode("Meshes");
        if (mesh_folder_node) {
            MITK_INFO << "[sv4guiPurkinjeNetworkEdit::CreateQtPartControl] Meshes folder not null";
            mesh_folder_node->SetVisibility(false);
        } else {
            MITK_INFO << "[sv4guiPurkinjeNetworkEdit::CreateQtPartControl] Meshes folder is null";
        }

        mitk::RenderingManager::GetInstance()->RequestUpdateAll();
        m_init = false;
    }

    AddObservers();
}

//-----------------
// MeshSurfaceName
//-----------------
// Process mesh surface name type-in.
//
void sv4guiPurkinjeNetworkEdit::MeshSurfaceName()
{
  std::string msgPrefix = "[sv4guiPurkinjeNetworkEdit::MeshSurfaceName] "; 
  MITK_INFO << msgPrefix; 
  std::string meshName = ui->meshSurfaceNameLineEdit->text().trimmed().toStdString();

  if (meshName == "") {
    QMessageBox::warning(NULL,"Mesh surface name is empty", "Please give a mesh surface name.");
    return;
  }
}

//-----------------------
// MeshSurfaceStartPoint 
//-----------------------
// Process mesh surface start point type-in.
//
void sv4guiPurkinjeNetworkEdit::MeshSurfaceStartPoint()
{
  std::string x = ui->startPointXLineEdit->text().trimmed().toStdString();
  std::string y = ui->startPointYLineEdit->text().trimmed().toStdString();
  std::string z = ui->startPointZLineEdit->text().trimmed().toStdString();
}

//----------
// showMesh
//----------
// Process ths show mesh check box select.
//
void sv4guiPurkinjeNetworkEdit::showMesh(bool state)
{
  if (!state) {
      GetDataStorage()->Remove(m_MeshNode);
  } else {
      auto networkNode = GetDataStorage()->GetNamedNode("Purkinje-Network");
      if (networkNode) {
          GetDataStorage()->Add(m_MeshNode, networkNode);
      }
  }

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void sv4guiPurkinjeNetworkEdit::showNetwork(bool state)
{
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::showNetwork] state " << state;

  if (!state) {
      GetDataStorage()->Remove(m_1DNode);
  } else {
      auto image_folder_node = GetDataStorage()->GetNamedNode("Purkinje-Network");
      if(image_folder_node) {
          MITK_INFO << "[sv4guiPurkinjeNetworkEdit::showMesh] Add m_MeshNode to image_folder_node";
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
  mitk::DataNode::Pointer m_ProjFolderNode = m_DataStorage->GetNode(isProjFolder);

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
      m_PurkinjeNetworkNode = node;
  }

  // SetMeshInformation();
}


// --------------------
//  SetMeshInformation
// --------------------
// Get model and mesh data neeeded to set the model surface meshes
// on which to create purkinje networks.

void sv4guiPurkinjeNetworkEdit::SetMeshInformation()
{
  std::string msgPrefix = "[sv4guiPurkinjeNetworkEdit::SetMeshInformation] ";
  MITK_INFO << msgPrefix; 
  m_ModelFolderNode = GetModelFolderDataNode();
  m_MeshFolderNode = GetMeshFolderDataNode();

  // Check that a model node exists (will always be true?).
  auto modelNodes = m_DataStorage->GetDerivations(m_ModelFolderNode,mitk::NodePredicateDataType::New("sv4guiModel"));
  if (modelNodes->size() == 0) {
    MITK_WARN << msgPrefix << "Model data node not found!";
    return; 
  }
  auto modelName = modelNodes->GetElement(0)->GetName();
  MITK_INFO << msgPrefix << "Model name '" << modelName << "'";

  // Check that a model has been created.
  //
  auto modelNode = m_DataStorage->GetNamedDerivedNode(modelName.c_str(),m_ModelFolderNode);
  auto model = dynamic_cast<sv4guiModel*>(modelNode->GetData());
  if (model == nullptr) {
    MITK_WARN << msgPrefix << "No model has been created!";
    return;
  }
  modelNode->SetVisibility(false);

  // Check that a mesh node exists (will always be true?).
  auto meshNodes = m_DataStorage->GetDerivations(m_MeshFolderNode,mitk::NodePredicateDataType::New("sv4guiMitkMesh"));
  if (meshNodes->size() == 0) {
    MITK_WARN << msgPrefix << "Mesh data node not found!";
    return;
  }
  auto meshName = meshNodes->GetElement(0)->GetName();
  MITK_INFO << msgPrefix << "Mesh name '" << meshName << "'";

  // Check that a mesh has been created.
  //
  auto meshNode = m_DataStorage->GetNamedDerivedNode(meshName.c_str(),m_MeshFolderNode);
  auto mitkMesh = dynamic_cast<sv4guiMitkMesh*>(meshNode->GetData());
  if (mitkMesh == nullptr) {
    MITK_WARN << msgPrefix << "No mesh has been created!";
    return;
  }
  meshNode->SetVisibility(false);

  // Get the mesh and its surface from the Meshes data node.
  //
  sv4guiMesh* mesh = GetDataNodeMesh();
  if (mesh == nullptr) {
    MITK_WARN << msgPrefix << "No mesh found!";
    return;
  }

  m_MeshContainer->SetSurfaceMesh(mesh);
  m_SurfaceNetworkMesh = mesh;
  vtkSmartPointer<vtkPolyData> meshSurface = mesh->GetSurfaceMesh();
  if (meshSurface.GetPointer() == nullptr) {
    MITK_WARN << msgPrefix << "No mesh surface found!";
    return;
  }

  vtkPolyData* geom = meshSurface.GetPointer();
  if (geom == nullptr) { 
    MITK_WARN << msgPrefix << "No model faces associated with mesh.";
    return;
  }

  // Show the mesh in the graphics window.
  showMesh(true);
  ui->meshCheckBox->setChecked(1);
  ui->meshCheckBox->isChecked();

  // Get the surface mesh associated with each model face.
  //
  sv4guiModelElement* modelElement = model->GetModelElement();
  m_MeshContainer->SetModelElement(modelElement);

  if (modelElement != nullptr) {
    std::vector<sv4guiModelElement::svFace*> faces = modelElement->GetFaces() ;
    MITK_INFO << msgPrefix << "Number of model faces " << faces.size();
    for (const auto& face : faces) { 
      MITK_INFO << msgPrefix << "Face id " << face->id << " name '" << face->name << "'";
      int faceID = modelElement->GetFaceIdentifierFromInnerSolid(face->id);
      vtkSmartPointer<vtkPolyData> facePolyData = vtkSmartPointer<vtkPolyData>::New();
      PlyDtaUtils_GetFacePolyData(geom, &faceID, facePolyData);
      MITK_INFO << msgPrefix << "   Num tri  " << facePolyData->GetNumberOfCells();
      MITK_INFO << msgPrefix << "   Face ptr " << facePolyData;
    }

    m_MeshContainer->SetModelFaces(faces);
  }

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

// -----------------------
//  GetMeshFolderDataNode 
// -----------------------
// Get the pointer to the mesh folder data node.

mitk::DataNode::Pointer sv4guiPurkinjeNetworkEdit::GetMeshFolderDataNode()
{
  auto projFolderNode = getProjectNode();
  mitk::DataNode::Pointer meshFolderNode;
  auto meshFolderNodes = m_DataStorage->GetDerivations(projFolderNode, mitk::NodePredicateDataType::New("sv4guiMeshFolder"));
  if (meshFolderNodes->size() > 0) {
    meshFolderNode = meshFolderNodes->GetElement(0);
  }
  return meshFolderNode;
}

// -----------------------
//  GetModelFolderDataNode 
// -----------------------
// Get the pointer to the model folder data node.

mitk::DataNode::Pointer sv4guiPurkinjeNetworkEdit::GetModelFolderDataNode()
{
  auto projFolderNode = getProjectNode();
  mitk::DataNode::Pointer modelFolderNode;
  auto modelFolderNodes = m_DataStorage->GetDerivations(projFolderNode, mitk::NodePredicateDataType::New("sv4guiModelFolder"));
  if (modelFolderNodes->size() > 0) {
    modelFolderNode = modelFolderNodes->GetElement(0);
  }

  return modelFolderNode;
}

// -----------------
//  GetDataNodeMesh
// -----------------
//
// Get the mesh from the Meshes data node.

sv4guiMesh* sv4guiPurkinjeNetworkEdit::GetDataNodeMesh()
{
  auto projFolderNode = getProjectNode();
  auto meshFolderNodes = m_DataStorage->GetDerivations(projFolderNode, mitk::NodePredicateDataType::New("sv4guiMeshFolder"));
  sv4guiMesh* mesh = nullptr;

  if (meshFolderNodes->size() > 0) {
    m_MeshFolderNode = meshFolderNodes->GetElement(0);
    auto meshNodes = GetDataStorage()->GetDerivations(m_MeshFolderNode);
    for (auto it = meshNodes->begin(); it != meshNodes->end(); ++it) {
      sv4guiMitkMesh* mitkMesh = dynamic_cast<sv4guiMitkMesh*>((*it)->GetData());
      if (mitkMesh) {
        mesh = mitkMesh->GetMesh();
        break; 
      }
    }
  }

  return mesh;
}

mitk::DataNode::Pointer sv4guiPurkinjeNetworkEdit::getProjectNode()
{
  mitk::NodePredicateDataType::Pointer isProjFolder = mitk::NodePredicateDataType::New("sv4guiProjectFolder");
  //mitk::NodePredicateDataType::Pointer isProjFolder = mitk::NodePredicateDataType::New("svProjectFolder");
  mitk::DataNode::Pointer projFolderNode = m_DataStorage->GetNode (isProjFolder);
  return projFolderNode;
}

//---------------
// CreateNetwork 
//---------------
// Create a Purkinje Network. 
//
void sv4guiPurkinjeNetworkEdit::CreateNetwork()
{
  std::string msgPrefix = "[sv4guiPurkinjeNetworkEdit::CreateNetwork] ";
  MITK_INFO << msgPrefix; 

  // Check that a face is selected.
  if (!m_MeshContainer->HaveSelectedFace()) {
    MITK_ERROR << msgPrefix << "No mesh face seleced.";
    return;
  }
  auto faceName = m_MeshContainer->GetSelectedFaceName();
  MITK_INFO << msgPrefix << "Face name " << faceName;

  // Get the network start point and second point defining 
  // the direction of the initial segment.
  if (!m_MeshContainer->HaveNetworkPoints()) {
    MITK_ERROR << msgPrefix << "No start point seleced.";
    return;
  }
  std::array<double,3> firstPoint, secondPoint;
  m_MeshContainer->GetNetworkPoints(firstPoint, secondPoint);
  MITK_INFO << msgPrefix << " First point" << firstPoint[0] << " " << firstPoint[1] << "  " << firstPoint[2];

  // Get the project data node. 
  mitk::DataNode::Pointer projFolderNode = getProjectNode();
  std::string projPath = "";
  projFolderNode->GetStringProperty("project path", projPath);
  QString QprojPath = QString(projPath.c_str());
  MITK_INFO << msgPrefix << "projPath " << projPath;

  // Create a Purkinje Network model.
  sv4guiPurkinjeNetworkModel pnetModel(faceName, firstPoint, secondPoint);
  SetModelParameters(pnetModel);
  SetModelMesh(pnetModel);
  auto outputPath = projPath + "/" + m_StoreDir.toStdString() + "/";
  pnetModel.GenerateNetwork(outputPath);

  // Read the generated network (1D elements).
  LoadNetwork(pnetModel.networkFileName);

}

//--------------------
// SetModelParameters
//---------------------
void sv4guiPurkinjeNetworkEdit::SetModelParameters(sv4guiPurkinjeNetworkModel& model)
{
  model.numBranchGenerations = ui->numBranchGenSpinBox->value();
  model.avgBranchLength = ui->avgBranchLengthSpinBox->value();
  model.avgBranchAngles = ui->avgBranchAnglesSpinBox->value();
  model.repulsiveParameter = ui->repulsiveParameterSpinBox->value();
  model.repulsiveParameter = ui->repulsiveParameterSpinBox->value();
  model.branchSegLength = ui->branchSegLengthSpinBox->value();
}

//--------------
// SetModelMesh 
//--------------
void sv4guiPurkinjeNetworkEdit::SetModelMesh(sv4guiPurkinjeNetworkModel& model)
{
  model.meshPolyData = m_MeshContainer->GetSelectedFacePolyData();
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
  m_SurfaceNetworkMesh = new sv4guiMesh();
  m_SurfaceNetworkMesh->ReadVolumeFile(fileName);
  m_1DContainer->SetSurfaceNetworkMesh(m_SurfaceNetworkMesh);
}

void sv4guiPurkinjeNetworkEdit::SelectMesh()
{
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::SelectMesh] ";
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

      m_SurfaceNetworkMesh = new sv4guiMesh();
      m_SurfaceNetworkMesh->ReadSurfaceFile(m_MeshFileName.toStdString());
      auto polyMesh = m_SurfaceNetworkMesh->GetSurfaceMesh();

      auto points = polyMesh->GetPoints();
      auto numPoints = points->GetNumberOfPoints();
      MITK_INFO << "[sv4guiPurkinjeNetworkEdit::LoadMesh] Number of points " << numPoints; 

      auto polygons = polyMesh->GetPolys();
      auto numPolys = polygons->GetNumberOfCells();
      MITK_INFO << "[sv4guiPurkinjeNetworkEdit::LoadMesh] Number of triangles " << numPolys; 

      m_MeshContainer->SetSurfaceMesh(m_SurfaceNetworkMesh);

      // Write mesh to project.
      QFileInfo fileInfo(m_MeshFileName);
      QString outFileName(fileInfo.fileName());
      mitk::DataNode::Pointer projFolderNode = getProjectNode();
      std::string projPath = "";
      projFolderNode->GetStringProperty("project path", projPath);
      QString QprojPath = QString(projPath.c_str());
      m_MeshOutputFileName = QprojPath + "/" + m_StoreDir + "/" + outFileName;
      MITK_INFO << "[sv4guiPurkinjeNetworkEdit::LoadMesh] m_MeshOutputFileName " <<m_MeshOutputFileName.toStdString();
      m_SurfaceNetworkMesh->WriteSurfaceFile(m_MeshOutputFileName.toStdString());
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

//--------------
// AddObservers
//--------------
// Add connecting events (e.g. selecting a mesh face) to callbacks
// within this class. The callbacks will update gui widgets with
// data (e.g. the selected mesh face name).
//
// Observer functions (e.g. sv4guiPurkinjeNetworkMeshSelectFaceEvent() ) 
// are defined in sv4gui_PurkinjeNetworkInteractor.h.
//
void sv4guiPurkinjeNetworkEdit::AddObservers()
{
  if (m_MeshContainer.IsNull()) {
    return;
  }
 
  // Connect selecting a mesh face event. 
  if (m_MeshSelectFaceObserverTag == -1) {
    itk::SimpleMemberCommand<sv4guiPurkinjeNetworkEdit>::Pointer command = 
        itk::SimpleMemberCommand<sv4guiPurkinjeNetworkEdit>::New();
    command->SetCallbackFunction(this, &sv4guiPurkinjeNetworkEdit::UpdateFaceSelection);
    m_MeshSelectFaceObserverTag = m_MeshContainer->AddObserver(sv4guiPurkinjeNetworkMeshSelectFaceEvent(), command);
  }

  // Connect selecting a start point event. 
  if (m_MeshSelectStartPointObserverTag == -1) {
    itk::SimpleMemberCommand<sv4guiPurkinjeNetworkEdit>::Pointer command =
        itk::SimpleMemberCommand<sv4guiPurkinjeNetworkEdit>::New();
    command->SetCallbackFunction(this, &sv4guiPurkinjeNetworkEdit::UpdateStartPointSelection);
    m_MeshSelectStartPointObserverTag = m_MeshContainer->AddObserver(sv4guiPurkinjeNetworkMeshSelectStartPointFaceEvent(), 
        command);
  }
}

//---------------------
// UpdateFaceSelection
//---------------------
// Update the face name GUI widget with the value of the selected face name.
//
void sv4guiPurkinjeNetworkEdit::UpdateFaceSelection()
{
  //std::string msgPrefix = "[sv4guiPurkinjeNetworkEdit::UpdateFaceSelection] ";
  //MITK_INFO << msgPrefix;
  auto faceName = m_MeshContainer->GetSelectedFaceName();
  //MITK_INFO << msgPrefix << "Face name " << faceName;
  ui->meshSurfaceNameLineEdit->setText(QString::fromStdString(faceName));
}

//---------------------------
// UpdateStartPointSelection
//---------------------------
// Update the start point GUI widgets with the value of the selected start point.
//
void sv4guiPurkinjeNetworkEdit::UpdateStartPointSelection()
{
  char x[20], y[20], z[20];
  if (m_MeshContainer->HaveSelectedFace()) {
    auto point = m_MeshContainer->GetPickedPoint();
    sprintf(x, "%g", point[0]);
    sprintf(y, "%g", point[1]);
    sprintf(z, "%g", point[2]);
  } else {
    sprintf(x, "%s", "");
    sprintf(y, "%s", "");
    sprintf(z, "%s", "");
  }

  ui->startPointXLineEdit->setText(QString::fromStdString(x));
  ui->startPointYLineEdit->setText(QString::fromStdString(y));
  ui->startPointZLineEdit->setText(QString::fromStdString(z));

}




