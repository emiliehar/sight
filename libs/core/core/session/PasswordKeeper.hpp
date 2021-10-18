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

#include "core/config.hpp"
#include <core/crypto/secure_string.hpp>
#include <core/macros.hpp>

namespace sight::core::session
{

/// Class designed to hold and check in a somewhat secure way a global password and "instance" passwords.
class CORE_CLASS_API PasswordKeeper final
{
public:

    SIGHT_DECLARE_CLASS(PasswordKeeper);

    /// Delete default copy constructors and assignment operators
    PasswordKeeper(const PasswordKeeper&)            = delete;
    PasswordKeeper(PasswordKeeper&&)                 = delete;
    PasswordKeeper& operator=(const PasswordKeeper&) = delete;
    PasswordKeeper& operator=(PasswordKeeper&&)      = delete;

    /// Default constructor
    CORE_API PasswordKeeper() noexcept;

    /// Default destructor
    CORE_API ~PasswordKeeper() noexcept;

    /// Gets the global password hash
    CORE_API static core::crypto::secure_string getGlobalPasswordHash();

    /// Gets the global password
    CORE_API static core::crypto::secure_string getGlobalPassword();

    /// Sets the global password
    /// @param password the new global password
    CORE_API static void setGlobalPassword(const core::crypto::secure_string& password);

    /// Returns true if the password matches
    /// @param password the password to verify against global stored password
    CORE_API static bool checkGlobalPassword(const core::crypto::secure_string& password);

    /// Gets the password
    CORE_API core::crypto::secure_string getPasswordHash() const;

    /// Gets the password
    CORE_API core::crypto::secure_string getPassword() const;

    /// Sets the password
    /// @param password the new password
    CORE_API void setPassword(const core::crypto::secure_string& password);

    /// Returns true if the password matches
    /// @param password the password to verify against stored password
    CORE_API bool checkPassword(const core::crypto::secure_string& password) const;

private:

    /// PImpl
    class PasswordKeeperImpl;
    std::unique_ptr<PasswordKeeperImpl> m_pimpl;
};

} // namespace sight::core::session
