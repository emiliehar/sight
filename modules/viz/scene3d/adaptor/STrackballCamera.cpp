/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2021 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/STrackballCamera.hpp"

namespace sight::module::viz::scene3d::adaptor
{

static const std::string s_PRIORITY_CONFIG              = "priority";
static const std::string s_LAYER_ORDER_DEPENDANT_CONFIG = "layerOrderDependant";

static const core::com::Slots::SlotKeyType s_RESET_CAMERA_SLOT      = "resetCamera";
static const core::com::Slots::SlotKeyType s_LEFT_TO_RIGHT_SLOT     = "translateLeftToRightButton";
static const core::com::Slots::SlotKeyType s_LEFT_TO_MIDDLE_SLOT    = "translateLeftToMiddleButton";
static const core::com::Slots::SlotKeyType s_RESET_INTERACTION_SLOT = "resetDefaultInteraction";

//-----------------------------------------------------------------------------

STrackballCamera::STrackballCamera() noexcept
{
    newSlot(s_RESET_CAMERA_SLOT, &STrackballCamera::resetCamera, this);
    newSlot(s_LEFT_TO_RIGHT_SLOT, &STrackballCamera::translateLeftToRightButton, this);
    newSlot(s_LEFT_TO_MIDDLE_SLOT, &STrackballCamera::translateLeftToMiddleButton, this);
    newSlot(s_RESET_INTERACTION_SLOT, &STrackballCamera::resetDefaultInteraction, this);
}

//-----------------------------------------------------------------------------

STrackballCamera::~STrackballCamera() noexcept
{
}

//-----------------------------------------------------------------------------

void STrackballCamera::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_priority            = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_layerOrderDependant = config.get<bool>(s_LAYER_ORDER_DEPENDANT_CONFIG, m_layerOrderDependant);
}

//-----------------------------------------------------------------------------

void STrackballCamera::starting()
{
    this->initialize();

    const auto layer = this->getLayer();
    m_trackball =
        std::make_shared<sight::viz::scene3d::interactor::TrackballInteractor>(layer, m_layerOrderDependant);

    layer->addInteractor(m_trackball, m_priority);
}

//-----------------------------------------------------------------------------

void STrackballCamera::updating() noexcept
{
}

//-----------------------------------------------------------------------------

void STrackballCamera::stopping()
{
    const auto layer = this->getLayer();
    layer->removeInteractor(m_trackball);
}

//------------------------------------------------------------------------------

void STrackballCamera::resetCamera()
{
    m_trackball->resetCameraCoordinates();
}

//------------------------------------------------------------------------------

void STrackballCamera::translateLeftToRightButton()
{
    m_trackball->setInteractionMode(sight::viz::scene3d::interactor::TrackballInteractor::InteractionMode::LEFT_TO_RIGHT);
}

//------------------------------------------------------------------------------

void STrackballCamera::translateLeftToMiddleButton()
{
    m_trackball->setInteractionMode(
        sight::viz::scene3d::interactor::TrackballInteractor::InteractionMode::LEFT_TO_MIDDLE
    );
}

//------------------------------------------------------------------------------

void STrackballCamera::resetDefaultInteraction()
{
    m_trackball->setInteractionMode(sight::viz::scene3d::interactor::TrackballInteractor::InteractionMode::DEFAULT);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
