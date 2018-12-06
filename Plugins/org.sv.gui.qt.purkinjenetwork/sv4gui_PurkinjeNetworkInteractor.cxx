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

#include "sv4gui_PurkinjeNetworkInteractor.h"
#include "sv4gui_PurkinjeNetworkMeshContainer.h"

#include "mitkInteractionPositionEvent.h"
#include "mitkInternalEvent.h"
#include "mitkBaseRenderer.h"
#include "mitkRenderingManager.h"

#include <usModuleRegistry.h>
#include <usGetModuleContext.h>
#include <usModule.h>
#include <usModuleContext.h>

sv4guiPurkinjeNetworkInteractor::sv4guiPurkinjeNetworkInteractor(){
  m_selectedSeed.push_back(-1);
  m_selectedSeed.push_back(-1);
}

sv4guiPurkinjeNetworkInteractor::~sv4guiPurkinjeNetworkInteractor(){

}

void sv4guiPurkinjeNetworkInteractor::ConnectActionsAndFunctions()
{
  MITK_INFO << "connect actions and functions\n";
  /*
  CONNECT_CONDITION("is_over_seed", IsOverSeed);
  CONNECT_FUNCTION( "add_end_seed", AddEndSeed);
  CONNECT_FUNCTION( "delete_seed" , DeleteSeed);
  */
  CONNECT_FUNCTION( "add_start"    , AddStart);
  CONNECT_FUNCTION("select_point", SelectPoint);
}

void sv4guiPurkinjeNetworkInteractor::SelectPoint(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{
  MITK_INFO << "[sv4guiPurkinjeNetworkInteractor::SelectPoint] ";

  const mitk::InteractionPositionEvent* positionEvent = 
    dynamic_cast<const mitk::InteractionPositionEvent*>( interactionEvent );

  if ( positionEvent == NULL ) {
      MITK_INFO << "[sv4guiPurkinjeNetworkInteractor::SelectPoint]  positionEvent is null ";
      return;
  }

  sv4guiPurkinjeNetworkMeshContainer* mesh =
        static_cast< sv4guiPurkinjeNetworkMeshContainer* >( GetDataNode()->GetData() );

  if (mesh == NULL) {
      MITK_INFO << "[sv4guiPurkinjeNetworkInteractor::SelectPoint]  mesh is null ";
      return;
  }

  mitk::Point3D point3d = positionEvent->GetPositionInWorld();
  m_currentPickedPoint = point3d;
  MITK_INFO << "[sv4guiPurkinjeNetworkInteractor::SelectPoint] Point " << point3d[0] << " " <<
    point3d[1] << "  " << point3d[2]; 

  mesh->setPickedPoint(point3d);

  /*
  m_selectedSeed = seeds->findNearestSeed((double)point3d[0], (double)point3d[1], (double)point3d[2], 
    3*m_seedRadius);
  MITK_INFO << "[sv4guiPurkinjeNetworkInteractor::IsOverSeed] Selected seed " << m_selectedSeed[0] << 
    ", " << m_selectedSeed[1] << "\n";
  interactionEvent->GetSender()->GetRenderingManager()->RequestUpdateAll();
  */
  interactionEvent->GetSender()->GetRenderingManager()->RequestUpdateAll();
}

void sv4guiPurkinjeNetworkInteractor::AddStart(mitk::StateMachineAction*, 
    mitk::InteractionEvent* interactionEvent)
{
  //IsOverSeed(interactionEvent);
  sv4guiPurkinjeNetworkMeshContainer* mesh =
        static_cast< sv4guiPurkinjeNetworkMeshContainer* >( GetDataNode()->GetData() );

  if (mesh == NULL) {
      return;
  }

/*
  seeds->addStartSeed((double)m_currentPickedPoint[0],
    (double)m_currentPickedPoint[1],
  (double)m_currentPickedPoint[2]);

  m_currentStartSeed += 1;
  std::cout << m_currentStartSeed << "\n";
*/
  interactionEvent->GetSender()->GetRenderingManager()->RequestUpdateAll();
}

