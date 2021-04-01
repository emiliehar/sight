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

namespace sight::io::base
{
namespace session
{
/**
 * @brief Interface to define a location that holds what is needed to de/serialize a session.
 *
 */
template< class ARCHIVE >
class IO_BASE_CLASS_API ISession : public core::location::ILocation
{
public:
    SIGHT_DECLARE_CLASS(ISession)

    /// String serialization function
    inline std::string toString() const override
    {
        return get_archive_path().string();
    }

    //------------------------------------------------------------------------------

    /// Set the archive path and open the archive
    IO_BASE_API virtual void set_archive_path(const std::filesystem::path& archive_path) = 0;

    //------------------------------------------------------------------------------

    inline std::filesystem::path get_archive_path() const
    {
        return m_archive ? m_archive->getArchivePath() : "";
    }

    //------------------------------------------------------------------------------

    inline typename ARCHIVE::sptr get_archive() const
    {
        return m_archive;
    }

protected:

    /// Constructor
    IO_BASE_API ISession() = default;

    /// Destructor
    IO_BASE_API virtual ~ISession() = default;

private:

    typename ARCHIVE::sptr m_archive;
};

} // namespace session
} // namespace sight::io::base
