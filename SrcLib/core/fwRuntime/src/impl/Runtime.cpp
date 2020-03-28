/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fwRuntime/impl/Runtime.hpp"

#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/ExecutableFactory.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/IExecutable.hpp"
#include "fwRuntime/impl/ExtensionPoint.hpp"
#include "fwRuntime/impl/io/ModuleDescriptorReader.hpp"
#include "fwRuntime/impl/Module.hpp"
#include "fwRuntime/IPlugin.hpp"

#include <boost/dll/runtime_symbol_info.hpp>

#include <limits.h>

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <functional>

namespace fwRuntime
{

namespace impl
{
//------------------------------------------------------------------------------

std::shared_ptr<Runtime> Runtime::m_instance;

//------------------------------------------------------------------------------

Runtime::Runtime()
{
    auto execPath = ::boost::dll::program_location();

    // The program location is 'path/bin/executable', real working path is 'path'
    m_workingPath = std::filesystem::path(execPath.normalize().parent_path().parent_path().string());
}

//------------------------------------------------------------------------------

Runtime::~Runtime()
{
}

//------------------------------------------------------------------------------

void Runtime::addModule( std::shared_ptr< Module > module )
{
    m_modules.insert( module );
    std::for_each( module->extensionsBegin(), module->extensionsEnd(),
                   std::bind(&Runtime::addExtension, this, std::placeholders::_1));
    std::for_each( module->extensionPointsBegin(), module->extensionPointsEnd(),
                   std::bind(&Runtime::addExtensionPoint, this, std::placeholders::_1));
    std::for_each( module->executableFactoriesBegin(), module->executableFactoriesEnd(),
                   std::bind(&Runtime::addExecutableFactory, this, std::placeholders::_1));
}

//------------------------------------------------------------------------------

void Runtime::unregisterModule( std::shared_ptr< Module > module )
{
    std::for_each( module->executableFactoriesBegin(), module->executableFactoriesEnd(),
                   std::bind(&Runtime::unregisterExecutableFactory, this, std::placeholders::_1));
    std::for_each( module->extensionPointsBegin(), module->extensionPointsEnd(),
                   std::bind(&Runtime::unregisterExtensionPoint, this, std::placeholders::_1));
    std::for_each( module->extensionsBegin(), module->extensionsEnd(),
                   std::bind(&Runtime::unregisterExtension, this, std::placeholders::_1));
    m_modules.erase( module );
}

//------------------------------------------------------------------------------

void Runtime::addBundles( const std::filesystem::path& repository )
{
    FW_DEPRECATED_MSG("addBundles", "22.0");
    this->addModules(repository);
}

//------------------------------------------------------------------------------

void Runtime::addModules( const std::filesystem::path& repository )
{
    try
    {
        using ::fwRuntime::impl::io::ModuleDescriptorReader;
        const ModuleDescriptorReader::ModuleContainer modules = ModuleDescriptorReader::createModules( repository );
        std::for_each( modules.begin(), modules.end(), std::bind(&Runtime::addModule, this, std::placeholders::_1) );
    }
    catch(const std::exception& exception)
    {
        throw RuntimeException( std::string("Error while adding modules. ") + exception.what() );
    }
}

//------------------------------------------------------------------------------

void Runtime::addDefaultBundles()
{
    FW_DEPRECATED_MSG("addDefaultBundles", "22.0");
    this->addDefaultModules();
}

//------------------------------------------------------------------------------

void Runtime::addDefaultModules()
{
    // Modules location
    const auto location = this->getWorkingPath() / BUNDLE_RC_PREFIX;

    SLM_ASSERT("Default Modules location not found: " + location.string(), std::filesystem::exists(location));

    // Read modules
    this->addBundles(location);
    SLM_ASSERT("Couldn't load any module from path: " + location.string(), !this->getModules().empty());
}

//------------------------------------------------------------------------------

void Runtime::addExecutableFactory( std::shared_ptr< ExecutableFactory > factory )
{
    // Ensures no registered factory has the same identifier.
    const std::string type( factory->getType() );
    if( this->findExecutableFactory(type) != 0 )
    {
        throw RuntimeException(type + ": type already used by an executable factory.");
    }
    // Stores the executable factory.
    m_executableFactories.insert( factory );
}

//------------------------------------------------------------------------------

void Runtime::unregisterExecutableFactory( std::shared_ptr< ExecutableFactory > factory )
{
    // Ensures no registered factory has the same identifier.
    const std::string type( factory->getType() );
    SLM_WARN_IF("ExecutableFactory Type " + type + " not found.", this->findExecutableFactory(type) == 0 );
    // Removes the executable factory.
    m_executableFactories.erase(factory);
}

//------------------------------------------------------------------------------

std::shared_ptr< ExecutableFactory > Runtime::findExecutableFactory( const std::string& type ) const
{
    std::shared_ptr< ExecutableFactory > resFactory;
    for(const ExecutableFactoryContainer::value_type& factory : m_executableFactories)
    {
        if(factory->getType() == type && factory->isEnable())
        {
            resFactory = factory;
            break;
        }
    }
    return resFactory;
}

//------------------------------------------------------------------------------

void Runtime::addExtension( std::shared_ptr<Extension> extension)
{
    // Asserts no registered extension has the same identifier.
    const std::string identifier(extension->getIdentifier());
    if( !identifier.empty() && this->findExtension(identifier) != 0 )
    {
        throw RuntimeException(identifier + ": identifier already used by a registered extension.");
    }
    // Stores the extension.
    m_extensions.insert( extension );
}

//------------------------------------------------------------------------------

void Runtime::unregisterExtension( std::shared_ptr<Extension> extension)
{
    // Asserts no registered extension has the same identifier.
    const std::string identifier(extension->getIdentifier());
    SLM_WARN_IF("Extension " + identifier + " not found.",
                !identifier.empty() && this->findExtension(identifier) == 0 );
    // Removes the extension.
    m_extensions.erase( extension );
}

//------------------------------------------------------------------------------

Runtime::ExtensionContainer Runtime::getExtensions()
{
    return m_extensions;
}

//------------------------------------------------------------------------------

Runtime::ExtensionIterator Runtime::extensionsBegin()
{
    return m_extensions.begin();
}

//------------------------------------------------------------------------------

Runtime::ExtensionIterator Runtime::extensionsEnd()
{
    return m_extensions.end();
}

//------------------------------------------------------------------------------

void Runtime::addExtensionPoint( std::shared_ptr<ExtensionPoint> point)
{
    // Asserts no registered extension point has the same identifier.
    const std::string identifier(point->getIdentifier());
    if( this->findExtensionPoint(identifier) != nullptr)
    {
        throw RuntimeException(identifier + ": identifier already used by a registered extension point.");
    }
    // Stores the extension.
    m_extensionPoints.insert(point);
}

//------------------------------------------------------------------------------

void Runtime::unregisterExtensionPoint( std::shared_ptr<ExtensionPoint> point)
{
    // Asserts no registered extension point has the same identifier.
    const std::string identifier(point->getIdentifier());
    SLM_WARN_IF("ExtensionPoint " + identifier + " not found.", this->findExtensionPoint(identifier) == 0);

    m_extensionPoints.erase(point);
}

//------------------------------------------------------------------------------

std::shared_ptr< ::fwRuntime::Module >
Runtime::findBundle( const std::string& identifier, const Version& version ) const
{
    FW_DEPRECATED_MSG("findBundle", "22.0");

    return findModule(identifier, version);
}

//------------------------------------------------------------------------------

std::shared_ptr< ::fwRuntime::Module >
Runtime::findModule( const std::string& identifier, const Version& version ) const
{
    std::shared_ptr<Module> resModule;
    for(const std::shared_ptr<Module>& module :  m_modules)
    {
        if(module->getIdentifier() == identifier && module->getVersion() == version)
        {
            resModule = module;
            break;
        }
    }
    return std::move(resModule);
}

//------------------------------------------------------------------------------

std::shared_ptr< Module > Runtime::findEnabledModule( const std::string& identifier, const Version& version ) const
{
    std::shared_ptr<Module> resModule;
    for(const std::shared_ptr<Module>& module :  m_modules)
    {
        if(module->getIdentifier() == identifier && module->getVersion() == version && module->isEnable())
        {
            resModule = module;
            break;
        }
    }
    return resModule;
}

//------------------------------------------------------------------------------

Runtime* Runtime::getDefault()
{
    if(m_instance.get() == nullptr)
    {
        m_instance = std::shared_ptr<Runtime>(new Runtime());
    }
    return m_instance.get();
}

//------------------------------------------------------------------------------

Runtime& Runtime::get()
{
    if(m_instance.get() == nullptr)
    {
        m_instance = std::shared_ptr<Runtime>(new Runtime());
    }
    return *m_instance.get();
}

//------------------------------------------------------------------------------

std::shared_ptr<Extension> Runtime::findExtension( const std::string& identifier ) const
{
    std::shared_ptr<Extension> resExtension;
    for(const ExtensionContainer::value_type& extension :  m_extensions)
    {
        if(extension->getIdentifier() == identifier && extension->isEnable())
        {
            resExtension = extension;
            break;
        }
    }
    return resExtension;
}

//------------------------------------------------------------------------------

::fwRuntime::Runtime::ModuleContainer Runtime::getBundles()
{
    FW_DEPRECATED_MSG("getBundles", "22.0");
    return this->getModules();
}

//------------------------------------------------------------------------------

::fwRuntime::Runtime::ModuleContainer Runtime::getModules()
{
    ::fwRuntime::Runtime::ModuleContainer modules;
    std::copy(m_modules.begin(), m_modules.end(), std::inserter(modules, modules.begin()));
    return modules;
}

//------------------------------------------------------------------------------

std::shared_ptr<ExtensionPoint> Runtime::findExtensionPoint( const std::string& identifier ) const
{
    std::shared_ptr<ExtensionPoint> resExtensionPoint;
    for(const ExtensionPointContainer::value_type& extensionPoint :  m_extensionPoints)
    {
        if(extensionPoint->getIdentifier() == identifier && extensionPoint->isEnable())
        {
            resExtensionPoint = extensionPoint;
            break;
        }
    }
    return resExtensionPoint;
}

//------------------------------------------------------------------------------

IExecutable* Runtime::createExecutableInstance( const std::string& type )
{
    std::shared_ptr< ExecutableFactory > factory;

    // Retrieves the executable factory.
    factory = this->findExecutableFactory( type );
    if( factory == nullptr )
    {
        throw RuntimeException( type + ": no executable factory found for that type." );
    }

    // Creates the executable instance
    IExecutable* result( factory->createExecutable() );

    // Job's done.
    return result;
}

//------------------------------------------------------------------------------

IExecutable* Runtime::createExecutableInstance( const std::string& type,
                                                ConfigurationElement::sptr configurationElement )
{
    std::shared_ptr< ExecutableFactory > factory;

    // Retrieves the executable factory.
    factory = this->findExecutableFactory( type );

    // If there is no factory has been found, it is possible that
    // it has not been registered since the module of the given configuration element
    // is not started.
    // So we start that module and look for the executable factory one more type.
    if( factory == nullptr)
    {
        configurationElement->getModule()->start();
        factory = this->findExecutableFactory( type );
    }

    // If we still have not found any executable factory, then notify the problem.
    if( factory == nullptr)
    {
        throw RuntimeException( type + ": no executable factory found for that type." );
    }

    // Creates the executable instance
    IExecutable* result( nullptr );
    try
    {
        factory->getModule()->start();
        result = factory->createExecutable();
        result->setInitializationData( configurationElement );
    }
    catch( const std::exception& e )
    {
        std::string message( "Unable to create an executable instance. " );
        throw RuntimeException( message + e.what() );
    }
    // Job's done.
    return result;
}

//------------------------------------------------------------------------------

} // namespace impl

} // namespace fwRuntime
