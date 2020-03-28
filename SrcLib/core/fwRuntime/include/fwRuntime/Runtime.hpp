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

#pragma once

#include "fwCore/macros.hpp"

#include "fwRuntime/config.hpp"
#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/Version.hpp"

#include <filesystem>
#include <set>
#include <vector>

namespace fwRuntime
{
class Module;
struct ConfigurationElement;
struct Extension;
class IExecutable;
struct ExecutableFactory;
struct IPlugin;
}

namespace fwRuntime
{

/**
 * @brief   Defines the runtime class.
 */
class FWRUNTIME_CLASS_API Runtime
{
public:
    /**
     * @name    Type Definitions
     */
    //@{
    /// Defines the module container type.
    typedef std::set< std::shared_ptr<Module> >     ModuleContainer;
    /// Defines the extension container type.
    typedef std::set< std::shared_ptr<Extension> >  ExtensionContainer;
    /// Defines the extension container type.
    typedef ExtensionContainer::iterator ExtensionIterator;
    //@}

    /**
     * @brief   Retrieves the default runtime instance.
     */
    FWRUNTIME_API static Runtime* getDefault();
    FWRUNTIME_API static Runtime& get();

    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~Runtime();

    /**
     * @brief       Set the working path where Modules and share folder are located.
     *
     * @param[in]   workingPath a boost path.
     */
    FWRUNTIME_API void setWorkingPath(const std::filesystem::path& workingPath);

    /**
     * @brief       Get the path where Modules and share folder are located.
     *
     * @return      a boost path.
     */
    FWRUNTIME_API std::filesystem::path getWorkingPath() const;

    /**
     * @name    Modules
     *
     * @{
     */

    /**
     * @brief       Adds all modules found in the given path.
     *
     * @param[in]   repository  a path that may containing modules
     * @deprecated  Bundle has been renamed to Module, please use addModules() instead
     */
    [[deprecated]] FWRUNTIME_API virtual void addBundles( const std::filesystem::path& repository ) = 0;

    /**
     * @brief       Adds all modules found in the given path.
     *
     * @param[in]   repository  a path that may containing modules
     */
    FWRUNTIME_API virtual void addModules( const std::filesystem::path& repository ) = 0;

    /**
     * @brief       Adds all modules found at the default location.
     *
     * @remark      The given module state will be altered according to the current configuration rules.
     * @deprecated  Bundle has been renamed to Module, please use addDefaultModules() instead
     */
    [[deprecated]] FWRUNTIME_API virtual void addDefaultBundles() = 0;

    /**
     * @brief       Adds all modules found at the default location.
     *
     * @remark      The given module state will be altered according to the current configuration rules.
     */
    FWRUNTIME_API virtual void addDefaultModules() = 0;

    /**
     * @brief       Retrieves the module for the specified idenfier.
     *
     * @param[in]   identifier  a string containing a module identifier
     * @param[in]   version     the version of the module (undefined by default)
     *
     * @return      a shared pointer to the found module or null if none
     * @deprecated  Bundle has been renamed to Module, please use findModule() instead
     */
    [[deprecated]] FWRUNTIME_API virtual std::shared_ptr< Module > findBundle( const std::string& identifier,
                                                                               const Version& version =
                                                                                   Version() ) const = 0;

    /**
     * @brief       Retrieves the module for the specified idenfier.
     *
     * @param[in]   identifier  a string containing a module identifier
     * @param[in]   version     the version of the module (undefined by default)
     *
     * @return      a shared pointer to the found module or null if none
     */
    FWRUNTIME_API virtual std::shared_ptr< Module > findModule( const std::string& identifier,
                                                                const Version& version = Version() ) const = 0;
    //@}

    /**
     * @name    Executable, Executable Factories & Dynamic Executable Instanciation
     *
     * @{
     */

    /**
     * @brief   Create an instance of the given executable object type.
     *
     * An attempt is made to retrieve a registered executable factory. If none
     * is found, the creation will fail.
     *
     * @remark      This method will not try to load any module.
     *
     * @param[in]   type    a string containing an executable type
     *
     * @return      a pointer to the created executable instance
     */
    FWRUNTIME_API virtual IExecutable* createExecutableInstance( const std::string& type ) = 0;

    /**
     * @brief   Create an instance of the given executable object type and configuration element.
     *
     * An attempt is made to find a registered executable factory. If none
     * is found, the module of the given configuration element is started in the
     * hope it will register a executable factory for the given type. Then an
     * executable factory for the given type is searched once again and the
     * instantiation procedure goes further.
     *
     * @param[in]   type                    a string containing an executable type
     * @param[in]   configurationElement    a shared pointer to the configuration element to use for the executable
     * initialization
     *
     * @return  a pointer to the created executable instance
     */
    FWRUNTIME_API virtual IExecutable* createExecutableInstance( const std::string& type,
                                                                 SPTR(ConfigurationElement) configurationElement ) = 0;
    //@}

    /**
     * @name    Extensions
     *
     * @{
     */
    /**
     * @brief   Retrieves the extension collection.
     *
     * @return  an iterator
     */
    FWRUNTIME_API virtual ExtensionContainer getExtensions() = 0;

    /**
     * @brief       Retrieves the extension instance matching the specified identifier.
     *
     * @param[in]   identifier  a string containing an extension identifier
     *
     * @return      a shared pointer to the found extension instance or null if none
     */
    FWRUNTIME_API virtual std::shared_ptr< Extension > findExtension( const std::string& identifier ) const = 0;

    //@}

    /// Return all modules known by the runtime
    [[deprecated]] FWRUNTIME_API virtual ModuleContainer getBundles() = 0;

    /// Return all modules known by the runtime
    FWRUNTIME_API virtual ModuleContainer getModules() = 0;

protected:

    /**
     * @brief   Constructor.
     */
    Runtime();

private:

    /// Defines the executable factory container type.
    typedef std::set< std::shared_ptr< ExecutableFactory > > ExecutableFactoryContainer;

    /// Defines the plugin container type.
    typedef std::vector< std::shared_ptr<IPlugin> > PluginContainer;

    /// The runtime instance.
    static std::shared_ptr<Runtime> m_instance;

    /// Contains all registered extensions.
    ExtensionContainer m_extensions;

    /// Path where Modules and share folder are located.
    std::filesystem::path m_workingPath;
};

} // namespace fwRuntime
