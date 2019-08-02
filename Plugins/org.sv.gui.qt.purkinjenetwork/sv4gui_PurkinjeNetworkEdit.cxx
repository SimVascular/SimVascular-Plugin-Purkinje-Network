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
#include "sv4gui_PurkinjeNetworkModel.h"

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
  m_DataStorage = nullptr;
  m_ProjFolderNode = nullptr;
  m_MeshFolderNode = nullptr; 
  m_MeshMapper = nullptr;
  m_MeshSelectFaceObserverTag = -1;
  m_MeshSelectStartPointObserverTag = -1;
  m_ModelFolderNode = nullptr; 
  m_Parent = nullptr;
  m_PurkinjeNetworkNode = nullptr;
  m_SphereWidget = nullptr;

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

//---------------------
// CreateQtPartControl
//---------------------

void sv4guiPurkinjeNetworkEdit::CreateQtPartControl(QWidget* parent )
{
    std::string msgPrefix = "[sv4guiPurkinjeNetworkEdit::CreateQtPartControl] ";
    MITK_INFO << msgPrefix; 
    m_Parent = parent;
    ui->setupUi(parent);
    m_DisplayWidget = GetActiveStdMultiWidget();

    if (m_DisplayWidget == nullptr) {
      parent->setEnabled(false);
      MITK_ERROR << msgPrefix << "Plugin MeshEdit Init Error: No QmitkStdMultiWidget Available!";
      return;
    }

    // Initialize folder nodes.
    if (!Initialize()) {
      return;
    }

    // Define widget event handlers.
    connect(ui->buttonLoadMesh, SIGNAL(clicked()), this, SLOT(LoadMesh()));
    //connect(ui->selectMeshComboBox, SIGNAL(clicked()), this, SLOT(SelectMesh()));
    connect(ui->meshCheckBox, SIGNAL(clicked(bool)), this, SLOT(showMesh(bool)));
    connect(ui->meshSurfaceNameLineEdit, SIGNAL(returnPressed()), this, SLOT(MeshSurfaceName()));

    connect(ui->buttonLoadParameters, SIGNAL(clicked()), this, SLOT(LoadParameters()));
    connect(ui->buttonExportParameters, SIGNAL(clicked()), this, SLOT(ExportParameters()));
    connect(ui->startPointXLineEdit, SIGNAL(returnPressed()), this, SLOT(MeshSurfaceStartPoint()));
    connect(ui->startPointYLineEdit, SIGNAL(returnPressed()), this, SLOT(MeshSurfaceStartPoint()));
    connect(ui->startPointZLineEdit, SIGNAL(returnPressed()), this, SLOT(MeshSurfaceStartPoint()));
    connect(ui->secondPointXLineEdit, SIGNAL(returnPressed()), this, SLOT(MeshSurfaceSecondPoint()));
    connect(ui->secondPointYLineEdit, SIGNAL(returnPressed()), this, SLOT(MeshSurfaceSecondPoint()));
    connect(ui->secondPointZLineEdit, SIGNAL(returnPressed()), this, SLOT(MeshSurfaceSecondPoint()));

    connect(ui->buttonCreateNetwork, SIGNAL(clicked()), this, SLOT(CreateNetwork()));
    connect(ui->networkCheckBox, SIGNAL(clicked(bool)), this, SLOT(showNetwork(bool)));

    m_Interface = new sv4guiDataNodeOperationInterface();

    if (m_init){
        MITK_INFO << msgPrefix << "Making network node";

        // Create mesh container, node and mapper.
        //
        m_MeshContainer = sv4guiPurkinjeNetworkMeshContainer::New();
        auto meshNode = mitk::DataNode::New();
        meshNode->SetData(m_MeshContainer);
        meshNode->SetVisibility(true);
        meshNode->SetName("mesh");
        m_MeshNode = meshNode;
        // Create mesh node under 'Purkinje-Network' node.
        auto parentNode = GetDataStorage()->GetNamedNode("Purkinje-Network");
        if (parentNode) {
          GetDataStorage()->Add(m_MeshNode, parentNode);
        } else {
          MITK_INFO << msgPrefix << "No Purkinje-Network node.";
        }

        MITK_INFO << msgPrefix << "Make mapper.";
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
        node->SetName("1D Network");
        m_1DNode = node;
        // Create 1D network node under 'Purkinje-Network' node.
        if (parentNode) {
          GetDataStorage()->Add(m_1DNode, parentNode);
        }

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
  double point[3];
  point[0] = std::stod(ui->startPointXLineEdit->text().trimmed().toStdString());
  point[1] = std::stod(ui->startPointYLineEdit->text().trimmed().toStdString());
  point[2] = std::stod(ui->startPointZLineEdit->text().trimmed().toStdString());
  m_MeshContainer->SetFirstNetworkPoint(point);
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::MeshSurfaceStartPoint] set";
}

//-----------------------
// MeshSurfaceSecondPoint 
//-----------------------
// Process mesh surface second point type-in.
//
void sv4guiPurkinjeNetworkEdit::MeshSurfaceSecondPoint()
{
  double point[3];
  point[0] = std::stod(ui->secondPointXLineEdit->text().trimmed().toStdString());
  point[1] = std::stod(ui->secondPointYLineEdit->text().trimmed().toStdString());
  point[2] = std::stod(ui->secondPointZLineEdit->text().trimmed().toStdString());
  m_MeshContainer->SetSecondNetworkPoint(point);
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::MeshSurfaceSescontPoint] set";
}


//----------
// showMesh
//----------
// Process the show mesh check box select.
//
// Displays or hides the mesh in the graphics window.

void sv4guiPurkinjeNetworkEdit::showMesh(bool state)
{
  m_MeshNode->SetVisibility(state);
/*
  if (!state) {
      GetDataStorage()->Remove(m_MeshNode);
  } else {
      auto networkNode = GetDataStorage()->GetNamedNode("Purkinje-Network");
      if (networkNode) {
          GetDataStorage()->Add(m_MeshNode, networkNode);
      }
  }
*/

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

//-------------
// showNetwork
//-------------
// Process the show network check box select.
//
// Displays or hides the 1D network model in the graphics window.

void sv4guiPurkinjeNetworkEdit::showNetwork(bool state)
{
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::showNetwork] state " << state;
/*
  auto node = GetDataStorage()->GetNamedNode("Purkinje-Network");
  if (node) {
    MITK_INFO << "[sv4guiPurkinjeNetworkEdit::showMesh] Add m_1DNode to Purkinje-Network folder node.";
    GetDataStorage()->Add(m_1DNode, node);
  }
*/

  m_1DNode->SetVisibility(state);

/*
  if (!state) {
      GetDataStorage()->Remove(m_1DNode);
      modelNode->SetVisibility(false);
  } else {
      auto node = GetDataStorage()->GetNamedNode("Purkinje-Network");
      if(node) {
          MITK_INFO << "[sv4guiPurkinjeNetworkEdit::showMesh] Add m_MeshNode to image_folder_node";
          GetDataStorage()->Add(m_1DNode, node);
      }
  }
*/

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

//------------
// Initialize
//------------
// Initialize the data manager nodes.
//
// We need to check for a null project folders because the pulgin is created before
// a project is read in, need to disable this.

bool sv4guiPurkinjeNetworkEdit::Initialize()
{
  auto msgPrefix = "[sv4guiPurkinjeNetworkEdit::Initialize] ";
  MITK_INFO << msgPrefix << " Initialize";
  m_DataStorage = GetDataStorage();
  if (m_DataStorage == nullptr) { 
    MITK_INFO << msgPrefix << " m_DataStorage == nullptr";
    return false;
  }

  mitk::NodePredicateDataType::Pointer isProjFolder = mitk::NodePredicateDataType::New("sv4guiProjectFolder");
  mitk::DataNode::Pointer projFolderNode = m_DataStorage->GetNode(isProjFolder);
  if (projFolderNode == nullptr) { 
    MITK_INFO << msgPrefix << " projFolderNode == nullptr";
    return false;
  }
  m_ProjFolderNode = projFolderNode;

  // Get the project path.
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::Initialize] Making directory\n";
  QDir dir;
  std::string projPath = "";
  if (!projFolderNode){
    MITK_ERROR << msgPrefix << "Project folder node null, cannot get project path.";
    dir = QDir("not_found");
    return false;
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
      dir.mkdir(Qstore_dir);
    }
  }

  // Create the data folder.
  if (projFolderNode) {
    mitk::DataNode::Pointer node = m_DataStorage->GetNamedNode(PurkinjeNetwork_NODE_NAME);
    if (!node) {
      QString folderName = QString(PurkinjeNetwork_NODE_NAME.c_str());
      node = svProj.CreateDataFolder<PurkinjeNetworkFolder>(m_DataStorage, folderName, projFolderNode);
    }
    m_PurkinjeNetworkNode = node;
  }

  return true;
}


// --------------------
//  SetMeshInformation
// --------------------
// Get model and mesh data neeeded to set the model surface meshes
// on which to create purkinje networks.

void sv4guiPurkinjeNetworkEdit::SetMeshInformation()
{
  auto msgPrefix = "[sv4guiPurkinjeNetworkEdit::SetMeshInformation] ";
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
    QMessageBox::warning(m_Parent, "No mesh surface", "No surface mesh has been found. A mesh has not been generated?");
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
  if (m_DataStorage == nullptr) {
    return nullptr;
  }
  mitk::NodePredicateDataType::Pointer isProjFolder = mitk::NodePredicateDataType::New("sv4guiProjectFolder");
  mitk::DataNode::Pointer projFolderNode = m_DataStorage->GetNode(isProjFolder);
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
    MITK_WARN << msgPrefix << "No mesh face seleced.";
    QMessageBox::warning(m_Parent, "No mesh face selected", "A mesh face must be selected.");
    return;
  }
  auto faceName = m_MeshContainer->GetSelectedFaceName();
  MITK_INFO << msgPrefix << "Face name " << faceName;

  // Get the network start point and second point defining 
  // the direction of the initial segment.
  if (!m_MeshContainer->HaveNetworkPoints()) {
    MITK_WARN << msgPrefix << "No start point seleced.";
    QMessageBox::warning(m_Parent, "No start point selected", "A start point must be selected.");
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
  auto params = GetParametersFromGui();
  pnetModel.SetParameters(params);

  SetModelMesh(pnetModel);
  auto outputPath = projPath + "/" + m_StoreDir.toStdString() + "/";
  pnetModel.GenerateNetwork(outputPath);

  // Read the generated network (1D elements).
  LoadNetwork(pnetModel.networkFileName);

}

//----------------------
// GetParametersFromGui
//----------------------
// Get parameter values from the GUI.

std::map<std::string, std::string> sv4guiPurkinjeNetworkEdit::GetParametersFromGui()
{ 
  std::map<std::string, std::string> params;
  sv4guiPurkinjeNetworkModelParamNames paramNames;

  // Get network points.
  //
  std::array<double,3> point1, point2;
  m_MeshContainer->GetNetworkPoints(point1, point2);

  std::stringstream ss1, ss2;
  std::ostream_iterator<double> outIt1(ss1, " ");
  std::copy(point1.begin(), point1.end(), outIt1);
  params.insert(pair<std::string,std::string>(paramNames.FirstPoint, ss1.str()));

  std::ostream_iterator<double> outIt2(ss2, " ");
  std::copy(point2.begin(), point2.end(), outIt2);
  params.insert(pair<std::string,std::string>(paramNames.SecondPoint, ss2.str()));

  // Get other parameters.
  //
  auto avgBranchLength = std::to_string(ui->avgBranchLengthSpinBox->value());
  params.insert(pair<std::string,std::string>(paramNames.AvgBranchLength, avgBranchLength));

  auto branchAngle = std::to_string(ui->branchAngleSpinBox->value());
  params.insert(pair<std::string,std::string>(paramNames.BranchAngle, branchAngle));

  auto branchSegLength = std::to_string(ui->branchSegLengthSpinBox->value());
  params.insert(pair<std::string,std::string>(paramNames.BranchSegLength, branchSegLength));

  auto numBranchGenerations = std::to_string(ui->numBranchGenSpinBox->value());
  params.insert(pair<std::string,std::string>(paramNames.NumBranchGenerations, numBranchGenerations));

  auto repulsiveParameter = std::to_string(ui->repulsiveParameterSpinBox->value());
  params.insert(pair<std::string,std::string>(paramNames.RepulsiveParameter, repulsiveParameter));

  return params;
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

  if (ui->networkCheckBox->isChecked()) {
    showNetwork(true);
  } else {
    showNetwork(false);
  }
}

void sv4guiPurkinjeNetworkEdit::SelectMesh()
{
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::SelectMesh] ";
}



//----------
// LoadMesh
//----------
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

//----------------
// LoadParameters
//----------------
// Load compute parameters from a file.
//
void sv4guiPurkinjeNetworkEdit::LoadParameters()
{
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::LoadParameters] ";

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

      m_ParameterFileName = QFileDialog::getOpenFileName(NULL, tr("Import Parameter File (Choose File)"), lastFilePath,
        tr("Parameter file (*.txt)"));

      m_ParameterFileName = m_ParameterFileName.trimmed();

      if (m_ParameterFileName.isEmpty()) {
          return;
      }

      // Read file.
      MITK_INFO << "[sv4guiPurkinjeNetworkEdit::LoadParameters] Read paramaers " << m_ParameterFileName.toStdString();
      std::ifstream inFile(m_ParameterFileName.toStdString());
      std::string line;
      std::string name, v1, v2, v3;
      double point[3];
      sv4guiPurkinjeNetworkModelParamNames paramNames;

      while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        ss >> name;
        std::cout << " name " << name << std::endl;
        if (name == paramNames.FirstPoint) {
          ss >> v1 >> v2 >> v3;
          std::cout << " 1st v1 " << v1 << std::endl;
          std::cout << " 1st v2 " << v2 << std::endl;
          std::cout << " 1st v3 " << v3 << std::endl;
          point[0] = std::stod(v1);
          point[1] = std::stod(v2);
          point[2] = std::stod(v3);
          m_MeshContainer->SetFirstNetworkPoint(point);
          ui->startPointXLineEdit->setText(QString::fromStdString(v1));
          ui->startPointYLineEdit->setText(QString::fromStdString(v2));
          ui->startPointZLineEdit->setText(QString::fromStdString(v3));
        } else if (name == paramNames.SecondPoint) {
          ss >> v1 >> v2 >> v3;
          std::cout << " 2nd v1 " << v1 << std::endl;
          std::cout << " 2nd v2 " << v2 << std::endl;
          std::cout << " 2nd v3 " << v3 << std::endl;
          point[0] = std::stod(v1);
          point[1] = std::stod(v2);
          point[2] = std::stod(v3);
          m_MeshContainer->SetSecondNetworkPoint(point);
          ui->secondPointXLineEdit->setText(QString::fromStdString(v1));
          ui->secondPointYLineEdit->setText(QString::fromStdString(v2));
          ui->secondPointZLineEdit->setText(QString::fromStdString(v3));
        } else if (name == paramNames.NumBranchGenerations) {
          ss >> v1;
          ui->numBranchGenSpinBox->setValue(std::stoi(v1));
        } else if (name == paramNames.AvgBranchLength) {
          ss >> v1;
          ui->avgBranchLengthSpinBox->setValue(std::stod(v1));
        } else if (name == paramNames.BranchSegLength) {
          ss >> v1;
          ui->branchSegLengthSpinBox->setValue(std::stod(v1));
        }
      }
      inFile.close();
  }

  catch(...) {
      MITK_ERROR << "Error loading Purkinje parameter file.";
  }
}

//------------------
// ExportParameters
//------------------
// Export the Purkinje network parameters to a text file.

void sv4guiPurkinjeNetworkEdit::ExportParameters()
{
  MITK_INFO << "[sv4guiPurkinjeNetworkEdit::ExportParameters] ";

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

      auto fileName = QFileDialog::getSaveFileName(NULL, tr("Export Parameter File"), lastFilePath,
        tr("Parameter file (*.txt)"));

      fileName = fileName.trimmed();

      if (fileName.isEmpty()) {
          return;
      }

      auto parameters = GetParametersFromGui();

      // Write file.
      std::ofstream outFile(fileName.toStdString());
      std::map<std::string, std::string>::iterator it;
      for (it = parameters.begin(); it != parameters.end(); ++it) {
        outFile << it->first << " " << it->second << std::endl; 
      }
      outFile.close();
  }

  catch(...) {
      MITK_ERROR << "Error writing Purkinje parameter file.";
  }
}


//---------
// Visible
//---------

void sv4guiPurkinjeNetworkEdit::Visible()
{
  if (m_ProjFolderNode == nullptr) {
      return;
  }
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
  auto msgprefix = "[sv4guiPurkinjeNetworkEdit::AddObservers] ";
  MITK_INFO << msgprefix;
  if (m_MeshContainer.IsNull()) {
    MITK_INFO << msgprefix << "Mesh container is null";
    return;
  }
 
  // Connect selecting a mesh face event. 
  MITK_INFO << msgprefix << "Connect selecting a mesh face event";
  if (m_MeshSelectFaceObserverTag == -1) {
    itk::SimpleMemberCommand<sv4guiPurkinjeNetworkEdit>::Pointer command = 
        itk::SimpleMemberCommand<sv4guiPurkinjeNetworkEdit>::New();
    command->SetCallbackFunction(this, &sv4guiPurkinjeNetworkEdit::UpdateFaceSelection);
    m_MeshSelectFaceObserverTag = m_MeshContainer->AddObserver(sv4guiPurkinjeNetworkMeshSelectFaceEvent(), command);
  }

  // Connect selecting a start point event. 
  MITK_INFO << msgprefix << "Connect selecting a start point event";
  if (m_MeshSelectStartPointObserverTag == -1) {
    itk::SimpleMemberCommand<sv4guiPurkinjeNetworkEdit>::Pointer command =
        itk::SimpleMemberCommand<sv4guiPurkinjeNetworkEdit>::New();
    command->SetCallbackFunction(this, &sv4guiPurkinjeNetworkEdit::UpdateStartPointSelection);
    m_MeshSelectStartPointObserverTag = m_MeshContainer->AddObserver(sv4guiPurkinjeNetworkMeshSelectStartPointFaceEvent(), 
        command);
  }
  MITK_INFO << msgprefix << "Done! ";
}

//---------------------
// UpdateFaceSelection
//---------------------
// Update the face name GUI widget with the value of the selected face name.
//
void sv4guiPurkinjeNetworkEdit::UpdateFaceSelection()
{
  std::string msgPrefix = "[sv4guiPurkinjeNetworkEdit::UpdateFaceSelection] ";
  MITK_INFO << msgPrefix;
  MITK_INFO << msgPrefix << "---------- UpdateFaceSelection ----------";
  auto faceName = m_MeshContainer->GetSelectedFaceName();
  MITK_INFO << msgPrefix << "Face name " << faceName;
  ui->meshSurfaceNameLineEdit->setText(QString::fromStdString(faceName));

  ui->startPointXLineEdit->setText(" ");
  ui->startPointYLineEdit->setText(" ");
  ui->startPointZLineEdit->setText(" ");

  ui->secondPointXLineEdit->setText(" ");
  ui->secondPointYLineEdit->setText(" ");
  ui->secondPointZLineEdit->setText(" ");
}

//---------------------------
// UpdateStartPointSelection
//---------------------------
// Update the start point GUI widgets with the value of the selected start point.
//
void sv4guiPurkinjeNetworkEdit::UpdateStartPointSelection()
{

  auto msg = "[sv4guiPurkinjeNetworkEdit::UpdateStartPointSelection] ";
  auto reset = true;

  if (!m_MeshContainer->HaveNewNetworkPoints(reset)) {
      return;
  }

  MITK_INFO << msg << "---------- UpdateStartPointSelection ---------";
  MITK_INFO << msg << "New network points. ";

  char x1[20], y1[20], z1[20];
  char x2[20], y2[20], z2[20];

  if (m_MeshContainer == nullptr) { 
      MITK_WARN << msg << "m_MeshContainer is null. ";
      return;
  }

  MITK_INFO << msg << "check HaveSelectedFace"; 

  if (m_MeshContainer->HaveSelectedFace()) {
    MITK_INFO << msg << "HaveSelectedFace"; 
    //auto point = m_MeshContainer->GetPickedPoint();
    std::array<double,3> firstPoint, secondPoint;
    m_MeshContainer->GetNetworkPoints(firstPoint, secondPoint);
    MITK_INFO << msg << " First point" << firstPoint[0] << " " << firstPoint[1] << "  " << firstPoint[2];

    sprintf(x1, "%g", firstPoint[0]);
    sprintf(y1, "%g", firstPoint[1]);
    sprintf(z1, "%g", firstPoint[2]);
    sprintf(x2, "%g", secondPoint[0]);
    sprintf(y2, "%g", secondPoint[1]);
    sprintf(z2, "%g", secondPoint[2]);

  } else {
    sprintf(x1, "%s", " ");
    sprintf(y1, "%s", " ");
    sprintf(z1, "%s", " ");
    sprintf(x2, "%s", " ");
    sprintf(y2, "%s", " ");
    sprintf(z2, "%s", " ");
  }

  ui->startPointXLineEdit->setText(QString::fromStdString(x1));
  ui->startPointYLineEdit->setText(QString::fromStdString(y1));
  ui->startPointZLineEdit->setText(QString::fromStdString(z1));

  ui->secondPointXLineEdit->setText(QString::fromStdString(x2));
  ui->secondPointYLineEdit->setText(QString::fromStdString(y2));
  ui->secondPointZLineEdit->setText(QString::fromStdString(z2));

  MITK_INFO << msg << "done"; 

}
