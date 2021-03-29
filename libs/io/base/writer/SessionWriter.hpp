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
#include "io/base/writer/IObjectWriter.hpp"

#include <data/Object.hpp>

namespace sight::io::base
{
namespace writer
{

class SessionWriter final :
    public IObjectWriter,
    public location::Session
{
public:

    SIGHT_DECLARE_CLASS(SessionWriter, IObjectWriter, factory::New< SessionWriter >)
    SIGHT_ALLOW_SHARED_FROM_THIS()

    /// Default constructors, destructor and assignment operators
    SessionWriter()                                = delete;
    SessionWriter(const SessionWriter&)            = delete;
    SessionWriter(SessionWriter&&)                 = delete;
    SessionWriter& operator=(const SessionWriter&) = delete;
    SessionWriter& operator=(SessionWriter&&)      = delete;
    IO_BASE_API ~SessionWriter() override          = default;

    /// Constructor. Do nothing.
    inline SessionWriter(io::base::writer::IObjectWriter::Key key)
    {
    }

    /// Write the file
    IO_BASE_API void write() override;

    /// Defines extension supported by this writer ".sight"
    inline std::string extension() override
    {
        return ".sight";
    }
};

} // namespace writer
} // namespace sight::io::base
