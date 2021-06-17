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

#include "sv4gui_PurkinjeNetwork.h"
#include "sv4gui_Math3.h"

sv4guiPurkinjeNetwork::sv4guiPurkinjeNetwork()
    : m_CalculateBoundingBox(true)
    , m_PathID(-1)
    //, m_Method(sv3::PathElement::CONSTANT_TOTAL_NUMBER)
    , m_CalculationNumber(100)
    , m_Spacing(0)
    , m_DataModified(false)
    , m_ResliceSize(5.0)
    , m_AddingMode(SMART)
{
    this->InitializeEmpty();
}

sv4guiPurkinjeNetwork::sv4guiPurkinjeNetwork(const sv4guiPurkinjeNetwork &other)
    : BaseData(other)
    , m_PathID(other.m_PathID)
    //, m_Method(other.m_Method)
    , m_CalculationNumber(other.m_CalculationNumber)
    , m_Spacing(other.m_Spacing)
    //, m_PathElementSet(other.GetTimeSize())
    , m_DataModified(true)
    , m_CalculateBoundingBox(true)
    , m_ResliceSize(other.m_ResliceSize)
    , m_AddingMode(other.m_AddingMode)
    , m_Props(other.m_Props)
{
    for (std::size_t t = 0; t < other.GetTimeSize(); ++t) {
        // [davep] m_PathElementSet[t]=other.GetPathElement(t)->Clone();
    }
}

sv4guiPurkinjeNetwork::~sv4guiPurkinjeNetwork()
{
    this->ClearData();
}

void sv4guiPurkinjeNetwork::ClearData()
{
    //may need delele each arrays inside first.
    // [davep] m_PathElementSet.clear();
    Superclass::ClearData();
}

void sv4guiPurkinjeNetwork::InitializeEmpty()
{
/*

    Superclass::InitializeTimeGeometry(1);
    m_Initialized = true;
*/
}

bool sv4guiPurkinjeNetwork::IsEmptyTimeStep(unsigned int t) const
{
//    return IsInitialized() && (GetPathElement(t) == NULL);
    return false;
}

void sv4guiPurkinjeNetwork::Expand( unsigned int timeSteps )
{
/*
    unsigned int oldSize = m_PathElementSet.size();

    if ( timeSteps > oldSize )
    {
        Superclass::Expand( timeSteps );

        m_PathElementSet.resize( timeSteps );

        m_CalculateBoundingBox = true;

        this->InvokeEvent( sv4guiPurkinjeNetworkExtendTimeRangeEvent() );
    }
*/
}

unsigned int sv4guiPurkinjeNetwork::GetTimeSize() const
{
    //return m_PathElementSet.size();
    return 0; 
}

int sv4guiPurkinjeNetwork::GetSize( unsigned int t ) const
{
/*
    if(GetPathElement(t))
        return GetPathElement(t)->GetControlPointNumber();
    else
        return 0;
*/
   return 0;
}

/*
sv4guiPurkinjeNetworkElement* sv4guiPurkinjeNetwork::GetPathElement(unsigned int t ) const
{
    if ( t < m_PathElementSet.size() ) {
        return m_PathElementSet[t];
    } else {
        return NULL;
    }
}
*/

/*
void sv4guiPurkinjeNetwork::SetPathElement(sv4guiPurkinjeNetworkElement* pathElement, unsigned int t)
{
    if(t<m_PathElementSet.size())
    {
        m_PathElementSet[t]=pathElement;

        Modified();
        this->InvokeEvent( sv4guiPurkinjeNetworkSetEvent() );
    }
}
*/


int sv4guiPurkinjeNetwork::GetPathID() const
{
    return m_PathID;
}

void sv4guiPurkinjeNetwork::SetPathID(int pathID)
{
    m_PathID=pathID;
}

int sv4guiPurkinjeNetwork::GetMaxPathID(mitk::DataStorage::SetOfObjects::ConstPointer rs)
{
    int maxID=0;

    if(rs){

        for(int i=0;i<rs->size();i++){

            sv4guiPurkinjeNetwork* path=dynamic_cast<sv4guiPurkinjeNetwork*>(rs->GetElement(i)->GetData());
            if(path){
                if(maxID<path->GetPathID())
                {
                    maxID=path->GetPathID();
                }
            }
        }

    }

    return maxID;
}

void sv4guiPurkinjeNetwork::ExecuteOperation( mitk::Operation* operation )
{
/*
    int timeStep = -1;

    sv4guiPurkinjeNetworkOperation* pathOperation = dynamic_cast<sv4guiPurkinjeNetworkOperation*>(operation);

    if ( pathOperation )
    {
        timeStep = pathOperation->GetTimeStep();

    }else{
        MITK_ERROR << "No valid Path Operation for sv4guiPurkinjeNetwork" << std::endl;
        return;
    }

    if ( timeStep < 0 )
    {
        MITK_ERROR << "Time step (" << timeStep << ") outside of sv4guiPurkinjeNetwork time bounds" << std::endl;
        return;
    }

    sv4guiPurkinjeNetworkElement* originalPathElement=m_PathElementSet[timeStep];

    sv4guiPurkinjeNetworkElement* newPathElement=pathOperation->GetPathElement();
    int index = pathOperation->GetIndex();
    mitk::Point3D point=pathOperation->GetPoint();
    bool selected=pathOperation->GetSelected();

    m_OperationType= (sv4guiPurkinjeNetworkOperation::PathOperationType) operation->GetOperationType();

    switch (operation->GetOperationType())
    {

    case sv4guiPurkinjeNetworkOperation::OpINSERTCONTROLPOINT:
    {
        if(originalPathElement)
        {
            originalPathElement->InsertControlPoint(index,point);
            originalPathElement->DeselectControlPoint();
            originalPathElement->SetControlPointSelected(index,true);
            m_CalculateBoundingBox = true;
            m_NewControlPoint=point;
            m_DataModified=true;
            this->Modified();
            this->InvokeEvent( sv4guiPurkinjeNetworkPointInsertEvent() );
        }
    }
        break;

    case sv4guiPurkinjeNetworkOperation::OpMOVECONTROLPOINT:
    {
        if(originalPathElement)
        {
            originalPathElement->SetControlPoint(index,point);
            m_CalculateBoundingBox = true;
            m_NewControlPoint=point;
            m_DataModified=true;
            this->Modified();
            this->InvokeEvent( sv4guiPurkinjeNetworkPointMoveEvent() );
        }
    }
        break;

    case sv4guiPurkinjeNetworkOperation::OpREMOVECONTROLPOINT:
    {
        if(originalPathElement)
        {
            originalPathElement->RemoveControlPoint(index);
            m_CalculateBoundingBox = true;
            m_DataModified=true;
            this->Modified();
            this->InvokeEvent( sv4guiPurkinjeNetworkPointRemoveEvent() );
        }
    }
        break;

    case sv4guiPurkinjeNetworkOperation::OpSELECTCONTROLPOINT:
    {
        if(originalPathElement)
        {
            originalPathElement->SetControlPointSelected(index,selected);
            this->Modified();
            this->InvokeEvent( sv4guiPurkinjeNetworkPointSelectEvent() );
        }
    }
        break;

    case sv4guiPurkinjeNetworkOperation::OpDESELECTALL:
    {
        if(originalPathElement)
        {
            originalPathElement->DeselectControlPoint();
            this->Modified();
            this->InvokeEvent( sv4guiPurkinjeNetworkPointSelectEvent() );
        }
    }
        break;

    case sv4guiPurkinjeNetworkOperation::OpSETPATHELEMENT:
    {
        m_PathElementSet[timeStep]=newPathElement;
        m_CalculateBoundingBox = true;
        m_DataModified=true;
        this->Modified();
        this->InvokeEvent( sv4guiPurkinjeNetworkSetEvent() );
    }
        break;

    default:
        itkWarningMacro("sv4guiPurkinjeNetwork could not understrand the operation. Please check!");
        break;
    }

    mitk::OperationEndEvent endevent(operation);
    ((const itk::Object*)this)->InvokeEvent(endevent);

*/

}

void sv4guiPurkinjeNetwork::CalculateBoundingBox(double *bounds,unsigned int t)
{
/*
    sv4guiPurkinjeNetworkElement* pathElement=GetPathElement(t);
    if(pathElement)
    {
        pathElement->CalculateBoundingBox(bounds);
    }
*/
}

void sv4guiPurkinjeNetwork::SetSpacing(double spacing)
{
    m_Spacing=spacing;
}

double sv4guiPurkinjeNetwork::GetSpacing() const
{
    return m_Spacing;
}

/*
void sv4guiPurkinjeNetwork::SetMethod(sv3::PathElement::CalculationMethod method)\
{
    m_Method=method;
}
*/

/*

sv3::PathElement::CalculationMethod sv4guiPurkinjeNetwork::GetMethod() const
{
    return m_Method;
}
*/

void sv4guiPurkinjeNetwork::SetCalculationNumber(int number)
{
    m_CalculationNumber=number;
}

int sv4guiPurkinjeNetwork::GetCalculationNumber() const
{
    return m_CalculationNumber;
}

mitk::Point3D sv4guiPurkinjeNetwork::GetNewControlPoint()
{
    return m_NewControlPoint;
}

/*
sv4guiPurkinjeNetworkOperation::PathOperationType sv4guiPurkinjeNetwork::GetOperationType()
{
    return m_OperationType;
}
*/

void sv4guiPurkinjeNetwork::UpdateOutputInformation()
{
/*
    if ( this->GetSource( ) )
    {
        this->GetSource( )->UpdateOutputInformation( );
    }

    //
    // first make sure, that the associated time sliced geometry has
    // the same number of geometry 3d's as sv4guiPurkinjeNetworks are present
    //
    mitk::TimeGeometry* timeGeometry = GetTimeGeometry();
    if ( timeGeometry->CountTimeSteps() != m_PathElementSet.size() )
    {
        itkExceptionMacro(<<"timeGeometry->CountTimeSteps() != m_PathElementSet.size() -- use Initialize(timeSteps) with correct number of timeSteps!");
    }

    if (m_CalculateBoundingBox)
    {
        for ( unsigned int t = 0 ; t < m_PathElementSet.size() ; ++t )
        {
            double bounds[6] = {0};
            CalculateBoundingBox(bounds,t);
            this->GetGeometry(t)->SetFloatBounds(bounds);
        }

        m_CalculateBoundingBox = false;
    }

    this->GetTimeGeometry()->Update();
*/
}

void sv4guiPurkinjeNetwork::SetRequestedRegionToLargestPossibleRegion()
{
}

bool sv4guiPurkinjeNetwork::RequestedRegionIsOutsideOfTheBufferedRegion()
{
    return false;
}

bool sv4guiPurkinjeNetwork::VerifyRequestedRegion()
{
    return true;
}

void sv4guiPurkinjeNetwork::SetRequestedRegion(const DataObject * )
{
}

void sv4guiPurkinjeNetwork::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
/*
    Superclass::PrintSelf(os, indent);

    os << indent << "Number timesteps: " << m_PathElementSet.size() << "\n";

    for ( unsigned int t = 0 ; t < m_PathElementSet.size() ; ++t )
    {
        if(m_PathElementSet[t])
        {
            os << indent << "Timestep " << t << ": \n";
            itk::Indent nextIndent = indent.GetNextIndent();

            for(int i=0;i<m_PathElementSet[t]->GetControlPointNumber();i++)
            {
                os << nextIndent << "Point " << i << ": [";
                os << m_PathElementSet[t]->GetControlPoint(i)[0];
                os << ", " << m_PathElementSet[t]->GetControlPoint(i)[1];
                os << ", " << m_PathElementSet[t]->GetControlPoint(i)[2];
                os << "]";
                os << ", selected: " <<m_PathElementSet[t]->IsControlPointSelected(i) << "\n";

            }
        }
    }
*/
}

bool sv4guiPurkinjeNetwork::IsDataModified()
{
    return m_DataModified;
}

void sv4guiPurkinjeNetwork::SetDataModified(bool modified)
{
    m_DataModified=modified;
}

void sv4guiPurkinjeNetwork::SetProp(const std::string& key, std::string value)
{
    m_Props[key]=value;
}

std::string sv4guiPurkinjeNetwork::GetProp(const std::string& key) const
{
    std::map<std::string,std::string>* p=const_cast<std::map<std::string,std::string>*>(&m_Props);
    return (*p)[key];
}

bool Equal( const sv4guiPurkinjeNetwork* leftHandSide, const sv4guiPurkinjeNetwork* rightHandSide, mitk::ScalarType eps, bool verbose )
{
    if((leftHandSide == nullptr) || (rightHandSide == nullptr))
    {
        MITK_ERROR << "Equal( const sv4guiPurkinjeNetwork* leftHandSide, const sv4guiPurkinjeNetwork* rightHandSide, mitk::ScalarType eps, bool verbose ) does not work with NULL pointer input.";
        return false;
    }
    return Equal( *leftHandSide, *rightHandSide, eps, verbose);
}

bool Equal( const sv4guiPurkinjeNetwork& leftHandSide, const sv4guiPurkinjeNetwork& rightHandSide, mitk::ScalarType eps, bool verbose )
{
/*
    bool result = true;

    if( !mitk::Equal( *leftHandSide.GetGeometry(), *rightHandSide.GetGeometry(), eps, verbose) )
    {
        if(verbose)
            MITK_INFO << "[( sv4guiPurkinjeNetwork )] Geometries differ.";
        result = false;
    }

    if ( leftHandSide.GetSize() != rightHandSide.GetSize())
    {
        if(verbose)
            MITK_INFO << "[( sv4guiPurkinjeNetwork )] Number of control points differ.";
        result = false;
    }else if (leftHandSide.GetPathElement()->GetPathPointNumber()!=rightHandSide.GetPathElement()->GetPathPointNumber())
    {
        if(verbose)
            MITK_INFO << "[( sv4guiPurkinjeNetwork )] Number of path points differ.";
        result = false;
    }
    else
    {
        int numberOfIncorrectPoints = 0;
        int num=leftHandSide.GetSize();

        for(int i=0;i<num;i++)
        {
            if( !mitk::Equal( leftHandSide.GetPathElement()->GetControlPoint(i), rightHandSide.GetPathElement()->GetControlPoint(i), eps, verbose ) )
            {
                if(verbose)
                    MITK_INFO << "[( sv4guiPurkinjeNetwork )] control point values are different.";
                result = false;
                numberOfIncorrectPoints++;
            }
        }

        if((numberOfIncorrectPoints > 0) && verbose)
        {
            MITK_INFO << numberOfIncorrectPoints <<" of a total of " << leftHandSide.GetSize() << " control points are different.";
        }
    }
    return result;
*/
    return false;
}
