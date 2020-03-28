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

#include "fwRuntime/config.hpp"
#include "fwRuntime/impl/dl/Native.hpp"
#include "fwRuntime/RuntimeException.hpp"

#include <filesystem>
#include <memory>

namespace fwRuntime
{

class Module;

namespace impl
{

namespace dl
{

/**
 * @brief   Defines the module class.
 * This class is only a bridge to a native module implementor.
 */
class Library
{
public:
    /**
     * @brief       Constructor.
     *
     * @param[in]   modulePath      a path pointing the module to load without any extension
     *                          information
     */
    Library( const std::filesystem::path& modulePath ) noexcept;

    /**
     * @brief   Destructor : does nothing.
     */
    ~Library() noexcept
    {
    }

    /**
     * @brief   Tells if the module is loaded.
     *
     * @return  true or false
     */
    bool isLoaded() const noexcept
    {
        return m_implementor->isLoaded();
    }

    /**
     * @brief   Retrieves the file path of the library including the owning module's path.
     *
     * @return  a file path
     *
     * @see     getPath
     */
    const std::filesystem::path getFullPath() const
    {
        return m_implementor->getFullPath();
    }

    /**
     * @brief   Retrieves the file path of the library.
     *
     * @remark  This path is relative to the owning module's path.
     *
     * @return  a file path
     *
     * @see     getFullPath
     */
    const std::filesystem::path getPath() const
    {
        return m_implementor->getPath();
    }

    /**
     * @brief       Retrieves the address of a symbol specified by its name.
     *
     * @param[in]   name    a string containing the symbol name.
     *
     * @return      a pointer to the found symbol or null if none has been found
     */
    void* getSymbol(const std::string& name) const
    {
        return m_implementor->getSymbol(name);
    }

    /**
     * @brief   Loads the module.
     */
    void load()
    {
        m_implementor->load();
    }

    /**
     * @brief       Sets the module the library is attached to.
     *
     * @param[in]   module  a pointer to a module instance
     */
    void setModule(const ::fwRuntime::Module* module) noexcept
    {
        m_implementor->setModule(module);
    }

    /**
     * @brief   Undloads the module.
     */
    void unload()
    {
        m_implementor->unload();
    }

private:

    /**
     * @brief   The native module implementator.
     */
    std::unique_ptr< Native > m_implementor;

    /**
     * @brief   Assignment operator.
     *
     * @remark  Assignment is forbidden for this class.
     */
    void operator=(const Library&) noexcept
    {
    }
};

} // namespace dl

} // namespace impl

} // namespace fwRuntime
