/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "modules/ui/qt/SNotifier.hpp"

#include <core/base.hpp>
#include <core/com/Slots.hxx>

#include <service/macros.hpp>

#include <ui/base/GuiRegistry.hpp>

namespace sight::module::ui::qt
{

static const core::com::Slots::SlotKeyType s_POP_INFO_SLOT               = "popInfo";
static const core::com::Slots::SlotKeyType s_POP_SUCCESS_SLOT            = "popSuccess";
static const core::com::Slots::SlotKeyType s_POP_FAILURE_SLOT            = "popFailure";
static const core::com::Slots::SlotKeyType s_SHOW_PERMANENT_INFO_SLOT    = "showPermanentInfo";
static const core::com::Slots::SlotKeyType s_SHOW_PERMANENT_SUCCESS_SLOT = "showPermanentSuccess";
static const core::com::Slots::SlotKeyType s_SHOW_PERMANENT_FAILURE_SLOT = "showPermanentFailure";
static const core::com::Slots::SlotKeyType s_HIDE_PERMANENT_INFO_SLOT    = "hidePermanentInfo";
static const core::com::Slots::SlotKeyType s_HIDE_PERMANENT_SUCCESS_SLOT = "hidePermanentSuccess";
static const core::com::Slots::SlotKeyType s_HIDE_PERMANENT_FAILURE_SLOT = "hidePermanentFailure";

static const core::com::Slots::SlotKeyType s_SET_ENUM_PARAMETER_SLOT = "setEnumParameter";

//-----------------------------------------------------------------------------

SNotifier::SNotifier() noexcept
{
    // Initialize map to do conversions.
    m_positionMap["TOP_RIGHT"]       = sight::ui::base::dialog::NotificationDialog::Position::TOP_RIGHT;
    m_positionMap["TOP_LEFT"]        = sight::ui::base::dialog::NotificationDialog::Position::TOP_LEFT;
    m_positionMap["CENTERED_TOP"]    = sight::ui::base::dialog::NotificationDialog::Position::CENTERED_TOP;
    m_positionMap["CENTERED"]        = sight::ui::base::dialog::NotificationDialog::Position::CENTERED;
    m_positionMap["BOTTOM_RIGHT"]    = sight::ui::base::dialog::NotificationDialog::Position::BOTTOM_LEFT;
    m_positionMap["CENTERED_BOTTOM"] = sight::ui::base::dialog::NotificationDialog::Position::CENTERED_BOTTOM;

    newSlot(s_POP_INFO_SLOT, &SNotifier::popInfo, this);
    newSlot(s_POP_SUCCESS_SLOT, &SNotifier::popSuccess, this);
    newSlot(s_POP_FAILURE_SLOT, &SNotifier::popFailure, this);
    newSlot(s_SHOW_PERMANENT_INFO_SLOT, &SNotifier::showPermanentInfo, this);
    newSlot(s_SHOW_PERMANENT_SUCCESS_SLOT, &SNotifier::showPermanentSuccess, this);
    newSlot(s_SHOW_PERMANENT_FAILURE_SLOT, &SNotifier::showPermanentFailure, this);
    newSlot(s_HIDE_PERMANENT_INFO_SLOT, &SNotifier::hidePermanentInfo, this);
    newSlot(s_HIDE_PERMANENT_SUCCESS_SLOT, &SNotifier::hidePermanentSuccess, this);
    newSlot(s_HIDE_PERMANENT_FAILURE_SLOT, &SNotifier::hidePermanentFailure, this);
    newSlot(s_SET_ENUM_PARAMETER_SLOT, &SNotifier::setEnumParameter, this);
}

//-----------------------------------------------------------------------------

SNotifier::~SNotifier() noexcept
{
    m_positionMap.clear();
}

//-----------------------------------------------------------------------------

void SNotifier::configuring()
{
    const auto configTree = this->getConfigTree();
    m_defaultMessage = configTree.get<std::string>("message", m_defaultMessage);
    const std::string position = configTree.get<std::string>("position", "TOP_RIGHT");

    if(m_positionMap.find(position) != m_positionMap.end())
    {
        m_notifcationsPosition = m_positionMap[position];
    }
    else
    {
        SIGHT_ERROR(
            "Position '" + position + "' isn't a valid position value, accepted values are:"
                                      "TOP_RIGHT, TOP_LEFT, CENTERED_TOP, CENTERED, BOTTOM_RIGHT, BOTTOM_LEFT, CENTERED_BOTTOM."
        )
    }

    m_durationInMs = configTree.get<int>("duration", m_durationInMs);

    m_maxStackedNotifs = configTree.get<std::uint8_t>("maxNotifications", m_maxStackedNotifs);

    m_parentContainerID = configTree.get<std::string>("parent.<xmlattr>.uid", m_parentContainerID);
}

//-----------------------------------------------------------------------------

void SNotifier::starting()
{
    if(!m_parentContainerID.empty())
    {
        auto container = ::sight::ui::base::GuiRegistry::getSIDContainer(m_parentContainerID);

        if(!container)
        {
            container = ::sight::ui::base::GuiRegistry::getWIDContainer(m_parentContainerID);
        }

        // If we have an SID/WID set the container.
        if(container)
        {
            m_containerWhereToDisplayNotifs = container;
        }
    }
}

//-----------------------------------------------------------------------------

void SNotifier::stopping()
{
    while(!m_permanentInfoNotif.empty())
    {
        auto notif = m_permanentInfoNotif.front();
        notif->close();
        m_permanentInfoNotif.pop();
    }

    while(!m_permanentSuccessNotif.empty())
    {
        auto notif = m_permanentSuccessNotif.front();
        notif->close();
        m_permanentSuccessNotif.pop();
    }

    while(!m_permanentFailureNotif.empty())
    {
        auto notif = m_permanentFailureNotif.front();
        notif->close();
        m_permanentFailureNotif.pop();
    }

    m_popups.clear();
}

//-----------------------------------------------------------------------------

void SNotifier::updating()
{
}

//-----------------------------------------------------------------------------

void SNotifier::setEnumParameter(std::string _val, std::string _key)
{
    if(_key == "position")
    {
        if(m_positionMap.find(_val) != m_positionMap.end())
        {
            m_notifcationsPosition = m_positionMap[_val];
        }
        else
        {
            SIGHT_ERROR("Value '" + _val + "' is not handled for key " + _key);
        }
    }
    else
    {
        SIGHT_ERROR("Value '" + _val + "' is not handled for key " + _key);
    }
}

//-----------------------------------------------------------------------------

void SNotifier::popInfo(std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::INFO);
}

//-----------------------------------------------------------------------------

void SNotifier::popSuccess(std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::SUCCESS);
}

//-----------------------------------------------------------------------------

void SNotifier::popFailure(std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::FAILURE);
}

//-----------------------------------------------------------------------------

void SNotifier::showPermanentInfo(std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::INFO, true);
}

//------------------------------------------------------------------------------

void SNotifier::showPermanentSuccess(std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::SUCCESS, true);
}

//------------------------------------------------------------------------------

void SNotifier::showPermanentFailure(std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::FAILURE, true);
}

//------------------------------------------------------------------------------

void SNotifier::hidePermanentInfo()
{
    this->hidePermanentNotification(sight::ui::base::dialog::INotificationDialog::Type::INFO);
}

//------------------------------------------------------------------------------

void SNotifier::hidePermanentSuccess()
{
    this->hidePermanentNotification(sight::ui::base::dialog::INotificationDialog::Type::SUCCESS);
}

//------------------------------------------------------------------------------

void SNotifier::hidePermanentFailure()
{
    this->hidePermanentNotification(sight::ui::base::dialog::INotificationDialog::Type::FAILURE);
}

//-----------------------------------------------------------------------------

void SNotifier::showNotification(
    const std::string& _message,
    sight::ui::base::dialog::INotificationDialog::Type _type,
    bool permanent
)
{
    // If the maximum number of notification is reached, remove the oldest one.
    if(m_popups.size() >= m_maxStackedNotifs)
    {
        auto notif = m_popups.front();
        notif->close();
        this->onNotificationClosed(notif);
    }

    std::string messageToShow;

    if(_message.empty())
    {
        messageToShow = m_defaultMessage;
    }
    else
    {
        messageToShow = _message;
    }

    sight::ui::base::dialog::NotificationDialog::sptr notif =
        sight::ui::base::dialog::NotificationDialog::New();

    notif->setContainer(m_containerWhereToDisplayNotifs);

    notif->setMessage(messageToShow);
    notif->setType(_type);
    notif->setPosition(m_notifcationsPosition);
    notif->setIndex(static_cast<unsigned int>(m_popups.size()));
    if(permanent)
    {
        // if duration < 0, the notification will not be removed automatically
        notif->setDuration(-1);
        switch(_type)
        {
            case sight::ui::base::dialog::INotificationDialog::Type::INFO:
                m_permanentInfoNotif.push(notif);
                break;

            case sight::ui::base::dialog::INotificationDialog::Type::SUCCESS:
                m_permanentSuccessNotif.push(notif);
                break;

            case sight::ui::base::dialog::INotificationDialog::Type::FAILURE:
                m_permanentFailureNotif.push(notif);
                break;

            default:
                SIGHT_FATAL("Type is not managed");
        }
    }
    else
    {
        notif->setDuration(m_durationInMs);
    }

    notif->setClosedCallback(std::bind(&SNotifier::onNotificationClosed, this, notif));
    notif->show();

    m_popups.push_back(notif);
}

//------------------------------------------------------------------------------

void SNotifier::hidePermanentNotification(sight::ui::base::dialog::NotificationDialog::Type _type)
{
    sight::ui::base::dialog::NotificationDialog::sptr notif;
    switch(_type)
    {
        case sight::ui::base::dialog::INotificationDialog::Type::INFO:
            notif = m_permanentInfoNotif.front();
            m_permanentInfoNotif.pop();
            break;

        case sight::ui::base::dialog::INotificationDialog::Type::SUCCESS:
            notif = m_permanentSuccessNotif.front();
            m_permanentSuccessNotif.pop();
            break;

        case sight::ui::base::dialog::INotificationDialog::Type::FAILURE:
            notif = m_permanentFailureNotif.front();
            m_permanentFailureNotif.pop();
            break;

        default:
            SIGHT_FATAL("Type is not managed");
    }

    notif->close();
}

//------------------------------------------------------------------------------

void SNotifier::onNotificationClosed(sight::ui::base::dialog::NotificationDialog::sptr notif)
{
    // remove the notification from the container
    const auto notifItr = std::find(m_popups.begin(), m_popups.end(), notif);
    if(notifItr != m_popups.end())
    {
        m_popups.erase(notifItr);

        // move all the remaining notifications one index lower
        for(auto popup : m_popups)
        {
            popup->moveDown();
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module
