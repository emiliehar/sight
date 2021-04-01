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
#include "io/base/session/ISession.hpp"

#include <io/zip/IWriteArchive.hpp>

namespace sight::io::base
{
namespace session
{
/**
 * @brief Class to define a location that holds what is needed to serialize a session.
 *
 */
class IO_BASE_CLASS_API WriteSession : public ISession<io::zip::IWriteArchive>
{
public:
    SIGHT_DECLARE_CLASS(WriteSession)

    /// Factory
    inline static WriteSession::sptr New()
    {
        struct make_shared_enabler : public WriteSession {};
        return std::make_shared<make_shared_enabler>();
    }

    /// Set the archive path and open the archive in write mode
    IO_BASE_API void set_archive_path(const std::filesystem::path& archive_path) override;

protected:

    /// Constructor
    IO_BASE_API WriteSession() = default;

    /// Destructor
    IO_BASE_API virtual ~WriteSession() = default;
};

} // namespace session
} // namespace sight::io::base