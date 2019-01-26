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

#include "sv4gui_PurkinjeNetworkModel.h"
#include <mitkLogMacros.h>

#include <vtkXMLPolyDataWriter.h>

//-------------
// Constructor
//-------------
sv4guiPurkinjeNetworkModel::sv4guiPurkinjeNetworkModel(const std::string name, const std::array<double,3>& firstPoint,
    const std::array<double,3>& secondPoint) : name(name), firstPoint(firstPoint), secondPoint(secondPoint)
{

}

//------------
// Destructor
//------------
sv4guiPurkinjeNetworkModel::~sv4guiPurkinjeNetworkModel()
{
}

//-----------------
// GenerateNetwork
//-----------------
bool sv4guiPurkinjeNetworkModel::GenerateNetwork(const std::string outputPath)
{
  std::string msgPrefix = "[sv4guiPurkinjeNetworkModel::GenerateNetwork] ";
  MITK_INFO << msgPrefix << "Output path " << outputPath;

  // Write the surface mesh to a .vtp file.
  auto meshFileName = outputPath + "/" + this->name + ".vtp";
  WriteMesh(meshFileName);
  MITK_INFO << msgPrefix << "Input surface mesh file " << meshFileName;
 
  // Set output file prefix.
  auto outfile = outputPath + "/" + this->name;
  MITK_INFO << msgPrefix << "Output network file " << outfile;

  // Execute Python command to generate network. 
  auto cmd = CreateCommand(meshFileName, outfile);
  MITK_INFO << msgPrefix << "Execute cmd " << cmd;
  PyRun_SimpleString(cmd.c_str());
  MITK_INFO << msgPrefix << "Done!";

  // Set the name of the file containing the network of 1D elements.
  this->networkFileName = outputPath + "/" + this->name + ".vtu";
 
}

//---------------
// CreateCommand
//---------------

std::string sv4guiPurkinjeNetworkModel::CreateCommand(const std::string infile, const std::string outfile)
{
  std::string cmd;
  cmd += "import fractal_tree\n";
  cmd += "fractal_tree.run(";
  cmd += "infile='" + infile + "',";
  cmd += "outfile='" + outfile + "',";
  cmd += "init_node='[" + std::to_string(this->firstPoint[0]) + "," + std::to_string(this->firstPoint[1]) + 
      "," + std::to_string(this->firstPoint[2]) + "]',";
  cmd += "second_node='[" + std::to_string(this->secondPoint[0]) + "," + std::to_string(this->secondPoint[1]) + 
      "," + std::to_string(this->secondPoint[2]) + "]'";

  cmd += "num_branch_gen='" + std::to_string(this->numBranchGenerations) + "',";
  cmd += "avg_branch_length='" + std::to_string(this->avgBranchLength) + "',";
  cmd += "avg_branch_angles='" + std::to_string(this->avgBranchAngles) + "',";
  cmd += "repulsive_parameter='" + std::to_string(this->repulsiveParameter) + "',";
  cmd += "branch_seg_length='" + std::to_string(this->branchSegLength) + "',";
  cmd += ")\n"; 

  return cmd;
}

//-----------
// WriteMesh 
//-----------
bool sv4guiPurkinjeNetworkModel::WriteMesh(const std::string fileName)
{
  vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(fileName.c_str());
  writer->SetInputData(this->meshPolyData);
  writer->Write();
}


