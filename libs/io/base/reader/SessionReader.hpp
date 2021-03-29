/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/base/config.hpp"
#include "io/base/location/Session.hpp"
#include "io/base/reader/IObjectReader.hpp"

#include <data/Object.hpp>

namespace sight::io::base
{
namespace reader
{

class SessionReader final :
    public IObjectReader,
    public location::Session
{
public:

    SIGHT_DECLARE_CLASS(SessionReader, IObjectReader, factory::New< SessionReader >)
    SIGHT_ALLOW_SHARED_FROM_THIS()

    /// Delete default constructors and assignment operators
    SessionReader()                                = delete;
    SessionReader(const SessionReader&)            = delete;
    SessionReader(SessionReader&&)                 = delete;
    SessionReader& operator=(const SessionReader&) = delete;
    SessionReader& operator=(SessionReader&&)      = delete;

    /// Use default destructor
    IO_BASE_API ~SessionReader() override = default;

    /// Constructor. Do nothing.
    inline SessionReader(IObjectReader::Key key)
    {
    }

    /// Read the file with standard iostream API.
    IO_BASE_API void read() override;

    /// Defines extension supported by this reader ".sight"
    inline std::string extension() override
    {
        return ".sight";
    }
};

} // namespace reader
} // namespace sight::io::base
