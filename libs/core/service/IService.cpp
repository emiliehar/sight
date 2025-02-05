/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2022 IHU Strasbourg
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

#include "service/IService.hpp"

#include "core/thread/ActiveWorkers.hpp"

#include "service/registry/ObjectService.hpp"
#include "service/registry/Proxy.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/Convert.hpp>
#include <core/runtime/EConfigurationElement.hpp>
#include <core/thread/Worker.hpp>
#include <core/tools/fwID.hpp>

#include <functional>
#include <regex>

namespace sight::service
{

//-----------------------------------------------------------------------------

const core::com::Signals::SignalKeyType IService::s_STARTED_SIG          = "started";
const core::com::Signals::SignalKeyType IService::s_UPDATED_SIG          = "updated";
const core::com::Signals::SignalKeyType IService::s_SWAPPED_SIG          = "swapped";
const core::com::Signals::SignalKeyType IService::s_STOPPED_SIG          = "stopped";
const core::com::Signals::SignalKeyType IService::s_INFO_NOTIFIED_SIG    = "infoNotified";
const core::com::Signals::SignalKeyType IService::s_SUCCESS_NOTIFIED_SIG = "successNotified";
const core::com::Signals::SignalKeyType IService::s_WARNING_NOTIFIED_SIG = "warningNotified";
const core::com::Signals::SignalKeyType IService::s_FAILURE_NOTIFIED_SIG = "failureNotified";

const core::com::Slots::SlotKeyType IService::s_START_SLOT   = "start";
const core::com::Slots::SlotKeyType IService::s_STOP_SLOT    = "stop";
const core::com::Slots::SlotKeyType IService::s_UPDATE_SLOT  = "update";
const core::com::Slots::SlotKeyType IService::s_SWAP_SLOT    = "swap";
const core::com::Slots::SlotKeyType IService::s_SWAPKEY_SLOT = "swapKey";

//-----------------------------------------------------------------------------

IService::IService() :
    m_configuration(new core::runtime::EConfigurationElement("EmptyConfigurationElement")),
    m_globalState(STOPPED),
    m_updatingState(NOTUPDATING),
    m_configurationState(UNCONFIGURED)
{
    newSignal<StartedSignalType>(s_STARTED_SIG);
    newSignal<UpdatedSignalType>(s_UPDATED_SIG);
    newSignal<SwappedSignalType>(s_SWAPPED_SIG);
    newSignal<StoppedSignalType>(s_STOPPED_SIG);
    newSignal<InfoNotifiedSignalType>(s_INFO_NOTIFIED_SIG);
    newSignal<SuccessNotifiedSignalType>(s_SUCCESS_NOTIFIED_SIG);
    newSignal<WarningNotifiedSignalType>(s_WARNING_NOTIFIED_SIG);
    newSignal<FailureNotifiedSignalType>(s_FAILURE_NOTIFIED_SIG);

    m_slotStart   = newSlot(s_START_SLOT, &IService::startSlot, this);
    m_slotStop    = newSlot(s_STOP_SLOT, &IService::stopSlot, this);
    m_slotUpdate  = newSlot(s_UPDATE_SLOT, &IService::updateSlot, this);
    m_slotSwapKey = newSlot(s_SWAPKEY_SLOT, &IService::swapKeySlot, this);
}

//-----------------------------------------------------------------------------

IService::~IService()
{
    // check if the service manage outputs that are not maintained by someone else.
    if(!m_outputsMap.empty())
    {
        std::string objectKeys;
        for(const auto& obj : m_outputsMap)
        {
            const data::Object::wptr output = obj.second.get_shared();
            if(output.use_count() == 1)
            {
                if(!objectKeys.empty())
                {
                    objectKeys += ", ";
                }

                objectKeys += "'" + obj.first + "'(nbRef: " + std::to_string(output.use_count()) + ")";
            }
        }

        SIGHT_WARN_IF(
            "Service " + this->getID() + " still contains registered outputs: " + objectKeys + ". They will no "
                                                                                               "longer be maintained. You should call setOutput(key, nullptr) before stopping the service to inform "
                                                                                               "AppManager and other services that the object will be destroyed.",
            !objectKeys.empty()
        );
    }
}

//-----------------------------------------------------------------------------

void IService::info(std::ostream&)
{
}

//-----------------------------------------------------------------------------

void IService::setOutput(const IService::KeyType& key, const data::Object::sptr& object, size_t index)
{
    std::string outKey = key;

    if(m_keyGroupSize.find(key) != m_keyGroupSize.end())
    {
        outKey = KEY_GROUP_NAME(key, index);
    }

    if(service::OSR::isRegistered(outKey, service::IService::AccessType::OUTPUT, this->getSptr()))
    {
        service::OSR::unregisterServiceOutput(outKey, this->getSptr());
    }

    if(object != nullptr)
    {
        service::OSR::registerServiceOutput(object, outKey, this->getSptr());
    }
}

//------------------------------------------------------------------------------

void IService::registerInput(
    const data::Object::csptr& obj,
    const std::string& key,
    const bool autoConnect,
    const bool optional
)
{
    this->registerObject(obj->getID(), key, AccessType::INPUT, autoConnect, optional);

    service::OSR::registerServiceInput(obj, key, this->getSptr());
}

//------------------------------------------------------------------------------

void IService::unregisterInput(const std::string& key)
{
    this->unregisterObject(key, AccessType::INPUT);
}

//------------------------------------------------------------------------------

void IService::registerInOut(
    const data::Object::sptr& obj,
    const std::string& key,
    const bool autoConnect,
    const bool optional
)
{
    this->registerObject(obj, key, AccessType::INOUT, autoConnect, optional);
}

//------------------------------------------------------------------------------

void IService::unregisterInOut(const std::string& key)
{
    this->unregisterObject(key, AccessType::INOUT);
}

//------------------------------------------------------------------------------

void IService::registerObject(
    const data::Object::sptr& obj,
    const std::string& key,
    AccessType access,
    const bool autoConnect,
    const bool optional
)
{
    this->registerObject(key, access, autoConnect, optional);

    if(access == AccessType::INPUT)
    {
        m_inputsMap[key] = obj;
    }
    else if(access == AccessType::INOUT)
    {
        m_inOutsMap[key] = obj;
    }
    else if(access == AccessType::OUTPUT)
    {
        m_outputsMap[key] = obj;
    }

    service::OSR::registerService(obj, key, access, this->getSptr());
}

//------------------------------------------------------------------------------

void IService::registerObject(
    const std::string& objId,
    const service::IService::KeyType& key,
    const service::IService::AccessType access,
    const bool autoConnect,
    const bool optional
)
{
    this->registerObject(key, access, autoConnect, optional);
    SIGHT_ASSERT("Object id must be defined", !objId.empty());
    this->setObjectId(key, objId);
}

//------------------------------------------------------------------------------

void IService::unregisterObject(const std::string& key, AccessType access)
{
    service::OSR::unregisterService(key, access, this->getSptr());

    if(access == service::IService::AccessType::INPUT)
    {
        m_inputsMap.erase(key);
    }
    else if(access == service::IService::AccessType::INOUT)
    {
        m_inOutsMap.erase(key);
    }
    else
    {
        m_outputsMap.erase(key);
    }
}

//------------------------------------------------------------------------------

void IService::unregisterObject(const std::string& objId)
{
    auto itr = std::find_if(
        m_serviceConfig.m_objects.begin(),
        m_serviceConfig.m_objects.end(),
        [&](const ObjectServiceConfig& config)
        {
            return config.m_uid == objId;
        });

    if(itr == m_serviceConfig.m_objects.end())
    {
        SIGHT_ERROR("object '" + objId + "' is not registered");
        return;
    }

    m_serviceConfig.m_objects.erase(itr);
}

//-----------------------------------------------------------------------------

bool IService::hasObjectId(const KeyType& _key) const
{
    bool hasId = false;
    auto itr   = std::find_if(
        m_serviceConfig.m_objects.begin(),
        m_serviceConfig.m_objects.end(),
        [&](const ObjectServiceConfig& objCfg)
        {
            return objCfg.m_key == _key;
        });

    if(itr != m_serviceConfig.m_objects.end())
    {
        hasId = (!itr->m_uid.empty());
    }

    return hasId;
}

//-----------------------------------------------------------------------------

IService::IdType IService::getObjectId(const IService::KeyType& _key) const
{
    const ObjectServiceConfig& cfg = this->getObjInfoFromKey(_key);
    SIGHT_THROW_IF("Object key '" + _key + "' is not found for service '" + this->getID() + "'", cfg.m_uid.empty());
    return cfg.m_uid;
}

//-----------------------------------------------------------------------------

void IService::setObjectId(const IService::KeyType& _key, const IService::IdType& _id)
{
    auto keyItr = std::find_if(
        m_serviceConfig.m_objects.begin(),
        m_serviceConfig.m_objects.end(),
        [&](const ObjectServiceConfig& objCfg)
        {
            return objCfg.m_key == _key;
        });
    SIGHT_THROW_IF(
        "key '" + _key + "' is not regisreted for '" + this->getID() + "'.",
        keyItr == m_serviceConfig.m_objects.end()
    );
    ObjectServiceConfig& cfg = *keyItr;
    cfg.m_uid = _id;
}

//-----------------------------------------------------------------------------

void IService::setObjectId(const IService::KeyType& _key, const size_t index, const IService::IdType& _id)
{
    const std::string groupKey = KEY_GROUP_NAME(_key, index);

    this->setObjectId(groupKey, _id);
    if(index >= this->getKeyGroupSize(_key))
    {
        m_keyGroupSize[_key] = index + 1;
    }
}

//-----------------------------------------------------------------------------

void displayPt(::boost::property_tree::ptree& pt, std::string indent = "")
{
    SIGHT_ERROR(indent << " data : '" << pt.data() << "'");

    for(::boost::property_tree::ptree::value_type& v : pt)
    {
        SIGHT_ERROR((indent + "  '") << v.first << "':");
        displayPt(v.second, indent + "      ");
    }
}

//-----------------------------------------------------------------------------

void IService::setConfiguration(const core::runtime::ConfigurationElement::sptr _cfgElement)
{
    SIGHT_ASSERT("Invalid ConfigurationElement", _cfgElement);
    m_configuration      = _cfgElement;
    m_configurationState = UNCONFIGURED;
}

//-----------------------------------------------------------------------------

void IService::setConfiguration(const Config& _configuration)
{
    SIGHT_ASSERT("Invalid ConfigurationElement", _configuration.m_config);

    // TODO: Remove this ugly const_cast
    m_configuration      = core::runtime::ConfigurationElement::constCast(_configuration.m_config);
    m_configurationState = UNCONFIGURED;

    m_serviceConfig = _configuration;
}

//-----------------------------------------------------------------------------

void IService::setConfiguration(const ConfigType& ptree)
{
    core::runtime::ConfigurationElement::sptr ce;

    ConfigType serviceConfig;
    serviceConfig.add_child("service", ptree);

    ce = core::runtime::Convert::fromPropertyTree(serviceConfig);

    SIGHT_ASSERT("Invalid ConfigurationElement", ce);

    this->setConfiguration(ce);
}

//-----------------------------------------------------------------------------

core::runtime::ConfigurationElement::sptr IService::getConfiguration() const
{
    return m_configuration;
}

//-----------------------------------------------------------------------------

IService::ConfigType IService::getConfigTree() const
{
    const auto configTree = core::runtime::Convert::toPropertyTree(this->getConfiguration());

    // This is in case we get the configuration from a service::extension::Config
    auto srvConfig = configTree.get_child_optional("config");

    if(srvConfig.is_initialized())
    {
        return srvConfig.value();
    }
    else
    {
        srvConfig = configTree.get_child_optional("service");
        if(srvConfig.is_initialized())
        {
            return srvConfig.value();
        }

        return IService::ConfigType();
    }
}

//-----------------------------------------------------------------------------

void IService::configure()
{
    if(m_configurationState == UNCONFIGURED)
    {
        m_configurationState = CONFIGURING;
        if(m_globalState == STOPPED)
        {
            try
            {
                this->configuring();
            }
            catch(std::exception& e)
            {
                SIGHT_ERROR("Error while configuring service '" + this->getID() + "' : " + e.what());
            }
        }
        else if(m_globalState == STARTED)
        {
            this->reconfiguring();
        }

        m_configurationState = CONFIGURED;
    }
}

//-----------------------------------------------------------------------------

void IService::configure(const ConfigType& ptree)
{
    core::runtime::ConfigurationElement::sptr ce;

    ConfigType serviceConfig;
    serviceConfig.add_child("service", ptree);

    ce = core::runtime::Convert::fromPropertyTree(serviceConfig);

    SIGHT_ASSERT("Invalid ConfigurationElement", ce);

    this->setConfiguration(ce);
    this->configure();
}

//-----------------------------------------------------------------------------

void IService::reconfiguring()
{
    SIGHT_FATAL(
        "If this method (reconfiguring) is called, it must be overridden in the implementation ("
        << this->getClassname() << ", " << this->getID() << ")"
    );
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::start()
{
    if(!m_associatedWorker || core::thread::getCurrentThreadId() == m_associatedWorker->getThreadId())
    {
        return this->internalStart(false);
    }
    else
    {
        return m_slotStart->asyncRun();
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::stop()
{
    if(!m_associatedWorker || core::thread::getCurrentThreadId() == m_associatedWorker->getThreadId())
    {
        return this->internalStop(false);
    }
    else
    {
        return m_slotStop->asyncRun();
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::update()
{
    if(!m_associatedWorker || core::thread::getCurrentThreadId() == m_associatedWorker->getThreadId())
    {
        return this->internalUpdate(false);
    }
    else
    {
        return m_slotUpdate->asyncRun();
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::swapKey(const IService::KeyType& _key, data::Object::sptr _obj)
{
    if(!m_associatedWorker || core::thread::getCurrentThreadId() == m_associatedWorker->getThreadId())
    {
        return this->internalSwapKey(_key, _obj, false);
    }
    else
    {
        return m_slotSwapKey->asyncRun(_key, _obj);
    }
}

//-----------------------------------------------------------------------------

IService::GlobalStatus IService::getStatus() const noexcept
{
    return m_globalState;
}

//-----------------------------------------------------------------------------

bool IService::isStarted() const noexcept
{
    return m_globalState == STARTED;
}

//-----------------------------------------------------------------------------

bool IService::isStopped() const noexcept
{
    return m_globalState == STOPPED;
}

//-----------------------------------------------------------------------------

IService::ConfigurationStatus IService::getConfigurationStatus() const noexcept
{
    return m_configurationState;
}

//-----------------------------------------------------------------------------

IService::UpdatingStatus IService::getUpdatingStatus() const noexcept
{
    return m_updatingState;
}

//-----------------------------------------------------------------------------

void IService::setWorker(core::thread::Worker::sptr worker)
{
    m_associatedWorker = worker;
    core::com::HasSlots::m_slots.setWorker(m_associatedWorker);
}

//-----------------------------------------------------------------------------

core::thread::Worker::sptr IService::getWorker() const
{
    return m_associatedWorker;
}

//-----------------------------------------------------------------------------

IService::KeyConnectionsMap IService::getAutoConnections() const
{
    KeyConnectionsMap connections;
    return connections;
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::startSlot()
{
    return this->internalStart(true);
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::internalStart(bool _async)
{
    SIGHT_DEBUG("Starting service '" + this->getID() + "' [" + this->getClassname() + "]");
    SIGHT_FATAL_IF("Service " << this->getID() << " already started", m_globalState != STOPPED);

    this->connectToConfig();

    m_globalState = STARTING;

    PackagedTaskType task(std::bind(&IService::starting, this));
    SharedFutureType future = task.get_future();
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(const std::exception& e)
    {
        SIGHT_ERROR("Error while STARTING service '" + this->getID() + "' : " + e.what());
        SIGHT_ERROR("Service '" + this->getID() + "' is still STOPPED.");
        m_globalState = STOPPED;
        this->disconnectFromConfig();

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with ufuture.get()
            return future;
        }
        else
        {
            // Rethrow the same exception
            throw;
        }
    }
    m_globalState = STARTED;

    this->autoConnect();

    auto sig = this->signal<StartedSignalType>(s_STARTED_SIG);
    sig->asyncEmit();

    return future;
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::stopSlot()
{
    return this->internalStop(true);
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::internalStop(bool _async)
{
    SIGHT_DEBUG("Stopping service '" + this->getID() + "' [" + this->getClassname() + "]");
    SIGHT_FATAL_IF("Service " << this->getID() << " already stopped", m_globalState != STARTED);

    this->autoDisconnect();

    PackagedTaskType task(std::bind(&IService::stopping, this));
    SharedFutureType future = task.get_future();

    m_globalState = STOPPING;
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Error while STOPPING service '" + this->getID() + "' : " + e.what());
        SIGHT_ERROR("Service '" + this->getID() + "' is still STARTED.");
        m_globalState = STARTED;
        this->autoConnect();

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with ufuture.get()
            return future;
        }
        else
        {
            // Rethrow the same exception
            throw;
        }
    }
    m_globalState = STOPPED;

    auto sig = this->signal<StoppedSignalType>(s_STOPPED_SIG);
    sig->asyncEmit();

    this->disconnectFromConfig();

    return future;
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::swapKeySlot(const KeyType& _key, data::Object::sptr _obj)
{
    return this->internalSwapKey(_key, _obj, true);
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::internalSwapKey(const KeyType& _key, data::Object::sptr _obj, bool _async)
{
    SIGHT_DEBUG("Swapping service '" + this->getID() + "' [" + this->getClassname() + "]");
    SIGHT_FATAL_IF(
        "Service " << this->getID() << " is not STARTED, no swapping with Object "
        << (_obj ? _obj->getID() : "nullptr"),
        m_globalState != STARTED
    );

    auto fn = std::bind(static_cast<void (IService::*)(const KeyType&)>(&IService::swapping), this, _key);
    PackagedTaskType task(fn);
    SharedFutureType future = task.get_future();

    this->autoDisconnect();

    m_globalState = SWAPPING;
    task();
    m_globalState = STARTED;

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Error while SWAPPING service '" + this->getID() + "' : " + e.what());

        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with ufuture.get()
            return future;
        }
        else
        {
            // Rethrow the same exception
            throw;
        }
    }

    this->autoConnect();

    auto sig = this->signal<SwappedSignalType>(s_SWAPPED_SIG);
    sig->asyncEmit();

    return future;
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::updateSlot()
{
    return this->internalUpdate(true);
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::internalUpdate(bool _async)
{
    if(m_globalState != STARTED)
    {
        SIGHT_WARN(
            "INVOKING update WHILE STOPPED (" << m_globalState << ") on service '" << this->getID()
            << "' of type '" << this->getClassname() << "': update is discarded."
        );
        return SharedFutureType();
    }

    SIGHT_ASSERT(
        "INVOKING update WHILE NOT IDLE (" << m_updatingState << ") on service '" << this->getID()
        << "' of type '" << this->getClassname() << "'",
        m_updatingState == NOTUPDATING
    );

    PackagedTaskType task(std::bind(&IService::updating, this));
    SharedFutureType future = task.get_future();
    m_updatingState = UPDATING;
    task();

    try
    {
        // This allows to trigger the exception if there was one
        future.get();
    }
    catch(std::exception& e)
    {
        SIGHT_ERROR("Error while UPDATING service '" + this->getID() + "' : " + e.what());

        m_updatingState = NOTUPDATING;
        if(!_async)
        {
            // The future is shared, thus the caller can still catch the exception if needed with ufuture.get()
            return future;
        }
        else
        {
            // Rethrow the same exception
            throw;
        }
    }
    m_updatingState = NOTUPDATING;

    auto sig = this->signal<StartedSignalType>(s_UPDATED_SIG);
    sig->asyncEmit();

    return future;
}

//-----------------------------------------------------------------------------

void IService::connectToConfig()
{
    service::registry::Proxy::sptr proxy = service::registry::Proxy::getDefault();

    for(const auto& proxyCfg : m_proxies)
    {
        for(const auto& signalCfg : proxyCfg.second.m_signals)
        {
            SIGHT_ASSERT("Invalid signal source", signalCfg.first == this->getID());

            core::com::SignalBase::sptr sig = this->signal(signalCfg.second);
            SIGHT_ASSERT("Signal '" + signalCfg.second + "' not found in source '" + signalCfg.first + "'.", sig);
            try
            {
                proxy->connect(proxyCfg.second.m_channel, sig);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Signal '" + signalCfg.second + "' from '" + signalCfg.first + "' can not be connected to the"
                                                                                   " channel '" + proxyCfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                                   )
                );
            }
        }

        for(const auto& slotCfg : proxyCfg.second.m_slots)
        {
            SIGHT_ASSERT("Invalid slot destination", slotCfg.first == this->getID());

            core::com::SlotBase::sptr slot = this->slot(slotCfg.second);
            SIGHT_ASSERT("Slot '" + slotCfg.second + "' not found in source '" + slotCfg.first + "'.", slot);

            try
            {
                proxy->connect(proxyCfg.second.m_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slotCfg.second + "' from '" + slotCfg.first + "' can not be connected to the "
                                                                             "channel '" + proxyCfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                             )
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void IService::autoConnect()
{
    service::IService::KeyConnectionsMap connectionMap = this->getAutoConnections();

    SIGHT_ERROR_IF(
        "The service '" + this->getID() + "'(" + this->getClassname()
        + ") is set to 'autoConnect=\"true\"' but is has no object to connect",
        m_serviceConfig.m_globalAutoConnect && m_serviceConfig.m_objects.empty()
    );

    for(const auto& objectCfg : m_serviceConfig.m_objects)
    {
        if(m_serviceConfig.m_globalAutoConnect || objectCfg.m_autoConnect)
        {
            service::IService::KeyConnectionsType connections;
            if(!connectionMap.empty())
            {
                auto it = connectionMap.find(objectCfg.m_key);
                if(it != connectionMap.end())
                {
                    connections = it->second;
                }
                else
                {
                    // Special case if we have a key from a group we check with the name of the group
                    std::smatch match;
                    static const std::regex reg("(.*)#[0-9]+");
                    if(std::regex_match(objectCfg.m_key, match, reg) && match.size() == 2)
                    {
                        const std::string group = match[1].str();
                        auto itConnection       = connectionMap.find(group);
                        if(itConnection != connectionMap.end())
                        {
                            connections = itConnection->second;
                        }
                    }
                }

                SIGHT_ERROR_IF(
                    "Object '" + objectCfg.m_key + "' of '" + this->getID() + "'(" + this->getClassname()
                    + ") is set to 'autoConnect=\"true\"' but there is no connection available.",
                    connections.empty() && objectCfg.m_autoConnect
                );
            }
            else
            {
                SIGHT_ERROR(
                    "Object '" + objectCfg.m_key + "' of '" + this->getID() + "'(" + this->getClassname()
                    + ") is set to 'autoConnect=\"true\"' but there is no connection available."
                );
            }

            data::Object::csptr obj;

            switch(objectCfg.m_access)
            {
                case AccessType::INPUT:
                {
                    auto itObj = m_inputsMap.find(objectCfg.m_key);
                    if(itObj != m_inputsMap.end())
                    {
                        obj = itObj->second.getShared();
                    }

                    break;
                }

                case AccessType::INOUT:
                {
                    auto itObj = m_inOutsMap.find(objectCfg.m_key);
                    if(itObj != m_inOutsMap.end())
                    {
                        obj = itObj->second.getShared();
                    }

                    break;
                }

                case AccessType::OUTPUT:
                {
                    SIGHT_WARN("Can't autoConnect to an output for now");
                    auto itObj = m_outputsMap.find(objectCfg.m_key);
                    if(itObj != m_outputsMap.end())
                    {
                        obj = itObj->second.get_shared();
                    }

                    break;
                }
            }

            SIGHT_ASSERT(
                "Object '" + objectCfg.m_key
                + "' has not been found when autoConnecting service '" + m_serviceConfig.m_uid + "'.",
                (!objectCfg.m_optional && obj) || objectCfg.m_optional
            );

            if(obj)
            {
                m_autoConnections.connect(obj, this->getSptr(), connections);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void IService::disconnectFromConfig()
{
    service::registry::Proxy::sptr proxy = service::registry::Proxy::getDefault();

    for(const auto& proxyCfg : m_proxies)
    {
        for(const auto& signalCfg : proxyCfg.second.m_signals)
        {
            SIGHT_ASSERT("Invalid signal source", signalCfg.first == this->getID());

            core::com::SignalBase::sptr sig = this->signal(signalCfg.second);

            try
            {
                proxy->disconnect(proxyCfg.second.m_channel, sig);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Signal '" + signalCfg.second + "' from '" + signalCfg.first + "' can not be disconnected "
                                                                                   "from the channel '" + proxyCfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                                   )
                );
            }
        }

        for(const auto& slotCfg : proxyCfg.second.m_slots)
        {
            SIGHT_ASSERT("Invalid slot destination", slotCfg.first == this->getID());

            core::com::SlotBase::sptr slot = this->slot(slotCfg.second);
            try
            {
                proxy->disconnect(proxyCfg.second.m_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slotCfg.second + "' from '" + slotCfg.first + "' can not be disconnected from the "
                                                                             "channel '" + proxyCfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                             )
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void IService::autoDisconnect()
{
    m_autoConnections.disconnect();
}

//-----------------------------------------------------------------------------

void IService::addProxyConnection(const helper::ProxyConnections& proxy)
{
    m_proxies[proxy.m_channel] = proxy;
}

//-----------------------------------------------------------------------------

bool IService::hasObjInfoFromId(const std::string& objId) const
{
    auto itr = std::find_if(
        m_serviceConfig.m_objects.begin(),
        m_serviceConfig.m_objects.end(),
        [&](const ObjectServiceConfig& objCfg)
        {
            return objCfg.m_uid == objId;
        });

    return itr != m_serviceConfig.m_objects.end();
}

//------------------------------------------------------------------------------

bool IService::hasAllRequiredObjects() const
{
    bool hasAllObjects = true;

    for(const auto& objectCfg : m_serviceConfig.m_objects)
    {
        if(objectCfg.m_optional == false)
        {
            if(objectCfg.m_access == service::IService::AccessType::INPUT)
            {
                if(nullptr == this->getInput<data::Object>(objectCfg.m_key))
                {
                    SIGHT_DEBUG(
                        "The 'input' object with key '" + objectCfg.m_key + "' is missing for '" + this->getID()
                        + "'"
                    );
                    hasAllObjects = false;
                    break;
                }
            }
            else if(objectCfg.m_access == service::IService::AccessType::INOUT)
            {
                if(nullptr == this->getInOut<data::Object>(objectCfg.m_key))
                {
                    SIGHT_DEBUG(
                        "The 'input' object with key '" + objectCfg.m_key + "' is missing for '" + this->getID()
                        + "'"
                    );
                    hasAllObjects = false;
                    break;
                }
            }
        }
    }

    return hasAllObjects;
}

//------------------------------------------------------------------------------

const IService::ObjectServiceConfig& IService::getObjInfoFromId(const std::string& objId) const
{
    auto idItr = std::find_if(
        m_serviceConfig.m_objects.begin(),
        m_serviceConfig.m_objects.end(),
        [&](const ObjectServiceConfig& objCfg)
        {
            return objCfg.m_uid == objId;
        });
    SIGHT_THROW_IF(
        "Object '" + objId + "' is not regisreted for '" + this->getID() + "'.",
        idItr == m_serviceConfig.m_objects.end()
    );

    return *idItr;
}

//------------------------------------------------------------------------------

const IService::ObjectServiceConfig& IService::getObjInfoFromKey(const std::string& key) const
{
    auto keyItr = std::find_if(
        m_serviceConfig.m_objects.begin(),
        m_serviceConfig.m_objects.end(),
        [&](const ObjectServiceConfig& objCfg)
        {
            return objCfg.m_key == key;
        });
    SIGHT_THROW_IF(
        "key '" + key + "' is not regisreted for '" + this->getID() + "'.",
        keyItr == m_serviceConfig.m_objects.end()
    );

    return *keyItr;
}

//-----------------------------------------------------------------------------

void IService::registerObject(
    const service::IService::KeyType& key,
    const service::IService::AccessType access,
    const bool autoConnect,
    const bool optional
)
{
    auto itr = std::find_if(
        m_serviceConfig.m_objects.begin(),
        m_serviceConfig.m_objects.end(),
        [&](const ObjectServiceConfig& objInfo)
        {
            return objInfo.m_key == key;
        });
    if(itr == m_serviceConfig.m_objects.end())
    {
        ObjectServiceConfig objConfig;
        objConfig.m_key         = key;
        objConfig.m_access      = access;
        objConfig.m_autoConnect = autoConnect;
        objConfig.m_optional    = optional;

        m_serviceConfig.m_objects.push_back(objConfig);
    }
    else
    {
        ObjectServiceConfig& objConfig = *itr;
        objConfig.m_key         = key;
        objConfig.m_access      = access;
        objConfig.m_autoConnect = autoConnect;
        objConfig.m_optional    = optional;
    }
}

//-----------------------------------------------------------------------------

void IService::registerObjectGroup(
    const std::string& key,
    AccessType access,
    const std::uint8_t minNbObject,
    const bool autoConnect,
    const std::uint8_t maxNbObject
)
{
    for(std::uint8_t i = 0 ; i < maxNbObject ; ++i)
    {
        const bool optional = (i < minNbObject ? false : true);
        ObjectServiceConfig objConfig;
        objConfig.m_key         = KEY_GROUP_NAME(key, i);
        objConfig.m_access      = access;
        objConfig.m_autoConnect = autoConnect;
        objConfig.m_optional    = optional;

        m_serviceConfig.m_objects.push_back(objConfig);
    }

    m_keyGroupSize[key] = minNbObject;
}

//-----------------------------------------------------------------------------

/**
 * @brief Streaming a service
 * @see IService::operator<<(std::ostream & _ostream, IService& _service)
 * @note Invoke IService::info( std::ostream )
 */
std::ostream& operator<<(std::ostream& _ostream, IService& _service)
{
    _service.info(_ostream);
    return _ostream;
}

//-----------------------------------------------------------------------------

} // namespace sight::service
