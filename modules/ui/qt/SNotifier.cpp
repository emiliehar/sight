/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020-2022 IHU Strasbourg
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

static const core::com::Slots::SlotKeyType s_POP_INFO_SLOT                    = "popInfo";
static const core::com::Slots::SlotKeyType s_POP_SUCCESS_SLOT                 = "popSuccess";
static const core::com::Slots::SlotKeyType s_POP_WARNING_SLOT                 = "popWarning";
static const core::com::Slots::SlotKeyType s_POP_FAILURE_SLOT                 = "popFailure";
static const core::com::Slots::SlotKeyType s_SHOW_PERMANENT_INFO_SLOT         = "showPermanentInfo";
static const core::com::Slots::SlotKeyType s_SHOW_PERMANENT_SUCCESS_SLOT      = "showPermanentSuccess";
static const core::com::Slots::SlotKeyType s_SHOW_PERMANENT_FAILURE_SLOT      = "showPermanentFailure";
static const core::com::Slots::SlotKeyType s_HIDE_PERMANENT_NOTIFICATION_SLOT = "hidePermanentNotification";

static const core::com::Slots::SlotKeyType s_SET_ENUM_PARAMETER_SLOT = "setEnumParameter";

//-----------------------------------------------------------------------------

SNotifier::SNotifier() noexcept
{
    // Initialize map to do conversions.
    m_positionMap["TOP_RIGHT"]       = sight::ui::base::dialog::NotificationDialog::Position::TOP_RIGHT;
    m_positionMap["TOP_LEFT"]        = sight::ui::base::dialog::NotificationDialog::Position::TOP_LEFT;
    m_positionMap["CENTERED_TOP"]    = sight::ui::base::dialog::NotificationDialog::Position::CENTERED_TOP;
    m_positionMap["CENTERED"]        = sight::ui::base::dialog::NotificationDialog::Position::CENTERED;
    m_positionMap["BOTTOM_RIGHT"]    = sight::ui::base::dialog::NotificationDialog::Position::BOTTOM_RIGHT;
    m_positionMap["BOTTOM_LEFT"]     = sight::ui::base::dialog::NotificationDialog::Position::BOTTOM_LEFT;
    m_positionMap["CENTERED_BOTTOM"] = sight::ui::base::dialog::NotificationDialog::Position::CENTERED_BOTTOM;

    newSlot(s_POP_INFO_SLOT, &SNotifier::popInfo, this);
    newSlot(s_POP_SUCCESS_SLOT, &SNotifier::popSuccess, this);
    newSlot(s_POP_WARNING_SLOT, &SNotifier::popWarning, this);
    newSlot(s_POP_FAILURE_SLOT, &SNotifier::popFailure, this);
    newSlot(s_SET_ENUM_PARAMETER_SLOT, &SNotifier::setEnumParameter, this);

    const unsigned int index = 0;
    std::function<void(std::string)> infoTask(
        std::bind(&SNotifier::showPermanentInfo, this, index, std::placeholders::_1));
    auto infoSlot = core::com::newSlot(infoTask);
    this->m_slots(s_SHOW_PERMANENT_INFO_SLOT, infoSlot);
    std::function<void(std::string)> successTask(
        std::bind(&SNotifier::showPermanentSuccess, this, index, std::placeholders::_1));
    auto successSlot = core::com::newSlot(successTask);
    this->m_slots(s_SHOW_PERMANENT_SUCCESS_SLOT, successSlot);
    std::function<void(std::string)> failureTask(
        std::bind(&SNotifier::showPermanentFailure, this, index, std::placeholders::_1));
    auto failureSlot = core::com::newSlot(failureTask);
    this->m_slots(s_SHOW_PERMANENT_FAILURE_SLOT, failureSlot);
    std::function<void()> hideTask(std::bind(&SNotifier::hidePermanentNotification, this, index));
    auto slot = core::com::newSlot(hideTask);
    this->m_slots(s_HIDE_PERMANENT_NOTIFICATION_SLOT, slot);
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

    // Create slots for permanent messages
    m_nbPermanents = configTree.get<unsigned int>("nbPermanents", m_nbPermanents);
    for(unsigned int i = 1 ; i <= m_nbPermanents ; ++i)
    {
        const std::string infoName    = s_SHOW_PERMANENT_INFO_SLOT + std::to_string(i);
        const std::string successName = s_SHOW_PERMANENT_SUCCESS_SLOT + std::to_string(i);
        const std::string failureName = s_SHOW_PERMANENT_FAILURE_SLOT + std::to_string(i);
        const std::string hideName    = s_HIDE_PERMANENT_NOTIFICATION_SLOT + std::to_string(i);
        std::function<void(std::string)> infoTask(
            std::bind(&SNotifier::showPermanentInfo, this, i, std::placeholders::_1));
        auto infoSlot = core::com::newSlot(infoTask);
        infoSlot->setWorker(m_associatedWorker);
        this->m_slots(infoName, infoSlot);
        std::function<void(std::string)> successTask(
            std::bind(&SNotifier::showPermanentSuccess, this, i, std::placeholders::_1));
        auto successSlot = core::com::newSlot(successTask);
        successSlot->setWorker(m_associatedWorker);
        this->m_slots(successName, successSlot);
        std::function<void(std::string)> failureTask(
            std::bind(&SNotifier::showPermanentFailure, this, i, std::placeholders::_1));
        auto failureSlot = core::com::newSlot(failureTask);
        failureSlot->setWorker(m_associatedWorker);
        this->m_slots(failureName, failureSlot);
        std::function<void()> hideTask(std::bind(&SNotifier::hidePermanentNotification, this, i));
        auto slot = core::com::newSlot(hideTask);
        slot->setWorker(m_associatedWorker);
        this->m_slots(hideName, slot);
    }
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

    m_permanentNotifs.resize(m_nbPermanents + 1);
}

//-----------------------------------------------------------------------------

void SNotifier::stopping()
{
    for(auto list : m_permanentNotifs)
    {
        while(!list.empty())
        {
            auto notif = list.front();
            notif->close();
            list.pop();
        }
    }

    m_permanentNotifs.clear();

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

void SNotifier::popWarning(std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::WARNING);
}

//-----------------------------------------------------------------------------

void SNotifier::popFailure(std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::FAILURE);
}

//-----------------------------------------------------------------------------

void SNotifier::showPermanentInfo(unsigned int index, std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::INFO, true, index);
}

//------------------------------------------------------------------------------

void SNotifier::showPermanentSuccess(unsigned int index, std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::SUCCESS, true, index);
}

//------------------------------------------------------------------------------

void SNotifier::showPermanentWarning(unsigned int index, std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::WARNING, true, index);
}

//------------------------------------------------------------------------------

void SNotifier::showPermanentFailure(unsigned int index, std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::FAILURE, true, index);
}

//-----------------------------------------------------------------------------

void SNotifier::showNotification(
    const std::string& _message,
    sight::ui::base::dialog::INotificationDialog::Type _type,
    bool _permanent,
    unsigned int _index
)
{
    // If the maximum number of notification is reached, remove the oldest one.
    if(m_popups.size() >= m_maxStackedNotifs && !m_tempPopups.empty())
    {
        auto notif = m_tempPopups.front();
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
    if(_permanent)
    {
        // if duration < 0, the notification will not be removed automatically
        notif->setDuration(-1);
        m_permanentNotifs[_index].push(notif);
    }
    else
    {
        notif->setDuration(m_durationInMs);
        m_tempPopups.push_back(notif);
    }

    notif->setClosedCallback(std::bind(&SNotifier::onNotificationClosed, this, notif));
    notif->show();

    m_popups.push_back(notif);
}

//------------------------------------------------------------------------------

void SNotifier::hidePermanentNotification(unsigned int index)
{
    if(!m_permanentNotifs[index].empty())
    {
        auto notif = m_permanentNotifs[index].front();
        m_permanentNotifs[index].pop();
        notif->close();
    }
}

//------------------------------------------------------------------------------

void SNotifier::onNotificationClosed(sight::ui::base::dialog::NotificationDialog::sptr notif)
{
    if(this->getStatus() == service::IService::STARTED)
    {
        // remove the notification from the container
        const auto notifItr = std::find(m_popups.begin(), m_popups.end(), notif);
        if(notifItr != m_popups.end())
        {
            // move all the following notifications one index lower
            for(auto it = notifItr ; it != m_popups.end() ; ++it)
            {
                (*it)->moveDown();
            }

            m_popups.erase(notifItr);
        }

        m_tempPopups.remove(notif);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module
