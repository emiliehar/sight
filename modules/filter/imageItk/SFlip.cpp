/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "modules/filter/imageItk/SFlip.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <filter/imageItk/Flipper.hpp>

namespace sight::module::filter::imageItk
{

static const service::IService::KeyType s_IMAGE_IN  = "source";
static const service::IService::KeyType s_IMAGE_OUT = "target";

const core::com::Slots::SlotKeyType SFlip::s_FLIP_AXISX_SLOT = "flipAxisX";
const core::com::Slots::SlotKeyType SFlip::s_FLIP_AXISY_SLOT = "flipAxisY";
const core::com::Slots::SlotKeyType SFlip::s_FLIP_AXISZ_SLOT = "flipAxisZ";

//------------------------------------------------------------------------------

SFlip::SFlip()
{
    // Initialize the slots
    newSlot(s_FLIP_AXISX_SLOT, &SFlip::flipAxisX, this);
    newSlot(s_FLIP_AXISY_SLOT, &SFlip::flipAxisY, this);
    newSlot(s_FLIP_AXISZ_SLOT, &SFlip::flipAxisZ, this);
}

//------------------------------------------------------------------------------

SFlip::~SFlip()
{
}

//------------------------------------------------------------------------------

void SFlip::configuring()
{
}

//------------------------------------------------------------------------------

void SFlip::starting()
{
}

//------------------------------------------------------------------------------

void SFlip::updating()
{
    data::Image::csptr inImg = this->getInput<data::Image>(s_IMAGE_IN);

    SIGHT_ASSERT("No 'imageIn' found !", inImg);
    if(inImg)
    {
        data::mt::ObjectReadLock inImLock(inImg);

        data::Image::sptr outImg = data::Image::New();

        sight::filter::imageItk::Flipper::flip(inImg, outImg, m_flipAxes);

        this->setOutput(s_IMAGE_OUT, outImg);

        m_sigComputed->asyncEmit();
    }
    else
    {
        SIGHT_ERROR("An update was triggered with an empty image as input. No output image was created.");
    }
}

//------------------------------------------------------------------------------

void SFlip::stopping()
{
}

//------------------------------------------------------------------------------

void SFlip::flipAxisX()
{
    m_flipAxes[0] = !(m_flipAxes[0]);
    this->updating();
}

//------------------------------------------------------------------------------

void SFlip::flipAxisY()
{
    m_flipAxes[1] = !(m_flipAxes[1]);
    this->updating();
}

//------------------------------------------------------------------------------

void SFlip::flipAxisZ()
{
    m_flipAxes[2] = !(m_flipAxes[2]);
    this->updating();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SFlip::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_IMAGE_IN, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_IN, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

} // namespace sight::module::filter::imageItk
