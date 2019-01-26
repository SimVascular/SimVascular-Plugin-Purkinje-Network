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

// This class is used to represent a model of a Purkinje Network.
//
// The model includes 
//   1) Surface mesh on which the network is generated
//   2) Parameters used to generate the mesh
//   3) 1D element mesh representing the network

#ifndef SV4GUI_PURKINJENETWORK_MODEL_H
#define SV4GUI_PURKINJENETWORK_MODEL_H

#include <iostream>
#include <array>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

class sv4guiPurkinjeNetworkModel {

  public:
    sv4guiPurkinjeNetworkModel(const std::string name, const std::array<double,3>& firstPoint, 
        const std::array<double,3>& secondPoint);
    sv4guiPurkinjeNetworkModel() = delete; 
    ~sv4guiPurkinjeNetworkModel(); 
    bool GenerateNetwork(const std::string outputPath);
    bool WriteMesh(const std::string fileName);
    std::string CreateCommand(const std::string infile, const std::string outfile);

    std::string name; 
    std::string networkFileName; 
    std::array<double,3> firstPoint;
    std::array<double,3> secondPoint;
    int numBranchGenerations;
    float avgBranchLength;
    float branchAngle;
    float repulsiveParameter;
    float branchSegLength;
    vtkSmartPointer<vtkPolyData> meshPolyData;

};

#endif //SV4GUI_PURKINJENETWORK_MODEL_H
