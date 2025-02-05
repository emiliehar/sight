/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "modules/ui/qt/calibration/SDisplayCalibrationInfo.hpp"

#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/EConfigurationElement.hpp>

#include <data/CalibrationInfo.hpp>
#include <data/String.hpp>

#include <service/extension/AppConfig.hpp>
#include <service/macros.hpp>
#include <service/registry/Proxy.hpp>

#include <sstream>

namespace sight::module::ui::qt::calibration
{

//------------------------------------------------------------------------------

static const core::com::Slots::SlotKeyType s_DISPLAY_IMAGE_SLOT = "displayImage";
static const core::com::Slots::SlotKeyType s_STOP_CONFIG_SLOT   = "stopConfig";

static const std::string s_CLOSE_CONFIG_CHANNEL_ID = "CLOSE_CONFIG_CHANNEL";

static const std::string s_CALIBRATION_INFO_1 = "calInfo1";
static const std::string s_CALIBRATION_INFO_2 = "calInfo2";

//------------------------------------------------------------------------------

SDisplayCalibrationInfo::SDisplayCalibrationInfo() noexcept
{
    newSlot(s_DISPLAY_IMAGE_SLOT, &SDisplayCalibrationInfo::displayImage, this);
    newSlot(s_STOP_CONFIG_SLOT, &SDisplayCalibrationInfo::stopConfig, this);
}

//------------------------------------------------------------------------------

SDisplayCalibrationInfo::~SDisplayCalibrationInfo() noexcept
{
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::starting()
{
    m_proxychannel = this->getID() + "_stopConfig";
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::stopping()
{
    if(m_configMgr)
    {
        service::registry::Proxy::sptr proxies = service::registry::Proxy::getDefault();
        proxies->disconnect(m_proxychannel, this->slot(s_STOP_CONFIG_SLOT));
        m_configMgr->stopAndDestroy();
        m_configMgr.reset();
    }
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::configuring()
{
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::updating()
{
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::stopConfig()
{
    if(m_configMgr)
    {
        this->stopping();
    }
}

//------------------------------------------------------------------------------

void SDisplayCalibrationInfo::displayImage(size_t idx)
{
    if(!m_configMgr)
    {
        // Grab images from our composite data
        data::CalibrationInfo::csptr calInfo1 = this->getInput<data::CalibrationInfo>(s_CALIBRATION_INFO_1);
        SIGHT_ASSERT("Object " + s_CALIBRATION_INFO_1 + " is not a CalibrationInfo !", calInfo1);

        data::CalibrationInfo::csptr calInfo2 = this->getInput<data::CalibrationInfo>(s_CALIBRATION_INFO_2);

        std::string strConfig = "displayImageConfig";

        // Prepare configuration
        service::FieldAdaptorType replaceMap;

        data::Image::sptr img1 = calInfo1->getImage(idx);
        replaceMap["imageId1"] = img1->getID();
        data::PointList::sptr pointList1 = calInfo1->getPointList(img1);
        replaceMap["pointListId1"] = pointList1->getID();

        core::runtime::ConfigurationElement::csptr config;
        if(calInfo2)
        {
            strConfig = "displayTwoImagesConfig";

            data::Image::sptr img2 = calInfo2->getImage(idx);
            replaceMap["imageId2"] = img2->getID();
            data::PointList::sptr pointList2 = calInfo2->getPointList(img2);
            replaceMap["pointListId2"] = pointList2->getID();
        }

        replaceMap[s_CLOSE_CONFIG_CHANNEL_ID] = m_proxychannel;

        config =
            service::extension::AppConfig::getDefault()->getAdaptedTemplateConfig(strConfig, replaceMap, true);

        // Launch configuration
        m_configMgr = service::AppConfigManager::New();
        m_configMgr->service::IAppConfigManager::setConfig(config);
        m_configMgr->launch();

        // Proxy to be notified of the window closure
        service::registry::Proxy::sptr proxies = service::registry::Proxy::getDefault();
        proxies->connect(m_proxychannel, this->slot(s_STOP_CONFIG_SLOT));
    }
}

//------------------------------------------------------------------------------

} // uiCalibration
