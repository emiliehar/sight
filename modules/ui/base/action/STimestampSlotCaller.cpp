/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "modules/ui/base/action/STimestampSlotCaller.hpp"

#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>
#include <core/HiResClock.hpp>

#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <regex>

namespace sight::modules::ui::base
{
namespace action
{

fwServicesRegisterMacro(sight::ui::base::IAction, sight::modules::ui::base::action::STimestampSlotCaller,
                        sight::data::Object)

//-----------------------------------------------------------------------------

STimestampSlotCaller::STimestampSlotCaller()
{

}

//-----------------------------------------------------------------------------

STimestampSlotCaller::~STimestampSlotCaller()
{
}

//-----------------------------------------------------------------------------

void STimestampSlotCaller::configuring()
{
    this->initialize();

    ConfigurationType cfg = m_configuration->findConfigurationElement("slots");

    core::runtime::ConfigurationElementContainer slotCfgs = cfg->findAllConfigurationElement("slot");

    std::regex re("(.*)/(.*)");
    std::smatch match;
    std::string src, uid, key;

    for(ConfigurationType elem :  slotCfgs.getElements())
    {
        src = elem->getValue();
        if( std::regex_match(src, match, re) )
        {
            SLM_ASSERT("Wrong value for attribute src: "<<src, match.size() >= 3);
            uid.assign(match[1].first, match[1].second);
            key.assign(match[2].first, match[2].second);

            SLM_ASSERT("Missing hasSlotsId attribute", !uid.empty());
            SLM_ASSERT("Missing slotKey attribute", !key.empty());

            m_slotInfos.push_back( std::make_pair(uid, key) );
        }
    }
}

//-----------------------------------------------------------------------------

void STimestampSlotCaller::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void STimestampSlotCaller::updating()
{
    core::HiResClock::HiResClockType timestamp = core::HiResClock::getTimeInMilliSec();

    for(SlotInfoType info : m_slotInfos)
    {
        HasSlotIDType HasSlotId               = info.first;
        core::com::Slots::SlotKeyType slotKey = info.second;

        if (core::tools::fwID::exist(HasSlotId))
        {
            core::tools::Object::sptr obj      = core::tools::fwID::getObject(HasSlotId);
            core::com::HasSlots::sptr hasSlots = ::std::dynamic_pointer_cast< core::com::HasSlots >(obj);
            SLM_ASSERT("Object with id " << HasSlotId << " is not a HasSlots", hasSlots);

            core::com::SlotBase::sptr slot = hasSlots->slot(slotKey);

            slot->asyncRun(timestamp);
        }
    }
}

//-----------------------------------------------------------------------------

void STimestampSlotCaller::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

} // namespace action
} // namespace sight::modules::ui::base
