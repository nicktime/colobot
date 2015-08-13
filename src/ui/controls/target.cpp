/*
 * This file is part of the Colobot: Gold Edition source code
 * Copyright (C) 2001-2014, Daniel Roux, EPSITEC SA & TerranovaTeam
 * http://epsiteс.ch; http://colobot.info; http://github.com/colobot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://gnu.org/licenses
 */


#include "ui/controls/target.h"

#include "level/robotmain.h"

#include "object/object_manager.h"
#include "object/old_object.h"


namespace Ui
{
// Object's constructor.

CTarget::CTarget() : CControl()
{
}

// Object's destructor.

CTarget::~CTarget()
{
}


// Creates a new button.

bool CTarget::Create(Math::Point pos, Math::Point dim, int icon, EventType eventType)
{
    if ( eventType == EVENT_NULL )  eventType = GetUniqueEventType();

    CControl::Create(pos, dim, icon, eventType);

    return true;
}


// Management of an event.

bool CTarget::EventProcess(const Event &event)
{
    CObject*    pObj;

    if ( (m_state & STATE_VISIBLE) == 0 )  return true;
    if ( m_state & STATE_DEAD )  return true;

    CControl::EventProcess(event);

    if ( event.type == EVENT_MOUSE_MOVE )
    {
        m_main->SetFriendAim(false);

        if ( CControl::Detect(event.mousePos) )
        {
            pObj = DetectFriendObject(event.mousePos);
            if ( pObj == 0 )
            {
                m_engine->SetMouseType(Gfx::ENG_MOUSE_TARGET);

                m_event->AddEvent(Event(m_eventType));
                return false;
            }
            else
            {
                m_main->SetFriendAim(true);
                m_engine->SetMouseType(Gfx::ENG_MOUSE_NORM);
            }
        }
    }

    if (event.type == EVENT_MOUSE_BUTTON_DOWN &&
        event.GetData<MouseButtonEventData>()->button == MOUSE_BUTTON_LEFT &&
        (m_state & STATE_VISIBLE) &&
        (m_state & STATE_ENABLE))
    {
        if ( CControl::Detect(event.mousePos) )
        {
            if ( !m_main->GetFriendAim() )
            {
                m_event->AddEvent(Event(EVENT_OBJECT_FIRE));
                return false;
            }
        }
    }

    return true;
}


// Draws button.

void CTarget::Draw()
{
    // It is completely invisible!
}


// Returns the tooltip.

bool CTarget::GetTooltip(Math::Point pos, std::string &name)
{
    if ( (m_state & STATE_VISIBLE) == 0 )  return false;

    if ( (m_state&STATE_VISIBLE) && Detect(pos) )  // in the window?
    {
        if ( !m_main->GetFriendAim() )
        {
            m_tooltip = name;
            return true;  // does not detect objects below!
        }
    }

    return false;
}


// Detects the object aimed by the mouse.

CObject* CTarget::DetectFriendObject(Math::Point pos)
{
    int objRank = m_engine->DetectObject(pos);

    for (CObject* obj : CObjectManager::GetInstancePointer()->GetAllObjects())
    {
        if ( !obj->GetActive() )  continue;
        if ( obj->GetProxyActivate() )  continue;
        if ( obj->Implements(ObjectInterfaceType::Controllable) && dynamic_cast<CControllableObject*>(obj)->GetSelect() )  continue;

        CObject* target = nullptr;
        ObjectType type = obj->GetType();
        if ( type == OBJECT_DERRICK      ||
             type == OBJECT_FACTORY      ||
             type == OBJECT_REPAIR       ||
             type == OBJECT_DESTROYER    ||
             type == OBJECT_STATION      ||
             type == OBJECT_CONVERT      ||
             type == OBJECT_TOWER        ||
             type == OBJECT_RESEARCH     ||
             type == OBJECT_RADAR        ||
             type == OBJECT_INFO         ||
             type == OBJECT_ENERGY       ||
             type == OBJECT_LABO         ||
             type == OBJECT_NUCLEAR      ||
             type == OBJECT_PARA         ||
             type == OBJECT_SAFE         ||
             type == OBJECT_HUSTON       ||
             type == OBJECT_HUMAN        ||
             type == OBJECT_TECH         ||
             type == OBJECT_TOTO         ||
             type == OBJECT_MOBILEfa     ||
             type == OBJECT_MOBILEta     ||
             type == OBJECT_MOBILEwa     ||
             type == OBJECT_MOBILEia     ||
             type == OBJECT_MOBILEfc     ||
             type == OBJECT_MOBILEtc     ||
             type == OBJECT_MOBILEwc     ||
             type == OBJECT_MOBILEic     ||
             type == OBJECT_MOBILEfi     ||
             type == OBJECT_MOBILEti     ||
             type == OBJECT_MOBILEwi     ||
             type == OBJECT_MOBILEii     ||
             type == OBJECT_MOBILEfs     ||
             type == OBJECT_MOBILEts     ||
             type == OBJECT_MOBILEws     ||
             type == OBJECT_MOBILEis     ||
             type == OBJECT_MOBILErt     ||
             type == OBJECT_MOBILErc     ||
             type == OBJECT_MOBILErr     ||
             type == OBJECT_MOBILErs     ||
             type == OBJECT_MOBILEsa     ||
             type == OBJECT_MOBILEft     ||
             type == OBJECT_MOBILEtt     ||
             type == OBJECT_MOBILEwt     ||
             type == OBJECT_MOBILEit     ||
             type == OBJECT_MOBILEdr     )
        {
            target = obj;
        }
        else if ( (type == OBJECT_POWER  ||
                  type == OBJECT_ATOMIC ) &&
             IsObjectBeingTransported(obj) )  // battery used?
        {
            target = dynamic_cast<CTransportableObject*>(obj)->GetTransporter();
            if ( target->GetType() == OBJECT_MOBILEtg )
            {
                target = nullptr;
            }
        }

        for (int j=0 ; j<OBJECTMAXPART ; j++ )
        {
            int rank = obj->GetObjectRank(j);
            if ( rank == -1 )  continue;
            if ( rank != objRank )  continue;
            return target;
        }
    }

    return nullptr;
}

}
