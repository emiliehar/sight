/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "io/base/config.hpp"

#include <core/location/ILocation.hpp>

#include <filesystem>

namespace sight::io::base::location
{
/**
 * @brief Class to define a location that holds what is needed to de/serialize a session.
 * 
 */
class CORE_CLASS_API Session : public core::location::ILocation
{
public:
    SIGHT_DECLARE_CLASS(Session)

    /// Factory
    inline static Session::sptr New()
    {
        struct make_shared_enabler : public Session {};
        return std::make_shared<make_shared_enabler>();
    }

    /// String serialization function
    inline std::string toString() const override
    {
        return m_file.string();
    }

    //------------------------------------------------------------------------------

    inline void setFile(const std::filesystem::path& filePath)
    {
        m_file = filePath;
    }

    //------------------------------------------------------------------------------

    inline std::filesystem::path getFile()
    {
        return m_file;
    }

protected:

    /// Constructor
    CORE_API Session() = default;

    /// Destructor
    CORE_API virtual ~Session() = default;

private:

    /// The filesystem vector
    std::filesystem::path m_file;
};

} // namespace sight::io::base::location
